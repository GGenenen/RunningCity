/*******************************************************************************
* タイトル:		Shopプログラム
* プログラム名:	Shop.cpp
* 作成者:		GP11A132 99 外岡高明
* 作成日:		2022/05/26
*******************************************************************************/

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "Shop.h"
#include "field.h"
#include "player.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/


/*******************************************************************************
* 構造体定義
*******************************************************************************/


/*******************************************************************************
* プロトタイプ宣言(こんな関数を用意してありますよ宣言/関数の説明書)
*******************************************************************************/



/*******************************************************************************
* グローバル変数
*******************************************************************************/
SHOP g_Shop[SHOP_MAX];	// Shop型でプレイヤー１体分


int sentakushi = 0;


char doubutsu[SHOP_MAX][80] = {
	"ネコ                                                                    \n",
	"イヌ                                                                    \n",
	"ペンギン                                                                \n",
	"ライオン                                                                \n",
	"パンダ                                                                  \n",
	"イルカ                                                                  \n",
	"ゴリラ                                                                  \n",
	"キリン                                                                  \n",
	"ニワトリ                                                                \n",
	"トラ                                                                    \n",
	"EXIT SHOP                                                               \n",
};


// プレイヤーの初期化処理
void InitShop(void)
{
	// 構造体の初期化
}


// プレイヤーの終了処理
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


// プレイヤーの更新処理
void UpdateShop(void)
{

	// キー入力処理
	int key = _getch();
	if ((key == 0) || (key == 224))	// 特殊Keyならもう１度取得する
	{
		key = _getch();
	}

	// 押されたkeyの方向へ移動させる
	switch (key)
	{
	case 'w':
	case 0x48:	// ↑
		if (sentakushi > 0)
		{
			sentakushi--;
		}
		break;
	case 's':
	case 0x50:	// ↓
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


// プレイヤーの描画処理
void DrawShop(void)
{
	// For some reason the screen doesnt get cleared automatically so we will have to do this manually lol
	system("cls");
	//動物のリストから一つずつ
	for (int index = 0; index < SHOP_MAX; index++)
	{
		//printf("sentakushi; %d\n", sentakushi);
		if (index == sentakushi)
		{
			printf("→ %s", doubutsu[index]); // やじるしを表示する
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


// プレイヤーのアドレスを返す
SHOP *GetShop(void)
{
	//return &g_Shop;
	return NULL;
}


