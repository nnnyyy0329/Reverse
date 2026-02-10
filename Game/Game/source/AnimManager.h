#pragma once
#include "appframe.h"

// アニメーション管理クラス
class AnimManager
{
public:
	AnimManager();
	virtual ~AnimManager();

	bool LoadModel(const std::string& filePath);

	// アニメーションの追加
	int AddAnimation(const std::string& filePath, int loopCnt);

	// アニメーションの切り替え
	bool ChangeAnimation(int animIndex, float fBlendFrame);

	// アニメーションの更新
	void Update();

	// モデルの描画
	void Render();

	// モデルハンドルの取得
	int GetModelHandle() { return _modelHandle; }
	// モデルハンドルの設定
	void SetModelHandle(int handle) { _modelHandle = handle; }

	// アニメーションが終了したか
	bool IsAnimationFinished();

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
		int attachIndex;// アタッチインデックス
		float fTotalTime;// 総再生時間
		float fPlayTime;// 現在の再生時間
		float fCloseTime;// ブレンド終了までの時間
		float fCloseTotalTime;// ブレンド終了の総時間
		int loopCnt;// ループ回数(0:無限ループ　1:ループ無し　2以上:指定回数ループ)
		float fPlaySpeed;// 再生速度
		bool bIsFinished;// アニメーションが終わったか
	};

	// アニメーションリソース情報
	struct AnimResource
	{
		int handle;// アニメーションハンドル
		int loopCnt;// ループ回数
	};

	int _modelHandle;// モデルハンドル
	std::vector<AnimResource> _animResources;// アニメーションリソース
	std::vector<AnimationData*>  _activeAnims;// アクティブなアニメーション
	int _currentAnimIndex;// 現在のアニメーションインデックス

	// アニメーションのデタッチ
	void DetachAnimation(AnimationData* anim);
};

