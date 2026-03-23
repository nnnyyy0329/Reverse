// 担当 : 成田

#pragma once
#include "appframe.h"
#include "PlayerBase.h"
#include "AnimManager.h"
#include "AbilityBase.h"

// 変身設定用の名前空間
namespace TransformConstants
{
	constexpr float TRANSFORM_TIME_INCREMENT = 1.0f;	// 変身時間の増加量
}

// 変身関連アニメーション設定用の名前空間
namespace TransAnimConstants
{
	constexpr float ANIMATION_BLEND_TIME = 1.0f;	// アニメーションブレンド時間
	constexpr int ANIMATION_LOOP = 0;				// ループあり
	constexpr int ANIMATION_NONE_LOOP = 1;			// ループなし
}

// 能力選択関連の設定用名前空間
namespace AbilityConfig
{
	constexpr int DEFAULT_ABILITY_INDEX = 0;	// デフォルトのアビリティインデックス
}

// 前方宣言
class CameraManager;
class AbilitySelectScreen;

/// @brief プレイヤータイプ列挙型
enum class PLAYER_TYPE
{
	NONE,
	SURFACE,	///< 表プレイヤー
	INTERIOR,	///< 裏プレイヤー
	BULLET,		///< 弾丸プレイヤー
	_EOT_,
};

/// @brief プレイヤーマネージャークラス
class PlayerManager
{
public:
	PlayerManager();
	virtual ~PlayerManager();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	/// @brief デバッグ処理
	bool DebugProcess();

	/// @brief デバッグ描画
	bool DebugRender();


	/* プレイヤー管理 */

	/// @brief プレイヤーを登録する関数
	/// 
	/// @param type	プレイヤーのタイプ
	/// @param player	登録するプレイヤーのインスタンス
	/// 
	/// @return 登録成功ならtrue、同じタイプのプレイヤーが既に登録されている場合はfalseを返す
	bool RegisterPlayer(PLAYER_TYPE type, std::shared_ptr<PlayerBase> player);	

	/// @brief アビリティに応じたプレイヤー切り替え
	/// 
	/// @param ability	切り替えのトリガーとなるアビリティのタイプ
	void SwitchPlayerByAbility(ABILITY_TYPE ability);							


	/* プレイヤー切り替え管理 */	//--- 今後クラス分け予定--------------------------------------

	/// @brief エネルギーに応じたプレイヤー切り替え
	void SwitchPlayerByEnergy();												

	/// @brief プレイヤー切り替え処理
	///
	/// @param type 切り替えるプレイヤーのタイプ
	void SwitchPlayer(PLAYER_TYPE type);										

	/// @brief プレイヤー切り替え時の状態引き継ぎの設定
	/// 
	/// @param enable	状態引き継ぎを有効にするならtrue、無効にするならfalse
	void EnableStateTransfer(bool enable) { _bEnableStateTransfer = enable; }	

	/// @brief プレイヤー切り替え時の情報引き継ぎ処理
	///
	/// @param oldPlayer 切り替え前のプレイヤー
	/// @param newPlayer 切り替え後のプレイヤー
	void TransferPlayerConfig(PlayerBase* oldPlayer, PlayerBase* newPlayer);


	/* 変身管理 */		//--- 今後クラス分け予定 --------------------------------------

	/// @brief 変身開始処理
	///
	/// @param targetType 変身先のプレイヤータイプ
	void StartTransform(PLAYER_TYPE targetType);				

	/// @brief 即時変身処理
	///
	/// @param targetType 変身先のプレイヤータイプ
	void SwitchPlayerImmediate(PLAYER_TYPE targetType);			

	/// @brief 変身処理
	void UpdateTransform();										

	/// @brief 時間経過による変身終了処理
	void TransformFinishByTime();								

	/// @brief 変身終了処理
	void EndTransform();										

	/// @brief 変身中かどうかを取得
	/// 
	/// @return 変身中ならtrue、そうでないならfalse
	bool IsTransforming() const { return _bIsTransforming; }	


	/* 変身解除管理 */		// --- 今後クラス分け予定--------------------------------------

	/// @brief 変身解除開始処理
	void StartTransformCancel();											

	/// @brief 変身解除処理
	void UpdateTransformCancel();

	/// @brief 変身解除中の時間経過によるプレイヤー切り替え処理
	void SwitchPlayerByTime();	

	/// @brief 変身解除終了処理
	void EndTransformCancel();

	/// @brief 変身解除中かどうかを取得
	///
	/// @return 変身解除中ならtrue、そうでないならfalse
	bool IsTransformCanceling() const { return _bIsTransformCanceling; }	


	/*****ゲッターセッター*****/

	/// @brief アクティブプレイヤー取得(生ポインタ版)
	/// 
	/// @return アクティブプレイヤーのポインタ。存在しない場合はnullptrを返す
	PlayerBase* GetActivePlayer() const;									

	/// @brief アクティブプレイヤー取得(shared_ptr版)
	///
	/// @return アクティブプレイヤーのshared_ptr。存在しない場合はnullptrを返す
	std::shared_ptr<PlayerBase> GetActivePlayerShared() const;				

	/// @brief 指定したプレイヤータイプのプレイヤーを取得
	///
	/// @param type 取得したいプレイヤーのタイプ
	/// 
	/// @return 指定したタイプのプレイヤーのshared_ptr。存在しない場合はnullptrを返す
	std::shared_ptr<PlayerBase> GetPlayerByType(PLAYER_TYPE type) const;	

	/// @brief アクティブプレイヤーのタイプを取得
	///
	/// @return アクティブプレイヤーのタイプ
	PLAYER_TYPE GetActivePlayerType() const { return _eActivePlayerType; }	


	/* クラスセット */

	/// @brief カメラマネージャーを設定
	///
	/// @param cameraManager 設定するCameraManager
	void SetCameraManager(std::shared_ptr<CameraManager> cameraManager) { _cameraManager = cameraManager; }

	/// @brief 能力選択画面を設定
	///
	/// @param abilityScreen 設定するAbilitySelectScreen
	void SetAbilitySelectScreen(std::shared_ptr<AbilitySelectScreen> abilityScreen){_abilitySelectScreen = abilityScreen;}

private:

	/// @brief 変身タイマーを更新
	void UpdateTransformTime();									

	/// @brief 変身解除タイマーを更新
	void UpdateTransformCancelTime();

	/// @brief 変身関連のアニメーションを再生
	///
	/// @param animName 再生するアニメーションの名前
	void PlayTransConnectionAnim(const char* animName);			

	/// @brief 能力選択画面がアクティブかどうかをチェックする
	///
	/// @param isAbilityScreenActive 能力選択画面がアクティブならtrue、そうでないならfalseを返す参照
	void CheckAbilityScreenActive(bool& isAbilityScreenActive);	

	/// @brief 能力選択に応じた自動切り替え処理
	///
	/// @param isAbilityScreenActive 能力選択画面がアクティブかどうか
	void ProcessAutoSwitch(bool isAbilityScreenActive);			
	
	/// @brief アクティブプレイヤーの処理
	///
	/// @param isAbilityScreenActive 能力選択画面がアクティブかどうか
	void ProcessActivePlayer(bool isAbilityScreenActive);

	/// @brief 待機アニメーションに戻す処理
	void ReturnWaitAnim();

	/// @brief 変身後の体力回復処理
	///
	/// @param 変身先のプレイヤータイプ
	void RecoveryLifeByTransform(PLAYER_TYPE transformPlayerType);

protected:

	std::unordered_map<PLAYER_TYPE, std::shared_ptr<PlayerBase>> _players;	// プレイヤーのマップ
	
	std::shared_ptr<CameraManager> _cameraManager;				// カメラマネージャー
	std::shared_ptr<AbilitySelectScreen> _abilitySelectScreen;	// 能力選択画面

	// アクティブプレイヤー
	PlayerBase* _activePlayer;
	PLAYER_TYPE _eActivePlayerType;

	// 変身関係
	PLAYER_TYPE _eTransformTarget;	// 変身先のプレイヤータイプ
	float _fTransformTime;			// 変身タイマー
	float _fTransformMaxTime;		// 変身にかかる時間
	bool _bIsTransforming;			// 変身中か

	// 変身解除関係
	float _fTransformCancelTime;	// 変身解除タイマー
	float _fTransformCancelMaxTime;	// 変身解除にかかる時間
	bool _bIsTransformCanceling;	// 変身解除中か

	// プレイヤー切り替え時の状態引き継ぎフラグ
	bool _bEnableStateTransfer;		

};