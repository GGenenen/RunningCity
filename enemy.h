//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : GP11A132 21	ヒセイシン
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_MAX		(100)		// エネミーのMax人数

typedef enum {
	ENEMY_TYPE_BAT,			//0:バット
	ENEMY_TYPE_KAME,		//1:亀
	ENEMY_TYPE_OBS,			//2:OBS

	ENEMY_TYPE_MAX,

} enemy_type;

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY
{

	int			type;			//エネミーの種類(0:バット,1:亀,2:OBS)

	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	XMFLOAT3	move;			// 移動速度


	int			tblNo;			// 行動データのテーブル番号
	int			tblMax;			// そのテーブルのデータ数

	int			left;			//1.左へ移動　0.右へ移動
	BOOL		hitted;			//当たってたか？　T：当たった、F：まだ


	//INTERPOLATION_DATA* tbl_adr;			// アニメデータのテーブル先頭アドレス
	//int					tbl_size;			// 登録したテーブルのレコード総数
	//float				move_time;			// 実行時間
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);



