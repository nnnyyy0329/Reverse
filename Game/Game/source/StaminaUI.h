/*****************************************
* file   StaminaUI.h
* brief  �X�^�~�iUI�N���X
* author ���c �I�^
* date   2026/02/06
******************************************/

#pragma once
#include "appframe.h"

// �X�^�~�iUI�\���ݒ�p�萔
namespace StaminaRenderConfig
{
	// �摜�\���p
	const int DRAW_BAR_FRAME_X	= 216;
	const int DRAW_BAR_FRAME_Y	= 1000;
	const int DRAW_OFFSET_X		= 15;
	const int DRAW_OFFSET_Y		= 5;
}

/// @brief �X�^�~�iUI�N���X
class StaminaUI
{
public:

	StaminaUI();
	virtual ~StaminaUI();


	/* ��{�֐� */

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();


	/* �X�^�~�iUI�\���֌W�֐� */

	/// @brief �X�^�~�i�t���[���\���֐�
	void StaminaFrameRender();			

	/// @brief �Q�[�W�\���䗦�v�Z�֐�
	void GaugeRatioCalculation();			
	
	/// @brief �X�^�~�i�Q�[�W�\���֐�
	///
	/// @param ratio �Q�[�W�\���䗦
	void StaminaGaugeRender(float ratio);	

protected:

	int _iStaminaFrameHandle;	// �X�^�~�i�t���[���摜�n���h��
	int _iStaminaHandle;// �X�^�~�i�Q�[�W�摜�n���h��

	int _drawStaminaBarX;		// �X�^�~�i�o�[�̕`��ʒuX
	int _drawStaminaBarY;		// �X�^�~�i�o�[�̕`��ʒuY
	int _drawStaminaBarFrameX;	// �X�^�~�i�o�[�t���[���̕`��ʒuX
	int _drawStaminaBarFrameY;	// �X�^�~�i�o�[�t���[���̕`��ʒuY
};

