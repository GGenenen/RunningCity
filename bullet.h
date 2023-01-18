//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author :  GP11A132 21	�q�Z�C�V��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_MAX		(999)		// �o���b�g��Max��
#define BULLET_SPEED	(10.0f)		// �o���b�g�̈ړ��X�s�[�h


enum {

	BULLET_TYPE_BULLET,
	BULLET_TYPE_BOOM,

	BULLET_TYPE_MAX,

};



// �o���b�g�\����
struct BULLET
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���

	int					type;				// 0:BULLET, 1:BOOM

	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
void SetBullet(XMFLOAT3 pos);


