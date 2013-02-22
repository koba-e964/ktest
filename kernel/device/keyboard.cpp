#include "../all.h"
#include <stdio.h>
static char keybuf[256];

void wait_kbc_sendready(void)
{
	while(1)
	{
		int io_res=io_in8(PORT_KEYSTA);
		if((io_res&KEYSTA_SEND_NOTREADY)==0)
			return;
	}
	return;
}
void init_keyboard(void)
{
	wait_kbc_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE);
	wait_kbc_sendready();
	io_out8(PORT_KEYDAT,KBC_MODE);
	init_fifo8(&key_fifo,keybuf,256);
	return;
}
// キーボード割り込み時の処理（naskfunc.nasから呼ばれる）
void inthandler21(int *esp)
{
	io_out8(PIC0_OCW2,0x61);//IRQ-01(0x61) complete ->PIC0
	put_fifo8(&key_fifo,io_in8(PORT_KEYDAT));
}
int kb_getmappedchar(int keycode,int shift)
{
	 static int tbl[0x80]={
		 0 , KEY_ESC,
		        '1','2','3','4','5','6',  '7','8','9','0','-','^',KEY_BS,KEY_TAB,

		'q','w','e','r','t','y','u','i',  'o','p','`','[','\n',
		KEY_CTRL,

		'a','s', 'd','f','g','h','j','k','l',';',':',

		 0 ,KEY_LSHIFT,']',

		'z','x','c','v','b','n','m',',','.','/',
		KEY_RSHIFT,'*', KEY_ALT ,' ', 0 ,

		KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5, KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10, 0 , 0 , 
		KEY_10+'7' ,KEY_10+'8',KEY_10+'9',KEY_10+'-',
		KEY_10+'4',KEY_10+'5',KEY_10+'6',KEY_10+'+',
		KEY_10+'1',KEY_10+'2',KEY_10+'3',
		KEY_10+'0',KEY_10+'.',
		0,0,0,
		KEY_F11,  KEY_F12,0,0,0,  0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,'\\',0,0,0,0,    0,0,0,0,0,'\\',0,0,

		
	};
	static int shtbl[0x80]={
		0,KEY_ESC,
			'!','\"','#','$','%','&',    '\'','(',')', 0 ,'=','~',KEY_BS,KEY_TAB,
		'Q','W','E','R','T','Y','U','I','O','P','`','{','\n',KEY_CTRL,'A','S',
	
	};
	keycode&=0xff;
	if(keycode&0x80)
		return 0;
	if(shift==0)
		return tbl[keycode];
	else
		return shtbl[keycode];
}

int kb_getmappedsys(int syscode,int keycode)
{
	static int syse0[256]=
	{
		 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,   0         , 0  , 0    , 0         , 0            , 0          ,    0     ,  0       ,
		 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,   0         ,  0  ,  0  ,  0        , KEY_SYS_ENTER,KEY_SYS_CTRL,     0    ,   0       ,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, KEY_SYS_ALT,0,0,0,0,0,0,0,
		 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,   KEY_SYS_UP,  0 , 0    ,KEY_SYS_LEFT,   0         ,KEY_SYS_RIGHT,  0      ,   0  ,
		KEY_SYS_DOWN,            
	};
	syscode&=0xff;//1-byte uint
	switch(syscode)
	{
	case 0xe0:
		return syse0[keycode];
	case 0xe1:
		break;
	default:
		;
	}
	return 0;
}
int kb_event(void)
{
	char i;
	i=get_fifo8(&key_fifo);
	io_sti();
	if(i==(char)0xe0)//magic number 0x000000e0
	{
		char buf[16];
		int more;
		io_cli();
		more=get_fifo8(&key_fifo);
		io_sti();
		sprintf(buf,"\\s%02x",more);
		layer_print(layerctl,l_console,buf);
	}
	else
	{
		char buf[8];
		int mapped=kb_getmappedchar(i,0);
		if(0<mapped&&mapped<KEY_CHAR)
		{
			buf[0]=(char)mapped;
			buf[1]='\0';
		}
		else if(mapped>=KEY_10&&mapped<KEY_10_END)
		{
			buf[0]='k';
			buf[1]=(char)(mapped-KEY_10);
			buf[2]=0;
		}
		else if(mapped==0)
		{
			//sprintf(buf,"\\x%02x",i&0xff);
			buf[0]=0;
		}
		else
		{
			sprintf(buf,"\\k%04x",mapped);
		}
		layer_print(layerctl,l_console,buf);
	}
	return 0;
}
