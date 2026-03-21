// 担当 : 成田

#include "AttackBase.h"
#include "CharaBase.h"
#include "CameraManager.h"

AttackBase::AttackBase()
{    
    // コリジョンタイプ
    _eColType = COLLISION_TYPE::NONE;
    _eAttackState = ATTACK_STATE::INACTIVE;

	// 経過時間の初期化
	_fCurrentTime = 0.0f;

    // 攻撃コリジョンの初期化
    _stcAttackCol.attackColTop = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackColBottom = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackColR = 0.0f;
	_stcAttackCol.attackDir = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackDelay = 0.0f;
    _stcAttackCol.attackDuration = 0.0f;
    _stcAttackCol.attackRecovery = 0.0f;
    _stcAttackCol.damage = 0.0f;
	_stcAttackCol.attackState = ATTACK_STATE::INACTIVE;
	_stcAttackCol.attackMoveSpeed = 0.0f;
    _stcAttackCol.isActive = false;
	_stcAttackCol.canKnockback = false;
	_stcAttackCol.isAttackCancelByHit = false;

	// 攻撃移動情報の初期化
	_stcAttackMovement.moveDir = VGet(0.0f, 0.0f, 0.0f);
	_stcAttackMovement.moveDistance = 0.0f;
	_stcAttackMovement.moveSpeed = 0.0f;
	_stcAttackMovement.decayRate = 0.0f;
	_stcAttackMovement.canMove = false;

	// 攻撃コリジョンオフセットの初期化
	_stcColOffset.directionScale = 1.0f;
	_stcColOffset.useOwnerDirection = true;

    // 攻撃移動停止用
    _bCanAttackMovement = true;

	// 向き調整の初期化
	_canDirAdjust = false;

	// 元の攻撃コリジョン位置の初期化
	_originalColTop = VGet(0.0f, 0.0f, 0.0f);
	_originalColBottom = VGet(0.0f, 0.0f, 0.0f);

	// エフェクトの更新用変数の初期化
	_effectHandle = -1; // エフェクトハンドル

	// カメラマネージャーの初期化
	_cameraManager = nullptr;
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

	// 攻撃コリジョンの位置更新
    UpdateAttackColPos();

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
        _fCurrentTime = 0.0f;                   // 経過時間リセット
		_stcAttackCol.isActive = false;         // 攻撃判定は非アクティブに設定
		_stcAttackCol.isHit = false;            // ヒットフラグリセット
        _bCanAttackMovement = true;             // 攻撃移動可能フラグをリセット

        return true;
    }

    return false;
}

// 攻撃停止処理
bool AttackBase::ProcessStopAttack()
{
	_eAttackState = ATTACK_STATE::INACTIVE; // 攻撃状態を非アクティブに設定
	_stcAttackCol.isActive = false;         // 攻撃判定を非アクティブに設定
    _stcAttackCol.isHit = false;            // ヒットフラグリセット
    _bCanAttackMovement = true;             // 攻撃移動可能フラグをリセット
    _fCurrentTime = 0.0f;                   // 経過時間リセット

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
            _fCurrentTime += 1.0f;  // 経過時間を進める

			// 発生遅延時間を超えたら攻撃判定をアクティブにする
            if(_fCurrentTime >= _stcAttackCol.attackDelay)
            {
                _eAttackState = ATTACK_STATE::ACTIVE;
                _stcAttackCol.isActive = true;
            }

            break;
        }

		case ATTACK_STATE::ACTIVE:  // 攻撃判定中
        {
            _fCurrentTime += 1.0f;  // 経過時間を進める

			// 持続時間を超えたら後隙状態に移行する
            if(_fCurrentTime >= _stcAttackCol.attackDelay + _stcAttackCol.attackDuration)
            {
                _eAttackState = ATTACK_STATE::RECOVERY;
                _stcAttackCol.isActive = false;
            }

            break;
        }

		case ATTACK_STATE::RECOVERY:    // 後隙
        {
            _fCurrentTime += 1.0f;  // 経過時間を進める

			// 後隙時間を超えたら攻撃状態を非アクティブにする
            if(_fCurrentTime >= _stcAttackCol.attackDelay + _stcAttackCol.attackDuration + _stcAttackCol.attackRecovery)
            {
                _eAttackState = ATTACK_STATE::INACTIVE;
                _fCurrentTime = 0.0f;

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

    // 攻撃移動フラグが
    if(!_bCanAttackMovement){ return; }

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

// 攻撃移動の停止
void AttackBase::StopAttackMovement()
{
    // 攻撃移動可能フラグを無効にする
    _bCanAttackMovement = false;
}

// 攻撃コリジョンの位置更新
void AttackBase::UpdateAttackColPos()
{
    auto owner = GetOwner();
    if(!owner){ return; }

	// 所有者の位置と向きを取得
    VECTOR ownerPos = owner->GetPos();
    VECTOR ownerDir = owner->GetDir();

    // 前方オフセット計算
    VECTOR dirNorm = VNorm(ownerDir);
    VECTOR forwardOffset = VScale(dirNorm, _stcColOffset.directionScale);
    VECTOR basePos = VAdd(ownerPos, forwardOffset);

	// コリジョンタイプに応じて攻撃コリジョンの位置を更新
    switch(_eColType)
    {
		case COLLISION_TYPE::CAPSULE: // カプセルの場合
        {
			// AttackConfigで設定された攻撃コリジョンの値で位置を計算
            _stcAttackCol.attackColTop = VAdd(basePos, _originalColTop);
            _stcAttackCol.attackColBottom = VAdd(basePos, _originalColBottom);

            break;
        }

        case COLLISION_TYPE::CIRCLE: // 円の場合
		case COLLISION_TYPE::SPHERE: // 球の場合
        {
            // 未使用

            break;
        }
    }
}

// 攻撃コリジョンの位置計算
VECTOR AttackBase::CalculateAttackColPos(const VECTOR& basePos, const VECTOR& offset, const VECTOR& direction)
{
    // 所有者の向きを基準としない場合
	if(!_stcColOffset.useOwnerDirection)
    {
		// 単純なオフセット計算
		return VAdd(basePos, offset);
    }
    // 所有者の向きを基準とする場合
    else
    {
		// 所有者の向きを考慮したオフセット計算
		VECTOR dirNorm = VNorm(direction);
        VECTOR rotatedOffset = VGet
        (
            offset.x * dirNorm.x - offset.z * dirNorm.z,    // X軸オフセットの回転
            offset.y,                                       // Y軸オフセットはそのまま
            offset.x * dirNorm.z + offset.z * dirNorm.x     // Z軸オフセットの回転
		);

		// 方向スケールを適用
        if(_stcColOffset.directionScale != 0.0f)
        {
			VECTOR dirScale = VScale(dirNorm, _stcColOffset.directionScale);
			rotatedOffset = VAdd(rotatedOffset, dirScale);
		}

		// 最終的な位置を計算して返す
		return VAdd(basePos, rotatedOffset);
    }
}

// 向き調整更新
void AttackBase::UpdateAttackDirAdjust()
{
    if(!_canDirAdjust){ return; }   // 向き調整が可能な場合のみ処理

	auto owner = GetOwner();
	if(!owner){ return; }

	// 入力マネージャーーから移動入力を取得
	auto& im = InputManager::GetInstance();
	AnalogState analog = im.GetAnalog();

    // 入力があるなら向き調整
    if(abs(analog.lx)>im.GetAnalogMin() || abs(analog.ly)>im.GetAnalogMin())
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
    // 所有者取得
    auto owner = GetOwner();
    if(!owner){ return VGet(0.0f, 0.0f, 0.0f); }

    // 入力取得
    float inputX = analog.lx;
    float inputY = analog.ly;

    // 入力が小さければゼロベクトルを返す
    auto& im = InputManager::GetInstance();
    if(fabsf(inputX) <= im.GetAnalogMin() && fabsf(inputY) <= im.GetAnalogMin())
    {
		// 入力がない場合はゼロベクトルを返す
        return VGet(0.0f, 0.0f, 0.0f);
    }

    // カメラの水平角度を取得
    float cameraAngleH = 0.0f;
    {
        if(_cameraManager)
        {
			// カメラの水平角度を取得
            cameraAngleH = _cameraManager->GetCurrentCameraAngleH();
        }
        else
        {
            // CameraManager がない場合は所有者の向きを使う
            VECTOR ownerDir = owner->GetDir();
            cameraAngleH = atan2f(ownerDir.z, ownerDir.x);
        }
    }

    // カメラ基準の前方と右方向ベクトル（XZ 平面）
    VECTOR cameraForward = VGet(sinf(cameraAngleH), 0.0f, cosf(cameraAngleH));
    VECTOR cameraRight = VGet(cosf(cameraAngleH), 0.0f, -sinf(cameraAngleH));

    // 入力をカメラ基準のワールド方向に変換
    // PlayerBase 側の移動変換と同様に前方向は -inputY を使う（上下スティックの向き合わせ）
    VECTOR inputDir = VAdd
    (
        VScale(cameraForward, -inputY), // 前後
        VScale(cameraRight, inputX)     // 左右
    );

    // 正規化して返す
    float len = VSize(inputDir);
    if(len > 0.0f)
    {
		// 入力方向を正規化して返す
        return VScale(inputDir, 1.0f / len);
    }

    return VGet(0.0f, 0.0f, 0.0f);
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
    ATTACK_STATE attackState,
    float attackMoveSpeed,
	bool canKnockback
)
{
	_originalColTop = top;          // 元のコリジョン上部位置を保存
	_originalColBottom = bottom;    // 元のコリジョン下部位置を保存

	// 攻撃コリジョン情報を設定
	_stcAttackCol.attackColTop = top;                   // カプセル上部
	_stcAttackCol.attackColBottom = bottom;             // カプセル下部
	_stcAttackCol.attackColR = radius;                  // 半径
    _stcAttackCol.attackDir = attackDir;                // 攻撃方向
	_stcAttackCol.attackDelay = delay;                  // 発生遅延
	_stcAttackCol.attackDuration = duration;            // 持続時間
	_stcAttackCol.attackRecovery = recovery;            // 後隙
	_stcAttackCol.damage = damage;                      // ダメージ
	_stcAttackCol.attackState = attackState;            // 攻撃状態
	_stcAttackCol.attackMoveSpeed = attackMoveSpeed;    // 攻撃中の移動速度
	_stcAttackCol.canKnockback = canKnockback;          // 吹き飛ばし攻撃かどうか

	_eColType = COLLISION_TYPE::CAPSULE;    // コリジョンタイプをカプセルに設定
}

// カプセル攻撃データ設定
void AttackBase::SetCapsuleAttackData(const AttackCollision& data)
{
    _originalColTop = data.attackColTop;          // 元のコリジョン上部位置を保存
    _originalColBottom = data.attackColBottom;    // 元のコリジョン下部位置を保存

    // 攻撃コリジョン情報をコピー
    _stcAttackCol = data;

    // コリジョンタイプをカプセルに設定
    _eColType = COLLISION_TYPE::CAPSULE;
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
	_stcAttackCol.attackRecovery = recovery;                                            // 後隙
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
	_stcAttackCol.attackRecovery = recovery;                // 後隙
	_stcAttackCol.damage = damage;                          // ダメージ
	_stcAttackCol.isHit = hit;                              // ヒットフラグ

	_eColType = COLLISION_TYPE::SPHERE; // コリジョンタイプを球に設定
}

// 攻撃コリジョンオフセット設定
void AttackBase::SetCollisionOffset(const AttackColOffset& offset)
{
    _stcColOffset = offset;  // コリジョンオフセット情報を保存
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
        _hitChars.push_back(chara);
    }
}

// 当たったキャラを持っているかチェック
bool AttackBase::HasHitCharas(std::shared_ptr<CharaBase> chara)const
{
    if(chara == nullptr) { return false; }

    // 当たったキャラリストを検索
    for(const auto& hitChara : _hitChars)
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
    _hitChars.clear();
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
                static_cast<int>(_stcAttackCol.attackColTop.x), // 中心X座標
                static_cast<int>(_stcAttackCol.attackColTop.y), // 中心Y座標
                static_cast<int>(_stcAttackCol.attackColR),     // 半径
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

