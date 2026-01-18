#include "StateMelee.h"
#include "Enemy.h"

namespace {
	constexpr auto ATTACK_COLLISION_OFFSET_Z = 20.0f;// 攻撃コリジョン前方オフセット
	constexpr auto ATTACK_COLLISION_OFFSET_Y = 20.0f;// 攻撃コリジョンY位置オフセット
	constexpr auto ATTACK_COLLISION_HEIGHT = 60.0f;// 攻撃コリジョン高さ
	constexpr auto ATTACK_COLLISION_RADIUS = 40.0f;// 攻撃コリジョン半径

	// 個別の攻撃コリジョン設定
	EnemyAttackSettings MakeMeleeAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y + ATTACK_COLLISION_HEIGHT, ATTACK_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y, ATTACK_COLLISION_OFFSET_Z);
		settings.fRadius = ATTACK_COLLISION_RADIUS;
		settings.fDelay = 0.0f;
		settings.fDuration = 10.0f;
		settings.fRecovery = 10.0f;
		settings.fDamage = 10.0f;
		settings.ownerId = 0;// 仮(どういう用途？)
		return settings;
	}
	const EnemyAttackSettings MELEE_ATTACK_SETTINGS = MakeMeleeAttackSettings();
}

namespace Melee
{

	// プレイヤーを見つけたかチェックする(共通)
	// 見つけたら:DetectStateを返す。見つからない:nullptrを返す
	std::shared_ptr<EnemyState> TryDetectPlayer(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target) return nullptr;

		// 距離チェック
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		auto dist = VSize(vToTarget);// ターゲットまでの距離
		if(dist > owner->GetEnemyParam().visionRange) return nullptr;// 索敵距離外

		// 角度チェック
		VECTOR vDir = owner->GetDir();
		VECTOR vDirToTarget = VNorm(vToTarget);
		auto dot = VDot(vDir, vDirToTarget);
		auto limitCos = owner->GetEnemyParam().visionCos;
		// dotがlimitCos以上なら視界内
		if (dot >= limitCos) {
			// 視界内にいる
			return std::make_shared<Detect>();// 発見状態へ
		}

		return nullptr;
	}



	// 待機
	void Idle::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Idle::Update(Enemy* owner) {
		// 索敵チェック
		auto detectState = TryDetectPlayer(owner);
		if (detectState) return detectState;// 発見状態へ

		_fTimer++;

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().idleTime) {
			return std::make_shared<Move>();// 自動移動状態へ
		}

		return nullptr;
	}



	// 自動移動
	void Move::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		// ここでアニメーション設定

		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());// 初期位置へのベクトル
		auto dist = VSize(vToHome);// 初期位置までの距離
		auto limitRange = owner->GetEnemyParam().moveRadius;

		auto targetAngle = 0.0f;

		// 初期位置からの距離によって方向を変える
		if (dist > limitRange * 0.8f) {// 80%以上離れていたなら、初期位置を向く
			auto toHomeAngle = atan2f(vToHome.z, vToHome.x);// 初期位置への角度
			// ランダムな角度を足して少しばらつかせる
			auto randOffset = static_cast<float>(GetRand(90) - 45) * DEGREE_TO_RADIAN;
			targetAngle = toHomeAngle + randOffset;
		}
		else {
			// ランダムな方向に向く
			targetAngle = static_cast<float>(GetRand(359)) * DEGREE_TO_RADIAN;
		}

		VECTOR vDir = VGet(cosf(targetAngle), 0.0f, sinf(targetAngle));// XZ平面の角度の方向にある点のベクトルを計算
		owner->SetDir(vDir);
	}

	std::shared_ptr<EnemyState> Move::Update(Enemy* owner) {
		// 索敵チェック
		auto detectState = TryDetectPlayer(owner);
		if (detectState) return detectState;// 発見状態へ

		_fTimer++;

		// 移動範囲のチェック
		VECTOR vFromHome = VSub(owner->GetPos(), owner->GetHomePos());// 初期位置からのベクトル
		auto distSq = VSquareSize(vFromHome);// 平方根を使わない距離の2乗で
		auto limitRange = owner->GetEnemyParam().moveRadius;
		if (distSq > limitRange * limitRange) {// 範囲外なら
			// 初期位置の方向を向いているかチェック
			VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
			if (VDot(vToHome, owner->GetDir()) < 0) {// 内積が負なら逆方向を向いている
				return std::make_shared<Idle>();// 待機状態に戻る
			}
		}

		//　内積が正なら初期位置を向いているのでそのまま移動

		// 移動処理
		VECTOR vDir = owner->GetDir();
		auto speed = owner->GetEnemyParam().moveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);// 移動量を更新

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().moveTime) {
			return std::make_shared<Idle>();// 待機状態へ
		}
		return nullptr;
	}



	// 発見
	void Detect::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Detect::Update(Enemy* owner) {
		_fTimer++;

		// ターゲットの方向を向く
		auto target = owner->GetTarget();
		if (target) {
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vDir = VNorm(vToTarget);
			owner->SetDir(vDir);
		}

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().detectTime) {
			return std::make_shared<Chase>();// 追跡状態へ
		}
		return nullptr;
	}



	// 追跡
	std::shared_ptr<EnemyState> Chase::Update(Enemy* owner) {
		auto target = owner->GetTarget();
		// ターゲットがいない場合は待機状態へ
		if (!target) return std::make_shared<Idle>();

		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		auto dist = VSize(vToTarget);// ターゲットまでの距離
		
		// 追跡限界距離を超えたか
		if(dist > owner->GetEnemyParam().chaseLimitRange) {
			return std::make_shared<Idle>();// 待機状態へ
		}

		// 攻撃射程内か
		if (dist <= owner->GetEnemyParam().attackRange) {
			return std::make_shared<Attack>();// 攻撃状態へ
		}

		// 移動処理
		VECTOR vDir = VNorm(vToTarget);
		owner->SetDir(vDir);// ターゲットの方向を向く

		auto speed = owner->GetEnemyParam().moveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);// 移動量を更新

		return nullptr;
	}



	// 攻撃
	void Attack::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		owner->StartAttack(MELEE_ATTACK_SETTINGS);// 攻撃コリジョン開始
		// ここでアニメーション設定

	}

	std::shared_ptr<EnemyState> Attack::Update(Enemy* owner) {
		_fTimer++;

		// 攻撃中は移動しない
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// 攻撃コリジョンの位置更新
		owner->UpdateAttackTransform(MELEE_ATTACK_SETTINGS);

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().attackTime) {
			return std::make_shared<Chase>();// 追跡状態に戻ることで再度距離判定を行う
		}

		return nullptr;
	}

	void Attack::Exit(Enemy* owner) {
		owner->StopAttack();// 攻撃コリジョン停止
	}
}
