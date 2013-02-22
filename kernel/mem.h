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
		maxfrees,//frees�̍ő�l
		lostsize,//������s�T�C�Y
		losts;//������s��
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
	�f�o�b�O�p�֐�
	memmng�̊Ǘ��f�[�^���A
	fi�ɓ����B
	fi:buffer
	len:fi�̒���
	�߂�l:memmng�̊Ǘ��f�[�^�̒���
*/
#ifdef TEST
int dump_memlist(FreeInfo* fi,int len);
#endif

#endif
