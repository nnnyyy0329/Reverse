#include "AimCamera.h"
#include "PlayerBase.h"

namespace AimCameraConfig
{
	constexpr float AIM_SENSITIVITY = 0.035f;	// エイム感度
	constexpr float AIM_DISTANCE = 2000.0f;	// エイムカメラ距離
	constexpr float AIM_ANGLE_LIMIT = 1.4f;	// エイム垂直角度制限
}

AimCamera::AimCamera()
{
	// カメラ基本設定
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);
	_nearClip = 1.0f;
	_farClip = 3000.0f;

	// エイムモード設定
	_isAiming = false;
	_aimSensitivity = AimCameraConfig::AIM_SENSITIVITY;
	_aimDistance = AimCameraConfig::AIM_DISTANCE;
	_aimAngleH = 0.0f;
	_aimAngleV = 0.0f;
	_maxAimAngleV = AimCameraConfig::AIM_ANGLE_LIMIT;

	// エイム方向
	_aimDirection = VGet(0.0f, 0.0f, -1.0f);		// 初期エイム方向は前方
	_aimStartPlayerDir = VGet(0.0f, 0.0f, -1.0f);	// 初期エイム方向は前方
	_aimCameraOffset = VGet(60.0f, 160.0f, 100.0f);	// プレイヤー右後ろのオフセット

	// カメラ基準方向
	_cameraBaseDirection = VGet(0.0f, 0.0f, -1.0f);
	_cameraBaseRight = VGet(1.0f, 0.0f, 0.0f);
}

AimCamera::~AimCamera()
{

}

void AimCamera::Initialize()
{

}

void AimCamera::Process(InputManager* input, bool isInput)
{
	if(!_isAiming){ return; }

	const AnalogState& analog = input->GetAnalog();
	float analogMin = input->GetAnalogMin();
	float rx = analog.rx;
	float ry = analog.ry;

	// エイムカメラ操作
	if(isInput)
	{
		ControlAimCamera(rx, ry, analogMin);
	}

	// エイム方向計算
	CalculateAimDirection();

	// エイムカメラ更新
	UpdateAimCamera();
}

void AimCamera::SetUp()
{
	if(!_isAiming) return;

	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);

	// カメラの描画距離を設定
	SetCameraNearFar(_nearClip, _farClip);
}

void AimCamera::Render()
{
	if(!_isAiming){ return; }

	// エイムカーソル描画
	DrawAimCursor();
}

void AimCamera::DebugRender()
{
	if(!_isAiming) return;

	int x = 400;
	int y = 150;

	// エイムモード表示
	DrawFormatString(x, y, GetColor(255, 255, 0), "AimCamera - Aiming Mode");

	// エイム情報表示
	DrawFormatString
	(x, y + 20, GetColor(255, 255, 255), "AngleH: %3.2f, AngleV: %3.2f", _aimAngleH, _aimAngleV);

	// エイム方向表示
	DrawFormatString
	(x, y + 40, GetColor(255, 255, 255), "AimDir: (%3.2f, %3.2f, %3.2f)", _aimDirection.x, _aimDirection.y, _aimDirection.z);
}

// エイム開始	
void AimCamera::StartAiming()
{
	_isAiming = true;

	// プレイヤーの向きからエイム角度を初期化
	if(_targetPlayer)
	{
		VECTOR playerDir = _targetPlayer->GetDir();	// プレイヤーの向き

		// プレイヤーの向きから少し右後ろの角度を計算
		float playerAngle = atan2f(playerDir.x, playerDir.z);	// プレイヤーの水平角度取得
		_aimAngleH = playerAngle + 0.3f;						// 少し右向きに調整
		_aimAngleV = -0.1f;										// 少し下向き

		// 初期エイム方向を計算
		CalculateAimDirection();
	}
}

// エイム終了
void AimCamera::EndAiming()
{
	_isAiming = false;
}

// エイムモード中かチェック
bool AimCamera::IsAiming() const
{
	return _isAiming;
}

// エイム方向取得
VECTOR AimCamera::GetAimDirection() const
{
	return _aimDirection;
}

// エイムカメラ更新
void AimCamera::UpdateAimCamera()
{
	if(!_targetPlayer){ return; }

	VECTOR playerPos = _targetPlayer->GetPos();	// プレイヤー位置取得

	// カメラ角度から方向ベクトルを計算
	float cosV = cos(_aimAngleV);	// 垂直角度の余弦
	float sinV = sin(_aimAngleV);	// 垂直角度の正弦
	float cosH = cos(_aimAngleH);	// 水平角度の余弦
	float sinH = sin(_aimAngleH);	// 水平角度の正弦

	// カメラの向いている方向
	VECTOR cameraForward = VGet(cosV * sinH, sinV, cosV * cosH);

	// カメラの右方向
	VECTOR cameraRight = VNorm(VCross(VGet(0.0f, 1.0f, 0.0f), cameraForward));

	// カメラの上方向
	VECTOR cameraUp = VNorm(VCross(cameraForward, cameraRight));

	// プレイヤーからのオフセットを計算
	float rightOffset = _aimCameraOffset.x;	  // 右方向のオフセット
	float upOffset = _aimCameraOffset.y;	  // 上方向のオフセット  
	float backOffset = _aimCameraOffset.z;	  // 後ろ方向のオフセット

	// カメラ位置を計算
	VECTOR offsetPos = VGet(0.0f, 0.0f, 0.0f);
	offsetPos = VAdd(offsetPos, VScale(cameraRight, rightOffset));		// 右方向
	offsetPos = VAdd(offsetPos, VScale(cameraUp, upOffset));			// 上方向
	offsetPos = VAdd(offsetPos, VScale(cameraForward, -backOffset));	// 後方向

	// カメラ位置設定
	_vPos = VAdd(playerPos, offsetPos);

	// 注視点はプレイヤーの少し前方に設定
	float lookDistance = 1000.0f;
	_vTarget = VAdd(playerPos, VScale(cameraForward, lookDistance));

	// カメラの向いている方向にプレイヤーの向きを合わせる
	VECTOR playerDir = VGet(cosV * sinH, 0.0f, cosV * cosH);	// 水平面上のプレイヤー向きベクトル計算
	playerDir = VNorm(playerDir);
	_targetPlayer->SetDir(playerDir);
}

// エイムカメラ操作
void AimCamera::ControlAimCamera(float rx, float ry, float analogMin)
{
	// 水平回転
	if(abs(rx) > analogMin)
	{
		_aimAngleH += rx * _aimSensitivity;	// 右スティックのX軸で水平回転
	}

	// 垂直回転
	if(abs(ry) > analogMin)
	{
		_aimAngleV -= ry * _aimSensitivity;	// 右スティックのY軸で垂直回転

		// 垂直角度制限
		if(_aimAngleV > _maxAimAngleV) _aimAngleV = _maxAimAngleV;		// 上限
		if(_aimAngleV < -_maxAimAngleV) _aimAngleV = -_maxAimAngleV;	// 下限
	}
}

// エイム方向計算
void AimCamera::CalculateAimDirection()
{
	float cosV = cos(_aimAngleV);	// 垂直角度の余弦
	float sinV = sin(_aimAngleV);	// 垂直角度の正弦
	float cosH = cos(_aimAngleH);	// 水平角度の余弦
	float sinH = sin(_aimAngleH);	// 水平角度の正弦

	float x = cosV * sinH;	// エイム方向ベクトル計算
	float y = sinV;			// エイム方向ベクトル計算
	float z = cosV * cosH;	// エイム方向ベクトル計算

	// 正規化
	_aimDirection = VNorm(VGet(x, y, z));	// エイム方向ベクトル設定
}

// エイムカーソル描画
void AimCamera::DrawAimCursor()
{
	// カーソルの位置
	int screenCenterX = 1920 / 2;	// 画面中央座標
	int screenCenterY = 1080 / 2;	// 画面中央座標
	int cursorSize = 20;			// カーソルサイズ

	// 十字カーソル描画
	DrawLine(screenCenterX - cursorSize, screenCenterY, screenCenterX + cursorSize, screenCenterY, GetColor(255, 0, 0)); // 横線
	DrawLine(screenCenterX, screenCenterY - cursorSize, screenCenterX, screenCenterY + cursorSize, GetColor(255, 0, 0)); // 縦線

	// 中央に点を描画
	DrawCircle(screenCenterX, screenCenterY, 2, GetColor(255, 0, 0), TRUE);
}

// ターゲット設定
void AimCamera::SetTarget(std::shared_ptr<PlayerBase> target)
{
	_targetPlayer = target;
}
