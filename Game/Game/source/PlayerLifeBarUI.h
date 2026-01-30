#pragma once
#include "appframe.h"

class PlayerManager;

// プレイヤーライフバーUIクラス
class PlayerLifeBarUI
{
public:
	PlayerLifeBarUI();
	~PlayerLifeBarUI();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	void LifeBarRenderFrame();			// ライフバーフレーム描画
	void BarRatioCalculation();			// ライフバー比率計算
	void LifeBarRender(float ratio);	// ライフバー描画

	// プレイヤー管理クラスの設定
	void SetPlayerManager(std::shared_ptr<PlayerManager> playerManager){ _playerManager = playerManager; }

protected:
	// プレイヤーマネージャー
	std::shared_ptr<PlayerManager> _playerManager; 

	int _iLifeBar;		// ライフバー画像ハンドル
	int _iLifeBarFrame;	// ライフバーフレーム画像ハンドル

	int _drawLifeBarX;		// ライフバーの描画位置X
	int _drawLifeBarY;		// ライフバーの描画位置Y
	int _drawLifeBarFrameX;	// ライフバーフレームの描画位置X
	int _drawLifeBarFrameY;	// ライフバーフレームの描画位置Y

};

