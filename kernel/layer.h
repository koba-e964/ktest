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
	r1 <_ whole���ǂ�����Ԃ��B
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
		ex.) list=(2 3 0)�̎��A��ԏ��layerid=0��layer
	*/
	int list[MAX_LAYERS];//������list
	int xsize,ysize;//��ʂ̑傫��
	int height;//layer,list�̒���
	int last_repaint;//�Ō��repaint���Ăяo�����Ƃ��̎���
	bool scheduled;//repaint���\�肳��Ă��邩�ǂ����B1->�\��ς� 0->����
	i32 paint_buffer[PAINT_BUFFER_SIZE];
	Fifo32 paint_fifo;
}LayerCtl;
/**
	�{���̓T�C�Y�����R�ɐݒ肵����

	��ʑS�̂Ɠ����T�C�Y��layer�����
*/
uid_t layer_alloc(LayerCtl* ctl);

/**
	layer�̍����𒲐�
	newh:�V��������

	�ڍ�
	ctl->list��sh���܂܂�Ă���΁A�������菜���A
	newh�Ŏ�����鍂���ɂȂ�悤�ɁActl->list��sh������B
	���̌�ctl->height��K�؂ɂ���B(ctl->height += 0 or 1)
*/
int layer_updown(LayerCtl* ctl,uid_t sh,int newh);
int layer_draw(LayerCtl*,uid_t sh,char* bmp,Rect r,u32 flags);

/**
	layer�́A��ʌ��_����݂��T�C�Y��r�ɓ����
	
*/
int layer_getsize(LayerCtl*,uid_t,Rect* r);
/**
	layerctl���A���_(0,0),�E��(scrnx,scrny)�̒����`�i��ʑS�́j�Ƃ��Ē�`���B
*/
int layerctl_init(LayerCtl* lc,int scrnx,int scrny);

/**
	�ĕ`��
	rct:layerctl�̌��_����݂��`��͈�
*/
int layerctl_repaint(LayerCtl*,Rect rct);
/**
	��ʑS�̂̃T�C�Y�𓾂�B
*/
int layerctl_wholescreen(LayerCtl*,Rect* r);
int layer_free(LayerCtl* ctl,uid_t sh);



/**
	�w�肵��layer���Acolor�Ŗ��߂�B
	����́A
	{
		Rect whole;
		layer_getsize(ctl,lyid,&whole,flags);
		layer_boxfill8(ctl,lyid,whole,color,flags);
	}
	�Ɠ����B
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
