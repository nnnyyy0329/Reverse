#include "AnimManager.h"

AnimManager::AnimManager()
	: _modelHandle(-1)
	, _currentAnimIndex(-1)
{
}

AnimManager::~AnimManager()
{
	Release();
}

bool AnimManager::LoadModel(const std::string& filePath)
{
	// モデルの読み込み
	_modelHandle = MV1LoadModel(filePath.c_str());

	if (_modelHandle == -1)
	{
		return false;
	}

	return true;
}

int AnimManager::AddAnimation(const std::string& filePath, int loopCnt)
{
	// アニメーションの読み込み
	int animHandle = MV1LoadModel(filePath.c_str());

	if (animHandle == -1)
	{
		return -1;
	}

	// リソースリストに追加
	AnimResource res;
	res.handle = animHandle;
	res.loopCnt = loopCnt;
	_animResources.push_back(res);

	// インデックスを返す
	return static_cast<int>(_animResources.size() - 1);
}

bool AnimManager::ChangeAnimation(int animIndex, float fBlendFrame)
{
	// モデルがロードされているかチェック
	if (_modelHandle == -1)
	{
		return false;
	}

	// アニメーションインデックスの範囲チェック
	if (animIndex < 0 || animIndex >= static_cast<int>(_animResources.size()))
	{
		return false;
	}

	// 既存のアニメーションをブレンド終了状態に
	for (auto* anim : _activeAnims)
	{
		if (anim->fCloseTime <= 0.0f)
		{
			anim->fCloseTime = fBlendFrame;
			anim->fCloseTotalTime = fBlendFrame;
		}
	}

	// 新しいアニメーションを追加
	AnimationData* newAnim = new AnimationData();
	newAnim->attachIndex = MV1AttachAnim(_modelHandle, _animResources[animIndex].handle, -1, FALSE);
	newAnim->fTotalTime = MV1GetAttachAnimTotalTime(_modelHandle, newAnim->attachIndex);
	newAnim->fPlayTime = 0.0f;
	newAnim->fCloseTime = 0.0f;
	newAnim->fCloseTotalTime = 0.0f;
	newAnim->fFadeInTime = 0.0f;
	newAnim->fFadeInTotalTime = fBlendFrame; // フェードイン時間を設定
	newAnim->loopCnt = _animResources[animIndex].loopCnt;

	// 即切り替えの場合はブレンド率を1.0に設定
	float startBlendRate = (fBlendFrame <= 0.0f) ? 1.0f : 0.0f;
	MV1SetAttachAnimBlendRate(_modelHandle, newAnim->attachIndex, startBlendRate);

	_activeAnims.push_back(newAnim);
	_currentAnimIndex = animIndex;

	return true;
}

void AnimManager::Update()
{
	if (_modelHandle == -1) { return; }

	// アクティブなアニメーションを更新
	auto it = _activeAnims.begin();
	while (it != _activeAnims.end())
	{
		AnimationData* anim = *it;

		// 再生時間を進める
		anim->fPlayTime += anim->fPlaySpeed;

		anim->bIsFinished = false;

		// ループ処理
		if (anim->fPlayTime >= anim->fTotalTime)
		{
			if (anim->loopCnt == 0)
			{
				// 無限ループ
				anim->fPlayTime = 0.0f;
			}
			else if (anim->loopCnt > 1)
			{
				// 指定回数ループ
				anim->loopCnt--;
				anim->fPlayTime = 0.0f;
			}
			else
			{
				// ループ終了
				anim->fPlayTime = anim->fTotalTime;
				anim->bIsFinished = true;
			}
		}

		// アニメーション時間を設定
		MV1SetAttachAnimTime(_modelHandle, anim->attachIndex, anim->fPlayTime);

		// ブレンド率を計算
		float blendRate = 1.0f;
		// フェードアウト処理
		if (anim->fCloseTime > 0.0f)
		{
			anim->fCloseTime -= 1.0f;// フレームで減少

			if (anim->fCloseTotalTime > 0.0f)
			{
				blendRate = anim->fCloseTime / anim->fCloseTotalTime;
			}
			else
			{
				blendRate = 0.0f;
			}
		}
		// フェードイン処理
		else if (anim->fFadeInTotalTime > 0.0f && anim->fFadeInTime < anim->fFadeInTotalTime)
		{
			anim->fFadeInTime += 1.0f;// フレームで増加
			blendRate = anim->fFadeInTime / anim->fFadeInTotalTime;
			if (blendRate > 1.0f) { blendRate = 1.0f; }
		}

		// ブレンド率の範囲制限
		if (blendRate < 0.0f) { blendRate = 0.0f; }
		if (blendRate > 1.0f) { blendRate = 1.0f; }

		// ブレンド率を設定
		MV1SetAttachAnimBlendRate(_modelHandle, anim->attachIndex, blendRate);

		// ブレンド終了したアニメーションをデタッチ
		if (anim->fCloseTime <= 0.0f && anim->fCloseTotalTime > 0.0f)
		{
			DetachAnimation(anim);
			it = _activeAnims.erase(it);
			delete anim;
		}
		else
		{
			++it;
		}
	}
}

void AnimManager::Render()
{
	// モデルがロードされていない場合は何もしない
	if (_modelHandle == -1)
	{
		return;
	}

	// モデルを描画
	MV1DrawModel(_modelHandle);
}

void AnimManager::Release()
{
	// アクティブなアニメーションをデタッチ
	for (auto* anim : _activeAnims)
	{
		DetachAnimation(anim);
		delete anim;
	}
	_activeAnims.clear();

	// アニメーションリソースを解放
	for (auto& resource : _animResources)
	{
		MV1DeleteModel(resource.handle);
	}
	_animResources.clear();

	// モデルを解放
	if (_modelHandle != -1)
	{
		MV1DeleteModel(_modelHandle);
		_modelHandle = -1;
	}

	_currentAnimIndex = -1;
}

bool AnimManager::ChangeAnimationByName(const char* animName, float fBlendFrame, int loop, float fPlaySpeed)
{
	if (_modelHandle == -1)
	{
		return false;
	}

	if (animName == nullptr)
	{
		return false;
	}

	// モデル内のアニメーション名からインデックスを取得
	int animIndex = MV1GetAnimIndex(_modelHandle, animName);
	if (animIndex == -1)
	{
		// アニメーションが見つからなかった
		return false;
	}

	// 既存のアニメーションをブレンド終了状態に
	for (auto* anim : _activeAnims)
	{
		if (anim->fCloseTime <= 0.0f)
		{
			anim->fCloseTime = fBlendFrame;
			anim->fCloseTotalTime = fBlendFrame;
		}
	}

	// 新しいアニメーションを追加
	AnimationData* newAnim = new AnimationData();
	newAnim->attachIndex = MV1AttachAnim(_modelHandle, animIndex, -1, FALSE);
	newAnim->fTotalTime = MV1GetAttachAnimTotalTime(_modelHandle, newAnim->attachIndex);
	newAnim->fPlayTime = 0.0f;
	newAnim->fCloseTime = 0.0f;
	newAnim->fCloseTotalTime = 0.0f;
	newAnim->fFadeInTime = 0.0f;
	newAnim->fFadeInTotalTime = fBlendFrame; // フェードイン時間を設定
	newAnim->loopCnt = loop;
	newAnim->fPlaySpeed = fPlaySpeed;

	// 即切り替えの場合はブレンド率を1.0に設定
	float startBlendRate = (fBlendFrame <= 0.0f) ? 1.0f : 0.0f;
	MV1SetAttachAnimBlendRate(_modelHandle, newAnim->attachIndex, startBlendRate);

	_activeAnims.push_back(newAnim);
	_currentAnimIndex = animIndex;

	return true;
}

void AnimManager::DetachAnimation(AnimationData* anim)
{
	if (_modelHandle == -1 || anim == nullptr) 
	{
		return;
	}

	MV1DetachAnim(_modelHandle, anim->attachIndex);
}

bool AnimManager::IsAnimationFinished()
{
	// アクティブなアニメーションがない場合はtrueを返す
	if (_activeAnims.empty())
	{
		return true;
	}

	// 最新のアニメーションの終了状態を確認
	const AnimationData* currentAnim = _activeAnims.back();
	return currentAnim->bIsFinished;
}

// アニメーションの総再生時間を取得
float AnimManager::GetCurrentAnimTotalTime()
{
	// アクティブなアニメーションがない場合は0を返す
	if(_activeAnims.empty()){ return 0.0f; }

	// 最新のアニメーションの総再生時間を返す
	const AnimationData* currentAnim = _activeAnims.back();
	return currentAnim->fTotalTime;
}
