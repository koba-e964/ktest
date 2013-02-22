#include "./floppy.h"
#include <stdio.h>
#include "../util/naskfunc.h"


/**
	Track = 18 Sectors
	Cylinder = 2 Tracks = 36 Sectors;

	512 * 18 * 2 * 80 = 1474560
	0.5 KiB *18 * 2 * 80 = 1440 KiB
*/
#if 0
#undef  DEBUG_PRINT
#define DEBUG_PRINT(str) 
#endif

namespace device
{
	namespace 
	{
		const int SECTOR_SIZE=512,
		SECTOR_NUM=18,
		HEAD_NUM = 2,
		CYL_NUM=80;
		volatile bool fdc_inted=false;
	}
enum FloppyCmds
{
	READ_TRACK =2,
	SPECIFY =3,
	SENSE_DRIVE_STATUS =4,
	WRITE_DATA =5,
	READ_DATA =6,
	RECALIBRATE =7,
	SENSE_INT =8,
	WRITE_DELETED_DATA = 9,
	READ_ID = 10,
	READ_DELETED_DATA = 12,
	FORMAT_TRACK = 13,
	SEEK = 15,
	VERSION = 16,
	SCAN_EQUAL = 17,
	PERPENDICULAR_MODE =18,
	CONFIG =19,
	LOCK =20,
	VERIFY =22,
	SCAN_LO_OR_EQ =25,
	SCAN_HI_OR_EQ =29,
};

void send_fdc_cmd(char *cmd,int len)
{
	if(cmd[0] != 0x08)
		while((io_in8(0x03f4) &0x11) !=0){}
	else
		while((io_in8(0x03f4) & 0x10 ) != 0){}
	while(len > 0)
	{
		if((io_in8(0x03f4)&0xc0)!=0x80)
			continue;
		io_out8(0x03f5,*cmd);
		cmd++,len--;
	}
}
void get_fdc_cmd(char *cmd,int len)
{
	while(len > 0)
	{
		while((io_in8(0x03f4)&0xc0) != 0xc0){io_hlt();}
		*cmd=io_in8(0x03f5);
		cmd++;
		len--;
	}
}

int check_st0(char st0)
{
	switch(st0 & 0xc0)
	{
	case 0x00://success
		break;
	case 0x40://
		DEBUG_PRINT("Error 0x40(fdc)\n");
		return -1;
	case 0x80://mistake
		DEBUG_PRINT("Bad Command (fdc)\n");
		return -1;
	}
	return 0;
}

void fdc_queryint(void)
{
	io_sti();
	while(fdc_inted == false)io_hlt();
	char cmd[1]={0x08};
	send_fdc_cmd(cmd,1);
	char res[2];
	get_fdc_cmd(res,2);
	fdc_inted = false;
	check_st0(res[0]);
	
}
void fdc_recalibrate(void)//recalibrate
{
	char cmd[2]={0x07,0x00};
	send_fdc_cmd(cmd,2);
	fdc_queryint();
}
void fdc_seek(int cyl,int head)
{
	char cmd[3]={SEEK,head<<2,cyl};
	send_fdc_cmd(cmd,3);
	fdc_queryint();
}
	void fdc_read(int cyl,int head,int sect,int seccount)
	{
		//char cmd[9]={READ_DATA,head<<2,cyl,head,sect,2,seccount,0x1b,0xff};
		char cmd[9]={0xe6,head<<2,cyl,head,sect,0x02,0x12,1,0xff};
		for(int i=0;i<5;i++)
		{
			send_fdc_cmd(cmd,9);
			while(!fdc_inted)io_hlt();
			fdc_inted=false;
			char res[7];
			get_fdc_cmd(res,7);
			int result=check_st0(res[0]);
			if(result == 0)break;
			fdc_recalibrate();
			fdc_seek(cyl,head);
		}
	}
namespace floppy
{
	int disk_read(int addr,int cyl,int head)
	{
		io_out8(0x000b,0x06);
		io_out8(0x0005,0xff);
		io_out8(0x0005,18*2-1);//0x23ff bytes
		io_out8(4,addr&0xff);
		io_out8(4,(addr>>8)&0xff);
		io_out8(0x81,(addr>>16)&0xff);
		io_out8(0xa,0x02);
		fdc_recalibrate();
		fdc_seek(cyl,head);
		fdc_read(cyl,head,1,18);
		io_out8(0xa,0x06);
		return 0;
	}
	int disk_write(int addr,int cyl,int head)
	{
		return -1;//fail always
	}
	int disk_readall(void)
	{
		char* addr=(char*)0x26d400;//Free space
		//int len=0x2400;//9 Ki Bytes
		/**
		assert(len > 0);
		assert((addr &0xff0000)==((addr+len-1)&0xff0000));//[addr,addr+len) does not exceed a 64 KiB boundary.
		assert((addr+len)<=0x1<<24);//16MiB Limitation
		*/
		io_sti();
		io_out8(0x00d6,0xc0);
		io_out8(0x00c0,0x00);
		io_out8(0x000a,0x06);
		io_out8(0x03f2,0x1c);//Start motor
		uSleep(20);
		for(int i=ADDR_BOOTINFO->cyls;i<CYL_NUM;i++)
		{
			const int TRACK=SECTOR_NUM *SECTOR_SIZE;//0x2400 bytes
			disk_read((int)addr,i,0);
			memcpy((char*)ADDR_DISKIMG + (HEAD_NUM * i + 0) *TRACK,
				addr,TRACK);
			disk_read((int)addr,i,1);
			memcpy((char*)ADDR_DISKIMG + (HEAD_NUM * i + 1) *TRACK,
				addr,TRACK);
		}
	
		io_out8(0x03f2,0x0c);//Stop motor
		//test
		//calculating the checksum
		int sum=0;
		for(int i=0;i<CYL_NUM*HEAD_NUM*SECTOR_NUM* SECTOR_SIZE;i++)
		{
			sum += (((char*)ADDR_DISKIMG)[i] & 0xff)*i;
		}
		char str[256];
		sprintf(str,"fd_checksum=%08x\n",sum);
		DEBUG_PRINT(str);
		return 0;
	}
	bool bMoving=false;
	void start_motor(void)
	{
		if(bMoving)
			return;
		io_out8(0x03f2,0x1c);
		//uSleep(200);
		//thread_suspend(thread_current());
		bMoving=true;
	}
	void stop_motor(void)
	{
		if(!bMoving)
			return;
		io_out8(0x03f2,0x0c);
		bMoving=false;
	}
}//namespace floppy
}//namespace device
void inthandler26(int *esp)
{
	io_in8(0x03f4);
	io_out8(PIC0_OCW2,0x66);
	//DEBUG_PRINT("int26\n");
	device::fdc_inted=true;
	return;
}
