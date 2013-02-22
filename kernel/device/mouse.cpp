#include "../all.h"
static char mcbuf[0x100];
static int mx,my;
MouseBuf mbuf,moldbuf;
char mcursor[256];

void enable_mouse(void)
{
	init_fifo8(&mouse_fifo,mcbuf,0x100);
	wait_kbc_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_SENDTO_MOUSE);
	wait_kbc_sendready();
	io_out8(PORT_KEYDAT,MOUSECMD_ENABLE);
	mbuf.phase=0;//mouseの0xfaを待つてゐる段階
	return;
}
// マウス割り込み時の処理（naskfunc.nasから呼ばれる）
void inthandler2c(int *esp)
{
	char data;
	io_out8(PIC1_OCW2,0x64);//IRQ-12(0x64) ->PIC1
	io_out8(PIC0_OCW2,0x62);//IRQ-4(0x62) ->PIC0
	data=io_in8(PORT_KEYDAT);
	put_fifo8(&mouse_fifo,data);
}

int mouse_initdraw(void)
{
	lymouse=layer_alloc(layerctl);
	// マウスの座標計算
	mx = ((ADDR_BOOTINFO->scrnx - 16)      / 4) * 3;
	my = ((ADDR_BOOTINFO->scrny - 28 - 16) / 4) * 3;

	// マウス描画
	{
		Rect mouserect={mx,my,mx+16,my+16};
		init_mouse_cursor8(mcursor, COL8_INV);
		layer_draw(layerctl,lymouse,mcursor,mouserect,1);
	}
	return 0;
}

int mouse_event(void)
{
	const BootInfo *pbinfo=ADDR_BOOTINFO;
	int i=get_fifo8(&mouse_fifo);
	io_sti();
	if(mouse_decode(&mbuf,i)>0)
	{
		int xmov,ymov;
		xmov=mbuf.x;
		ymov=mbuf.y;
		//move mouse cursor 
		{
			int oldmx=mx,oldmy=my;
			Rect oldmouse={oldmx,oldmy,oldmx+CURSOR_XSIZE,oldmy+CURSOR_YSIZE};
			layer_boxfill8(layerctl,lymouse,
				oldmouse,COL8_INV,1);
			mx+=xmov;my+=ymov;
			//check bounds
			{
				if(mx<0)mx=0;
				if(my<0)my=0;
				if(mx>pbinfo->scrnx-CURSOR_XSIZE)mx=pbinfo->scrnx-CURSOR_XSIZE;
				if(my>pbinfo->scrny-CURSOR_YSIZE)my=pbinfo->scrny-CURSOR_YSIZE;
			}
			Rect mrect={mx,my,mx+CURSOR_XSIZE,my+CURSOR_YSIZE};
			layer_draw(layerctl,lymouse,mcursor,mrect,1);
		}
		if(xmov!=0 || ymov != 0)
			mouse_motion(mx,my);
		/* old
		if((mbuf.btn & MOUSE_L)&& !(moldbuf.btn & MOUSE_L))
			mouse_lclick(mx,my);
		*/
		int btn_down = (mbuf.btn&MOUSE_ALL)& ~moldbuf.btn;
		mouse_click(mx,my,btn_down);
		int btn_up=~mbuf.btn  & (moldbuf.btn&MOUSE_ALL);
		mouse_raise(mx,my,btn_up);
		moldbuf=mbuf;
	}
	return 0;
}

int mouse_decode(MouseBuf* mbuf,unsigned char data)
{
	switch(mbuf->phase)
	{
	case 0://Mouseの0xfaを待つてゐる
		if((data&0xff)==0xfa)
			++mbuf->phase;
		return 0;
	case 1:
		if((data&0xc8)==8)//data=0b00??1???
		{
			mbuf->buf[0]=data;
			++mbuf->phase;
		}
		return 0;
	case 2:
		mbuf->buf[1]=data;
		++mbuf->phase;
		return 0;
	case 3:
		mbuf->buf[2]=data;
		mbuf->phase=1;
		mbuf->btn=mbuf->buf[0]&7;
		mbuf->x=mbuf->buf[1];
		mbuf->y=mbuf->buf[2];
		if((mbuf->buf[0]&0x10)!=0)
			mbuf->x|=-0x100;
		if((mbuf->buf[0]&0x20)!=0)
			mbuf->y|=-0x100;
		mbuf->y=-mbuf->y;
		return 1;//end
	}
	return -1;
	
}

int mouse_motion(int mx,int my)
{
	int winID=getWindow(mx,my);
	if(winID >= 0)
	{
		sampleWH(winID,MSG_CURSOR,mx,my);
	}
	return 0;
}
//This macro is used in mouse_click() and mouse_raise().
#define macro0(bit,msg) do{if(flags & (bit))t_SendMessage(winID,msg,mx,my);}while(0)

int mouse_click(int mx,int my,int flags)
{
	int winID=getWindow(mx,my);
	if(winID >= 0)
	{
		macro0(MOUSE_L,MSG_LDOWN);
		macro0(MOUSE_R,MSG_RDOWN);
	}
	return 0;
}
int mouse_raise(int mx,int my,int flags)
{
	int winID=getWindow(mx,my);
	if(winID >= 0)
	{
		macro0(MOUSE_L,MSG_LUP);
		macro0(MOUSE_R,MSG_RUP);
	}
	return 0;
}
#undef macro0

