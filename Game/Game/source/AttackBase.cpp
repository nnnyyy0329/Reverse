// 担当 : 成田

#include "AttackBase.h"
#include "CharaBase.h"

AttackBase::AttackBase()
{    
    // 攻撃コリジョンの初期化
    _stcAttackCol.attackColTop = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackColBottom = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackColR = 0.0f;
	_stcAttackCol.attackDir = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackDelay = 0.0f;
    _stcAttackCol.attackDuration = 0.0f;
    _stcAttackCol.recovery = 0.0f;
    _stcAttackCol.damage = 0.0f;
    _stcAttackCol.currentTime = 0.0f;
    _stcAttackCol.isActive = false;
    _stcAttackCol.isHit = false;
	_stcAttackCol.attackState = ATTACK_STATE::INACTIVE;
	_stcAttackCol.attackMoveSpeed = 0.0f;

	// 攻撃移動情報の初期化
	_stcAttackMovement.moveDir = VGet(0.0f, 0.0f, 0.0f);
	_stcAttackMovement.moveDistance = 0.0f;
	_stcAttackMovement.moveSpeed = 0.0f;
	_stcAttackMovement.decayRate = 0.0f;
	_stcAttackMovement.canMove = false;

    // コリジョンタイプ
    _eColType = COLLISION_TYPE::NONE;
    _eAttackState = ATTACK_STATE::INACTIVE;
}

AttackBase::~AttackBase()
{

}

bool AttackBase::Initialize()
{
    return true;
}

bool AttackBase::Terminate()
{
    return true;
}

bool AttackBase::Process()
{
    // 攻撃状態更新処理
	UpdateAttackState();  

	// 攻撃中の移動更新処理
	UpdateAttackMove();

    return true;
}

bool AttackBase::Render()
{
    // デバッグ表示
    DrawAttackCollision();

    return true;
}

// 攻撃開始処理
bool AttackBase::ProcessStartAttack()
{
	// 非アクティブ状態の場合のみ攻撃開始処理を行う
    if(_eAttackState == ATTACK_STATE::INACTIVE)
    {
		_eAttackState = ATTACK_STATE::STARTUP;  // 発生前状態に移行
		_stcAttackCol.currentTime = 0.0f;       // 経過時間リセット
		_stcAttackCol.isActive = false;         // 攻撃判定は非アクティブに設定
		_stcAttackCol.isHit = false;            // ヒットフラグリセット

        return true;
    }

    return false;
}

// 攻撃停止処理
bool AttackBase::ProcessStopAttack()
{
	_eAttackState = ATTACK_STATE::INACTIVE; // 攻撃状態を非アクティブに設定
	_stcAttackCol.isActive = false;         // 攻撃判定を非アクティブに設定
	_stcAttackCol.currentTime = 0.0f;       // 経過時間リセット

    // ヒットリストクリア
    ClearHitCharas();
    return true;
}

// 攻撃状態更新
void AttackBase::UpdateAttackState()
{
	// 攻撃状態に応じて処理を分岐
    switch(_eAttackState)
    {
		case ATTACK_STATE::STARTUP: // 発生前
        {
			_stcAttackCol.currentTime += 1.0f;  // 経過時間を進める
            
			// 発生遅延時間を超えたら攻撃判定をアクティブにする
            if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay)
            {
                _eAttackState = ATTACK_STATE::ACTIVE;
                _stcAttackCol.isActive = true;
            }

            break;
        }

		case ATTACK_STATE::ACTIVE:  // 攻撃判定中
        {
			_stcAttackCol.currentTime += 1.0f;  // 経過時間を進める

			// 持続時間を超えたら後隙状態に移行する
            if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay + _stcAttackCol.attackDuration)
            {
                _eAttackState = ATTACK_STATE::RECOVERY;
                _stcAttackCol.isActive = false;
            }

            break;
        }

		case ATTACK_STATE::RECOVERY:    // 後隙
        {
			_stcAttackCol.currentTime += 1.0f;  // 経過時間を進める

			// 後隙時間を超えたら攻撃状態を非アクティブにする
            if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay + _stcAttackCol.attackDuration + _stcAttackCol.recovery)
            {
                _eAttackState = ATTACK_STATE::INACTIVE;
                _stcAttackCol.currentTime = 0.0f;

				// ヒットリストクリア
				ClearHitCharas();
            }

            break;
        }

        case ATTACK_STATE::INACTIVE:
        default:
        {
            _stcAttackCol.isActive = false;
            break;
        }
    }
}

// 攻撃中の移動更新
void AttackBase::UpdateAttackMove()
{
    switch(_stcAttackCol.attackState)
    {
        case ATTACK_STATE::STARTUP:
        case ATTACK_STATE::ACTIVE:
        case ATTACK_STATE::RECOVERY:
        {
            

            break;
        }

        default:
        {
            break;
        }
	}
}

// カプセル攻撃データ設定
void AttackBase::SetCapsuleAttackData
(
    VECTOR top,
    VECTOR bottom, 
    float radius, 
    VECTOR attackDir,
    float delay,
    float duration,
    float recovery,
    float damage,
    bool hit,
    ATTACK_STATE attackState,
    float attackMoveSpeed	
)
{
	_stcAttackCol.attackColTop = top;                   // カプセル上部
	_stcAttackCol.attackColBottom = bottom;             // カプセル下部
	_stcAttackCol.attackColR = radius;                  // 半径
    _stcAttackCol.attackDir = attackDir;                 // 攻撃方向
	_stcAttackCol.attackDelay = delay;                  // 発生遅延
	_stcAttackCol.attackDuration = duration;            // 持続時間
	_stcAttackCol.recovery = recovery;                  // 後隙
	_stcAttackCol.damage = damage;                      // ダメージ
	_stcAttackCol.isHit = hit;                          // ヒットフラグ
	_stcAttackCol.attackState = attackState;            // 攻撃状態
	_stcAttackCol.attackMoveSpeed = attackMoveSpeed;    // 攻撃中の移動速度

	_eColType = COLLISION_TYPE::CAPSULE;    // コリジョンタイプをカプセルに設定
}

// 円形攻撃データ設定
void AttackBase::SetCircleAttackData
(
    VECTOR center,
    float radius,
    float height,
    float delay,
    float duration,
    float recovery,
    float damage,
    bool hit
)
{
    // 高さを考慮した位置を設定
	_stcAttackCol.attackColTop = VGet(center.x, center.y + height * 0.5f, center.z);    // 中心位置から高さの半分を上下にずらす
	_stcAttackCol.attackColBottom = VGet(center.x, center.y - height * 0.5f, center.z); // 使用しない
	_stcAttackCol.attackColR = radius;                                                  // 半径
	_stcAttackCol.attackDelay = delay;                                                  // 発生遅延
	_stcAttackCol.attackDuration = duration;                                            // 持続時間 
	_stcAttackCol.recovery = recovery;                                                  // 後隙
	_stcAttackCol.damage = damage;                                                      // ダメージ
	_stcAttackCol.isHit = hit;                                                          // ヒットフラグ

	_eColType = COLLISION_TYPE::CIRCLE; // コリジョンタイプを円に設定
}

// 球攻撃データ設定
void AttackBase::SetSphereAttackData
(
    VECTOR center,
    float radius,
    float delay,
    float duration,
    float recovery,
    float damage,
    bool hit
)
{
    _stcAttackCol.attackColTop = center;                    // 中心点
    _stcAttackCol.attackColBottom = VGet(0.0f, 0.0f, 0.0f); // 使用しない
	_stcAttackCol.attackColR = radius;                      // 半径
	_stcAttackCol.attackDelay = delay;                      // 発生遅延
	_stcAttackCol.attackDuration = duration;                // 持続時間
	_stcAttackCol.recovery = recovery;                      // 後隙
	_stcAttackCol.damage = damage;                          // ダメージ
	_stcAttackCol.isHit = hit;                              // ヒットフラグ

	_eColType = COLLISION_TYPE::SPHERE; // コリジョンタイプを球に設定
}

// 攻撃コリジョン表示
void AttackBase::DrawAttackCollision()
{
    // デバッグ用：攻撃判定の可視化
    if(_stcAttackCol.isActive)
    {
        switch(_eColType)
        {
            // カプセル形状の描画（デバッグ用）
            case COLLISION_TYPE::CAPSULE:
            {
                DrawCapsule3D
                (
					_stcAttackCol.attackColTop,     // 上部座標
					_stcAttackCol.attackColBottom,  // 下部座標
					_stcAttackCol.attackColR,       // 半径
					8,                              // 分割数
                    GetColor(255, 0, 0),
                    GetColor(255, 0, 0),
                    TRUE
                );

                break;
            }

            // 円形の描画（デバッグ用）
            case COLLISION_TYPE::CIRCLE:
            {
                DrawCircle
                (
					static_cast<float>(_stcAttackCol.attackColTop.x),   // 中心X座標
					static_cast<float>(_stcAttackCol.attackColTop.y),   // 中心Y座標
					_stcAttackCol.attackColR,                           // 半径
                    GetColor(0, 255, 0),
                    TRUE
                );

                break;
            }

            // 球形の描画（デバッグ用）
            case COLLISION_TYPE::SPHERE:
            {
                DrawSphere3D
                (
					_stcAttackCol.attackColTop, // 中心位置
					_stcAttackCol.attackColR,   // 半径
					8,                          // 分割数
                    GetColor(0, 0, 255),
                    GetColor(0, 0, 255),
                    TRUE
                );

                break;
            }
        }
    }
}

// 当たったキャラを追加
void AttackBase::AddHitCharas(std::shared_ptr<CharaBase> chara)
{
    if(chara == nullptr) { return; }

    // 既に当たっているキャラは追加しない
    if(!HasHitCharas(chara))
    {
		// 当たったキャラリストに追加
        _hitCharas.push_back(chara);
    }
}

// 当たったキャラを持っているかチェック
bool AttackBase::HasHitCharas(std::shared_ptr<CharaBase> chara)const
{
    if(chara == nullptr) { return false; }

    // 当たったキャラリストを検索
    for(const auto& hitChara : _hitCharas)
    {
		// 一致するキャラがいるかチェック
        if(hitChara == chara)
        {
            // 見つかった
            return true;    
        }
    }

    // 見つからなかった
    return false;  
}

// 当たったキャラリストクリア
void AttackBase::ClearHitCharas()
{
    _hitCharas.clear();
}

// 攻撃中の移動更新
void AttackBase::UpdateAttackMove()
{
	// 所有者キャラを取得
	auto owner = GetOwner();
	if(!owner){ return; }

    // 派生クラスの移動計算を取得する
    AttackMovement movement = CalculateMovement(_eAttackState, owner);

	// 移動できるか
    if(movement.canMove)
    {
		// 移動適用
        ApplyMovement(movement, owner);
    }
}

// 移動適用
void AttackBase::ApplyMovement(const AttackMovement& movement, std::shared_ptr<CharaBase> owner)
{
	if(!movement.canMove || !owner) { return; }

    // 移動方向
	VECTOR moveDir;
	moveDir = owner->GetDir();

    // 正規化
    if(VSize(moveDir) > 0.0f)
    {
        VECTOR dirNorm = VNorm(moveDir);
	}

    // 移動量計算
	VECTOR vMove = VScale(movement.moveDir, movement.moveDistance);

	// 減衰処理
	vMove = VScale(vMove, movement.decayRate);

    // キャラの位置を加算
    VECTOR newPos = VAdd(owner->GetPos(), vMove);
	owner->SetPos(newPos);

}
