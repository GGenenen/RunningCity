//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author :  GP11A132 21	�q�Z�C�V��
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "sound.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH_BAT				(100)	// �o�b�g�L�����T�C�Y
#define TEXTURE_HEIGHT_BAT				(80)	// 

#define TEXTURE_WIDTH_KAME				(80)	// �T�L�����T�C�Y
#define TEXTURE_HEIGHT_KAME				(64)	// 

#define TEXTURE_WIDTH_OBS				(90)	//OBS�L�����T�C�Y
#define TEXTURE_HEIGHT_OBS				(280)	// 



#define TEXTURE_MAX					(4)		// �e�N�X�`���̐�

#define TEXTURE_BAT_PATTERN_DIVIDE_X	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_BAT_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_BAT_PATTERN_DIVIDE_X*TEXTURE_BAT_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enebat.PNG",		//0:bat
	"data/TEXTURE/enekame.PNG",		//1:kame
	"data/TEXTURE/ene01.PNG",		//2:bashira1
	"data/TEXTURE/ene02.PNG",		//3:bashira2

};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];		// �G�l�~�[�\����

static int		g_EnemyCnt = 0;

int type;
int time;
int cntenemy;

static INTERPOLATION_DATA g_MoveTbl0[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(0.0f,  0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),	120 },
	{ XMFLOAT3(100.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f,0.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f),	60 },
	{ XMFLOAT3(250.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
	{ XMFLOAT3(50.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	120 },
	{ XMFLOAT3(50.0f, 50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },

};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(1700.0f,   0.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(1700.0f,  SCREEN_HEIGHT, 0.0f),XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//���W									��]��							�g�嗦							����
	{ XMFLOAT3(3000.0f, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};




//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	time = 0;
	cntenemy = 0;

	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].hitted = FALSE;
		

		switch (i)
		{
		case ENEMY_TYPE_BAT:
			g_Enemy[i].use = FALSE;
			g_Enemy[i].pos = XMFLOAT3(SCREEN_WIDTH, 200.0f, 0.0f);	// ���S�_����\��
			g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Enemy[i].w = TEXTURE_WIDTH_BAT;
			g_Enemy[i].h = TEXTURE_HEIGHT_BAT;
			g_Enemy[i].texNo = 0;
			g_Enemy[i].hitted = FALSE;
			g_Enemy[i].type = 0;
			g_Enemy[i].countAnim = 0;
			g_Enemy[i].patternAnim = 0;

			g_Enemy[i].move = XMFLOAT3(10.0f, 0.0f, 0.0f);		// �ړ���

			g_Enemy[i].left = 1;			//���Ɉړ�

			g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

			//g_EnemyCnt++;
			break;

		case ENEMY_TYPE_KAME:
			g_Enemy[i].use = FALSE;
			g_Enemy[i].pos = XMFLOAT3(SCREEN_WIDTH, 330.0f, 0.0f);	// ���S�_����\��
			g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Enemy[i].w = TEXTURE_WIDTH_KAME;
			g_Enemy[i].h = TEXTURE_HEIGHT_KAME;
			g_Enemy[i].texNo = 1;
			g_Enemy[i].type = 1;

			g_Enemy[i].move = XMFLOAT3(8.0f, 0.0f, 0.0f);		// �ړ���

			g_Enemy[i].left = 1;			//���Ɉړ�
			g_Enemy[i].hitted = FALSE;
			break;

		case ENEMY_TYPE_OBS:
			g_Enemy[i].use = FALSE;
			g_Enemy[i].pos = XMFLOAT3(SCREEN_WIDTH, 140.0f, 0.0f);	// ���S�_����\��
			g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Enemy[i].w = TEXTURE_WIDTH_OBS;
			g_Enemy[i].h = TEXTURE_HEIGHT_OBS;
			g_Enemy[i].texNo = (rand() % 2) + 2;
			g_Enemy[i].type = 2;

			g_Enemy[i].move = XMFLOAT3(8.0f, 0.0f, 0.0f);		// �ړ���

			g_Enemy[i].left = 1;			//���Ɉړ�
			g_Enemy[i].hitted = FALSE;
			break;
		}
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// �����Ă�G�l�~�[�̐�

	time++;


	if (time == 120)
	{

		cntenemy = rand() % 3;
		g_Enemy[cntenemy].use = TRUE;
		g_Enemy[cntenemy].hitted = FALSE;

		switch (g_Enemy[cntenemy].type)
		{
		case ENEMY_TYPE_BAT:
			g_Enemy[cntenemy].pos = XMFLOAT3(SCREEN_WIDTH, 200.0f, 0.0f);	// ���S�_����\��
			break;
		case ENEMY_TYPE_KAME:
			g_Enemy[cntenemy].pos = XMFLOAT3(SCREEN_WIDTH, 330.0f, 0.0f);	// ���S�_����\��
			break;
		case ENEMY_TYPE_OBS:
			g_Enemy[cntenemy].texNo = (rand() % 2) + 2;
			g_Enemy[cntenemy].pos = XMFLOAT3(SCREEN_WIDTH, 140.0f, 0.0f);	// ���S�_����\��
			break;

		}

		time = 0;
	}


	for (int i = 0; i < ENEMY_MAX; i++)
	{
		


			// �����Ă�G�l�~�[��������������
			if (g_Enemy[i].use == TRUE)
			{
				// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
				XMFLOAT3 pos_old = g_Enemy[i].pos;

				// �A�j���[�V����  
				g_Enemy[i].countAnim += 1.0f;
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				else
				{
					float speed = g_Enemy[i].move.x;	//�t���O���g���������ړ�
					{
						if (g_Enemy[i].left == 0)
						{
							g_Enemy[i].pos.x += speed;
						}
						else
						{
							g_Enemy[i].pos.x -= speed;
						}
					}

				}

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				{
					PLAYER* player = GetPlayer();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (player[0].use == TRUE && g_Enemy[j].hitted == FALSE)
						{


							if (g_Enemy[j].use == TRUE) {

								XMFLOAT3 pos = player[0].pos;

								if (player[0].jump==true)
								{
									pos.y += player[0].jumpY;
								}else if (player[0].down==true)
								{
									pos.y -= player[0].downY;
								}
								BOOL ans = CollisionBB(g_Enemy[j].pos, g_Enemy[j].w / 2, g_Enemy[j].h / 2,
									pos, player[0].w / 2, player[0].h / 2);

								// �������Ă���H
								if (ans == TRUE)
								{

									if (player[0].hpkeep == FALSE) {

										switch (g_Enemy[j].type) {
											
										case ENEMY_TYPE_BAT:
											AddScore(-100);
											player[0].hp -= 1;
											PlaySound(SOUND_LABEL_SE_hit);
											break;

										case ENEMY_TYPE_KAME:
											AddScore(-50);
											player[0].hp -= 1;
											PlaySound(SOUND_LABEL_SE_hit);
											break;
										case ENEMY_TYPE_OBS:
											AddScore(-30);
											player[0].hp -= 1;
											PlaySound(SOUND_LABEL_SE_hit);
											break;
										}
										g_Enemy[j].use = FALSE;
										// //�����������̏���
										//player[0].hp -= 1;	// �f�o�b�O�ňꎞ�I�ɖ��G�ɂ��Ă�����
										//PlaySound(SOUND_LABEL_SE_bomb000);
									}

									g_Enemy[j].hitted = TRUE;
								}
							}


						}
					}
				}

				g_EnemyCnt++;		// �����Ă��G�̐�

			}



		
	}


	//// �G�l�~�[�S�Ń`�F�b�N
	//if (g_EnemyCnt <= 0)
	//{
	//	SetFade(FADE_OUT, MODE_RESULT);
	//}

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================

void DrawEnemy(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();



	for (int i = 0; i < ENEMY_MAX; i++)
	{


		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Enemy[i].w;		// �G�l�~�[�̕\����
			float ph = g_Enemy[i].h;		// �G�l�~�[�̕\������

			//// �A�j���[�V�����p
			//float tw = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_BAT_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			//float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_BAT_PATTERN_DIVIDE_Y) * th;	// �e�N�X�`���̍���Y���W

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			switch (g_Enemy[i].type)
			{
			case 0:
				// �A�j���[�V�����p
				tw = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				th = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				tx = (float)(g_Enemy[i].patternAnim % TEXTURE_BAT_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				ty = (float)(g_Enemy[i].patternAnim / TEXTURE_BAT_PATTERN_DIVIDE_Y) * th;	// �e�N�X�`���̍���Y���W


				break;
			case 1:
				tw = 1.0f;	// �e�N�X�`���̕�
				th = 1.0f;	// �e�N�X�`���̍���
				tx = 0.0f;	// �e�N�X�`���̍���X���W
				ty = 0.0f;	// �e�N�X�`���̍���Y���W
				break;

			case 2:
				tw = 1.0f;	// �e�N�X�`���̕�
				th = 1.0f;	// �e�N�X�`���̍���
				tx = 0.0f;	// �e�N�X�`���̍���X���W
				ty = 0.0f;	// �e�N�X�`���̍���Y���W
				break;
			}

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}



}



//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}





