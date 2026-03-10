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

	// エイム処理
	bool IsAiming() const;// エイムモード中かチェック
	VECTOR GetAimDirection() const;// エイム方向取得

	float GetAngleH() override { return _fAngleH; }

	// エイムカメラ更新
	void UpdateAimCamera();// エイムカメラ更新
	void ControlAimCamera();// エイムカメラ操作
	void DrawAimCursor();// エイムカーソル描画

protected:
	bool _isAiming;// エイムモードフラグ

	// エイム方向
	VECTOR _aimDirection;// エイム方向ベクトル
	VECTOR _aimStartPlayerDir;// エイム開始時のプレイヤー方向ベクトル

	// カメラ基準方向
	VECTOR _cameraBaseDirection;
	VECTOR _cameraBaseRight;

	// ターゲットプレイヤー
	std::shared_ptr<PlayerBase> _targetPlayer;

	// プレイヤー右後ろのオフセット
	VECTOR _aimCameraOffset; 
};

