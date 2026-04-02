/*****************************************
* file   PlayerLifeBarUI.h
* brief  プレイヤーライフバーUIクラス
* author 成田 悠真
* date   2026/01/30
******************************************/

#pragma once
#include "appframe.h"

// プレイヤーライフバーUI表示の設定用名前空間
namespace PlayerLifeBarConfig
{
	constexpr int DRAW_BAR_FRAME_X = 677.5f;
	constexpr int DRAW_BAR_FRAME_Y = 1030;
	constexpr int DRAW_OFFSET_X = 1;
	constexpr int DRAW_OFFSET_Y = 1;


	/* ひんさんが追加していたコード */

	//// 追加：縦方向スケール
	//constexpr float LIFE_BAR_SCALE_Y = 0.2f;
}

// 点滅エフェクトの設定用名前空間
namespace FlashEffectConfig
{
	constexpr float FLASH_SPEED = 0.5f;			// 点滅速度(フレーム)
	constexpr float FLASH_DURATION = 60.0f;		// 点滅継続時間(フレーム)
	constexpr float FLASH_MAX_ALPHA = 200.0f;	// 点滅の最大アルファ値
}

// 前方宣言
class PlayerManager;

/// @brief プレイヤーライフバーUIクラス
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

	/// @brief ライフバーフレームの描画
	void LifeBarRenderFrame();

	/// @brief ライフバーの表示比率計算
	void BarRatioCalculation();

	/// @brief ライフバーの描画
	///
	/// @param ratio ライフバーの表示比率
	void LifeBarRender(float ratio);
	
	/// @brief 点滅エフェクトの開始
	///
	/// @param duration 点滅継続時間(フレーム)。FlashEffectConfig::FLASH_DURATIONを初期で使用
	void StartFlashEffect(float duration = FlashEffectConfig::FLASH_DURATION);

	/// @brief 点滅エフェクトの更新
	void UpdateFlashEffect();

	/// @brief 点滅エフェクトの描画
	///
	/// @param clipW 点滅エフェクトのクリップ幅
	/// @param graphH 点滅エフェクトのグラフ高さ
	void FlashEffectRender(int clipW, int graphH);


	/* クラスのセット関係関数 */

	/// @brief プレイヤーマネージャーの設定関数
	///
	/// @param playerManager 設定するプレイヤーマネージャーのshared_ptr
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

