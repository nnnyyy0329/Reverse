#include "GameCamera.h"
#include "PlayerBase.h"

namespace
{
	constexpr float ROTATE_SPEED = 0.02f;// 回転速度
	
	// 角度制限
	constexpr float ANGLE_V_MIN     = -0.349f;// 仰角上限（上方向：約-20度）
	constexpr float ANGLE_V_MAX     =  0.873f;// 仰角下限（下方向：約50度）
	constexpr float ANGLE_V_DEFAULT =  0.262f;// デフォルト俯瞰角度（約15度）
	
	// オートリセンター用定数
	constexpr float AUTO_CENTER_TIMER_MAX = 60.0f;// 右スティック未入力の待機フレーム数（1秒）
}

GameCamera::~GameCamera()
{
}

GameCamera::GameCamera()
{
	// カメラの初期設定
	_vPos          = VGet(0.0f, 0.0f, 0.0f);
	_vTarget       = VGet(0.0f, 0.0f, 0.0f);
	_nearClip      = 1.f;
	_farClip       = 5000.f;
	_distance      = 450.0f;
	_angleH        = 0.0f;
	_angleV        = ANGLE_V_DEFAULT;// 俯瞰気味（約15度）に設定
	_angleVMin     = ANGLE_V_MIN;
	_angleVMax     = ANGLE_V_MAX;
	_targetOffset  = VGet(0.0f, 150.0f, 0.0f);
	
	// フェードアウト初期化
	_bIsCharNearCamera = false;
	
	// オートリセンター初期化
	_fNoRightStickTime  = 0.0f;
	_fAutoCenterSpeed   = 0.03f;// 補間速度
	_fAutoCenterDeadZone = 0.1f;// デッドゾーン（ラジアン、急旋回時の揺れ防止）
}

void GameCamera::Process(InputManager* input, bool isInput)
{
	const AnalogState& analog = input->GetAnalog();
	float analogMin = input->GetAnalogMin();
	
	float lx = analog.lx;// 左スティックX
	float ly = analog.ly;// 左スティックY
	float rx = analog.rx;// 右スティックX
	float ry = analog.ry;// 右スティックY
	
	// カメラ更新処理
	UpdateCamera();
	
	// カメラ位置の更新（衝突判定含む）
	UpdateCameraPos();
	
	// カメラ操作処理
	ControlCamera(lx, ly, rx, ry, analogMin);
}

void GameCamera::SetUp()
{
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
	
	// カメラからどれだけ離れた距離からどこまで描画するかを設定
	SetCameraNearFar(_nearClip, _farClip);
}

void GameCamera::Render()
{
}

// デバッグ用描画処理
void GameCamera::DebugRender()
{
	int x = 400;
	int y = 50;
	// 座標系表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "GameCamera Pos: (%3.2f, %3.2f, %3.2f)", _vPos.x, _vPos.y, _vPos.z);
}

// 更新処理
void GameCamera::UpdateCamera()
{
	// カメラ位置と注視点を更新
	if (_targetObject)
	{
		// ターゲットの位置を取得
		VECTOR targetPos = _targetObject->GetPos();

		// 位置更新
		VECTOR newPos = VAdd(targetPos, _posOffset);
		_vPos = newPos;

		// 注視点更新
		VECTOR newTarget = VAdd(targetPos, _targetOffset);
		_vTarget = newTarget;
	}
}

// カメラ位置の更新
void GameCamera::UpdateCameraPos()
{
	/*
	* 球面座標系から直交座標系へ変換
	* Y軸(高さ) = 距離 * sin(垂直角度)
	* 水平半径  = 距離 * cos(垂直角度)
	* X軸 = 水平半径 * cos(水平角度)
	* Z軸 = 水平半径 * sin(水平角度)
	*/

	// 高さ(y)と水平面でのターゲットまでの距離(r)を計算
	float y = sinf(_angleV) * _distance;
	float r = cosf(_angleV) * _distance;

	// 水平平面上での位置(x,z)を計算
	float x = cosf(_angleH) * r;
	float z = sinf(_angleH) * r;

	// 相対位置をターゲットの座標に足して理想カメラ位置を計算
	VECTOR idealPos = VAdd(_vTarget, VGet(x, y, z));
	_vPos = idealPos;
}
	
// カメラ操作処理
void GameCamera::ControlCamera(float lx, float ly, float rx, float ry, float analogMin)
{
	// 手動カメラ回転
	bool isRightStickInput = (fabsf(rx) > analogMin || fabsf(ry) > analogMin);

	if (isRightStickInput)
	{
		// 右スティック入力があれば水平・垂直角度を更新
		_angleH -= rx * ROTATE_SPEED;
		_angleV += ry * ROTATE_SPEED;

		// 垂直角度制限
		if (_angleV < _angleVMin) _angleV = _angleVMin;
		if (_angleV > _angleVMax) _angleV = _angleVMax;

		// 右スティック入力でオートリセンタータイマーをリセット
		_fNoRightStickTime = 0.0f;
	}
	else
	{
		// 右スティック未入力ならタイマーを加算
		_fNoRightStickTime += 1.0f;
	}

	// オートリセンター
	bool isLeftStickInput = (fabsf(lx) > analogMin || fabsf(ly) > analogMin);

	// 左スティック入力あり かつ 右スティック未入力が一定時間続いた場合
	if (isLeftStickInput && _fNoRightStickTime >= AUTO_CENTER_TIMER_MAX && _targetObject)
	{
		// プレイヤーの進行方向を取得
		VECTOR playerDir = _targetObject->GetDir();
		float  playerDirLen = sqrtf(playerDir.x * playerDir.x + playerDir.z * playerDir.z);

		if (playerDirLen > 0.001f)
		{
			// 進行方向の真後ろ角度を目標角度として Slerp（球面線形補間）
			float targetAngleH = atan2f(-playerDir.z, -playerDir.x);

			// 最短回転のため角度差を -π～π に正規化
			float diff = targetAngleH - _angleH;
			while (diff > DX_PI_F) diff -= 2.0f * DX_PI_F;
			while (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

			// 角度差が小さい場合は補間しない（急旋回時の揺れ防止）
			if (fabsf(diff) > _fAutoCenterDeadZone)
			{
				_angleH += diff * _fAutoCenterSpeed;
			}
		}
	}
}

void GameCamera::SetTarget(std::shared_ptr<PlayerBase> target)
{
	_targetObject = target;
	
	// カメラ位置と注視点の初期設定
	if(_targetObject)
	{
		const VECTOR targetPos = _targetObject->GetPos();
		_vPos    = VAdd(targetPos, _posOffset);
		_vTarget = VAdd(targetPos, _targetOffset);
	}
}