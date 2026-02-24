#include "PlayerAbsorbAttackSystem.h"

namespace AbsorbAttackConstants
{
	//constexpr int ABSORB_INPUT_KEY = PAD_INPUT_6;	// 吸収攻撃の入力キー
}

PlayerAbsorbAttackSystem::PlayerAbsorbAttackSystem()
{
	_bIsInitialized = false;	// 初期化フラグ
}

PlayerAbsorbAttackSystem::~PlayerAbsorbAttackSystem()
{
	// 終了処理
	Terminate();
}

bool PlayerAbsorbAttackSystem::Initialize(std::weak_ptr<CharaBase> owner)
{
	if(_bIsInitialized){ return false; }	// すでに初期化されている場合は何もしない

	// 吸収攻撃クラスインスタンスの作成
	MakeAbsorbAttack();

	// 初期化時の所有者設定
	InitializeSetOwner(owner);

	_bIsInitialized = true;	// 初期化完了

	return true;
}

// 吸収攻撃クラスインスタンスの作成
void PlayerAbsorbAttackSystem::MakeAbsorbAttack()
{
	// 吸収攻撃オブジェクト生成
	_absorbAttack = std::make_unique<AbsorbAttack>();
}

// 初期化時の所有者設定
void PlayerAbsorbAttackSystem::InitializeSetOwner(std::weak_ptr<CharaBase> owner)
{
	_owner = owner;	// 所有者設定

	// オーナーの設定
	auto ownerPtr = _owner.lock();
	if(ownerPtr)
	{
		// 吸収攻撃オブジェクトに所有者を設定
		_absorbAttack->SetOwner(ownerPtr);
	}
}

bool PlayerAbsorbAttackSystem::Terminate()
{
	if(!_bIsInitialized){ return false; }	// 初期化されていない場合は何もしない

	// 攻撃中なら停止
	if(IsAbsorbAttacking())
	{
		StopAbsorbAttack();
	}

	_absorbAttack.reset();	// 吸収攻撃オブジェクトの解放
	_owner.reset();			// 所有者の解放

	_bIsInitialized = false;	// 終了完了

	return true;
}

bool PlayerAbsorbAttackSystem::Process()
{
	if(!_bIsInitialized || !_absorbAttack){ return false; }	// 初期化されていない、または攻撃オブジェクトがない場合は処理しない

	// 吸収攻撃の更新処理
	_absorbAttack->Process();

	// 状態更新処理
	ProcessAbsorbAttackState();

	// アクティブ状態でのみ吸収処理を実行
	if(IsAbsorbAttacking())
	{
		auto owner = _owner.lock();
		if(owner)
		{
			// 吸収処理
			_absorbAttack->ProcessAbsorb(owner);
		}
	}

	return true;
}

bool PlayerAbsorbAttackSystem::Render()
{
	return true;
}

// デバッグ描画
void PlayerAbsorbAttackSystem::DebugRender()
{
	if(!_bIsInitialized || !_absorbAttack){ return; }	// 初期化されていない、またはオブジェクトがない場合はスキップ

	// 吸収攻撃のデバッグ描画
	_absorbAttack->DebugRender();
}

// 吸収攻撃の設定
void PlayerAbsorbAttackSystem::SetAbsorbConfig(const AbsorbConfig& config)
{
	if (_absorbAttack)
	{
		// 吸収攻撃オブジェクトに設定を渡す
		_absorbAttack->SetAbsorbConfig(config);
	}
}

// 吸収攻撃の設定取得
AbsorbConfig PlayerAbsorbAttackSystem::GetAbsorbConfig() const
{
	if (_absorbAttack)
	{
		// 吸収攻撃オブジェクトから設定を取得して返す
		return _absorbAttack->GetAbsorbConfig();
	}

	// デフォルトの設定を返す
	return AbsorbConfig(); 
}

// 攻撃開始
void PlayerAbsorbAttackSystem::StartAbsorbAttack()
{
	if(!CanStartAbsorbAttack() || !_absorbAttack) return;

	// 攻撃開始
	_absorbAttack->ProcessStartAttack();
}

// 攻撃停止
void PlayerAbsorbAttackSystem::StopAbsorbAttack()
{
	if(!_absorbAttack){ return; }

	// 攻撃停止
	_absorbAttack->ProcessStopAttack();
}

// 吸収攻撃の更新処理
void PlayerAbsorbAttackSystem::ProcessAbsorb()
{
	if(!IsAbsorbAttacking()){ return; }	// 攻撃オブジェクトがない場合はスキップ

	auto owner = _owner.lock();
	if(!owner){ return; }	// 所有者がいない場合はスキップ

	// 吸収処理
	_absorbAttack->ProcessAbsorb(owner);
}

// 吸収攻撃の入力処理
void PlayerAbsorbAttackSystem::ProcessAbsorbInput(int key)
{
	//if(key && CanStartAbsorbAttack())
	//{
	//	// 攻撃開始
	//	StartAbsorbAttack();
	//}

	//// 長押し処理
	//if(IsAbsorbAttacking())
	//{
	//	bool isInputActive = key;							// 入力があるかどうか
	//	_absorbAttack->ProcessAbsorbByInput(isInputActive);	// 入力による吸収処理
	//}
}

// 吸収攻撃の状態更新
void PlayerAbsorbAttackSystem::ProcessAbsorbAttackState()
{
	
}

// 吸収攻撃を開始できるかどうか
bool PlayerAbsorbAttackSystem::CanStartAbsorbAttack() const
{
	auto owner = _owner.lock();
	if(!owner){ return false; }	// 所有者がいない場合は攻撃開始できないと判断

	return !IsAbsorbAttacking();	// 吸収攻撃中でない場合に攻撃開始できると判断
}

// 吸収攻撃中かどうか
bool PlayerAbsorbAttackSystem::IsAbsorbAttacking() const
{
	if(!_absorbAttack){ return false; }	// 攻撃オブジェクトがない場合はスキップ

	// 吸収攻撃がアクティブ状態である場合に吸収攻撃中と判断
	return _absorbAttack->IsAbsorbActive();
}