//=============================================================================
//
// バレット処理 [bullet.h]
// Author :  GP11A132 21	ヒセイシン
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_MAX		(999)		// バレットのMax数
#define BULLET_SPEED	(10.0f)		// バレットの移動スピード


enum {

	BULLET_TYPE_BULLET,
	BULLET_TYPE_BOOM,

	BULLET_TYPE_MAX,

};



// バレット構造体
struct BULLET
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量

	int					type;				// 0:BULLET, 1:BOOM

	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
void SetBullet(XMFLOAT3 pos);


