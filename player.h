//=============================================================================
//
// プレイヤー処理 [player.h]
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
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT	(12)	// 16分身

enum
{
	CHAR_DIR_GO1,
	CHAR_DIR_GO2,
	CHAR_DIR_UP,
	CHAR_DIR_DOWN,

	CHAR_DIR_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号

	int			dir;			// 向き（0/1:go 2:up 3:dowm ）
	BOOL		moving;			// 移動中フラグ

	BOOL		dash;			// ダッシュ中フラグ
	XMFLOAT3	move;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント

	BOOL		down;			// ダウンフラグ
	float		downY;			// ダウンの高さ
	int			downCnt;		// ダウン中のカウント

	BOOL		hitted;			//当たってたか？　T：当たった、F：まだ
	BOOL		hpkeep;			//無敵フラグ 
	int			hp;				//プレイヤーのHP
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);



