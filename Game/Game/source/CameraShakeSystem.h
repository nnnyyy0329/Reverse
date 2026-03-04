#pragma once
#include "appframe.h"

// カメラ振動設定
struct CameraShakeConfig
{
	float duration;		// 持続時間
	float magnitude;	// 振動の強さ
	bool isActive;		// 振動が有効かどうか
};

// カメラ振動システム
class CameraShakeSystem
{
public:
	CameraShakeSystem();
	~CameraShakeSystem();

	bool Initialize();	// 初期化
	bool Terminate();	// 終了
	bool Process();		// 更新
	bool Render();		// 描画

	bool DebugRender();	// デバッグ描画

	// 振動処理関数
	void StartShake(float magnitude, float duration);	// 振動開始
	void StopShake();									// 振動停止
	void UpdateShake();									// 振動更新
	bool IsShaking() const { return _stcShakeConfig.isActive; }	// 振動中かどうか

	// デバッグ表示
	void DrawShakeConfig();	// 振動設定のデバッグ表示

	// カメラシェイクオフセットの取得
	VECTOR GetShakeOffset() const { return _shakeOffset; }

protected:
	// カメラ振動設定
	CameraShakeConfig _stcShakeConfig;

	// 振動のオフセット
	VECTOR _shakeOffset;
	float _currentTime;

private:
	// 振動計算処理
	VECTOR CalculateShakeOffset();

};

