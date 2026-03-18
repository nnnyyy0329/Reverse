#include "AttackEffectSystem.h"

// シングルトンインスタンス初期化
AttackEffectSystem* AttackEffectSystem::_instance = nullptr;

// シングルトンアクセス
AttackEffectSystem* AttackEffectSystem::GetInstance()
{
	return _instance;
}

// シングルトンインスタンス生成
void AttackEffectSystem::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new AttackEffectSystem();	// インスタンス生成
		_instance->Initialize();				// 初期化
	}
}

// シングルトンインスタンス破棄
void AttackEffectSystem::DestroyInstance()
{
	if(_instance != nullptr)
	{
		delete _instance;		// インスタンス破棄
		_instance = nullptr;	// ポインタをnullptrに設定
	}
}
AttackEffectSystem::AttackEffectSystem()
{
	// アクティブなエフェクトの管理マップ初期化
	_activeEffects.clear();

	// 追跡するエフェクトの管理マップ初期化
	_trackedEffects.clear();
}

AttackEffectSystem::~AttackEffectSystem()
{
	// 管理マップクリア
	_activeEffects.clear();
	_trackedEffects.clear();
}

bool AttackEffectSystem::Initialize()
{
	return true;
}

bool AttackEffectSystem::Terminate()
{
	// 管理マップクリア
	_activeEffects.clear();
	_trackedEffects.clear();

	return true;
}

bool AttackEffectSystem::Process()
{
	// 追跡エフェクトの位置更新
	UpdateTrackedEffects();

	return true;
}

bool AttackEffectSystem::Render()
{
	return true;
}

//int AttackEffectSystem::PlayEffect(const AttackEffectConfig& config, const VECTOR& pos, const VECTOR& dir)
//{
//	if(!config.isActiveEffect || config.effectName.empty())
//	{
//		return -1;
//	}
//
//	// オフセット計算
//	VECTOR worldOffset = GeometryUtility::TransOffsetToWorld(config.effectOffset, dir);
//	VECTOR effectPos = VAdd(pos, worldOffset);
//
//	// エフェクト再生
//	int handle = EffectServer::GetInstance()->Play(config.effectName, effectPos);
//
//	// 向き設定
//	VECTOR dirNorm = VNorm(dir);
//	float rotY = atan2f(dirNorm.x, dirNorm.z);
//	VECTOR rotation = VGet(0.0f, rotY, 0.0f);
//	EffectServer::GetInstance()->SetRot(handle, rotation);
//
//	// インスタンス登録
//	EffectInstanceInfo instance;
//	instance.effectHandle = handle;
//	instance.currentPos = effectPos;
//	_activeEffects[handle] = instance;
//
//	// サウンド処理
//	ProcessSound(config);
//
//	// カメラシェイク処理
//	ProcessCameraShake(config);
//
//	// ヒットストップ処理
//	ProcessHitStop(config);
//
//	// 再生したエフェクトのハンドルを返す
//	return handle;
//}


int AttackEffectSystem::PlayTrackedEffect
(
	const AttackEffectConfig& config,
	const VECTOR& initialPos,
	const VECTOR& dir,
	int frameIndex,
	AnimManager* animManager
)
{
	if(!config.isActiveEffect || config.effectName.empty())
	{
		return -1;
	}

	// オフセット計算
	VECTOR worldOffset = GeometryUtility::TransOffsetToWorld(config.effectOffset, dir);
	VECTOR effectPos = VAdd(initialPos, worldOffset);

	// エフェクト再生
	int handle = EffectServer::GetInstance()->Play(config.effectName, effectPos);

	// エフェクト処理
	ProcessEffect(handle, dir, config.effectRotation);

	// 追跡エフェクト情報
	TrackedEffectInfo trackedInfo;

	trackedInfo.effectHandle = handle;
	trackedInfo.attachFrameIndex = frameIndex;
	trackedInfo.effectOffset = worldOffset;
	trackedInfo.effectRotation = config.effectRotation;
	trackedInfo.animManager = animManager;

	_trackedEffects[handle] = trackedInfo;

	// サウンド処理
	ProcessSound(config);

	// カメラシェイク処理
	ProcessCameraShake(config);

	// ヒットストップ処理
	ProcessHitStop(config);

	// 再生したエフェクトのハンドルを返す
	return handle;
}

void AttackEffectSystem::StopEffect(int effectHandle)
{
	auto activeEffect = _activeEffects.find(effectHandle);

	// エフェクトが見つかった場合のみ停止処理
	if(activeEffect != _activeEffects.end())
	{
		// エフェクト停止
		_activeEffects.erase(activeEffect);
	}

	auto trackedEffect = _trackedEffects.find(effectHandle);

	// 追跡エフェクトが見つかった場合のみ停止処理
	if(trackedEffect != _trackedEffects.end())
	{
		// エフェクト停止
		_trackedEffects.erase(trackedEffect);
	}
}

void AttackEffectSystem::UpdateTrackedEffects()
{
	if(_trackedEffects.empty())
	{
		return;
	}

	auto trackedEffect = _trackedEffects.begin();

	// すべての追跡エフェクトを更新
	while(trackedEffect != _trackedEffects.end())
	{
		// 追跡エフェクト情報取得
		TrackedEffectInfo& info = trackedEffect->second;

		// アニメーションマネージャーとフレームインデックスが有効な場合のみ位置更新
		if(info.animManager && info.attachFrameIndex >= 0)
		{
			// モデルハンドル取得
			int modelHandle = info.animManager->GetModelHandle();

			// モデルハンドルが有効な場合のみ位置更新
			if(modelHandle >= 0)
			{
				// フレームインデックスから座標取得
				VECTOR framePos = MV1GetFramePosition(modelHandle, info.attachFrameIndex);

				// オフセット計算
				VECTOR offsetPos = VAdd(framePos, info.effectOffset);

				// エフェクト位置更新
				EffectServer::GetInstance()->SetPos(info.effectHandle, offsetPos);

				// エフェクト回転更新
				EffectServer::GetInstance()->SetRot(info.effectHandle, info.effectRotation);
			}
		}

		// 次の追跡エフェクトへ
		++trackedEffect;
	}
}

void AttackEffectSystem::ProcessEffect(int handle, const VECTOR& dir, const VECTOR& customRotation)
{
	// カスタム回転が指定されている場合
	if(VSize(customRotation) > 0.0001f)
	{
		// カスタム回転を使用
		EffectServer::GetInstance()->SetRot(handle, customRotation);
	}
	// カスタム回転が指定されていない場合
	else
	{
		// 攻撃方向から回転を計算
		VECTOR dirNorm = VNorm(dir);
		float rotY = atan2f(dirNorm.x, dirNorm.z);
		VECTOR rotation = VGet(0.0f, rotY, 0.0f);
		EffectServer::GetInstance()->SetRot(handle, rotation);
	}
}

void AttackEffectSystem::ProcessSound(const AttackEffectConfig& config)
{
	// サウンドが有効な場合のみ処理
	if(!config.isActiveSound){ return; }

	// サウンド名が空でない場合のみ
	if(!config.soundName.empty())
	{
		// サウンド再生
		SoundServer::GetInstance()->Play(config.soundName, DX_PLAYTYPE_BACK);
	}
}

void AttackEffectSystem::ProcessCameraShake(const AttackEffectConfig& config)
{
	// カメラシェイクが有効な場合のみ処理
	if(!config.isActiveCameraShake){ return; }


}

void AttackEffectSystem::ProcessHitStop(const AttackEffectConfig& config)
{
	// ヒットストップが有効な場合のみ処理
	if(!config.isActiveHitStop){ return; }


}