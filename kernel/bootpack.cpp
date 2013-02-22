// bootpack.c
#include <stdio.h>
#include "all.h"

#define REQ_TIMER 0x0001
#define REQ_KEYBOARD 0x0002
#define REQ_MOUSE 0x0004

/**
	Global Variables.
*/

BOOTINFO* const pbinfo = (PBOOTINFO) ADDR_BOOTINFO;
/*
	現在あるリクエストの一覧
	Deprecated:req_fifo(buffer)に移行す。
*/


/**
	
*/
int inf_loop(void* win)
{
	int j;
	char str[256];
	int cnt=0;
	for(j=0;1;++j)
	{
		cnt++;
		sprintf(str,"%x,cur=%d\n",cnt,thread_current());
		//DEBUG_PRINT(str);
		uSleep(100);
	}
	return -1;
}


char closing[0x101]=
	"................"
	".*............*."
	"..*..........*.."
	"...*........*..."
	"....*......*...."
	".....*....*....."
	"......*..*......"
	".......**......."
	".......**......."
	"......*..*......"
	".....*....*....."
	"....*......*...."
	"...*........*..."
	"..*..........*.."
	".*............*."
	"................";

int closingButtonHandler(int handle,int msg,int arg0,int arg1)
{
	switch(msg)
	{
	case MSG_LDOWN:
		DEBUG_PRINT("cl");
		/**
			Destroys its parent window.
		*/
		WindowInfo wi;
		getWindowInfo(handle,&wi);
		t_DestroyWindow(wi.parent);
		return 0;
	case MSG_DESTROY:
		DEBUG_PRINT("\r\nchild-destroyed\r\n");
		return 0;
	}
	return 0;
}

int sampleWH(int handle, int msg, int arg0, int arg1 )
{
	const int row_width=3;
	//return 0;//-----------------------------------------------
	switch(msg)
	{
	case WH_INIT:
		return 0;
	case MSG_CREATE:
	{
		uid_t winlay=winctl.winlay;
		int x=arg0&0xffff,y=arg0>>16;
		int width=arg1&0xffff,height=arg1>>16;
		int right_end  = x + width;
		int bottom_end = y + height;
		char str[0x200];
		sprintf(str,"create handle=%d,x=%d,y=%d,width=%d,height=%d\n",handle,x,y,width,height);
		DEBUG_PRINT(str);
		Rect caption_r=
		{x,           y,            right_end,          y+20};
		Rect rightrow =
		{right_end-row_width,y+20,         right_end,          bottom_end};
		Rect leftrow  =
		{x,           y+20,         x+row_width, bottom_end};
		Rect bottomrow=
		{x+row_width, bottom_end-row_width,right_end-row_width,bottom_end};
		layer_boxfill8(layerctl,winlay, caption_r ,COL8_BLUE,1);
		layer_boxfill8(layerctl,winlay, rightrow  ,COL8_BLUE,1);
		layer_boxfill8(layerctl,winlay, leftrow   ,COL8_BLUE,1);
		layer_boxfill8(layerctl,winlay, bottomrow ,COL8_BLUE,1);
		Rect closing_rect={right_end-20,y+row_width ,right_end-4,y+18};
	
		{
			int i;
			char bitmap[0x100];
			for(i=0;i<0x100;i++)
			{
				bitmap[i]= closing[i]=='*' ?COL8_BLACK:COL8_WHITE;
			}
			layer_draw(layerctl,winlay,bitmap,closing_rect,1);
		}

		Rect innerrow={x+row_width,y+20,right_end-row_width,bottom_end-row_width};
		layer_boxfill8(layerctl,winlay,innerrow,COL8_GRAY,1);

		WindowInfo closingButton={{right_end-20,y+row_width,16,18-row_width},
			closingButtonHandler,handle};
		createComponent(0x10 , &closingButton);


	}
		return 0;
	case MSG_CURSOR:
		return 0;
	case MSG_LDOWN:
	{
		WindowInfo wi;
		DEBUG_PRINT("L");
		getWindowInfo(handle,&wi);
		int x=wi.reg.x;
		int y=wi.reg.y;
		int right_end = x + wi.reg.width;
		Rect closing_rect={right_end-20,y+row_width ,right_end-4,y+18};
		Rect mouseptr={arg0-1,arg1-1,arg0,arg1};
		if(rect_checkbounds(mouseptr,closing_rect)==0)
			t_DestroyWindow(handle);//A single left click causes a destruction.
		return 0;
	}
	case MSG_RDOWN:
		DEBUG_PRINT("r");
		return 0;
	case MSG_DESTROY:
		DEBUG_PRINT("destroyed\n");
		return 0;
	case MSG_REPAINT:
	{
		int x=arg0&0xffff,y=arg0>>16;
		int width=arg1&0xffff,height=arg1>>16;
		char str[0x100];
		sprintf(str,"rep handle=%d,x=%d,y=%d,width=%d,height=%d\n",handle,x,y,width,height);
		DEBUG_PRINT(str);
	}
		break;
	default:
		return 0;
	}
	return 0;
}


u32 get_request(void)
{
	u32 flags=0;
	if(status_fifo32(&timerctl.notifyIDs)>0)
		flags|=REQ_TIMER;
	if(status_fifo8(&key_fifo)>0)
		flags|=REQ_KEYBOARD;
	if(status_fifo8(&mouse_fifo)>0)
		flags|=REQ_MOUSE;
	return flags;
}

void req_handler(u32 reqs);
extern ThreadCtl *ptr_thctl;

namespace tmpCode
{
	int readFileAll(fs::FAT12Info *file,int size,char *buffer)
	{
		using namespace fs;
		int success=0;//byteCount of successfully read
		int clust=file->clust_lo16;
		char str[0x100];
		for(int i=0;i <size ; i+=0x200)
		{
			sprintf(str,"clust_no=%x\n",clust);
			DEBUG_PRINT(str);
			if(clust <= 1 || clust == 0xff7)
			{
				DEBUG_PRINT("error: invalid clustor number\n");
				return -1;
			}
			if(clust <= 1 || clust >= 0xff7)
				break;
			if(size - i >= 0x200)
			{
				fat12img->lread(clust+fat12::CLUST_SECT_DIFF,buffer+i);
				success += 0x200;
			}
			else
			{
				char copy[0x200];
				fat12img->lread(clust+fat12::CLUST_SECT_DIFF,copy);
				memcpy(buffer + i,(const void*)copy,size - i);
				success+=size-i;
				break;
			}
			clust=fat12img->next(clust);
			if(clust <= -2 )
			{
				DEBUG_PRINT("Invalid next clustor@ tmpCode::readFile\n");
				return -1;
			}
			if(clust == -1)
				break;//end
		}
		return success;
	}
	int createOneWindow(void)
	{
		WindowInfo wi;
		char str[0x100];
		wi.reg.x=150;
		wi.reg.y=340;
		wi.reg.width=500;
		wi.reg.height=300;
		wi.wh = sampleWH;
		wi.parent=-1;
		int win1 = t_CreateWindow("test1",&wi);
		sprintf(str,"winID=%d",win1);
		DEBUG_PRINT(str);
		return 0;
	}
}


void HariMain(void)
{
	char str[512];
	u32 memtotal;

	init_gdtidt();
	init_pic();


	io_sti(); // IDT/PICの初期化が終わったのでCPUの割り込み禁止を解除
	
	

	io_out8(PIC0_IMR, 0x00); // 1111 1001（PIC1とキーボードを許可）->all
	io_out8(PIC1_IMR, 0x00); // 1110 1111（マウスを許可）->all
		
	set_palette(COL8_BLACK, COL8_DARKGRAY, init_palette());



//Memory
	{
		MemMng* mem=MEMMNG_ADDR;
		memtotal=memtest(0x00400000,0xbfffffff);
		memmng_init(mem);
		memmng_free(mem,(char*)0x500000,memtotal-0x500000);
	}
//init of LayerCtl
	LayerCtl* lc=(LayerCtl*)memmng_alloc_4k(sizeof(LayerCtl));
	layerctl=lc;
	layerctl_init(lc,pbinfo->scrnx,pbinfo->scrny);
	{
		uid_t lybg=layer_alloc(lc);
		layer_fill8(lc,lybg,COL8_GREEN,1);
	}
	uid_t lycons=layer_alloc(lc);
	l_console=lycons;
	(void)mouse_initdraw();

	
	
	//Devices
	init_keyboard();
	enable_mouse();

	//Checking Memory
	{
		sprintf(str,"memfree:0x00400000-0x%08x\r\n",memtotal);
		layer_print(lc,lycons,str);
	}
	init_pit();
	set_timer(100,SET_TIMER_PERIODIC);
	int win0;

	//Window
	
	{
		window_init();
		WindowInfo wi;
		wi.reg.x=450;
		wi.reg.y=140;
		wi.reg.width=500;
		wi.reg.height=600;
		wi.wh = sampleWH;
		wi.parent=-1;
		win0 = t_CreateWindow("test0",&wi);
		sprintf(str,"winID=%d",win0);
		tmpCode::createOneWindow();
		DEBUG_PRINT(str);
	}
	
//Thread
	{
		int thid;
		threadctl_init();
		thid=thread_create(inf_loop,(void*)win0,0,0);
		sprintf(str,"thid=%d\n",thid);
		DEBUG_PRINT(str);
		load_tr(TASK_BOOT*8);
		dump_threadlist();
		thread_terminate(thid);
		proc::ProcCtl::init();
	}
	device::floppy::disk_readall();//reads all contents of disk
	
//filesystem
	{
		using namespace fs;
		//using namespace device::floppy;
		unsigned char* diskimg=(unsigned char*)ADDR_DISKIMG;
		int i;
		fat12img =mem::talloc<FAT12Image>();
		fat12img->ctor(diskimg);
		
		//Changing to buffering mode
		fat12img->chmod(FAT12Image::BUFF_STORED);
		{
			//calculating sum in buffering mode
			int sum=0;
			for(int i=0;i < device::floppy::F35_SECTOR_COUNT;i++)//2880
			{
				char buf[0x200];
				fat12img->lread(i,buf);
				for(int j=0;j<0x200;j++)
					sum+=(buf[j]&0xff)*(i*0x200+j);
			}
			sprintf(str,"checksum2 := %08x\n",sum);
			DEBUG_PRINT(str);
		}
	}
//io-manager init
	io::iomng::IOManager::init();
	//Creating new process
	proc::procctl->start_app("sum.hrb");

	while(1) 
	{
		u32 reqs=get_request();
		req_handler(reqs);//メイン処理
	}
	while(1)
		io_hlt(); // HLT命令でCPUを休ませておく
}
void req_handler(u32 reqs)
{
	int eflags=load_eflags();
	io_cli();
	if(reqs==0)
	{
		io_stihlt();
		return;
	}
	if(reqs&REQ_TIMER)
	{
		int id=get_fifo32(&timerctl.notifyIDs);
		int count=timerctl.count;
		io_sti();
		(void)id;(void)count;//reference
	}
	if(reqs&REQ_KEYBOARD)
	{
		(void)kb_event();//io_sti in  this  function
	}
	if(reqs&REQ_MOUSE)
	{
		(void)mouse_event();//io_sti() in this function
	}
	store_eflags(eflags);
}
