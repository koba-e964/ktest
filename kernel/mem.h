#ifndef _MEM_H_
#define _MEM_H_
#include "all.h"
typedef struct FreeInfo
{
	char* start;
	char* end;
}FreeInfo;
#define MEMMNG_FREES 0xffa
#define MEMMNG_ADDR ((MemMng*)0x3c0000)
typedef struct MemoryManager
{
	int nFrees,
		maxfrees,//freesの最大値
		lostsize,//解放失敗サイズ
		losts;//解放失敗回数
	FreeInfo free[MEMMNG_FREES];//about 32KiB
}MemMng;
uint memtest(uint start,uint end);

void memmng_init(MemMng*);
uint memmng_total(MemMng*);
void* memmng_alloc(MemMng*,uint size);
uint memmng_free(MemMng*,void* start,int size);

void* memmng_alloc_4k(int size);
int memmng_free_4k(void* ptr,int size);

namespace mem
{
	template<class T>
	inline T* talloc(void)
	{
		return (T*)memmng_alloc_4k(sizeof(T));
	}
	template<class T>
	inline int tfree(T* ptr)
	{
		return memmng_free_4k((void*) ptr , sizeof(T));
	}
	void dump_memory(char *addr,int len);
}//namespace mem
/**
	デバッグ用関数
	memmngの管理データを、
	fiに入れる。
	fi:buffer
	len:fiの長さ
	戻り値:memmngの管理データの長さ
*/
#ifdef TEST
int dump_memlist(FreeInfo* fi,int len);
#endif

#endif
