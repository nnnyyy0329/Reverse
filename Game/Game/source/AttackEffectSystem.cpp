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
	/* 攻撃エフェクトの設定初期化 */

	// エフェクト設定
	_stcAttackEffectConfig.isActiveEffect = false;
	_stcAttackEffectConfig.effectName = "";
	_stcAttackEffectConfig.effectOffset = VGet(0.0f, 0.0f, 0.0f);

	// サウンド設定
	_stcAttackEffectConfig.isActiveSound = false;
	_stcAttackEffectConfig.soundName = "";

	// カメラシェイク設定
	_stcAttackEffectConfig.isActiveCameraShake = false;
	_stcAttackEffectConfig.cameraShakeMagnitude = 0.0f;
	_stcAttackEffectConfig.cameraShakeDuration = 0.0f;

	// ヒットストップ設定
	_stcAttackEffectConfig.isActiveHitStop = false;
	_stcAttackEffectConfig.hitStopDuration = 0.0f;
}

AttackEffectSystem::~AttackEffectSystem()
{
}

bool AttackEffectSystem::Initialize()
{
	return true;
}

bool AttackEffectSystem::Terminate()
{
	return true;
}

bool AttackEffectSystem::Process()
{
	return true;
}

bool AttackEffectSystem::Render()
{
	return true;
}

// 攻撃演出の処理関数
void AttackEffectSystem::AttackEffect(const AttackEffectConfig& config, const VECTOR& pos, const VECTOR& dir)
{
	// エフェクト処理
	ProcessEffect(config, pos, dir);

	// サウンド処理
	ProcessSound(config);

	// カメラシェイク処理
	ProcessCameraShake(config);

	// ヒットストップ処理
	ProcessHitStop(config);
}

// 攻撃エフェクト処理
void AttackEffectSystem::ProcessEffect(const AttackEffectConfig& config, const VECTOR& pos, const VECTOR& dir)
{
	// エフェクトが有効な場合のみ処理
	if(!config.isActiveEffect){ return; }

	// エフェクト名が空でない場合のみ
	if(!config.effectName.empty())
	{
		// オフセット値をワールド座標に変換
		VECTOR worldOffset = GeometryUtility::TransOffsetToWorld(config.effectOffset, dir);

		// オフセット値を現在の位置に適応
		VECTOR effectPos = VAdd(pos, worldOffset);

		// エフェクト再生して、ハンドルを取得
		auto handle = EffectServer::GetInstance()->Play(config.effectName, effectPos);

		// エフェクトの向きを攻撃方向に合わせる
		VECTOR dirNorm = VNorm(dir);							// 攻撃方向の正規化
		float rotY = atan2f(dirNorm.x, dirNorm.z);				// Y軸回転角度を計算
		VECTOR rotation = VGet(0.0f, rotY, 0.0f);				// エフェクトの回転量を設定
		EffectServer::GetInstance()->SetRot(handle, rotation);	// 回転量をエフェクトに適応
	}
}

// 攻撃サウンド処理
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

// カメラシェイク処理
void AttackEffectSystem::ProcessCameraShake(const AttackEffectConfig& config)
{
	// カメラシェイクが有効な場合のみ処理
	if(!config.isActiveCameraShake){ return; }


}

// ヒットストップ処理
void AttackEffectSystem::ProcessHitStop(const AttackEffectConfig& config)
{
	// ヒットストップが有効な場合のみ処理
	if(!config.isActiveHitStop){ return; }


}