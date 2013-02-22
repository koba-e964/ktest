#include "layer.h"

#include <stddef.h>

int rect_and(Rect* r1,Rect r2)
{
	using namespace util::macros;
	if(r1->left<r2.left)r1->left=min(r2.left,r1->right);
	if(r1->right>r2.right)r1->right=max(r2.right,r1->left);
	if(r1->top<r2.top)r1->top=min(r2.top,r1->bottom);
	if(r1->bottom>r2.bottom)r1->bottom=min(r2.bottom,r1->top);
	return 0;
}
/**
if r1 is included in r2, return 0;
else return -1;
*/
int rect_checkbounds(Rect r1,Rect r2)
{
	if
	(
		r1.left>=r2.left &&
		r1.right<=r2.right &&
		r1.top>=r2.top &&
		r1.bottom<=r2.bottom
	)
		return 0;
	return -1;
}

int layerctl_init(LayerCtl* lc,int scrnx,int scrny)
{
	int i;
	for(i=0;i<MAX_LAYERS;i++)
	{
		lc->layer[i].buf=(char*)0;//'NULL' means that the layer isn't alloc'ed
		lc->layer[i].x=0;
		lc->layer[i].y=0;
		lc->layer[i].xsize=0;
		lc->layer[i].ysize=0;
		lc->list[i] = -1;//void
	}
	lc->xsize=scrnx;
	lc->ysize=scrny;
	lc->height=0;
	init_fifo32(&lc->paint_fifo,lc->paint_buffer,PAINT_BUFFER_SIZE);
	return 0;
}
uid_t layer_alloc(LayerCtl* lc)
{
	//find the layer that is not alloc'ed
	int i;//i is the layer-id to be alloc'ed
	int xysize=lc->xsize*lc->ysize;
	for(i=0;i<MAX_LAYERS;i++)
	{
		if(lc->layer[i].buf==(char*)0)
		{
			break;
		}
		if(i==MAX_LAYERS-1)
		{
			uid_t err={-1};
			return err;
		}
	}
	lc->layer[i].buf=(char*)memmng_alloc_4k(xysize);
	memset(lc->layer[i].buf,COL8_INV,xysize);
	//layerの大きさ==画面の大きさ
	{
		lc->layer[i].x=0;
		lc->layer[i].y=0;
		lc->layer[i].xsize=lc->xsize;
		lc->layer[i].ysize=lc->ysize;	
	}
	uid_t lid={i};
	layer_updown(lc,lid,lc->height);
	return lid;
}

int layer_getsize(LayerCtl* ctl,uid_t lyid,Rect* r)
{
	int id=lyid.id;
	if(id<0||id>=ctl->height)
		return -1;
	Layer l=ctl->layer[id];
	r->left=0;
	r->top=0;
	r->right=l.xsize;
	r->bottom=l.ysize;
	return 0;
}

int layer_updown(LayerCtl* lc,uid_t lay,int newh)
{
	if(newh<0 && newh > lc->height)
	{
		return -1;
	}
	int i;
	for(i=0;i< lc->height ; i++)
	{
		if(lc->list[i] == lay.id)
			break;
	}
	if(i<lc->height)
		lc->height--;
	if(newh > lc->height)
	{
		return -1;
	}
	insert_array(lc->list,newh,&lay.id,1,lc->height,MAX_LAYERS,sizeof(int));
	lc->height++;
	return 0;
}

int layer_draw(LayerCtl* ctl,uid_t lyid,char* bmp,Rect r,u32 flags)
{
	char* target=ctl->layer[lyid.id].buf;
	int ix,iy;
	Rect whole;
	layer_getsize(ctl,lyid,&whole);
	if(rect_checkbounds(r,whole)<0)
	{
		return -2;
	}
	int  moffset=r.top*(r.right - r.left)+r.left;
	for(iy=r.top;iy<r.bottom;iy++)
	{
		int bmpybase=iy * (r.right -r.left);
		int targetybase=iy * whole.right;
		for(ix=r.left;ix<r.right;)
		{
			if(r.right - ix >= 4)//int size
			{
				*(int*)(target+targetybase+ix)=*(int*)(bmp + bmpybase + ix - moffset);
				ix+=4;
				continue;
			}
			target[targetybase+ix]=bmp[bmpybase+ix-moffset];
			ix++;
		}
	}
	if((flags&LAYER_REPAINT)!=0 && layerctl_repaint(ctl,r)<0)
	{
		return -1;
	}
	return 0;
}
/**
LayerCtl.last_repaintを見て、さっき呼び出したばかりであるときは少し待つ。

*/
#define REPAINT_TIMEOUT 200
static int repaint_proc(int a0,int a1)
{
	LayerCtl *ctl=(LayerCtl*)a0;
	int left,right,top,bottom;
	int ef=load_eflags();
	io_cli();
	left=get_fifo32(&(ctl->paint_fifo));
	right=get_fifo32(&(ctl->paint_fifo));
	top=get_fifo32(&(ctl->paint_fifo));
	bottom=get_fifo32(&(ctl->paint_fifo));
	Rect dim={left,top,right,bottom};
	store_eflags(ef);
	layerctl_repaint(ctl,dim);
	ctl->scheduled=false;
	return 0;
}
int layerctl_repaint(LayerCtl* ctl,Rect r)
{
	char* vram=ADDR_BOOTINFO->vram;
	Rect whole;
	int ix,iy,cnt;
	if(0 && timerctl.count - ctl->last_repaint <= REPAINT_TIMEOUT)
	{//スケジュールを登録
		put_fifo32(&ctl->paint_fifo,r.left);
		put_fifo32(&ctl->paint_fifo,r.right);
		put_fifo32(&ctl->paint_fifo,r.top);
		put_fifo32(&ctl->paint_fifo,r.bottom);
		if(ctl->scheduled)
		{
			//すでに予定されている。何もしない。
			return 0;
		}
		set_timer_proc(REPAINT_TIMEOUT,(u32)0/*no flags*/,repaint_proc,(int)ctl,0);
		ctl->scheduled=true;
	}
	layerctl_wholescreen(ctl,&whole);
	if(rect_checkbounds(r,whole)<0){return -2;}
	for(cnt=0;cnt<ctl->height;cnt++)
	{
		uid_t lyid={ctl->list[cnt]}; //{cnt};
		Rect currect;
		layer_getsize(ctl,lyid,&currect);
		if(rect_checkbounds(whole,currect)<0)//currect is supposed to be ==whole
		{
			return -3;
		}
	}
	for(cnt=0;cnt<ctl->height;cnt++)
	{
		char* curbuf=ctl->layer[ctl->list[cnt]].buf;
		for(iy=r.top;iy<r.bottom;iy++)
		{
			int offsetbase=whole.right*iy;
			for(ix=r.left;ix<r.right;)
			{
				int offset=offsetbase+ix;
				if(r.right - ix >= 4 )
				{
					if( 
		curbuf[offset+0] != COL8_INV &&curbuf[offset+1] != COL8_INV &&
		curbuf[offset+2] != COL8_INV &&curbuf[offset+3] != COL8_INV )
					{
						*(int*)(vram+offset)=*(int*)(curbuf+offset);
						ix+=4;
						continue;
					}
					if(*(int*)(curbuf+offset)==COL8_INV*0x1010101)
					{
						ix+=4;
						continue;
					}
				}	
				char tmp=curbuf[offset];
				if(tmp!=(char)COL8_INV)
					vram[offset]=tmp;
				ix++;
			}
		}
	}
	ctl->last_repaint=timerctl.count;
	return 0;
}

int layerctl_wholescreen(LayerCtl* lc,Rect* r)
{
	r->left=0;
	r->top=0;
	r->right=lc->xsize;
	r->bottom=lc->ysize;
	return 0;
}
int layer_free(LayerCtl* ctl,uid_t lyid)
{
	int id=lyid.id;
	memmng_free_4k(ctl->layer[id].buf,ctl->xsize*ctl->ysize);
	erase_array(ctl->layer,id,id+1,ctl->height,sizeof(Layer));
	ctl->height--;
	return 0;
}

#ifndef XCHAR
#define XCHAR 120
#endif

#ifndef YCHAR
#define YCHAR 45
#endif

const int line_interval=10;
int layer_print(LayerCtl* ctl,uid_t lyid,char* str)
{
	static int gl_xpos=0,gl_ypos=0;
	static char conbuf[XCHAR*YCHAR];
	static bool locking=false;
	bool line=false;
	while((*str)!='\0')
	{
		switch(*str)
		{
		case '\r':
			gl_xpos=0;
			break;
		case '\n':
		io_cli();
			while(locking)
				io_hlt();
			locking=true;
		io_sti();

			gl_xpos=0;
			++gl_ypos;
			if(gl_ypos>=YCHAR)
			{
				gl_ypos--;
				int i,j,k;
				for(k=0;k<YCHAR-1;k++)
				{
					for(i=0;i<XCHAR;i++)
					{
						conbuf[k*XCHAR+i]=conbuf[k*XCHAR+XCHAR+i];
					}
				}
				for(i=0;i<XCHAR;i++)
					conbuf[YCHAR*XCHAR-XCHAR+i]='\0';
				line=true;
			io_cli();
				locking=false;
			io_sti();

				break;
			}
		io_cli();
			locking=false;
		io_sti();
			break;
		default:
			{
				Rect filled={gl_xpos<<3,gl_ypos<<4,(gl_xpos<<3)+7,(gl_ypos<<4)+15};
				layer_boxfill8(ctl,lyid,filled,COL8_INV,1);
			}
			layer_putfont8(ctl,lyid,gl_xpos<<3,gl_ypos<<4,COL8_RED,hankaku+(*str)*16,1);
			conbuf[gl_ypos*XCHAR+gl_xpos]=*str;
			++gl_xpos;
			if(gl_xpos>=XCHAR)
			{
				layer_print(ctl,lyid,"\n");
			}
		}
		++str;
	}
	static int lastline= -1;
	if(lastline >= 0 && timerctl.count < lastline + line_interval)
		return -1;
	lastline = timerctl.count;
	if(line)
		for(int i=0;i<YCHAR;i++)
			for(int j=0;j<XCHAR;j++)
				layer_putfont8(ctl,lyid,j<<3,i<<4,COL8_RED,hankaku+conbuf[i*XCHAR+j]*16,1);
	Rect consrect={0,0,XCHAR<<3,YCHAR<<4};
	//layerctl_repaint(ctl,consrect);
	return 0;
}

int layer_fill8(LayerCtl *lc,uid_t lyid,char color,u32 flags)
{
	int id=lyid.id;
	if(id<0||id>=lc->height)
		{return -1;}
	Rect layer;
	layer_getsize(lc,lyid,&layer);
	layer_boxfill8(lc,lyid,layer,color,flags);
	return 0;
}

int layer_boxfill8(LayerCtl* ctl,uid_t lyid,Rect filled,char colour,u32 flags)
{
	Rect layer;
	layer_getsize(ctl,lyid,&layer);
	if(rect_checkbounds(filled,layer)<0)
	{
		return -1;
	}
	int xysize=(filled.right-filled.left)*(filled.bottom-filled.top);
	char* tmp=(char*)memmng_alloc_4k(xysize);
	memset(tmp,colour,xysize);
	layer_draw(ctl,lyid,tmp,filled,flags);
	memmng_free_4k(tmp,xysize);
	return -2;
}
int layer_putfont8(LayerCtl* lc,uid_t lyid,int xpos,int ypos,
		char colour,char* font,u32 flags)
{
	Rect area={xpos,ypos,xpos+8,ypos+16};
	{
		Rect layer;
		layer_getsize(lc,lyid,&layer);
		if(rect_checkbounds(area,layer)<0)
			return -1;
	}
	char bmp[128];
	int i;
	for(i=0;i<16;i++)
	{
		char bits=font[i];
		int j;
		for(j=7;j>=0;j--)
		{
			if((bits&(0x80>>j))!=0)
				bmp[i*8+j]=colour;
			else
				bmp[i*8+j]=COL8_INV;
		}
	}
	if(layer_draw(lc,lyid,bmp,area,flags)<0)return -125;
	return 0;	
}
int layer_putstr8(LayerCtl* lc,uid_t lyid,int xpos,int ypos,char colour,char *str,char *font,u32 flags)
{
	for(;*str != '\0';str++)
	{
		layer_putfont8(lc,lyid,xpos,ypos,colour,font+((str[0] &0xff)<<4),flags);
		xpos+=8;
	}
	return 0;
}

