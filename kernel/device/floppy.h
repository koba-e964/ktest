#ifndef DISK_H_094daaaa
#define DISK_H_094daaaa
namespace device
{
	namespace floppy
	{
		void* const FREE_SPACE=(void*)0x26d400;//thru 0x26f800,9KiB,in 64KiB border
		const int SECTOR_SIZE=0x200;
		const int TRACK_SIZE=0x2400;//18 Sector,9 KiB
		const int CYLINDER_SIZE=0x4800;//2 tracks,18KiB
		const int F35_SECTOR_COUNT=2880;
	
	/**
		The contents to write into the disk must be in one 64KiB block.
	*/
		int disk_read(int addr,int cyl,int head);
		int disk_write(int addr,int cyl,int head);
		int disk_readall(void);
		void start_motor(void);
		void stop_motor(void);
	}
}
extern "C" void inthandler26(int *esp);

#endif
