//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author :  GP11A132 21	ヒセイシン
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH_BAT				(100)	// バットキャラサイズ
#define TEXTURE_HEIGHT_BAT				(80)	// 

#define TEXTURE_WIDTH_KAME				(80)	// 亀キャラサイズ
#define TEXTURE_HEIGHT_KAME				(64)	// 

#define TEXTURE_WIDTH_OBS				(90)	//OBSキャラサイズ
#define TEXTURE_HEIGHT_OBS				(280)	// 



#define TEXTURE_MAX					(4)		// テクスチャの数

#define TEXTURE_BAT_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_BAT_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_BAT_PATTERN_DIVIDE_X*TEXTURE_BAT_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enebat.PNG",		//0:bat
	"data/TEXTURE/enekame.PNG",		//1:kame
	"data/TEXTURE/ene01.PNG",		//2:bashira1
	"data/TEXTURE/ene02.PNG",		//3:bashira2

};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];		// エネミー構造体

static int		g_EnemyCnt = 0;

int type;
int time;
int cntenemy;

static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(0.0f,  0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),	120 },
	{ XMFLOAT3(100.0f,  50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f,0.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f),	60 },
	{ XMFLOAT3(250.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
	{ XMFLOAT3(50.0f, 250.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	120 },
	{ XMFLOAT3(50.0f, 50.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },

};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(1700.0f,   0.0f, 0.0f),		XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	60 },
	{ XMFLOAT3(1700.0f,  SCREEN_HEIGHT, 0.0f),XMFLOAT3(0.0f, 0.0f, 6.28f),	XMFLOAT3(2.0f, 2.0f, 1.0f),	60 },
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
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
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	time = 0;
	cntenemy = 0;

	// エネミー構造体の初期化
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].hitted = FALSE;
		

		switch (i)
		{
		case ENEMY_TYPE_BAT:
			g_Enemy[i].use = FALSE;
			g_Enemy[i].pos = XMFLOAT3(SCREEN_WIDTH, 200.0f, 0.0f);	// 中心点から表示
			g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Enemy[i].w = TEXTURE_WIDTH_BAT;
			g_Enemy[i].h = TEXTURE_HEIGHT_BAT;
			g_Enemy[i].texNo = 0;
			g_Enemy[i].hitted = FALSE;
			g_Enemy[i].type = 0;
			g_Enemy[i].countAnim = 0;
			g_Enemy[i].patternAnim = 0;

			g_Enemy[i].move = XMFLOAT3(10.0f, 0.0f, 0.0f);		// 移動量

			g_Enemy[i].left = 1;			//左に移動

			g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

			//g_EnemyCnt++;
			break;

		case ENEMY_TYPE_KAME:
			g_Enemy[i].use = FALSE;
			g_Enemy[i].pos = XMFLOAT3(SCREEN_WIDTH, 330.0f, 0.0f);	// 中心点から表示
			g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Enemy[i].w = TEXTURE_WIDTH_KAME;
			g_Enemy[i].h = TEXTURE_HEIGHT_KAME;
			g_Enemy[i].texNo = 1;
			g_Enemy[i].type = 1;

			g_Enemy[i].move = XMFLOAT3(8.0f, 0.0f, 0.0f);		// 移動量

			g_Enemy[i].left = 1;			//左に移動
			g_Enemy[i].hitted = FALSE;
			break;

		case ENEMY_TYPE_OBS:
			g_Enemy[i].use = FALSE;
			g_Enemy[i].pos = XMFLOAT3(SCREEN_WIDTH, 140.0f, 0.0f);	// 中心点から表示
			g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Enemy[i].w = TEXTURE_WIDTH_OBS;
			g_Enemy[i].h = TEXTURE_HEIGHT_OBS;
			g_Enemy[i].texNo = (rand() % 2) + 2;
			g_Enemy[i].type = 2;

			g_Enemy[i].move = XMFLOAT3(8.0f, 0.0f, 0.0f);		// 移動量

			g_Enemy[i].left = 1;			//左に移動
			g_Enemy[i].hitted = FALSE;
			break;
		}
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// 生きてるエネミーの数

	time++;


	if (time == 120)
	{

		cntenemy = rand() % 3;
		g_Enemy[cntenemy].use = TRUE;
		g_Enemy[cntenemy].hitted = FALSE;

		switch (g_Enemy[cntenemy].type)
		{
		case ENEMY_TYPE_BAT:
			g_Enemy[cntenemy].pos = XMFLOAT3(SCREEN_WIDTH, 200.0f, 0.0f);	// 中心点から表示
			break;
		case ENEMY_TYPE_KAME:
			g_Enemy[cntenemy].pos = XMFLOAT3(SCREEN_WIDTH, 330.0f, 0.0f);	// 中心点から表示
			break;
		case ENEMY_TYPE_OBS:
			g_Enemy[cntenemy].texNo = (rand() % 2) + 2;
			g_Enemy[cntenemy].pos = XMFLOAT3(SCREEN_WIDTH, 140.0f, 0.0f);	// 中心点から表示
			break;

		}

		time = 0;
	}


	for (int i = 0; i < ENEMY_MAX; i++)
	{
		


			// 生きてるエネミーだけ処理をする
			if (g_Enemy[i].use == TRUE)
			{
				// 地形との当たり判定用に座標のバックアップを取っておく
				XMFLOAT3 pos_old = g_Enemy[i].pos;

				// アニメーション  
				g_Enemy[i].countAnim += 1.0f;
				if (g_Enemy[i].countAnim > ANIM_WAIT)
				{
					g_Enemy[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				else
				{
					float speed = g_Enemy[i].move.x;	//フラグを使った往復移動
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

				// 移動が終わったらエネミーとの当たり判定
				{
					PLAYER* player = GetPlayer();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
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

								// 当たっている？
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
										// //当たった時の処理
										//player[0].hp -= 1;	// デバッグで一時的に無敵にしておくか
										//PlaySound(SOUND_LABEL_SE_bomb000);
									}

									g_Enemy[j].hitted = TRUE;
								}
							}


						}
					}
				}

				g_EnemyCnt++;		// 生きてた敵の数

			}



		
	}


	//// エネミー全滅チェック
	//if (g_EnemyCnt <= 0)
	//{
	//	SetFade(FADE_OUT, MODE_RESULT);
	//}

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================

void DrawEnemy(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();



	for (int i = 0; i < ENEMY_MAX; i++)
	{


		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Enemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Enemy[i].w;		// エネミーの表示幅
			float ph = g_Enemy[i].h;		// エネミーの表示高さ

			//// アニメーション用
			//float tw = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_X;	// テクスチャの幅
			//float th = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			//float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_BAT_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			//float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_BAT_PATTERN_DIVIDE_Y) * th;	// テクスチャの左上Y座標

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			switch (g_Enemy[i].type)
			{
			case 0:
				// アニメーション用
				tw = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_X;	// テクスチャの幅
				th = 1.0f / TEXTURE_BAT_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				tx = (float)(g_Enemy[i].patternAnim % TEXTURE_BAT_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				ty = (float)(g_Enemy[i].patternAnim / TEXTURE_BAT_PATTERN_DIVIDE_Y) * th;	// テクスチャの左上Y座標


				break;
			case 1:
				tw = 1.0f;	// テクスチャの幅
				th = 1.0f;	// テクスチャの高さ
				tx = 0.0f;	// テクスチャの左上X座標
				ty = 0.0f;	// テクスチャの左上Y座標
				break;

			case 2:
				tw = 1.0f;	// テクスチャの幅
				th = 1.0f;	// テクスチャの高さ
				tx = 0.0f;	// テクスチャの左上X座標
				ty = 0.0f;	// テクスチャの左上Y座標
				break;
			}

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}



}



//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}





