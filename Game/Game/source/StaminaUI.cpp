#include "StaminaUI.h"
#include "StaminaManager.h"

// �X�^�~�iUI�\���ݒ�p�萔�̃G�C���A�X
namespace SRC = StaminaRenderConfig;

StaminaUI::StaminaUI()
{
	_iStaminaFrameHandle = ResourceServer::GetInstance()->GetHandle("StaminaFrame");
	_iStaminaHandle = ResourceServer::GetInstance()->GetHandle("Stamina");

	_drawStaminaBarX = SRC::DRAW_BAR_FRAME_X + SRC::DRAW_OFFSET_X;
	_drawStaminaBarY = SRC::DRAW_BAR_FRAME_Y + SRC::DRAW_OFFSET_Y;
	_drawStaminaBarFrameX = SRC::DRAW_BAR_FRAME_X;
	_drawStaminaBarFrameY = SRC::DRAW_BAR_FRAME_Y;
}

StaminaUI::~StaminaUI()
{
	// �f�X�g���N�^
	Terminate();
}

bool StaminaUI::Initialize()
{
	return true;
}

bool StaminaUI::Terminate()
{
	// �摜�̊J��
	DeleteGraph(_iStaminaFrameHandle);
	DeleteGraph(_iStaminaHandle);

	return true;
}

bool StaminaUI::Process()
{
	return true;
}

bool StaminaUI::Render()
{
	// Z�o�b�t�@������
	SetUseZBuffer3D(FALSE);

	// �X�^�~�i�t���[���\���֐�
	//StaminaFrameRender();

	// �X�^�~�i�Q�[�W�\���֐�
	//GaugeRatioCalculation();

	// Z�o�b�t�@�L����
	SetUseZBuffer3D(TRUE);

	return true;
}

void StaminaUI::StaminaFrameRender()
{
	DrawGraph(_drawStaminaBarFrameX, _drawStaminaBarFrameY, _iStaminaFrameHandle, TRUE);
}

void StaminaUI::GaugeRatioCalculation()
{
	// �X�^�~�i�}�l�[�W���[���猻�݂̃X�^�~�i�ʂ�擾
	float currentStamina = StaminaManager::GetInstance()->GetCurrentStamina();
	float maxStamina = StaminaManager::GetInstance()->GetMaxStamina();

	// �ő�X�^�~�i��0�ȉ��̏ꍇ�͕`�悵�Ȃ�
	if(maxStamina <= 0.0f) return;  

	// �X�^�~�i�̔䗦��v�Z
	float staminaRatio = currentStamina / maxStamina;
	if(staminaRatio > 1.0f){ staminaRatio = 1.0f; }
	if(staminaRatio < 0.0f){ staminaRatio = 0.0f; }

	// �X�^�~�i�Q�[�W�\���֐�
	StaminaGaugeRender(staminaRatio);
}

void StaminaUI::StaminaGaugeRender(float ratio)
{
	// �䗦��0�ȉ��̏ꍇ�͕`�悵�Ȃ�
	if(ratio <= 0.0f){ return; }	

	// �摜�T�C�Y
	int graphW, graphH;
	GetGraphSize(_iStaminaHandle, &graphW, &graphH);

	// �`�悷�镝
	int clipW = static_cast<int>(graphW * ratio);

	// �N���b�s���O�̈��ݒ�
	SetDrawArea(_drawStaminaBarX, _drawStaminaBarY, _drawStaminaBarX + clipW, _drawStaminaBarY + graphH);

	// �X�^�~�i�Q�[�W�`��
	DrawGraph(_drawStaminaBarX, _drawStaminaBarY, _iStaminaHandle, TRUE);

	// �N���b�s���O�̈����ɖ߂�
	SetDrawAreaFull();
}
