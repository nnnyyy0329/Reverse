#pragma once
#include "CameraBase.h"


// ゲームカメラクラス
class GameCamera : public CameraBase
{
public:
	// 注視点調整設定
	struct LookAtSettings
	{
		VECTOR worldOffset;// ワールド基準オフセット
		VECTOR localOffset;// ローカル基準オフセット
		float lerpRate;// 注視点の補間率(1=即時)
	};

	GameCamera();
	virtual ~GameCamera() {};

	void Process() override;
	void DebugRender() override;	

	void SetTarget(std::shared_ptr<PlayerBase> target) override;

	void ApplyShake(const VECTOR& shakeOffset) override;

	void Reset() override;

	void SetLookAtSettings(const LookAtSettings& settings) { _lookAtSettings = settings; }
	const LookAtSettings& GetLookAtSettings() { return _lookAtSettings; }

protected:
	void UpdateCamera();// カメラ更新
	void ControlCamera();// 入力によるカメラ操作

	// 注視点をオフセットを適用して求める
	VECTOR CalculateLookAtPoint();

	// ターゲットとなるゲームオブジェクト
	std::shared_ptr<PlayerBase> _targetObject;

	float _fAutoFollowTimer = 0.0f;

	LookAtSettings _lookAtSettings;
};
