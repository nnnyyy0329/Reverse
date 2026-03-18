#include "AimCamera.h"
#include "PlayerBase.h"

namespace
{
	constexpr float AIM_SENSITIVITY = 0.035f;// エイム感度
	constexpr float AIM_DISTANCE = 2000.0f;// エイムカメラ距離
	constexpr float AIM_ANGLE_LIMIT = 1.4f;// エイム垂直角度制限
}

AimCamera::AimCamera()
{
	_fNearClip = 1.0f;
	_fFarClip = 3000.0f;
	_fDistance = AIM_DISTANCE;
	_fAngleH = 0.0f;
	_fAngleV = 0.0f;

	// エイムモード設定
	_isAiming = false;
	// エイム方向
	_aimDirection = VGet(0.0f, 0.0f, -1.0f);// 初期エイム方向は前方
	_aimCameraOffset = VGet(60.0f, 160.0f, -100.0f);// プレイヤー右後ろのオフセット
}

AimCamera::~AimCamera()
{
}

void AimCamera::Process()
{
	if(!_isAiming){ return; }

	// エイムカメラ操作
	ControlAimCamera();

	// エイムカメラ更新
	UpdateAimCamera();
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
	(x, y + 20, GetColor(255, 255, 255), "AngleH: %3.2f, AngleV: %3.2f", _fAngleH, _fAngleV);

	// エイム方向表示
	DrawFormatString
	(x, y + 40, GetColor(255, 255, 255), "AimDir: (%3.2f, %3.2f, %3.2f)", _aimDirection.x, _aimDirection.y, _aimDirection.z);
}

void AimCamera::OnEnter()
{
	_isAiming = true;

	// プレイヤーの向きからエイム角度を初期化
	if (_targetPlayer)
	{
		VECTOR playerDir = _targetPlayer->GetDir();// プレイヤーの向き

		// プレイヤーの向きから少し右後ろの角度を計算
		float playerAngle = atan2f(playerDir.x, playerDir.z);// プレイヤーの水平角度
		_fAngleH = NormalizeAngleRad(playerAngle + 0.3f);// 少し右向きに調整
		_fAngleV = -0.1f;// 少し下向き
	}
}

void AimCamera::OnExit()
{
	_isAiming = false;
}

// エイムモード中かチェック
bool AimCamera::IsAiming() const
{
	return _isAiming;
}

// ターゲット設定
void AimCamera::SetTarget(std::shared_ptr<PlayerBase> target)
{
	_targetPlayer = target;
}

// エイムカメラ更新
void AimCamera::UpdateAimCamera()
{
	if(!_targetPlayer){ return; }

	// 角度から基準軸を計算
	VECTOR forward = CalcForward(_fAngleH, _fAngleV);// 前方ベクトル
	VECTOR right = CalcRight(_fAngleH);// 右方向ベクトル
	VECTOR up = VCross(forward, right);// 上方向ベクトル

	// プレイヤー基準位置でオフセットを適用
	VECTOR basePos = _targetPlayer->GetPos();
	VECTOR offset = VGet(0.0f, 0.0f, 0.0f);
	offset = VAdd(offset, VScale(right, _aimCameraOffset.x));
	offset = VAdd(offset, VScale(up, _aimCameraOffset.y));
	offset = VAdd(offset, VScale(forward, _aimCameraOffset.z));

	_vPos = VAdd(basePos, offset);

	// カメラ前方の一定距離先を注視点に設定
	_vTarget = VAdd(_vPos, VScale(forward, _fDistance));

	_aimDirection = forward;// エイム方向を更新
}

// エイムカメラ操作
void AimCamera::ControlAimCamera()
{
	auto& im = InputManager::GetInstance();
	const AnalogState& analog = im.GetAnalog();
	float analogMin = im.GetAnalogMin();
	float rx = analog.rx;
	float ry = analog.ry;

	// 水平回転
	if(abs(rx) > analogMin)
	{
		_fAngleH += rx * AIM_SENSITIVITY;// 右スティックのX軸で水平回転
		_fAngleH = NormalizeAngleRad(_fAngleH);
	}

	// 垂直回転
	if(abs(ry) > analogMin)
	{
		_fAngleV -= ry * AIM_SENSITIVITY;// 右スティックのY軸で垂直回転
		_fAngleV = ClampVerticalAngle(_fAngleV, AIM_ANGLE_LIMIT);
	}
}

// エイムカーソル描画
void AimCamera::DrawAimCursor()
{
	// カーソルの位置
	int screenCenterX = 1920 / 2;// 画面中央座標
	int screenCenterY = 1080 / 2;// 画面中央座標
	int cursorSize = 20;// カーソルサイズ

	// 十字カーソル描画
	DrawLine(screenCenterX - cursorSize, screenCenterY, screenCenterX + cursorSize, screenCenterY, GetColor(255, 0, 0));// 横線
	DrawLine(screenCenterX, screenCenterY - cursorSize, screenCenterX, screenCenterY + cursorSize, GetColor(255, 0, 0));// 縦線

	// 中央に点を描画
	DrawCircle(screenCenterX, screenCenterY, 2, GetColor(255, 0, 0), TRUE);
}
