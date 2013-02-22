#ifndef FS_FAT12_H_ee5c9a7f
#define FS_FAT12_H_ee5c9a7f
/****
	fs/fat12.h
*/
#include "../types.h"
extern "C"
void* memcpy(void *dest,const void *src,size_t size);
namespace fs
{
	enum FAT12Consts
	{
		FAT12_FILEINFO_OFFSET =0x2600,//0x200+0x200*18
		FAT12_SECTOR_SIZE     =0x0200,//ADDR_DISKIMG+0x0e‚Ì’l*0x0200
		//FAT12_FAT_LEN=2880,
		FAT12_CONTENTS_OFFSET =0x3e00,//0x2600+0x200*12
		FAT12_SECTOR_COUNT=2880,//Deprecated. Consider using device::FLOPPY_35_SECTOR_COUNT (device/floppy.h).
	};
	namespace fat12
	{
		const int CLUST_SECT_DIFF=0x1f;
		/**
			filename‚ð8.3Œ`Ž®‚É•ÏŠ·‚·‚é.
			ext‚Æ‚µ‚Ä‚Í‚ ‚è“¾‚é’·‚³(1,2,3)‚Ì‚¤‚¿Å’Z‚Ì‚à‚Ì‚ð‘I‚Ô.‚»‚êˆÈŠO‚Í0‚Æ‚È‚é.
			name‚Æ‚µ‚Ä‚Ílen(ext)=0‚Ìê‡‚Ífilename,‚»‚êˆÈŠO‚Ìê‡‚Í'.'‚ðŠÜ‚Ü‚È‚¢ext‚ðœ‚­filename
			‚É‚È‚é.
			len(name)>8 ‚Ìê‡‚ÍŽ¸”s‚·‚é.
			ÅI“I‚ÉŠi”[‚³‚ê‚é•¶Žš—ñ‚ªˆø”‚Ì’·‚³(8+1,3+1)‚É•s‘«‚·‚éê‡,‚ ‚Ü‚è‚Í'\0'‚Å–„‚ß‚ç‚ê‚é.
			“Á‚É,null-terminated‚È•¶Žš—ñ‚É‚È‚é.
			@retval ¬Œ÷‚Ìê‡‚Í0,Ž¸”s‚Ìê‡‚Í•‰‚Ì’l
		*/
		int nameParse(const char *filename,char name[8+1],char ext[3+1]);
	}
	/**
		File entry type in FAT12.
		The prefix of each member variable shows ... .
		m -> made
		a -> accessed
		w -> wrote
		{type} :	0x01 -> Read-Only
				0x02 -> Hidden
				0x04 -> System
				0x08 -> Volume Label
				0x10 -> Directory
				0x20 -> Compressed
				0x0f -> Long file name
		{lower_case} :	0x08 -> 8 chars in filename is lowercase.
				0x10 -> The extension is lowercase.
	*/
	typedef struct{
		char name[8],ext[3],type;
		char lower_case,m_10milli;
		short m_time,m_date;
		short a_date,clust_hi16,w_time,w_date,clust_lo16;
		/*{clust_hi16} is only used in FAT32 in order to store the higher 16 bits of cluster number.*/
		unsigned int size;
	}FAT12Info;

	struct FAT12LongFilenameEntry
	{
		char number;
		char name1[10];
		char attr, zero ,checksum;
		char name2[12];
		short zero2;
		char name3[4];
		
	};
	
	/**
		@retval fileno
	*/
	void fat12_readfat(int fat[2880*2],unsigned char const*img);
	
	int fat12_search(char const* name,FAT12Info *finfo,int max);
	/**
		We have to use this class carefully because some member functions in this class
		return pointers the instance retains.
	*/
	class FAT12Image
	{
	public:
		enum reading_mode{
			ALL_READ=1,
			BUFF_STORED=2,
		};
		FAT12Image(void);
		void ctor(uchar* rawimage);
		void chmod(reading_mode rm);
		/**
			Searches the directory entries from dirAddr to dirAddr+num*0x20 in disk image.
			returns the file entry whose name is {filename + "." + ext }.
			If it was not found ,this function returns NULL.
			cutOrFill({filename},8);
			cutOrFill({ext},3);
			cutOrFill(name,len):if(name.length >= len) cut;
			else fill the blank with ' ' (0x20).
			Example: cutOrFill("dde",2) -> "dd"
			cutOrFill("wwd",4) -> "wwd\x20"
			@retval The ptr to FileEntry, or NULL.
		*/
		FAT12Info *getInfo(int dirAddr,int num,const char *filename,const char *ext);
		inline FAT12Info *getInfoByIndex(int dirAddr,int index)
		{
			return ((FAT12Info*)(this->raw+dirAddr))+index;
		}
		/**
		@retval -1 : End. otherwise -> the next clustor number of {clust}.
		*/
		int next(int clust)const;
		int lread(int linSectNO,char buf[512]);
		int lwrite(int linSectNO,const char buf[512]);
		/**
			Writes FATs,FileDirectories,and Contents into the floppy disk.
			Before unmounting the disk,we should call this function.
		*/
		int update(void);
	public:
		int fat[FAT12_SECTOR_COUNT * 2];
		bool fat_mod;//If fat was modified
		reading_mode mode;//All-read or Buffering-Stored
		uchar *raw;
		//cyl_buf contains the data(a cylinder) (excluding fat)read recently.
		uchar cyl_buf[18*2*FAT12_SECTOR_COUNT];//Buffer of a cylinder (18 KiB)
		int cyl_buf_num;//The numbre of track whose data cyl_buf contains.
		bool cyl_buf_mod;//If cyl_buf was modified.
	private:
		/**
		* Checks if 2 FATs are identical.
		* @retval 0 -> identical , -1 -> not
		*/
		int checkDual(void);
	};

}//namespace fs
#endif
