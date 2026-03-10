#include "CameraShakeSystem.h"
#include "CameraBase.h"

// 振動関連定数
namespace ShakeContants
{
	constexpr float SHAKE_SCALE = 100.0f;		// 振動のスケール
	constexpr float RAND_NORMALIZE = 100.0f;	// 乱数正規化値
	constexpr int RAND_RANGE = 200;				// 乱数の範囲
}

// 振動設定定数
namespace ShakeConfig
{
	constexpr float INCREMENT_TIME = 1.0f;	// 振動時間の増加量
}

CameraShakeSystem::CameraShakeSystem()
{
	_stcShakeConfig.magnitude = 0.0f;	// 振動の大きさ
	_stcShakeConfig.duration = 0.0f;	// 持続時間
	_stcShakeConfig.isActive = false;	// 振動が有効かどうか

	_shakeOffset = VGet(0.0f, 0.0f, 0.0f);	// 振動のオフセット
	_currentTime = 0.0f;					// 現在の振動時間
}

CameraShakeSystem::~CameraShakeSystem()
{

}

bool CameraShakeSystem::Initialize()
{
	return true;
}

bool CameraShakeSystem::Terminate()
{
	return true;
}

void CameraShakeSystem::Process()
{
	// 振動の更新処理
	UpdateShake();
}

bool CameraShakeSystem::Render()
{
	return true;
}

bool CameraShakeSystem::DebugRender()
{
	// 振動設定のデバッグ表示
	DrawShakeConfig();

	return true;
}

// 振動開始
void CameraShakeSystem::StartShake(float magnitude, float duration)
{
	_stcShakeConfig.magnitude = magnitude;	// 振動の大きさ設定
	_stcShakeConfig.duration = duration;	// 持続時間設定
	_stcShakeConfig.isActive = true;		// 振動有効化
	_currentTime = 0.0f;					// 振動時間リセット
}

// 振動停止
void CameraShakeSystem::StopShake()
{
	_stcShakeConfig.isActive = false;		// 振動無効化
	_shakeOffset = VGet(0.0f, 0.0f, 0.0f);	// 振動オフセットリセット
}

// 振動の更新処理
void CameraShakeSystem::UpdateShake()
{
	if(!_stcShakeConfig.isActive){ return; }	// 振動が有効でない場合は処理しない

	// 振動時間を増加
	_currentTime += ShakeConfig::INCREMENT_TIME;	

	// 振動時間が持続時間を超えた場合は振動を停止
	if(_currentTime >= _stcShakeConfig.duration)
	{
		StopShake();
		return;
	}

	// 振動オフセットの計算
	_shakeOffset = CalculateShakeOffset();
}

// 振動オフセットの計算
VECTOR CameraShakeSystem::CalculateShakeOffset()
{
	// 範囲を[-1, 1]に正規化したX軸の振動オフセット
	float randX = static_cast<float>(GetRand(ShakeContants::RAND_RANGE) - ShakeContants::RAND_NORMALIZE) / ShakeContants::SHAKE_SCALE;	

	// 範囲を[-1, 1]に正規化したY軸の振動オフセット
	float randY = static_cast<float>(GetRand(ShakeContants::RAND_RANGE) - ShakeContants::RAND_NORMALIZE) / ShakeContants::SHAKE_SCALE;	

	// 時間経過で振動の大きさを減衰させるための係数
	float decay = 1.0f - (_currentTime / _stcShakeConfig.duration);	// 持続時間に対する現在の時間の割合を計算

	// 減衰した振動の大きさを計算
	VECTOR decayResult=VGet
	(
		randX * _stcShakeConfig.magnitude * decay,	// X軸の振動オフセット
		randY * _stcShakeConfig.magnitude * decay,	// Y軸の振動オフセット
		0.0f										// Z軸の振動はなし
	);

	// 計算した振動オフセットを返す
	return decayResult;	
}

// 振動設定のデバッグ表示
void CameraShakeSystem::DrawShakeConfig()
{
	int x = 400;
	int y = 120;

	// 振動設定の表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "Amplitude: %.1f", _stcShakeConfig.magnitude);
	y += 20;

	// 振動時間の表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "Time: %.2f/%.2f", _currentTime, _stcShakeConfig.duration);
	y += 20;

	// 振動オフセットの表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "Offset: (%.1f, %.1f)", _shakeOffset.x, _shakeOffset.y);
	y += 20;
}

void CameraShakeSystem::Apply(CameraBase* camera)
{
	if (camera && IsShaking())
	{
		camera->ApplyShake(GetShakeOffset());
	}
}

bool CameraShakeSystem::IsFinished()
{
	// 振動が終わっていれば終了扱い
	return !IsShaking();
}