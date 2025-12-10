#pragma once
#include "PlayerBase.h"

// カメラ
class Camera {
public:
	VECTOR	_vPos;					// 位置
	VECTOR	_vTarget;				// 距離
	float	_clipNear, _clipFar;	// クリップ
};

class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

protected:

	// カメラ
	Camera	_cam;
	// 3Dモデル描画用
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	// マップ用
	int _handleMap;
	int _handleSkySphere;
	int _frameMapCollision;
	// デバッグ用
	bool	_bViewCollision;

};

