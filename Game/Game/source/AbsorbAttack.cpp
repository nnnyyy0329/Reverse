#include "AbsorbAttack.h"
#include "CharaBase.h"
#include "EnergyManager.h"

// 吸収量に関する定数
namespace AbsorbAmount
{
	constexpr float ABSORB_ENERGY = 1.0;	// 吸収エネルギー量
	constexpr float ABSORB_HP = 0.5f;		// 吸収HP量
}

// 吸収時間に関する定数
namespace AbsorbTime
{
	constexpr float ABSORB_COOLDOWN = 30.0f;		// 吸収のクールダウン時間
	constexpr float DECREMENT_ABSORB_TIMER = 1.0f;	// 吸収タイマーの減算量
	constexpr float ABSORB_UP_TIME = -300.0f;		// 吸収量が上昇するまでの時間
}

AbsorbAttack::AbsorbAttack() : AttackBase() // 親クラスのコンストラクタ呼び出し
{
	_stcAbsorbConfig.absorbRate			= 0.0f;						// 吸収率の初期化
	_stcAbsorbConfig.energyAbsorbRate	= 0.0f;						// エネルギー吸収率の初期化
	_stcAbsorbConfig.hpAbsorbRate		= 0.0f;						// HP吸収率の初期化
	_stcAbsorbConfig.absorbRange		= 0.0f;						// 吸収範囲の初期化
	_stcAbsorbConfig.absorbAngle		= 0.0f;						// 吸収範囲の角度の初期化
	_stcAbsorbConfig.absorbDivision		= 0.0f;						// 吸収範囲の分割数の初期化
	_stcAbsorbConfig.isActive			= false;					// 吸収攻撃がアクティブかどうかの初期化
	_stcAbsorbConfig.absorbEffectName	= "";						// 吸収エフェクト名の初期化
	_stcAbsorbConfig.effectOffset		= VGet(0.0f, 0.0f, 0.0f);	// エフェクト位置オフセットの初期化

	_fAbsorbCooldown = AbsorbTime::ABSORB_COOLDOWN;	// 吸収のクールダウン時間の初期化
	_fAbsorbTimer = 0.0f;							// 吸収タイマーの初期化
	_bIsInputActive = false;						// 入力が有効かどうかの初期化
	_bIsAbsorbIncreasing = false;					// 吸収量が上昇するかのフラグの初期化
}

AbsorbAttack::~AbsorbAttack()
{

}

bool AbsorbAttack::Process()
{
	// 状態更新処理
	ProcessAbsorbAttackState();

	// 吸収タイマーの更新
	ProcessDecrementTimer();

	// 基底攻撃クラスの入力による向き回転処理	
	UpdateAttackDirAdjust();

	return true;
}

bool AbsorbAttack::ProcessStartAttack()
{
	// 基底クラスの攻撃開始処理
	AttackBase::ProcessStartAttack();

	// 吸収攻撃の開始処理
	_stcAbsorbConfig.isActive = true;

	return true;
}

bool AbsorbAttack::ProcessStopAttack()
{
	if(!IsAbsorbActive()){ return false; }	// 吸収がアクティブでない場合は処理しない

	_eAttackState = ATTACK_STATE::RECOVERY;	// 状態を硬直中に遷移
	_stcAttackCol.isActive = false;			// 攻撃判定を非アクティブにする
	_fCurrentTime = 0.0f;		// タイマーリセット

	// 吸収攻撃の停止処理は、基底クラスの攻撃停止処理を呼び出すことで完了する
	return AttackBase::ProcessStopAttack();
}

void AbsorbAttack::ProcessDecrementTimer()
{
	// 吸収タイマーが0以上の場合
	if(_fAbsorbTimer > 0.0f)
	{
		_fAbsorbTimer -= AbsorbTime::DECREMENT_ABSORB_TIMER;	// タイマーを減算

		// 吸収量が上昇するまでの時間を、吸収時間が下回った場合
		if(_fAbsorbTimer < AbsorbTime::ABSORB_UP_TIME)
		{
			_bIsAbsorbIncreasing = true;	// 吸収量が上昇するフラグを立てる
		}
	}
}

void AbsorbAttack::ProcessAbsorbByInput(bool key)
{
	// 入力がある場合のみ処理
	if(key)
	{
		if(!IsAbsorbActive()){ return; }	// 吸収がアクティブでない場合は処理しない

		_bIsInputActive = true;	// 入力が有効に設定
	}
	// 入力がない場合
	else
	{
		_bIsInputActive = false;	// 入力が無効に設定

		// 吸収処理停止
		ProcessStopAttack();
	}
}

void AbsorbAttack::SetAbsorbConfig(const AbsorbConfig& config)
{
	_stcAbsorbConfig = config;	// 吸収設定を保存
}

AbsorbConfig AbsorbAttack::GetAbsorbConfig() const
{
	return _stcAbsorbConfig;	// 吸収設定を返す
}

void AbsorbAttack::ProcessAbsorb(std::shared_ptr<CharaBase> owner)
{
	// 対象が有効でない場合は処理しない
	if(!owner){ return; }	

	// クールダウン中の場合は処理しない
	if(_fAbsorbTimer > 0.0f){ return; }

	// エネルギー吸収処理
	ProcessEnergyAbsorb(owner);

	// HP吸収処理
	//ProcessHPAbsorb(owner);

	// 吸収エフェクト処理
	ProcessAbsorbEffect(owner->GetPos());

	// 吸収サウンド処理
	ProcessAbsorbSound(owner);

	_fAbsorbTimer = _fAbsorbCooldown;	// タイマーをリセット
}

void AbsorbAttack::ProcessEnergyAbsorb(std::shared_ptr<CharaBase> owner)
{
	// エネルギー取得
	EnergyManager::GetInstance()->AddEnergy(AbsorbAmount::ABSORB_ENERGY * _stcAbsorbConfig.energyAbsorbRate);
}

void AbsorbAttack::ProcessHPAbsorb(std::shared_ptr<CharaBase> owner)
{
	// HP吸収量計算
	float hpAbsorbAmount = AbsorbAmount::ABSORB_HP * _stcAbsorbConfig.hpAbsorbRate;

	// HPを回復
	owner->SetLife(owner->GetLife() + hpAbsorbAmount);
}

void AbsorbAttack::ProcessAbsorbEffect(const VECTOR& ownerPos)
{
	// エフェクト名が設定されている場合のみ再生
	if(!_stcAbsorbConfig.absorbEffectName.empty())
	{
		//// オフセット値をワールド座標に変換
		//VECTOR worldOffset = GeometryUtility::TransOffsetToWorld(config.effectOffset, dir);

		//// オフセット値を現在の位置に適応
		//VECTOR effectPos = VAdd(pos, worldOffset);

		//// エフェクト再生して、ハンドルを取得
		//auto handle = EffectServer::GetInstance()->Play(config.effectName, effectPos);

		//// エフェクトの向きを攻撃方向に合わせる
		//VECTOR dirNorm = VNorm(dir);							// 攻撃方向の正規化
		//float rotY = atan2f(dirNorm.x, dirNorm.z);				// Y軸回転角度を計算
		//VECTOR rotation = VGet(0.0f, rotY, 0.0f);				// エフェクトの回転量を設定
		//EffectServer::GetInstance()->SetRot(handle, rotation);	// 回転量をエフェクトに適応


		// 吸収エフェクト再生
		EffectServer::GetInstance()->Play(_stcAbsorbConfig.absorbEffectName, ownerPos);
	}
}

void AbsorbAttack::ProcessAbsorbSound(std::shared_ptr<CharaBase> owner)
{
	// サウンド名が設定されている場合のみ再生
	if(_stcAbsorbConfig.absorbSoundName.empty()){ return; }

	// サウンド再生
	SoundServer::GetInstance()->Play(_stcAbsorbConfig.absorbSoundName, DX_PLAYTYPE_BACK);
}

void AbsorbAttack::ProcessAbsorbAttackState()
{
	// 攻撃状態に応じた処理
	switch(_eAttackState)
	{
		case ATTACK_STATE::STARTUP: // 発生前状態
		{
			_fCurrentTime += 1.0f;	// タイマー加算

			// 発生遅延時間を超えた場合
			if(_fCurrentTime >= _stcAttackCol.attackDelay)
			{
				_fCurrentTime = 0.0f;					// タイマーリセット
				_stcAbsorbConfig.isActive = true;		// 吸収攻撃をアクティブにする
				_eAttackState = ATTACK_STATE::ACTIVE;	// 状態遷移
			}

			break;
		}

		case ATTACK_STATE::ACTIVE: // 攻撃判定中状態
		{
			break;
		}

		case ATTACK_STATE::RECOVERY: // 硬直中状態
		{
			_fCurrentTime += 1.0f;	// タイマー加算

			// 後隙時間を超えた場合
			if(_fCurrentTime >= _stcAttackCol.attackRecovery)
			{
				_eAttackState = ATTACK_STATE::INACTIVE;	// 状態遷移
				_fCurrentTime = 0.0f;					// タイマーリセット
				ClearHitCharas();						// ヒットリストクリア
			}

			break;
		}

		case ATTACK_STATE::INACTIVE: // 非アクティブ状態
		default:
		{
			break;
		}
	}
}

void AbsorbAttack::DebugRender()
{
	// 吸収攻撃の吸収範囲を描画
	if(IsAbsorbActive())
	{
		// 所有者の位置と方向を取得
		auto owner = GetOwner();
		if(owner)
		{
			VECTOR ownerPos = owner->GetPos();
			VECTOR ownerDir = owner->GetDir();

			// 扇形の吸収範囲を描画
			DrawAbsorbRange(ownerPos, ownerDir);
		}
	}

	// 吸収攻撃の状態を描画
	DrawAbsorbAttackState();

	// 吸収攻撃のタイマーを描画
	DrawAbsorbTimer();
}

void AbsorbAttack::DrawAbsorbRange(const VECTOR& ownerPos, const VECTOR& ownerDir)
{
	if(!_stcAbsorbConfig.isActive){ return; }	// 吸収攻撃がアクティブでない場合は描画しない

	// 扇形データ設定
	SectorData sectorData;

	sectorData.center		= ownerPos;
	sectorData.direction	= ownerDir;
	sectorData.range		= _stcAbsorbConfig.absorbRange;
	sectorData.angle		= _stcAbsorbConfig.absorbAngle;
	sectorData.heightOffset = 1.0f;

	// 色設定
	int fillColor = GetColor(0, 255, 150);
	int lineColor = GetColor(255, 255, 0);

	// 扇形描画
	GeometryUtility::DrawSector(sectorData, _stcAbsorbConfig.absorbDivision, fillColor, lineColor);
}

void AbsorbAttack::DrawAbsorbAttackState()
{
	// 状態文字列の初期化
	std::string stateStr;

	// 状態に応じた文字列を設定
	switch(_eAttackState)
	{
		case ATTACK_STATE::INACTIVE: // 非アクティブ状態
			stateStr = "State: INACTIVE";
			break;

		case ATTACK_STATE::STARTUP:	// 発生前状態
			stateStr = "State: STARTUP";
			break;

		case ATTACK_STATE::ACTIVE: // 攻撃判定中状態
			stateStr = "State: ACTIVE";
			break;

		case ATTACK_STATE::RECOVERY: // 硬直中状態
			stateStr = "State: RECOVERY";
			break;

		default:
			stateStr = "State: UNKNOWN";
			break;
	}

	// 状態文字列を画面に描画
	DrawFormatString(10, 310, GetColor(255, 255, 255), stateStr.c_str());
}

void AbsorbAttack::DrawAbsorbTimer()
{
	std::string timerStr = "Absorb Timer: " + std::to_string(_fAbsorbTimer);

	// タイマー文字列を画面に描画
	DrawFormatString(10, 330, GetColor(255, 255, 255), timerStr.c_str());
}