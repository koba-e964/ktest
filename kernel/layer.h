#ifndef SHEET_H_6ab1
#define SHEET_H_6ab1
#include "all.h"
#include <stddef.h>

enum LayerConsts{
	LAYER_REPAINT=1,
	MAX_LAYERS=0x100,
};

//invisible
#define COL8_INV 0x7f

typedef struct
{
	int left;
	int top;
	int right;
	int bottom;
}Rect;
int rect_and(Rect* r1,Rect r2);
/**
	r1 <_ wholeかどうかを返す。
	@retval 0 iff r1 <_ whole
		-1 otherwise
*/
int rect_checkbounds(Rect r1, Rect whole);
typedef struct
{
	char* buf;
	int x,y,xsize,ysize;
}Layer;
#define PAINT_BUFFER_SIZE 200
typedef struct
{
	Layer layer[MAX_LAYERS];
	/**
		ex.) list=(2 3 0)の時、一番上はlayerid=0のlayer
	*/
	int list[MAX_LAYERS];//高さのlist
	int xsize,ysize;//画面の大きさ
	int height;//layer,listの長さ
	int last_repaint;//最後にrepaintを呼び出したときの時刻
	bool scheduled;//repaintが予定されているかどうか。1->予約済み 0->無し
	i32 paint_buffer[PAINT_BUFFER_SIZE];
	Fifo32 paint_fifo;
}LayerCtl;
/**
	本当はサイズを自由に設定したい

	画面全体と同じサイズのlayerを作る
*/
uid_t layer_alloc(LayerCtl* ctl);

/**
	layerの高さを調節
	newh:新しい高さ

	詳細
	ctl->listにshが含まれていれば、それを取り除き、
	newhで示される高さになるように、ctl->listにshを入れる。
	その後ctl->heightを適切にする。(ctl->height += 0 or 1)
*/
int layer_updown(LayerCtl* ctl,uid_t sh,int newh);
int layer_draw(LayerCtl*,uid_t sh,char* bmp,Rect r,u32 flags);

/**
	layerの、画面原点からみたサイズをrに入れる
	
*/
int layer_getsize(LayerCtl*,uid_t,Rect* r);
/**
	layerctlを、原点(0,0),右下(scrnx,scrny)の長方形（画面全体）として定義す。
*/
int layerctl_init(LayerCtl* lc,int scrnx,int scrny);

/**
	再描画
	rct:layerctlの原点からみた描画範囲
*/
int layerctl_repaint(LayerCtl*,Rect rct);
/**
	画面全体のサイズを得る。
*/
int layerctl_wholescreen(LayerCtl*,Rect* r);
int layer_free(LayerCtl* ctl,uid_t sh);



/**
	指定したlayerを、colorで埋める。
	これは、
	{
		Rect whole;
		layer_getsize(ctl,lyid,&whole,flags);
		layer_boxfill8(ctl,lyid,whole,color,flags);
	}
	と等価。
*/
int layer_fill8(LayerCtl*,uid_t,char color,u32 flags);
int layer_boxfill8(LayerCtl* ctl,uid_t,Rect filled,char colour,u32 flags);
int layer_putfont8(LayerCtl* lc,uid_t lyid,int xpos,int ypos,
		char color,char* fontdata,u32 flags);
int layer_putstr8(LayerCtl* lc,uid_t lyid,int xpos,int ypos,char color,char* string,char* font,u32 flags);

int layer_print(LayerCtl* ctl,uid_t lyid,char* str);

//test routine
int layers_test(void);


global LayerCtl* layerctl;
global uid_t l_console;


#if 1
#define DEBUG_PRINT(str) layer_print(layerctl,l_console,str)
#else
#define DEBUG_PRINT(str) /**/
#endif

#endif
