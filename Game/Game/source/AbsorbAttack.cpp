#include "AbsorbAttack.h"
#include "CharaBase.h"
#include "EnergyManager.h"

// 吸収攻撃の設定に関する定数
namespace AbsorbConstants
{
	constexpr float DEFAULT_ABSORB_RATE			= 0.5f;				// デフォルトの吸収率
	constexpr float DEFAULT_ENERGY_ABSORB_RATE	= 0.5f;				// デフォルトのエネルギー吸収率
	constexpr float DEFAULT_HP_ABSORB_RATE		= 0.5f;				// デフォルトのHP吸収率
	constexpr float DEFAULT_ABSORB_RANGE		= 50.0f;			// デフォルトの吸収範囲
	constexpr float DEFAULT_ABSORB_ANGLE		= DX_PI_F / 3.0f;	// デフォルトの吸収範囲の角度（60度）
	constexpr int DEFAULT_ABSORB_DIVISION		= 16;				// デフォルトの吸収範囲の分割数
}

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
}

AbsorbAttack::AbsorbAttack() : AttackBase() // 親クラスのコンストラクタ呼び出し
{
	_stcAbsorbConfig.absorbRate			= AbsorbConstants::DEFAULT_ABSORB_RATE;			// 吸収率の初期化
	_stcAbsorbConfig.energyAbsorbRate	= AbsorbConstants::DEFAULT_ENERGY_ABSORB_RATE;	// エネルギー吸収率の初期化
	_stcAbsorbConfig.hpAbsorbRate		= AbsorbConstants::DEFAULT_HP_ABSORB_RATE;		// HP吸収率の初期化
	_stcAbsorbConfig.absorbRange		= AbsorbConstants::DEFAULT_ABSORB_RANGE;		// 吸収範囲の初期化
	_stcAbsorbConfig.absorbAngle		= AbsorbConstants::DEFAULT_ABSORB_ANGLE;		// 吸収範囲の角度の初期化
	_stcAbsorbConfig.absorbDivision		= AbsorbConstants::DEFAULT_ABSORB_DIVISION;		// 吸収範囲の分割数の初期化
	_stcAbsorbConfig.absorbEffectName	= "";											// 吸収エフェクト名の初期化

	_fAbsorbCooldown = AbsorbTime::ABSORB_COOLDOWN;	// 吸収のクールダウン時間の初期化
	_fAbsorbTimer = 0.0f;							// 吸収タイマーの初期化
	_bIsInputActive = false;						// 入力が有効かどうかの初期化
}

AbsorbAttack::~AbsorbAttack()
{

}

// 更新
bool AbsorbAttack::Process()
{
	// 状態更新処理
	ProcessAbsorbAttackState();

	// 吸収タイマーの更新
	ProcessDecrementTimer();

	return true;
}

// 攻撃開始
bool AbsorbAttack::ProcessStartAttack()
{
	// 基底クラスの攻撃開始処理
	AttackBase::ProcessStartAttack();

	return true;
}

// 攻撃停止
bool AbsorbAttack::ProcessStopAttack()
{
	if(!IsAbsorbActive()){ return false; }	// 吸収がアクティブでない場合は処理しない

	_eAttackState = ATTACK_STATE::RECOVERY;	// 状態を硬直中に遷移
	_stcAttackCol.isActive = false;			// 攻撃判定を非アクティブにする
	_stcAttackCol.currentTime = 0.0f;		// タイマーリセット

	// 吸収攻撃の停止処理は、基底クラスの攻撃停止処理を呼び出すことで完了する
	return AttackBase::ProcessStopAttack();
}

// タイマー更新処理
void AbsorbAttack::ProcessDecrementTimer()
{
	// 吸収タイマーが0以上の場合
	if(_fAbsorbTimer > 0.0f)
	{
		_fAbsorbTimer -= AbsorbTime::DECREMENT_ABSORB_TIMER;	// タイマーを減算

		// タイマーが0未満になった場合
		if(_fAbsorbTimer <= 0.0f)
		{
			_fAbsorbTimer = 0.0f;	// タイマーを0にリセット
		}
	}
}

// 入力による吸収処理
void AbsorbAttack::ProcessAbsorbByInput(int key)
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

// 吸収情報を設定する関数
void AbsorbAttack::SetAbsorbConfig(const AbsorbConfig& config)
{
	_stcAbsorbConfig = config;	// 吸収設定を保存
}

// 吸収攻撃の設定取得
AbsorbConfig AbsorbAttack::GetAbsorbConfig() const
{
	return _stcAbsorbConfig;	// 吸収設定を返す
}

// 吸収処理
void AbsorbAttack::ProcessAbsorb(std::shared_ptr<CharaBase> owner)
{
	// 対象が有効でない場合は処理しない
	if(!owner){ return; }	

	// クールダウン中の場合は処理しない
	if(_fAbsorbTimer > 0.0f){ return; }	

	// 扇形データ設定
	SectorData sectorData;

	sectorData.center		= owner->GetPos();
	sectorData.direction	= owner->GetDir();
	sectorData.range		= _stcAbsorbConfig.absorbRange;
	sectorData.angle		= _stcAbsorbConfig.absorbAngle;
	sectorData.heightOffset = 0.0f;

	// エネルギー吸収処理
	ProcessEnergyAbsorb(owner);

	// HP吸収処理
	//ProcessHPAbsorb(owner);

	// 吸収エフェクト処理
	ProcessAbsorbEffect(owner->GetPos());

	_fAbsorbTimer = _fAbsorbCooldown;	// タイマーをリセット
}

// エネルギー吸収処理
void AbsorbAttack::ProcessEnergyAbsorb(std::shared_ptr<CharaBase> owner)
{
	// エネルギー取得
	EnergyManager::GetInstance()->AddEnergy(AbsorbAmount::ABSORB_ENERGY * _stcAbsorbConfig.energyAbsorbRate);
}

// HP吸収処理
void AbsorbAttack::ProcessHPAbsorb(std::shared_ptr<CharaBase> owner)
{
	// HP吸収量計算
	float hpAbsorbAmount = AbsorbAmount::ABSORB_HP * _stcAbsorbConfig.hpAbsorbRate;

	// HPを回復
	owner->SetLife(owner->GetLife() + hpAbsorbAmount);
}

// 吸収エフェクト処理
void AbsorbAttack::ProcessAbsorbEffect(const VECTOR& ownerPos)
{
	// エフェクト名が設定されている場合のみ再生
	if(!_stcAbsorbConfig.absorbEffectName.empty())
	{
		// 吸収エフェクト再生
		EffectServer::GetInstance()->Play(_stcAbsorbConfig.absorbEffectName, ownerPos);
	}
}

// 吸収攻撃の状態更新
void AbsorbAttack::ProcessAbsorbAttackState()
{
	// 攻撃状態に応じた処理
	switch(_eAttackState)
	{
		case ATTACK_STATE::STARTUP: // 発生前状態
		{
			_stcAttackCol.currentTime += 1.0f;	// タイマー加算

			// 発生遅延時間を超えた場合
			if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay)
			{
				_stcAttackCol.currentTime = 0.0f;		// タイマーリセット
				_eAttackState = ATTACK_STATE::ACTIVE;	// 状態遷移
			}

			break;
		}

		case ATTACK_STATE::ACTIVE: // 攻撃判定中状態
		{
			// 長押し中はACTIVEを維持する

			break;
		}

		case ATTACK_STATE::RECOVERY: // 硬直中状態
		{
			_stcAttackCol.currentTime += 1.0f;	// タイマー加算

			// 後隙時間を超えた場合
			if(_stcAttackCol.currentTime >= _stcAttackCol.recovery)
			{
				_eAttackState = ATTACK_STATE::INACTIVE;	// 状態遷移
				_stcAttackCol.currentTime = 0.0f;		// タイマーリセット
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

// デバッグ描画
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
}

// 吸収範囲の扇形描画
void AbsorbAttack::DrawAbsorbRange(const VECTOR& ownerPos, const VECTOR& ownerDir)
{
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