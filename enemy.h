//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : GP11A132 21	�q�Z�C�V��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_MAX		(100)		// �G�l�~�[��Max�l��

typedef enum {
	ENEMY_TYPE_BAT,			//0:�o�b�g
	ENEMY_TYPE_KAME,		//1:�T
	ENEMY_TYPE_OBS,			//2:OBS

	ENEMY_TYPE_MAX,

} enemy_type;

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY
{

	int			type;			//�G�l�~�[�̎��(0:�o�b�g,1:�T,2:OBS)

	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	move;			// �ړ����x


	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;			// ���̃e�[�u���̃f�[�^��

	int			left;			//1.���ֈړ��@0.�E�ֈړ�
	BOOL		hitted;			//�������Ă����H�@T�F���������AF�F�܂�


	//INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	//int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	//float				move_time;			// ���s����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);



