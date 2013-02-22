#include "fat12.h"
#include <string.h>
#include <stdio.h>
#include "../layer.h"
namespace fs
{
	namespace fat12
	{
		int nameParse(const char *filename,char name[8+1],char ext[3+1])
		{
			int len=strlen(filename);
			if(len>=8+1+3+1)//too long,max=8+'.'+3
			{
				return (int)0xf11e0002;//filename too long
			}
			int extlen=0;
			for(int i=1;i<=3;i++)
			{
				if(filename[len-i-1]=='.')
				{
					extlen=i;
					break;
				}
			}
			int namelen=len-extlen-(extlen==0?0:1);
			if(namelen>=8+1)
			{
				return (int)0xf11e0003;
			}
			memcpy(name,filename,namelen);
			memset(name+namelen,0,8+1-namelen);
			memcpy(ext,filename+len-extlen,extlen);
			memset(ext+extlen,0,3+1-extlen);
			return 0;
		}
	}
	void fat12_readfat(int fat[2880*2],unsigned char const* img)
	{
		int NumFat=2;//It seems to be constant. 2 FATs.
		int i, j = 0;
		int fat_sect=*(unsigned short const*)(img + 0x0e);
		img += fat_sect * FAT12_SECTOR_SIZE; //getting address of FAT in diskimg
		for (i = 0; i < 2880; i += 2) {
			fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
			fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
			j += 3;
		}
	}
	FAT12Image::FAT12Image(void){}
	void FAT12Image::ctor(uchar *rawimg)
	{
		char str[256];
		sprintf(str,"sizeof F12LFN = %d\n",sizeof(FAT12LongFilenameEntry));
		DEBUG_PRINT(str);
		fat12_readfat(this->fat,rawimg);
		this->fat_mod=false;
		this->mode=ALL_READ;
		this->raw = rawimg;
		//cyl_buf is not used now.
		this->cyl_buf_num=-1;
		this->cyl_buf_mod=false;
		if(checkDual() != 0)DEBUG_PRINT("2 FATs are not identical.\n");
		else DEBUG_PRINT("2 FATs are identical.\n");
	}
	void FAT12Image::chmod(reading_mode rm)
	{
		this->mode=rm;
	}
	FAT12Info *FAT12Image::getInfo(int dirAddr,int num,const char* filename,const char *ext)
	{
		FAT12Info *start=(FAT12Info*)(this->raw + dirAddr);
		char names[11];
		for(int i=0;i<11;i++)
			names[i]=0x20;//white space,' '.
		for(int i=0;i<8;i++)
		{
			if(filename[i] == (char)0)
				break;
			names[i]=filename[i];
		}
		for(int i=0;i<3;i++)
		{
			if(ext[i] == 0)break;
			names[8+i] = ext[i];
		}
		for(int i=0;i<11;i++)
		{//To Upper Case if lower case
			if((uint)(names[i]-'a') <= (uint)('z' -'a'))//To reduce the number of comparison
				names[i] -= 0x20;//in ASCII code,'A' thru 'Z' , 'a' thru 'z' are continuous,respectively.
		}
		for(int i=0;i<num;i++)
		{//Compares one by one
			FAT12Info *current=start + i;
			/*Because FAT12Info::name[8] & FAT12Info::ext[3] are continuous,
			this hack works. */
			if(memcmp(current->name,names,11) != 0)
				continue;
			//We found the file entry.
			return current;
		}
		return (FAT12Info*)0;//No entry was found.
	}
	int FAT12Image::next(int clustno)const
	{
		if(clustno <= 1)
			return -3;
		if(clustno >= FAT12_SECTOR_COUNT)
			return -3;
		int val=fat[clustno];
		if(val >= 2 && val <=0xff6)
			return val;
		if(val == 0xff7)//ˆÙí
			return -2;
		if(val >= 0xff8 && val <=0xfff)//end
			return -1;
		return -4;//Internal Error.
	}
	int FAT12Image::lread(int clust,char buf[0x200])
	{
		using namespace device::floppy;
		if(clust < 0 || clust >= F35_SECTOR_COUNT)
			return -1;//error
		switch(this->mode)
		{
		case ALL_READ:
			memcpy(buf,(const char*)this->raw + (0x4200 - 2 * 0x200 )+ clust *0x200 ,0x200);
			break;
		case BUFF_STORED:
		{
			//Checks if target track is buffered or not
			int cyl_num=clust/(18*2);//0..79,The number of cylinder to read
			if(cyl_num == this->cyl_buf_num)
			{//The data is buffered
				memcpy(buf,this->cyl_buf+FAT12_SECTOR_SIZE*(clust-(CYLINDER_SIZE/SECTOR_SIZE)*cyl_num),SECTOR_SIZE);//writing now...
			}
			else
			{//not buffered
				start_motor();
				if(cyl_buf_mod)//we must write out the buffer
				{
					memcpy(FREE_SPACE,this->cyl_buf,TRACK_SIZE);
					disk_write((int)FREE_SPACE,this->cyl_buf_num,0);
					memcpy(FREE_SPACE,this->cyl_buf+TRACK_SIZE,TRACK_SIZE);
					disk_write((int)FREE_SPACE,this->cyl_buf_num,1);
				}
				disk_read((int)FREE_SPACE,cyl_num,0);
				memcpy(this->cyl_buf,FREE_SPACE,TRACK_SIZE);

				disk_read((int)FREE_SPACE,cyl_num,1);
				memcpy(this->cyl_buf+TRACK_SIZE,FREE_SPACE,TRACK_SIZE);

				this->cyl_buf_num=cyl_num;
				cyl_buf_mod=false;

				stop_motor();

				//Copies to the given buffer {buf}
				memcpy(buf,this->cyl_buf+0x200*(clust - 18 * 2 * cyl_num),0x200);
			}
			break;
		}
		default:
			break;
		}
		return 0;
	}
	int FAT12Image::checkDual(void)
	{
		int fatSize= (*(unsigned short const*)(raw + 0x16))*FAT12_SECTOR_SIZE;
		//Check if 2 FATs are completely equal
		uchar *fat_area=raw+ *(unsigned short const*)(raw + 0x0e)* FAT12_SECTOR_SIZE;
		int res = memcmp(fat_area,fat_area+fatSize,fatSize);
		return (res == 0) ? 0 : -1;
	}
}

