// �S�� : ���c

#include "PlayerBase.h"
#include "StaminaManager.h"

namespace
{
	constexpr float DODGE_STAMINA_COST = 10.0f;	// �����̃X�^�~�i�����
}

// ����񏉊���
void PlayerBase::InitializeDodgeData()
{
	if(_dodgeSystem == nullptr){ return; }

	// �q�N���X��������̎擾
	DodgeConfig playerDodgeConfig = GetDodgeConfig();

	// �L�����^�C�v�ɉ��������L�����^�C�v�����
	DODGE_CHARA dodgeCharaType = DODGE_CHARA::NONE;

	// �L�����^�C�v�ɉ��������L�����^�C�v�����
	switch(GetCharaType())
	{
		case CHARA_TYPE::SURFACE_PLAYER: // �\�v���C���[
		{
			dodgeCharaType = DODGE_CHARA::SURFACE_PLAYER;
			break;
		}

		case CHARA_TYPE::INTERIOR_PLAYER: // ���v���C���[
		{
			dodgeCharaType = DODGE_CHARA::INTERIOR_PLAYER;
			break;
		}

		case CHARA_TYPE::BULLET_PLAYER: // �e�v���C���[
		{
			dodgeCharaType = DODGE_CHARA::BULLET_PLAYER;
			break;
		}

		default:
			return;
	}

	// �v���C���[�̉��ݒ��o�^
	_dodgeSystem->RegisterDodgeCharaConfig(dodgeCharaType, playerDodgeConfig);
}

// ���Ăяo��
void PlayerBase::CallProcessDodge()
{
	if(_dodgeSystem == nullptr) { return; }	// ���V�X�e�����Ȃ���Ώ������Ȃ�

	// ������
	ProcessDodge();

	// ���I���`�F�b�N
	ProcessEndDodge();
}

// ������
void PlayerBase::ProcessDodge()
{
	// �����͂������������Ăяo��
	if(CanStartDodge())
	{
		// ���J�n
		ProcessStartDodge();

		// ���J�n���ɃX�^�~�i�����
		//StaminaManager::GetInstance()->ConsumeStamina(DODGE_STAMINA_COST);
	}
}

// ���J�n����
void PlayerBase::ProcessStartDodge()
{
	// ���\���`�F�b�N
	if(!CanDodge()){ return; }	// ���s�\�Ȃ珈�����Ȃ�

	// �L�����^�C�v�ɉ��������L�����^�C�v�����
	DODGE_CHARA dodgeCharaType = DODGE_CHARA::NONE;

	// �L�����^�C�v�ɉ��������L�����^�C�v�����
	switch(GetCharaType())
	{
		case CHARA_TYPE::SURFACE_PLAYER: // �\�v���C���[
		{
			dodgeCharaType = DODGE_CHARA::SURFACE_PLAYER;
			break;
		}

		case CHARA_TYPE::INTERIOR_PLAYER: // ���v���C���[
		{
			dodgeCharaType = DODGE_CHARA::INTERIOR_PLAYER;
			break;
		}

		case CHARA_TYPE::BULLET_PLAYER: // �e�v���C���[
		{
			dodgeCharaType = DODGE_CHARA::BULLET_PLAYER;
			break;
		}

		default:
			return; 
	}

	// ���Ăяo��
	_dodgeSystem->CallDodge(shared_from_this(), dodgeCharaType);
	_playerState.combatState = PLAYER_COMBAT_STATE::DODGE;
}

// ���I������
void PlayerBase::ProcessEndDodge()
{
	if(_playerState.combatState != PLAYER_COMBAT_STATE::DODGE){ return; }	// ��𒆂łȂ���Ώ������Ȃ�

	// �����I�������ꍇ
	if(!_dodgeSystem->IsDodging() && _dodgeSystem->GetDodgeState() == DODGE_STATE::INACTIVE)
	{
		// �Â��X�e�[�^�X��ێ�
		PlayerState oldStatus = _playerState;

		// �X�e�[�^�X���Z�b�g
		_playerState.StateReset();

		// �ҋ@��Ԃɖ߂�
		_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;	

		_oldPlayerState = oldStatus;	// �Â��X�e�[�^�X��U����Ԃɐݒ�
		ProcessPlayAnimation();			// �A�j���[�V�����؂�ւ����s
		_oldPlayerState = _playerState;	// �؂�ւ���ɍX�V
	}
}

// ����J�n�ł��邩�`�F�b�N
bool PlayerBase::CanStartDodge()
{
	auto& im = InputManager::GetInstance();

	if((_playerState.IsStateMoving())							&&	// �ړ���Ԃ�
		_playerState.IsInAttackState(PLAYER_ATTACK_STATE::NONE)	&&	// ��������̍U����Ԃł͂Ȃ�
		_playerState.IsInShootState(PLAYER_SHOOT_STATE::NONE)	&&	// ��������̔��ˏ�Ԃł͂Ȃ�
		!_playerState.IsStateCombat()							&&	// �����Ԓ��ł͂Ȃ�
		StaminaManager::GetInstance()->CanDodgeByCost()			&&	// �X�^�~�i�����\�ȗʂ�
		im.IsTrigger(INPUT_ACTION::DODGE))							// �����͂���������
	{
		return true;
	}

	// ���J�n�s�\
	return false;
}

// ���\���`�F�b�N
bool PlayerBase::CanDodge()
{
	if(_dodgeSystem != nullptr										||	// ���V�X�e�����L����
		_dodgeSystem->CanDodge()									||	// ��𒆂łȂ�
		!IsAttacking()												||	// �U�����łȂ�
		!_playerState.IsInCombatState(PLAYER_COMBAT_STATE::HIT)		||	// ��e���łȂ�
		!_playerState.IsInCombatState(PLAYER_COMBAT_STATE::DEATH))		// ���S������Ȃ��Ȃ�
	{
		return true;
	}

	// ���s�\
	return false;
}

// ��𒆂��`�F�b�N
bool PlayerBase::IsDodging()
{
	if(_dodgeSystem == nullptr) { return false; }

	// ��𒆂��`�F�b�N
	if(_playerState.IsInCombatState(PLAYER_COMBAT_STATE::DODGE)	||	// ��𒆂�
		_dodgeSystem->IsDodging())									// ������łɂ��Ă���
	{
		return true;
	}

	return false;
}

// ���G��Ԃ��`�F�b�N
bool PlayerBase::IsInvincible()
{
	if(_dodgeSystem == nullptr) { return false; }

	// ���G��Ԃ��`�F�b�N
	return _dodgeSystem->GetIsInvincible();
}