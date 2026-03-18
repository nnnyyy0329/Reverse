#pragma once
#include "appframe.h"
#include "ICameraAddon.h"

class CameraBase;

// カメラ振動設定
struct CameraShakeConfig
{
	float duration;// 持続時間
	float magnitude;// 振動の強さ
	bool isActive;// 振動が有効かどうか
};

// カメラ振動システム
class CameraShakeSystem : public ICameraAddon
{
public:
	CameraShakeSystem();
	virtual ~CameraShakeSystem();

	bool Initialize();// 初期化
	bool Terminate();// 終了
	void Process() override;// 更新
	bool Render();// 描画
	bool DebugRender();// デバッグ描画

	void Apply(CameraBase* camera) override;
	bool IsFinished() override;

	// 振動処理関数
	void StartShake(float magnitude, float duration);// 振動開始
	void StopShake();// 振動停止
	void UpdateShake();// 振動更新
	bool IsShaking() { return _stcShakeConfig.isActive; }// 振動中かどうか

	// デバッグ表示
	void DrawShakeConfig();	// 振動設定のデバッグ表示

	// カメラシェイクオフセットの取得
	VECTOR GetShakeOffset() { return _shakeOffset; }

	void SetUseHorizontalOnly(bool only) { _bUseHorizontalOnly = only; }

protected:
	// カメラ振動設定
	CameraShakeConfig _stcShakeConfig;

	// 振動のオフセット
	VECTOR _shakeOffset;
	float _currentTime;

private:
	// 振動計算処理
	VECTOR CalculateShakeOffset();

	bool _bUseHorizontalOnly;
};

