#pragma once
#include "CameraBase.h"

class PlayerBase;

// 照準用カメラクラス
class AimCamera : public CameraBase
{
public:
	AimCamera();
	virtual ~AimCamera();

	void Process() override;
	void Render() override;
	void DebugRender() override;

	void OnEnter() override;
	void OnExit() override;

	void SetTarget(std::shared_ptr<PlayerBase> target) override;
	void ApplyShake(const VECTOR& shakeOffset) override {};

	bool IsAiming() const;// エイムモード中かチェック

	VECTOR GetCameraDir() override { return _aimDirection; }

	float GetAngleH() override { return _fAngleH; }

	// エイムカメラ更新
	void UpdateAimCamera();// エイムカメラ更新
	void ControlAimCamera();// エイムカメラ操作
	void DrawAimCursor();// エイムカーソル描画

protected:
	bool _isAiming = false;// エイムモードフラグ

	VECTOR _aimDirection;// エイム方向ベクトル

	std::shared_ptr<PlayerBase> _targetPlayer;// ターゲットプレイヤー

	VECTOR _aimCameraOffset;// ローカルオフセット
};

