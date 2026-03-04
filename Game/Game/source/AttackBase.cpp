// 担当 : 成田

#include "AttackBase.h"
#include "CharaBase.h"

namespace MoveConstants
{
	constexpr float ATTACK_MOVE_DECAY_RATE = 0.9f; // 攻撃中の移動減衰率
}

namespace DirAdjustConstants
{
	constexpr float INTERP_CLAMP = 1.0f;        // 補間速度の上限
	constexpr float ROTATE_DIVISOR = 150.0f;    // 回転速度の割る量
}

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
	_stcAttackCol.canKnockback = false;

	// 攻撃移動情報の初期化
	_stcAttackMovement.moveDir = VGet(0.0f, 0.0f, 0.0f);
	_stcAttackMovement.moveDistance = 0.0f;
	_stcAttackMovement.moveSpeed = 0.0f;
	_stcAttackMovement.decayRate = 0.0f;
	_stcAttackMovement.canMove = false;

    // コリジョンタイプ
    _eColType = COLLISION_TYPE::NONE;
    _eAttackState = ATTACK_STATE::INACTIVE;

	// 向き調整の初期化
	_canDirAdjust = false;
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

    // 攻撃中の移動更新
    UpdateAttackMove();

	// 攻撃中の向き調整更新
	UpdateAttackDirAdjust();

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
    // 所有者キャラを取得
    auto owner = GetOwner();
    if(!owner){ return; }

    // 移動速度が設定されており、現在の状態が指定された攻撃状態と一致するなら
    if(_stcAttackCol.attackMoveSpeed > 0.0f && _eAttackState == _stcAttackCol.attackState)
    {
        // 攻撃中の移動処理
        ProcessAttackMovement();
    }
}

// 移動適用
void AttackBase::ProcessAttackMovement()
{
    auto owner = GetOwner();
    if(!owner) return;

    // 所有者キャラから現在の向きを取得
    VECTOR currentDir = owner->GetDir();

    VECTOR moveDir;

    // 攻撃方向が設定されている方を優先
    if(VSize(_stcAttackCol.attackDir) > 0.0f)
    {
        moveDir = VNorm(_stcAttackCol.attackDir);
    }
    else
    {
        moveDir = VNorm(currentDir);
    }

    // 移動量計算
    VECTOR vMove = VScale(moveDir, _stcAttackCol.attackMoveSpeed);

    // 減衰処理
    //vMove = VScale(vMove, ATTACK_MOVE_DECAY_RATE);

    // キャラの位置を加算
    VECTOR currentPos = owner->GetPos();
    VECTOR newPos = VAdd(currentPos, vMove);
    owner->SetPos(newPos);

    // モデルの位置も更新
    AnimManager* animManager = owner->GetAnimManager();
    if(animManager != nullptr)
    {
        // モデルの位置を更新
        MV1SetPosition(animManager->GetModelHandle(), newPos);
    }
}

// 向き調整更新
void AttackBase::UpdateAttackDirAdjust()
{
    if(!_canDirAdjust){ return; }   // 向き調整が可能な場合のみ処理

	auto owner = GetOwner();
	if(!owner){ return; }

	// 入力マネージャーーから移動入力を取得
	auto im = InputManager::GetInstance();
	const auto& analog = im->GetAnalog();

    // 入力があるなら向き調整
    if(abs(analog.lx)>im->GetAnalogMin() || abs(analog.ly)>im->GetAnalogMin())
    {
		// カメラ基準で入力方向を計算
        VECTOR inputDir = CalculateInputDir(analog);

        // 現在の向き
		VECTOR currentDir = owner->GetDir();

        // 内積で角度計算
		float dot = VDot(currentDir, inputDir);     // 内積を計算

        // クランプ
        dot = std::max
        (
			-DirAdjustConstants::INTERP_CLAMP,              // 下限
			std::min(DirAdjustConstants::INTERP_CLAMP, dot) // 上限
        );

		float angleDiff = acos(dot);    // 角度差を計算

        // 回転速度
		const float MAX_ROTATE_PER_FRAME = 
            DX_PI_F / DirAdjustConstants::ROTATE_DIVISOR;   // フレームごとの最大回転角度を定数で定義

        // 最大回転角度を制限
        if(angleDiff > MAX_ROTATE_PER_FRAME)
        {
            // 制限角度で補間
			float interp = MAX_ROTATE_PER_FRAME / angleDiff; // 線形補間で新しい向きを計算

			// 現在の向きと入力方向を補間して新しい向きを計算
            VECTOR newDir = VNorm(VAdd
            (
				VScale(currentDir, 1.0f - interp),   // 現在の向きの影響
				VScale(inputDir, interp)             // 入力方向の影響
            ));

			// 新しい向きを所有者に設定
            owner->SetDir(newDir);
        }
		// 角度差が小さい場合は直接入力方向を設定
        else
        {
            // 角度差が小さい場合は直接設定
            owner->SetDir(inputDir);
        }
    }
}

// 入力方向計算関数
VECTOR AttackBase::CalculateInputDir(const AnalogState& analog)
{
	auto owner = GetOwner();
	if(!owner){ return VGet(0.0f, 0.0f, 0.0f); }

	float inputX = -analog.lx;   // 左スティックのX軸入力
	float inputY = -analog.ly;   // 左スティックのY軸入力

    // 所有者の向きを基準にする
	VECTOR ownerDir = owner->GetDir();
    float currentAngle = atan2(ownerDir.z, ownerDir.x);

    // カメラ基準のベクトル計算
    VECTOR cameraForward = VGet(cos(currentAngle), 0.0f, sin(currentAngle));
    VECTOR cameraRight=VGet(cos(currentAngle + DX_PI_F / 2.0f), 0.0f, sin(currentAngle + DX_PI_F / 2.0f));

    // 入力方向計算
    VECTOR inputDir = VAdd
    (
        VScale(cameraForward, inputY),  // 前後移動
        VScale(cameraRight, inputX)     // 左右移動
    );

    // 正規化
    float len = VSize(inputDir);
    if(len > 0.0f)
    {
		// 入力方向を正規化して返す
        return VScale(inputDir, 1.0f / len);
    }

    return VGet(0, 0, 0);
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
    float attackMoveSpeed,
	bool canKnockback
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
	_stcAttackCol.canKnockback = canKnockback;        // 吹き飛ばし攻撃かどうか

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

// 向き調整データ設定
void AttackBase::SetDirAdjustData(bool canAdjust)
{
    _canDirAdjust = canAdjust;      // 向き調整可能フラグ
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

// 攻撃コリジョン表示
void AttackBase::DrawAttackCollision()
{
    // デバッグ用：攻撃判定の可視化
    if(!_stcAttackCol.isActive){ return; }

	// コリジョンタイプに応じて描画
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

