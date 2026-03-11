#pragma once
#include "appframe.h"

// アニメーション管理クラス
class AnimManager
{
public:
	AnimManager();
	virtual ~AnimManager();

	bool LoadModel(const std::string& filePath);

	// アニメーションの更新
	void Update();

	// モデルの描画
	void Render();

	// モデルハンドルの取得
	int GetModelHandle() { return _modelHandle; }
	// モデルハンドルの設定
	void SetModelHandle(int handle) { _modelHandle = handle; }

	// アニメーションが終了したか
	bool IsAnimationFinished() const;

	// 現在のアニメーション番号
	int GetCurrentAnimIndex() { return _currentAnimIndex; }

	// アニメーション名からインデックスを取得してアニメーションを切り替え
	bool ChangeAnimationByName(const char* animName, float fBlendFrame, int loopCnt, float fPlaySpeed = 1.0f);

	// リソースの開放
	void Release();

	// アニメーションの総再生時間を取得
	float GetCurrentAnimTotalTime();

private:
	// アニメーション情報
	struct AnimationData
	{
		int attachIndex = -1;// アタッチインデックス
		float fTotalTime = 0.0f;// 総再生時間
		float fPlayTime = 0.0f;// 現在の再生時間
		float fCloseTime = 0.0f;// ブレンド終了までの時間
		float fCloseTotalTime = 0.0f;// ブレンド終了の総時間
		float fFadeInTime = 0.0f;// フェードイン時間
		float fFadeInTotalTime = 0.0f;// フェードインの総時間
		int loopCnt = 0;// ループ回数(0:無限ループ　1:ループ無し　2以上:指定回数ループ)
		float fPlaySpeed  = 1.0f;// 再生速度
		bool bIsFinished = false;// アニメーションが終わったか
	};

	int _modelHandle = -1;

	std::vector<std::unique_ptr<AnimationData>> _activeAnims;
	int _currentAnimIndex = -1;// 現在のアニメーションインデックス

	// アニメーションのデタッチ
	void DetachAnimation(AnimationData* anim);
};

