#pragma once
#include "appframe.h"

namespace FlashEffectConfig
{
	constexpr float FLASH_SPEED = 0.5f;			// 点滅速度(フレーム)
	constexpr float FLASH_DURATION = 60.0f;		// 点滅継続時間(フレーム)
	constexpr float FLASH_MAX_ALPHA = 200.0f;	// 点滅の最大アルファ値
}

class PlayerManager;

// プレイヤーライフバーUIクラス
class PlayerLifeBarUI
{
public:

	PlayerLifeBarUI();
	~PlayerLifeBarUI();


	/* 基本関数 */

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();


	/*ライフバー描画関係関数 */

	// ライフバーフレーム描画
	void LifeBarRenderFrame();

	// ライフバー比率計算
	void BarRatioCalculation();

	// ライフバー描画
	void LifeBarRender(float ratio);
	
	/// @brief 点滅エフェクトの開始
	///
	/// @param duration 点滅継続時間(フレーム)。FlashEffectConfig::FLASH_DURATIONを初期で使用
	void StartFlashEffect(float duration = FlashEffectConfig::FLASH_DURATION);

	// 点滅中の更新
	void UpdateFlashEffect();

	// 点滅エフェクトの描画
	void FlashEffectRender(int clipW, int graphH);

	/* クラスのセット関係関数 */

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


private:

	float _flashTimer;		// 点滅タイマー
	float _flashDuration;	// 点滅継続時間
	bool _isFlashing;		// 点滅中フラグ

};

