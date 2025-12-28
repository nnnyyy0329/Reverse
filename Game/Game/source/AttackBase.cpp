// 作成 : 成田

#include "AttackBase.h"

AttackBase::AttackBase()
{    
    // 攻撃コリジョンの初期化
    _stcAttackCol.attackColTop = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackColBottom = VGet(0.0f, 0.0f, 0.0f);
    _stcAttackCol.attackColR = 0.0f;
    _stcAttackCol.attackDelay = 0.0f;
    _stcAttackCol.attackDuration = 0.0f;
    _stcAttackCol.recovery = 0.0f;
    _stcAttackCol.damage = 0.0f;

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
    return true;
}

bool AttackBase::Render()
{
    // デバッグ表示
    DrawDebug();

    return true;
}

bool AttackBase::ProcessStartAttack()
{
    if(_eAttackState == ATTACK_STATE::INACTIVE)
    {
        _eAttackState = ATTACK_STATE::STARTUP;
        _stcAttackCol.currentTime = 0.0f;
        _stcAttackCol.isActive = false;
        return true;
    }

    return false;
}

bool AttackBase::ProcessStopAttack()
{
    _eAttackState = ATTACK_STATE::INACTIVE;
    _stcAttackCol.isActive = false;
    _stcAttackCol.currentTime = 0.0f;
    return true;
}

void AttackBase::UpdateAttackState()
{
    switch(_eAttackState)
    {
        case ATTACK_STATE::STARTUP:
        {
            _stcAttackCol.currentTime += 1.0f; // フレーム単位で管理
            
            if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay)
            {
                _eAttackState = ATTACK_STATE::ACTIVE;
                _stcAttackCol.isActive = true;
            }

            break;
        }

        case ATTACK_STATE::ACTIVE:
        {
            _stcAttackCol.currentTime += 1.0f;

            if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay + _stcAttackCol.attackDuration)
            {
                _eAttackState = ATTACK_STATE::RECOVERY;
                _stcAttackCol.isActive = false;
            }

            break;
        }

        case ATTACK_STATE::RECOVERY:
        {
            _stcAttackCol.currentTime += 1.0f;

            if(_stcAttackCol.currentTime >= _stcAttackCol.attackDelay + _stcAttackCol.attackDuration + _stcAttackCol.recovery)
            {
                _eAttackState = ATTACK_STATE::INACTIVE;
                _stcAttackCol.currentTime = 0.0f;
            }

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
    float delay,
    float duration,
    float recovery,
    float damage
)
{
    _stcAttackCol.attackColTop = top;
    _stcAttackCol.attackColBottom = bottom;
    _stcAttackCol.attackColR = radius;
    _stcAttackCol.attackDelay = delay;
    _stcAttackCol.attackDuration = duration;
    _stcAttackCol.recovery = recovery;
    _stcAttackCol.damage = damage;

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
    float damage
)
{
    // 高さを考慮した位置を設定
    _stcAttackCol.attackColTop = VGet(center.x, center.y + height * 0.5f, center.z);
    _stcAttackCol.attackColBottom = VGet(center.x, center.y - height * 0.5f, center.z);
    _stcAttackCol.attackColR = radius;
    _stcAttackCol.attackDelay = delay;
    _stcAttackCol.attackDuration = duration;
    _stcAttackCol.recovery = recovery;
    _stcAttackCol.damage = damage;

    _eColType = COLLISION_TYPE::CIRCLE;
}

// 球攻撃データ設定
void AttackBase::SetSphereAttackData
(
    VECTOR center,
    float radius,
    float delay,
    float duration,
    float recovery,
    float damage
)
{
    _stcAttackCol.attackColTop = center;                    // 中心点
    _stcAttackCol.attackColBottom = VGet(0.0f, 0.0f, 0.0f); // 使用しない
    _stcAttackCol.attackColR = radius;
    _stcAttackCol.attackDelay = delay;
    _stcAttackCol.attackDuration = duration;
    _stcAttackCol.recovery = recovery;
    _stcAttackCol.damage = damage;

    _eColType = COLLISION_TYPE::SPHERE;
}

void AttackBase::DrawDebug()
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
                    _stcAttackCol.attackColTop,
                    _stcAttackCol.attackColBottom,
                    _stcAttackCol.attackColR,
                    8,
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
                    (float)_stcAttackCol.attackColTop.x,
                    (float)_stcAttackCol.attackColTop.y,
                    _stcAttackCol.attackColR,
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
                    _stcAttackCol.attackColTop,
                    _stcAttackCol.attackColR,
                    8,
                    GetColor(0, 0, 255),
                    GetColor(0, 0, 255),
                    TRUE
                );
                break;
            }
        }
    }
}