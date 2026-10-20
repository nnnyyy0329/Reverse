#include "ShieldBase.h"
#include"CharaBase.h"
#include "StaminaManager.h"

namespace
{
	constexpr float DECREMENT_COUNT = 1.0f;
	constexpr float SHIELD_COST_STAMINA = 0.5f;
}

ShieldBase::ShieldBase()
{
	_eShieldChara = SHIELD_CHARA::NONE;
	_eShieldState = SHIELD_STATE::INACTIVE;

	_fStartupTimer = 0.0f;				// �����O�^�C�}�[
	_fRecoveryTimer = 0.0f;				// �d���^�C�}�[
	_bIsBlocking = false;				// �u���b�N���t��
	_stcShieldConfig = ShieldConfig();	// �V�[���h�ݒ菉����
}

ShieldBase::~ShieldBase()
{

}

bool ShieldBase::Initialize()
{
	return true;
}

bool ShieldBase::Terminate()
{
	return true;
}

bool ShieldBase::Process()
{
	// �V�[���h��ԍX�V
	UpdateShieldState();

	return true;
}

bool ShieldBase::Render()
{
	return true;
}

void ShieldBase::ActivateShield()
{
	// ��A�N�e�B�u��ԂȂ�
	if(_eShieldState == SHIELD_STATE::INACTIVE)
	{
		// �A�N�e�B�u�Ɉڍs
		_eShieldState = SHIELD_STATE::STARTUP;			// �V�[���h��Ԃ𔭐��O�ɐݒ�
		_fStartupTimer = _stcShieldConfig.startTime;	// �����O�^�C�}�[�Z�b�g
	}
}

void ShieldBase::DeactivateShield()
{
	// �A�N�e�B�u���Ȃ�
	if(_eShieldState == SHIELD_STATE::ACTIVE)
	{
		_eShieldState = SHIELD_STATE::RECOVERY;				// �V�[���h��Ԃ��A�N�e�B�u�ɐݒ�
		_fRecoveryTimer = _stcShieldConfig.recoveryTime;	// �d���^�C�}�[�Z�b�g
	}
	// �����O��ԂȂ�
	else if(_eShieldState == SHIELD_STATE::STARTUP)
	{
		_eShieldState = SHIELD_STATE::INACTIVE;				// �V�[���h��Ԃ�d���ɐݒ�
		_fStartupTimer = 0.0f;								// �����O�^�C�}�[���Z�b�g
	}
}

bool ShieldBase::CanBlockAttack(const VECTOR& attackPos, const VECTOR& attackDir) const
{
	// �V�[���h���A�N�e�B�u�łȂ��ꍇ�̓u���b�N�s��
	if(!IsActive()) { return false; }

	// ���L�҂����Ȃ��ꍇ�̓u���b�N�s��
	auto owner = GetOwner();
	if(!owner) { return false; } 

	// ���L�҈ʒu�ƑO���x�N�g���擾
	VECTOR ownerPos = GetOwnerPos();
	VECTOR ownerDir = GetOwnerDir();

	// �U���ʒu���V�[���h�L���͈͓���`�F�b�N
	if(!IsInShieldRange(attackPos)) { return false; }

	// �U�����L���ȕ������炩�`�F�b�N
	if(!IsAttackFromValidDirection(attackDir)) { return false; }

	// ���ׂĂ̏���𖞂����Ă���ꍇ�̓u���b�N�\
	return GeometryUtility::IsAttackFromFront(ownerPos, ownerDir, attackDir, _stcShieldConfig.dotThreshold);
}

bool ShieldBase::IsInShieldRange(const VECTOR& targetPos) const
{
	// ���L�҂����Ȃ��ꍇ�̓u���b�N�s��
	auto owner = GetOwner();
	if(!owner) { return false; } 

	// ���L�҈ʒu�擾
	VECTOR ownerPos = GetOwnerPos();

	// ���L�҂���Ώۂւ̋�����v�Z
	float distance = VSize(VSub(targetPos, ownerPos));

	// �V�[���h�L����������`�F�b�N
	return distance <= _stcShieldConfig.blockDistance;
}

bool ShieldBase::IsAttackFromValidDirection(const VECTOR& attackDir) const
{
	// ���L�҂����Ȃ��ꍇ�̓u���b�N�s��
	auto owner = GetOwner();
	if(!owner) { return false; } 

	// ���L�҈ʒu�ƌ���
	VECTOR ownerPos = GetOwnerPos();
	VECTOR ownerDir = GetOwnerDir();

	// �U���҂��珊�L�҂ւ̕���
	VECTOR attackToOwner = VScale(attackDir, -1.0f);

	// ���L�ґO���ƍU�������̊p�x����v�Z
	float attackDiff = GeometryUtility::GetAngleDeg(ownerDir, attackToOwner);

	// �p�x�͈͓���`�F�b�N
	return attackDiff <= _stcShieldConfig.blockAngleRange;
}

void ShieldBase::SuccessBlock(const VECTOR& attackDir)
{
	_bIsBlocking = true;	// �u���b�N��
}

void ShieldBase::UpdateGuardInput(InputManager* input)
{
	// �����ꂽ��
	if(input->IsHold(INPUT_ACTION::ABILITY))
	{
		// �K�[�h�J�n
		StartGuard();
	}
	// �����ꂽ��
	else
	{
		// �K�[�h�I��
		StopGuard();
	}
}

void ShieldBase::StartGuard()
{
	if(!HasStamina()) { return; }	// �X�^�~�i�`�F�b�N

	// �V�[���h����
	ActivateShield();
}

void ShieldBase::StopGuard()
{
	// �V�[���h���
	DeactivateShield();
}

bool ShieldBase::HasStamina() const
{
	return StaminaManager::GetInstance()->CanShieldByCost();
}

void ShieldBase::UpdateShieldState()
{
	switch(_eShieldState)
	{
		case SHIELD_STATE::STARTUP: // �����O
		{
			_fStartupTimer -= DECREMENT_COUNT;	// �d���^�C�}�[��i�߂�

			if(_fStartupTimer <= 0.0f)
			{
				_eShieldState = SHIELD_STATE::ACTIVE;		// �V�[���h��Ԃ�A�N�e�B�u�ɐݒ�
			}

			break;
		}

		case SHIELD_STATE::ACTIVE: // �A�N�e�B�u��
		{
			// �X�^�~�i����
			StaminaManager::GetInstance()->ConsumeStamina(SHIELD_COST_STAMINA);

			break;
		}

		case SHIELD_STATE::RECOVERY: // �d����
		{
			_fRecoveryTimer -= DECREMENT_COUNT;	// �d���^�C�}�[��i�߂�

			// �d�����Ԃ𒴂�����
			if(_fRecoveryTimer <= 0.0f)
			{
				_eShieldState = SHIELD_STATE::INACTIVE;	// �V�[���h��Ԃ��A�N�e�B�u�ɐݒ�
				_bIsBlocking = false;					// �u���b�N���t���O���
			}

			break;
		}
	}
}

VECTOR ShieldBase::GetOwnerPos() const
{
	auto owner = GetOwner();
	if(owner)
	{
		// ���L�҈ʒu�擾
		return owner->GetPos();
	}

	// ���L�҂����Ȃ��ꍇ�̓[��
	return VGet(0.0f, 0.0f, 0.0f);
}

VECTOR ShieldBase::GetOwnerDir() const
{
	auto owner = GetOwner();
	if(owner)
	{
		// ���L�ґO���擾
		return owner->GetDir();
	}
	
	// ���L�҂����Ȃ��ꍇ�̓[��
	return VGet(0.0f, 0.0f, 0.0f);
}

void ShieldBase::DebugRender()
{
	// �V�[���h�L���͈̓f�o�b�O�`��
	DrawShieldRangeDebug();
}

void ShieldBase::DrawShieldRangeDebug()
{
	if(_eShieldState != SHIELD_STATE::ACTIVE){ return; }

	auto owner = GetOwner();
	if(!owner) { return; } 

	// ���L�҈ʒu�ƑO���x�N�g���擾
	VECTOR ownerPos = GetOwnerPos();
	VECTOR ownerDir = GetOwnerDir();

	// �V�[���h�L���͈͕`��

}


