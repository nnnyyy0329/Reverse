#include "StateMelee.h"
#include "Enemy.h"
#include "EnemyState_impl.h"

namespace
{
	// 攻撃コリジョン設定用定数
	constexpr auto ATTACK_COLLISION_OFFSET_Z = 80.0f;	// 攻撃コリジョン前方オフセット
	constexpr auto ATTACK_COLLISION_OFFSET_Y = 60.0f;	// 攻撃コリジョンY位置オフセット
	constexpr auto ATTACK_COLLISION_HEIGHT = 60.0f;		// 攻撃コリジョン高さ
	constexpr auto ATTACK_COLLISION_RADIUS = 40.0f;		// 攻撃コリジョン半径

	// 突進攻撃コリジョン設定用定数
	constexpr auto RUSH_COLLISION_OFFSET_Z = 50.0f;		// 突進攻撃コリジョン前方オフセット
	constexpr auto RUSH_COLLISION_OFFSET_Y = 80.0f;		// 突進攻撃コリジョンY位置オフセット
	constexpr auto RUSH_COLLISION_HEIGHT = 100.0f;		// 突進攻撃コリジョン高さ
	constexpr auto RUSH_COLLISION_RADIUS = 50.0f;		// 突進攻撃コリジョン半径

	// 距離判定用定数
	constexpr auto CONFRONT_ENTER_DISTANCE = 300.0f;	// 対峙状態への遷移距離
	constexpr auto CONFRONT_KEEP_DISTANCE = 250.0f;		// 対峙時の維持距離
	constexpr auto CONFRONT_DISTANCE_TOLERANCE = 30.0f;	// 対峙時の距離許容範囲

	constexpr auto ATTACK_START_DISTANCE = 200.0f;		// 攻撃開始距離
	constexpr auto ATTACK_EXECUTE_DISTANCE = 100.0f;	// 攻撃実行可能距離

	constexpr auto RUSH_TARGET_REACHED_DISTANCE = 5.0f;	// 突進攻撃ターゲット到達判定距離

	constexpr auto LOST_NEARBY_HOME = 10.0f;			// 帰還完了判定距離

	// 時間制御用定数
	constexpr auto COUNTER_EXECUTE_TIME = 60.0f;		// カウンター攻撃実行時間

	constexpr auto RUSH_CHARGE_TIME = 30.0f;			// 突進攻撃溜め時間
	constexpr auto RUSH_EXECUTE_MAX_TIME = 120.0f;		// 突進攻撃最大実行時間
	constexpr auto RUSH_WAIT_AFTER_REACH = 30.0f;		// 突進攻撃到達後の待機時間
	constexpr auto RUSH_RECOVERY_TIME = 90.0f;			// 突進攻撃後隙時間

	constexpr auto CONFRONT_MIN_DURATION = 90.0f;		// 対峙最小時間
	constexpr auto CONFRONT_MAX_DURATION = 180.0f;		// 対峙最大時間

	constexpr auto COUNTER_DELAY = 33.0f;				// カウンター攻撃発生遅延
	constexpr auto COUNTER_DURATION = 10.0f;			// カウンター攻撃持続時間

	constexpr auto RUSH_DAMAGE = 1.0f;					// 突進攻撃ダメージ量

	// 確率制御用定数
	constexpr auto CONFRONT_PROBABILITY = 0;			// 対峙状態への遷移確率(%)
	constexpr auto RUSH_PROBABILITY = 0;				// 突進攻撃選択確率(%)

	// 速度制御用定数
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度

	constexpr auto CONFRONT_CIRCLE_SPEED = 1.5f;		// 対峙の回り込み速度
	constexpr auto CONFRONT_APPROACH_SPEED = 0.5f;		// 対峙時の接近速度
	constexpr auto CONFRONT_RETREAT_SPEED = 1.0f;		// 対峙時の後退速度

	constexpr auto ATTACK_APPROACH_SPEED = 2.0f;		// 攻撃開始時の接近速度

	constexpr auto RUSH_INITIAL_SPEED = 10.0f;			// 突進攻撃初速
	constexpr auto RUSH_DECELERATION_RATE = 0.95f;		// 突進攻撃減速率
	constexpr auto RUSH_MIN_SPEED_THRESHOLD = 0.1f;		// 突進攻撃最小速度閾値(これ以下で到達とみなす)

	// アニメーション制御用定数
	constexpr auto BLEND_FRAME = 10.0f;					// アニメーションブレンドフレーム数
	constexpr auto ANIM_LOOP_COUNT = 0;					// アニメーションループ回数(0=無限)
	constexpr auto ANIM_PLAY_COUNT = 1;					// アニメーション再生回数
	constexpr auto ANIM_SPEED_NORMAL = 1.0f;			// アニメーション再生速度(通常)
	constexpr auto ANIM_SPEED_FAST = 1.5f;				// アニメーション再生速度(速い)

	// 時間ランダム幅定数
	constexpr auto CONFRONT_DURATION_RANGE = 45.0f;		// 対峙時間のランダム幅

	constexpr auto FAN_VISON_HALF_ANGLE = 45.0f;// 扇形視界の半角(度)
	const float FAN_VISION_COS = cosf(FAN_VISON_HALF_ANGLE * DEGREE_TO_RADIAN);// cos値
	constexpr auto TARGET_DETECT_RADIUS = 20.0f;// ターゲット検出に幅を持たせる(半径)

	// 突進攻撃コリジョン設定生成
	EnemyAttackSettings MakeRushSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, RUSH_COLLISION_OFFSET_Y + RUSH_COLLISION_HEIGHT, RUSH_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, RUSH_COLLISION_OFFSET_Y, RUSH_COLLISION_OFFSET_Z);
		settings.fRadius = RUSH_COLLISION_RADIUS;
		settings.fDelay = 0.0f;
		settings.fDuration = 15.0f;
		settings.fRecovery = 0.0f;
		settings.fDamage = RUSH_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	// 反撃攻撃コリジョン
	EnemyAttackSettings MakeCounterSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y + ATTACK_COLLISION_HEIGHT, ATTACK_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y, ATTACK_COLLISION_OFFSET_Z);
		settings.fRadius = ATTACK_COLLISION_RADIUS;
		settings.fDelay = COUNTER_DELAY;
		settings.fDuration = COUNTER_DURATION;
		settings.fRecovery = 0.0f;
		settings.fDamage = 1.0f;
		settings.ownerId = 0;
		return settings;
	}

	// 定数として保存
	const EnemyAttackSettings RUSH_SETTINGS = MakeRushSettings();
	const EnemyAttackSettings COUNTER_SETTINGS = MakeCounterSettings();
}

namespace Melee
{
	// 扇形視界判定
	bool IsTargetVisibleFan(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target) { return false; }

		// Y成分を除去して水平方向ベクトルを取得
		VECTOR vToTarget = mymath::FlattenVector(VSub(target->GetPos(), owner->GetPos()));

		// FlattenVectorはゼロベクトルを返す可能性があるため元の距離で判定
		VECTOR vRaw = VSub(target->GetPos(), owner->GetPos());
		vRaw.y = 0.0f;
		float dist = VSize(vRaw);

		if (dist <= TARGET_DETECT_RADIUS) { return true; }// 発見したとみなす
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }

		// 角度チェック: 敵の向きも水平化して正規化
		VECTOR vOwnerDir = mymath::FlattenVector(owner->GetDir());

		float dot = VDot(vOwnerDir, vToTarget);
		if (dot < FAN_VISION_COS) { return false; }

		// 障害物チェック
		VECTOR vStart = owner->GetPos();
		VECTOR vEnd = target->GetPos();
		if (!owner->CheckLineOfSight(vStart, vEnd)) { return false; }

		return true;
	}



	// 攻撃ステート開始
	void AttackStart::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 攻撃タイプ決定処理
		if (mymath::RandomRange(0.0f, 100.0f) < RUSH_PROBABILITY)
		{
			_eAttackType = AttackType::RUSH;
		}
		else
		{
			_eAttackType = AttackType::NORMAL;
		}

		// アニメーション設定
	}

	std::shared_ptr<EnemyState> AttackStart::Update(Enemy* owner)
	{
		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// ターゲット存在チェック
		if (!targetInfo.bExist)
		{
			return TransitionToLostNoTarget<LostTarget>(owner);
		}

		// タイマー更新
		_fTimer++;

		// 追跡限界距離チェック
		auto result = TransitionToLostOverChaseLimit<LostTarget>(owner, targetInfo.fDist);
		if (result) { return result; }

		// 移動可能範囲外チェック
		auto areaResult = TransitionToLostOutsideArea<LostTarget>(owner);
		if (areaResult) { return areaResult; }

		// 攻撃タイプ別の遷移処理
		if (_eAttackType == AttackType::RUSH)
		{
			return std::make_shared<RushCharge>();
		}
		else
		{
			if (targetInfo.fDist <= ATTACK_EXECUTE_DISTANCE)
			{
				return std::make_shared<AttackCharge>();
			}
		}

		// ターゲット方向へ回転・移動
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		MoveToTarget(owner, targetInfo.vDir, ATTACK_APPROACH_SPEED);

		return nullptr;
	}

	

	// 突進攻撃溜め
	void RushCharge::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ターゲット方向保存
		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			_vRushDir = targetInfo.vDir;
		}
		else
		{
			_vRushDir = owner->GetDir();
		}

		// アニメーション設定
	}

	std::shared_ptr<EnemyState> RushCharge::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		StopMove(owner);

		// ターゲット方向へ追従
		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
			_vRushDir = targetInfo.vDir;
		}

		// 溜め時間終了チェック
		if (_fTimer >= RUSH_CHARGE_TIME)
		{
			return std::make_shared<RushExecute>(_vRushDir);
		}

		return nullptr;
	}

	// 突進攻撃実行
	void RushExecute::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fReachTimer = 0.0f;
		_bHasCollision = false;
		_fCurrentSpeed = RUSH_INITIAL_SPEED;
		_bReachedTarget = false;

		// 水平方向のみの移動にする
		_vRushDir = mymath::FlattenVector(_vRushDir);

		// 突進開始位置を保存
		_vStartPos = owner->GetPos();

		// ターゲット位置を保存
		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			_vTargetPos = targetInfo.target->GetPos();
			_vTargetPos.y = owner->GetPos().y;// Y座標は合わせる

			// 突進方向を再計算
			VECTOR vToTarget = VSub(_vTargetPos, owner->GetPos());
			VECTOR vRushDirFlat = mymath::FlattenVector(vToTarget);
			if (VSquareSize(vRushDirFlat) > 0.0001f)
			{
				_vRushDir = vRushDirFlat;
			}
		}
		else
		{
			// ターゲットがいない場合は現在の向き + 適当な距離
			_vTargetPos = VAdd(owner->GetPos(), VScale(_vRushDir, 300.0f));
		}

		// 方向設定
		owner->SetDir(_vRushDir);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_NORMAL);
		}

		// エフェクト

		// 突進攻撃コリジョン生成
		owner->StartAttack(RUSH_SETTINGS);
		_bHasCollision = true;
	}

	std::shared_ptr<EnemyState> RushExecute::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 現在位置取得
		VECTOR vCurrentPos = owner->GetPos();

		// ターゲット位置までの距離計算
		VECTOR vToTarget = VSub(_vTargetPos, vCurrentPos);
		vToTarget.y = 0.0f;
		float distToTarget = VSize(vToTarget);

		// ターゲット位置に到達したかチェック
		if (distToTarget <= RUSH_TARGET_REACHED_DISTANCE || _bReachedTarget || _fCurrentSpeed < RUSH_MIN_SPEED_THRESHOLD)
		{
			if (!_bReachedTarget)
			{
				_bReachedTarget = true;
				_fReachTimer = 0.0f;
			}

			// 到達後タイマー更新
			_fReachTimer++;

			// 到達後は停止
			StopMove(owner);

			// 突進攻撃コリジョン更新
			if (_bHasCollision)
			{
				owner->UpdateAttackTransform(RUSH_SETTINGS);
			}

			// 到達後一定時間経過で後隙へ
			if (_fReachTimer >= RUSH_WAIT_AFTER_REACH)
			{
				return std::make_shared<RushRecovery>();
			}

			return nullptr;
		}

		// 速度減衰処理
		_fCurrentSpeed *= RUSH_DECELERATION_RATE;

		// 残り距離が速度より小さいなら距離分だけ移動
		float moveDistance = _fCurrentSpeed;
		if (distToTarget < moveDistance)
		{
			moveDistance = distToTarget;
		}

		// 突進移動
		MoveToTarget(owner, _vRushDir, moveDistance);

		// 突進攻撃コリジョン更新
		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(RUSH_SETTINGS);
		}

		// 最大時間経過チェック
		if (_fTimer >= RUSH_EXECUTE_MAX_TIME)
		{
			return std::make_shared<RushRecovery>();
		}

		return nullptr;
	}

	void RushExecute::Exit(Enemy* owner)
	{
		// 突進攻撃コリジョン停止処理
		if (_bHasCollision)
		{
			owner->StopAttack();
			_bHasCollision = false;
		}
	}

	// 突進攻撃後隙
	void RushRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> RushRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止
		StopMove(owner);

		// 硬直時間終了チェック
		if (_fTimer >= RUSH_RECOVERY_TIME)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}


	// 対峙
	void Confront::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fDuration = CalcRandomRangeTime(CONFRONT_MIN_DURATION + (CONFRONT_MAX_DURATION - CONFRONT_MIN_DURATION) * 0.5f, CONFRONT_DURATION_RANGE);

		// 移動方向決定
		_direction = (mymath::RandomRange(0.0f, 1.0f) < 0.5f) ? 1 : -1;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> Confront::Update(Enemy* owner)
	{
		auto targetInfo = GetTargetInfo(owner);

		// ターゲット存在チェック
		if (!targetInfo.bExist)
		{
			return TransitionToLostNoTarget<LostTarget>(owner);
		}

		VECTOR vToTarget = targetInfo.vToTarget;
		vToTarget.y = 0.0f;
		float dist = VSize(vToTarget);

		// 追跡限界距離チェック
		auto result = TransitionToLostOverChaseLimit<LostTarget>(owner, dist);
		if (result) { return result; }

		// 移動可能範囲外チェック
		auto areaResult = TransitionToLostOutsideArea<LostTarget>(owner);
		if (areaResult) { return areaResult; }

		// 攻撃開始距離チェック
		if (dist <= ATTACK_START_DISTANCE)
		{
			return std::make_shared<AttackStart>();
		}

		// ターゲット方向の正規化ベクトル計算
		VECTOR vToTargetNorm = VNorm(vToTarget);

		// 上方向ベクトル
		VECTOR vUp = VGet(0.0f, 1.0f, 0.0f);

		// 横方向ベクトル計算
		VECTOR vRight = VCross(vToTargetNorm, vUp);

		// 距離差分計算
		float fDistanceDiff = dist - CONFRONT_KEEP_DISTANCE;

		// 距離調整ベクトル計算
		VECTOR vDistanceAdjust = VGet(0.0f, 0.0f, 0.0f);
		if (fDistanceDiff > CONFRONT_DISTANCE_TOLERANCE)
		{
			// 遠すぎる場合は接近
			vDistanceAdjust = VScale(vToTargetNorm, CONFRONT_APPROACH_SPEED);
		}
		else if (fDistanceDiff < -CONFRONT_DISTANCE_TOLERANCE)
		{
			// 近すぎる場合は後退
			vDistanceAdjust = VScale(vToTargetNorm, -CONFRONT_RETREAT_SPEED);
		}

		// 回り込みベクトル計算
		VECTOR vCircle = VScale(vRight, _direction * CONFRONT_CIRCLE_SPEED);

		// 合成移動ベクトル計算
		VECTOR vMove = VAdd(vDistanceAdjust, vCircle);

		// ターゲット方向設定
		owner->SetDir(vToTargetNorm);

		// 移動処理
		owner->SetMove(vMove);

		// 時間経過チェック
		_fTimer++;
		if (_fTimer >= _fDuration)
		{
			return std::make_shared<AttackStart>();
		}

		return nullptr;
	}



	// 反撃
	void CounterAttack::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bHasCollision = false;

		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			owner->SetDir(targetInfo.vDir);
		}

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_FAST);
		}

		// 攻撃コリジョン生成
		owner->StartAttack(COUNTER_SETTINGS);
		_bHasCollision = true;
	}

	std::shared_ptr<EnemyState> CounterAttack::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(COUNTER_SETTINGS);
		}

		if (_fTimer >= COUNTER_EXECUTE_TIME)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}

	void CounterAttack::Exit(Enemy* owner)
	{
		if (_bHasCollision)
		{
			owner->StopAttack();
			_bHasCollision = false;
		}
	}
}