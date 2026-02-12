#pragma once
#include "appframe.h"

class PlayerBase;

// 標準用カメラクラス
class AimCamera
{
public:
	AimCamera();
	~AimCamera();

	void Initialize();																						// 初期化
	void Process(int key, int trg, float lx, float ly, float rx, float ry, float analogMin, bool isInput);	// 更新
	void SetUp();																							// カメラセットアップ
	void Render();																							// 描画
	void DebugRender();																						// デバッグ描画

	// エイム処理
	void StartAiming();				// エイムモード開始
	void EndAiming();				// エイムモード終了
	bool IsAiming() const;			// エイムモード中かチェック
	VECTOR GetAimDirection() const;	// エイム方向取得

	// エイムカメラ更新
	void UpdateAimCamera();										// エイムカメラ更新
	void ControlAimCamera(float rx, float ry, float analogMin);	// エイムカメラ操作
	void CalculateAimDirection();								// エイム方向計算
	void DrawAimCursor();										// エイムカーソル描画

	// ゲッターセッター
	float GetAimAngleH() const { return _aimAngleH; }	// エイム水平角度取得

	// ターゲット設定
	void SetTarget(std::shared_ptr<PlayerBase> target);

protected:
	// カメラ基本設定
	VECTOR _vPos;		// カメラ位置
	VECTOR _vTarget;	// カメラ注視点
	float _nearClip;	// ニアクリップ距離
	float _farClip;		// ファークリップ距離
	
	// エイムモード設定
	bool _isAiming;			// エイムモードフラグ
	float _aimSensitivity;	// エイム感度
	float _aimDistance;		// エイム距離
	float _aimAngleH;		// エイム水平角度
	float _aimAngleV;		// エイム垂直角度
	float _maxAimAngleV;	// エイム垂直角度最大値

	// エイム方向
	VECTOR _aimDirection;		// エイム方向ベクトル
	VECTOR _aimStartPlayerDir;	// エイム開始時のプレイヤー方向ベクトル

	// カメラ基準方向
	VECTOR _cameraBaseDirection;
	VECTOR _cameraBaseRight;

	// ターゲットプレイヤー
	std::shared_ptr<PlayerBase> _targetPlayer;

	// プレイヤー右後ろのオフセット
	VECTOR _aimCameraOffset; 
};

