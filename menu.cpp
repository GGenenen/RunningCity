/*******************************************************************************
* �^�C�g��:		Shop�v���O����
* �v���O������:	Shop.cpp
* �쐬��:		GP11A132 99 �O������
* �쐬��:		2022/05/26
*******************************************************************************/

/*******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "main.h"
#include "Shop.h"
#include "field.h"
#include "player.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/


/*******************************************************************************
* �\���̒�`
*******************************************************************************/


/*******************************************************************************
* �v���g�^�C�v�錾(����Ȋ֐���p�ӂ��Ă���܂���錾/�֐��̐�����)
*******************************************************************************/



/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
SHOP g_Shop[SHOP_MAX];	// Shop�^�Ńv���C���[�P�̕�


int sentakushi = 0;


char doubutsu[SHOP_MAX][80] = {
	"�l�R                                                                    \n",
	"�C�k                                                                    \n",
	"�y���M��                                                                \n",
	"���C�I��                                                                \n",
	"�p���_                                                                  \n",
	"�C���J                                                                  \n",
	"�S����                                                                  \n",
	"�L����                                                                  \n",
	"�j���g��                                                                \n",
	"�g��                                                                    \n",
	"EXIT SHOP                                                               \n",
};


// �v���C���[�̏���������
void InitShop(void)
{
	// �\���̂̏�����
}


// �v���C���[�̏I������
void UninitShop(void)
{

}


void ShopTransaction(void)
{
	system("cls");
	printf("You have bought %s\n", doubutsu[sentakushi]);
	DeductCoin(100);
	printf("You have %d coins now.\n", CurrentBalance());
	rewind(stdin);
	(void)_getch();
}


// �v���C���[�̍X�V����
void UpdateShop(void)
{

	// �L�[���͏���
	int key = _getch();
	if ((key == 0) || (key == 224))	// ����Key�Ȃ�����P�x�擾����
	{
		key = _getch();
	}

	// �����ꂽkey�̕����ֈړ�������
	switch (key)
	{
	case 'w':
	case 0x48:	// ��
		if (sentakushi > 0)
		{
			sentakushi--;
		}
		break;
	case 's':
	case 0x50:	// ��
		if (sentakushi < (SHOP_MAX - 1))
		{
			sentakushi++;
		}
		break;
	case 13:	// enter key
		if (sentakushi == 10) // exit game option
		{
			// change game mode
			SetMode(GAME_FIELD);
			// reset sentakushi back to 0
			sentakushi = 0;
		}
		else
		{
			ShopTransaction();
		}
		break;
	default:
		break;
	}
}


// �v���C���[�̕`�揈��
void DrawShop(void)
{
	// For some reason the screen doesnt get cleared automatically so we will have to do this manually lol
	system("cls");
	//�����̃��X�g��������
	for (int index = 0; index < SHOP_MAX; index++)
	{
		//printf("sentakushi; %d\n", sentakushi);
		if (index == sentakushi)
		{
			printf("�� %s", doubutsu[index]); // �₶�邵��\������
		}
		else
		{
			printf("%d. %s", index + 1, doubutsu[index]);
		}
	}
}



//void DisplayShops()
//{
//	// self explanatory
//	printf("\nShops: %d", g_Shop.Shops);
//}


// �v���C���[�̃A�h���X��Ԃ�
SHOP *GetShop(void)
{
	//return &g_Shop;
	return NULL;
}


