//=============================================================================
//
// プレイヤー処理 [player.cpp]
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
#define TEXTURE_WIDTH				(200/2)	// キャラサイズ
#define TEXTURE_HEIGHT				(200/2)	// 
#define TEXTURE_MAX					(5)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(40)		// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(210.0f)	// ジャンプの高さ

// ダウン処理
#define	PLAYER_DOWN_CNT_MAX			(40)		// 30フレームで着地する
#define	PLAYER_DOWN_Y_MAX			(-10.0f)	// ダウンの高さ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/cha.PNG",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/cha001.PNG",
	"data/TEXTURE/heart000.PNG",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(300.0f, 320.0f, 0.0f);	// 中心点から表示
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].hp = 6;
		g_Player[i].hitted = FALSE;
		g_Player[i].hpkeep = FALSE;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Player[i].dir = CHAR_DIR_GO1;					// 下向きにしとくか
		g_Player[i].moving = FALSE;							// 移動中フラグ
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;

		// ジャンプの初期化
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;

		// ダウンの初期化
		g_Player[i].down = FALSE;
		g_Player[i].downCnt = 0;
		g_Player[i].downY = 0.0f;



		// 分身用
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Player[i].pos;

			// 分身用
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			// アニメーション

			//if (g_Player[i].moving == FALSE)
			//{
			//	g_Player[i].countAnim += 1.0f;

			//	if (g_Player[i].countAnim > ANIM_WAIT)
			//	{
			//		g_Player[i].countAnim = 0.0f;
			//		// パターンの切り替え
			//		g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
			//	}

			//}


			//g_Player[i].countAnim += 1.0f;
			//if (g_Player[i].countAnim > ANIM_WAIT)
			//{
			//	g_Player[i].countAnim = 0.0f;
			//	// パターンの切り替え
			//	g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
			//}



			g_Player[i].countAnim += 1.0f;

			if (g_Player[i].countAnim > ANIM_WAIT)
			{
				g_Player[i].countAnim = 0.0f;

				if (g_Player[i].patternAnim == 3)
				{
					g_Player[i].dir = 1;
				}
				else if (g_Player[i].patternAnim == 7)
				{
					g_Player[i].dir = 0;
				}

				g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
			}




			// キー入力で移動 
			{

				//float speed = g_Player[i].move.x;

				//g_Player[i].moving = FALSE;
				//g_Player[i].dash = FALSE;

				//if (GetKeyboardPress(DIK_C) || IsButtonPressed(0, BUTTON_A))
				//{
				//	speed *= 4;
				//	g_Player[i].dash = TRUE;
				//}


				//if (GetKeyboardPress(DIK_DOWN))
				//{
				//	g_Player[i].pos.y += speed;
				//	g_Player[i].dir = CHAR_DIR_DOWN;
				//	g_Player[i].moving = TRUE;
				//}
				//else if (GetKeyboardPress(DIK_UP))
				//{
				//	g_Player[i].pos.y -= speed;
				//	g_Player[i].dir = CHAR_DIR_UP;
				//	g_Player[i].moving = TRUE;
				//}

				//if (GetKeyboardPress(DIK_RIGHT))
				//{
				//	g_Player[i].pos.x += speed;
				//	g_Player[i].dir = CHAR_DIR_RIGHT;
				//	g_Player[i].moving = TRUE;
				//}
				//else if (GetKeyboardPress(DIK_LEFT))
				//{
				//	g_Player[i].pos.x -= speed;
				//	g_Player[i].dir = CHAR_DIR_LEFT;
				//	g_Player[i].moving = TRUE;
				//}

				//// ゲームパッドでで移動処理
				//if (IsButtonPressed(0, BUTTON_DOWN))
				//{
				//	g_Player[i].pos.y += speed;
				//	g_Player[i].dir = CHAR_DIR_DOWN;
				//	g_Player[i].moving = TRUE;
				//}
				//else if (IsButtonPressed(0, BUTTON_UP))
				//{
				//	g_Player[i].pos.y -= speed;
				//	g_Player[i].dir = CHAR_DIR_UP;
				//	g_Player[i].moving = TRUE;
				//}

				//if (IsButtonPressed(0, BUTTON_RIGHT))
				//{
				//	g_Player[i].pos.x += speed;
				//	g_Player[i].dir = CHAR_DIR_RIGHT;
				//	g_Player[i].moving = TRUE;
				//}
				//else if (IsButtonPressed(0, BUTTON_LEFT))
				//{
				//	g_Player[i].pos.x -= speed;
				//	g_Player[i].dir = CHAR_DIR_LEFT;
				//	g_Player[i].moving = TRUE;
				//}


				// ジャンプ処理中？
				if (g_Player[i].jump == TRUE)
				{
					float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[i].jumpCnt;
					float y = PLAYER_JUMP_Y_MAX * cosf(XM_PI / 2 + angle);
					g_Player[i].jumpY = y;

					g_Player[i].jumpCnt++;
					if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
					{
						g_Player[i].jump = FALSE;
						g_Player[i].jumpCnt = 0;
						g_Player[i].jumpY = 0.0f;
						g_Player[i].dir = CHAR_DIR_GO1;
					}

				}
				// ジャンプボタン押した？
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_UP)) || (IsButtonPressed(0, BUTTON_UP)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = 0;
					g_Player[i].jumpY = 0.0f;
					g_Player[i].dir = CHAR_DIR_UP;
					g_Player[i].hpkeep = TRUE;
				}
				g_Player[i].hpkeep = FALSE;



				// ダウン処理中？
				if (g_Player[i].down == TRUE)
				{
					float angle = (XM_PI / PLAYER_DOWN_CNT_MAX) * g_Player[i].downCnt;
					float y = PLAYER_DOWN_Y_MAX * cosf(XM_PI / 2 + angle);
					g_Player[i].jumpY = y;

					g_Player[i].downCnt++;
					if (g_Player[i].downCnt > PLAYER_DOWN_CNT_MAX)
					{
						g_Player[i].down = FALSE;
						g_Player[i].downCnt = 0;
						g_Player[i].downY = 0.0f;
						g_Player[i].dir = CHAR_DIR_GO1;
					}

				}

				// ダウンボタン押した？
				else if ((g_Player[i].down == FALSE) && ((GetKeyboardTrigger(DIK_DOWN)) || IsButtonPressed(0, BUTTON_DOWN)))
				{
					g_Player[i].down = TRUE;
					g_Player[i].downCnt = 0;
					g_Player[i].downY = 0.0f;
					g_Player[i].dir = CHAR_DIR_DOWN;
					g_Player[i].hpkeep = TRUE;
				}
				g_Player[i].hpkeep = FALSE;


				// MAP外チェック
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;


				// 移動が終わったらエネミーとの当たり判定
				//{
				//	ENEMY* enemy = GetEnemy();

				//	// エネミーの数分当たり判定を行う
				//	for (int j = 0; j < ENEMY_MAX; j++)
				//	{
				//		// 生きてるエネミーと当たり判定をする
				//		if (enemy[j].use == TRUE&&enemy[j].hitted==FALSE)
				//		{
				//			XMFLOAT3 pos = g_Player[0].pos;

				//			if (g_Player[i].jump)
				//			{
				//				pos.y += g_Player[0].jumpY;
				//			}
				//			
				//			if (g_Player[i].down)
				//			{
				//				pos.y -= g_Player[0].downY;
				//			}
				//			BOOL ans = CollisionBB(pos, g_Player[0].w/2, g_Player[0].h/2,
				//				enemy[j].pos, enemy[j].w/2, enemy[j].h/2);
				//			// 当たっている？
				//			if (ans == TRUE)
				//			{
				//				enemy[j].hitted = TRUE;

				//				if (g_Player[0].hpkeep == FALSE) {
				//					switch (enemy[j].type) {

				//					case ENEMY_TYPE_BAT:
				//						AddScore(-100);
				//						g_Player[0].hp -= 1;
				//						PlaySound(SOUND_LABEL_SE_hit);
				//						break;

				//					case ENEMY_TYPE_KAME:
				//						AddScore(-50);
				//						g_Player[0].hp -= 1;
				//						PlaySound(SOUND_LABEL_SE_hit);
				//						break;
				//					case ENEMY_TYPE_OBS:
				//						AddScore(-30);
				//						g_Player[0].hp -= 1;
				//						PlaySound(SOUND_LABEL_SE_hit);
				//						break;
				//					}
				//					enemy[j].use = FALSE;
				//				}
				//			}
				//		}
				//	}
				//}

				// バレット処理
				if (GetKeyboardTrigger(DIK_SPACE))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					SetBullet(pos);
				}
				if (IsButtonTriggered(0, BUTTON_B))
				{
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += g_Player[i].jumpY;
					SetBullet(pos);
				}

			}
		}
	}


	// 現状をセーブする
	if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
	}



	if (g_Player[0].hp <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}




#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes

			{	// 影表示
				SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				float pw = g_Player[i].w - 30.0f;		// プレイヤーの表示幅
				float ph = g_Player[i].h / 5;		// プレイヤーの表示高さ

				py += 45.0f;		// 足元に表示				

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す

			}

			// プレイヤーの分身を描画
			if (g_Player[i].dash)
			{	// ダッシュ中だけ分身処理
				DrawPlayerOffset(i);
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = g_Player[i].w;		// プレイヤーの表示幅
			float ph = g_Player[i].h;		// プレイヤーの表示高さ

			py += g_Player[i].jumpY;		// ジャンプ中の高さを足す

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f;	// テクスチャの幅
			//float th = 1.0f;	// テクスチャの高さ
			//float tx = 0.0f;	// テクスチャの左上X座標
			//float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

		// ゲージのテスト
		{
			// 下敷きのゲージ（枠的な物）
			// テクスチャ設定

			for (int i = 0; i < g_Player[0].hp; i++) {

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
				//ゲージの位置やテクスチャー座標を反映
				//float px = 20.0f;		// ゲージの表示位置X
				float py = 20.0f;		// ゲージの表示位置Y
				float pw = 30;		// ゲージの表示幅
				float ph = 30;		// ゲージの表示高さ
				float px = 20.0f + ((pw + 5) * (i));

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}






			//// HP数に従ってゲージの長さを表示してみる
			//
			//// テクスチャ設定
			//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			////ゲージの位置やテクスチャー座標を反映

			//pw = pw * ((float)g_Player[i].hp / PLAYER_MAX);

			//// １枚のポリゴンの頂点とテクスチャ座標を設定
			//SetSpriteLTColor(g_VertexBuffer,
			//	px, py, pw, ph,
			//	tx, ty, tw, th,
			//	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

			//// ポリゴン描画
			//GetDeviceContext()->Draw(4, 0);

		}





	}



}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;		// プレイヤーの表示幅
		float ph = g_Player[no].h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


