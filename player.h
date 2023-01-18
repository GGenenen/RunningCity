//=============================================================================
//
// �v���C���[���� [player.h]
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
#define PLAYER_MAX			(1)		// �v���C���[��Max�l��

#define	PLAYER_OFFSET_CNT	(12)	// 16���g

enum
{
	CHAR_DIR_GO1,
	CHAR_DIR_GO2,
	CHAR_DIR_UP,
	CHAR_DIR_DOWN,

	CHAR_DIR_MAX
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�

	int			dir;			// �����i0/1:go 2:up 3:dowm �j
	BOOL		moving;			// �ړ����t���O

	BOOL		dash;			// �_�b�V�����t���O
	XMFLOAT3	move;			// �ړ����x
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// �c���|���S���̍��W

	BOOL		jump;			// �W�����v�t���O
	float		jumpY;			// �W�����v�̍���
	int			jumpCnt;		// �W�����v���̃J�E���g

	BOOL		down;			// �_�E���t���O
	float		downY;			// �_�E���̍���
	int			downCnt;		// �_�E�����̃J�E���g

	BOOL		hitted;			//�������Ă����H�@T�F���������AF�F�܂�
	BOOL		hpkeep;			//���G�t���O 
	int			hp;				//�v���C���[��HP
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);



