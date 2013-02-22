#include "all.h"
#include <string.h>
#include <stdio.h>

uint memtest_sub(char*,char*);
extern "C"
uint memtest_sub_iasm(char* start,char* end);


#define EFLAGS_AC_BIT 0x40000
#define CR0_CACHE_DISABLE 0x40000000

uint memtest(uint start,uint end)
{
	char b486=0;
	uint eflg,cr0,i;


	eflg=load_eflags();
	eflg|=EFLAGS_AC_BIT;
	store_eflags(eflg);
	eflg=load_eflags();
	if((eflg&EFLAGS_AC_BIT)!=0)//on 386,even if you let ac=1,ac will automatically cleared.
		b486=1;
	eflg&=~EFLAGS_AC_BIT;
	store_eflags(eflg);
	if(b486!=0)
	{
		cr0=load_cr0();
		cr0|=CR0_CACHE_DISABLE;//CPU is prevented from caching.
		store_cr0(cr0);
	}

	//i=memtest_sub((char*)start,(char*)end);
	i=memtest_sub_iasm((char*)start,(char*)end);
	if(b486!=0)
	{
		cr0=load_cr0();
		cr0&=~CR0_CACHE_DISABLE;//CPU is allowed to cache.
		store_cr0(cr0);
	}

	return i;
}
void memmng_init(MemMng* memmng)
{
	memmng->nFrees=0;
	memmng->maxfrees=0;
	memmng->lostsize=0;
	memmng->losts=0;
	memset(memmng->free,0xcc,sizeof(FreeInfo)*MEMMNG_FREES);
}
uint memmng_free(MemMng* mem,void* addr,int size)
{
	//freeÇÕaddrèá
	//Ç‹Ç√ Ç¢ÇÈÇ◊Ç´ÇŒÇµÇÂÇ Ç›Ç¬Ç≠
	int ind=0;
	char* start=(char*)addr;
	char* end=start+size;
	
	for(;ind<mem->nFrees;ind++)
	{
		if(mem->free[ind].end < start)//[ind].start [ind].end start end
			continue;
		if(mem->free[ind].end==start)
		{
			mem->free[ind].end=end;
			if(ind<MEMMNG_FREES-1 && end==mem->free[ind+1].start)
			{
				mem->free[ind].end=mem->free[ind+1].end;
				//erase
				erase_array(mem->free,ind+1,ind+2,mem->nFrees,sizeof(FreeInfo));
			}
			return 0;//success
		}
		if(mem->free[ind].start < end)//not start end [ind].start [ind].end
		{
			mem->lostsize+=size;
			mem->losts+=1;
			return (uint)-1;//failure
		}
		if(end==mem->free[ind].start)
		{
			mem->free[ind].start=start;
			return 0;//success
		}
		break;
	}
	FreeInfo insertee={start,end};
	if(0<=insert_array(mem->free,ind,&insertee,1,mem->nFrees,MEMMNG_FREES,sizeof(FreeInfo)))
	{
		mem->nFrees++;
		make_max(mem->maxfrees,mem->nFrees);
		return 0;//success
	}
	return (uint)-1;//failure
}
void* memmng_alloc(MemMng* mem,uint size)
{
	int ind,mark=-1;
	for(ind=0;ind<mem->nFrees;ind++)
	{
		FreeInfo cur=mem->free[ind];
		uint cursize=cur.end-cur.start;
		if(mark == -1 && cursize==size)
		{
			mark=ind;
		}
		if(cursize>size)
		{
			char* ptr=cur.start;
			mem->free[ind].start+=size;
			return ptr;
		}
		//---------------------------------
	}
	if(mark==-1)
	{
		return NULL;
	}
	//[mark]
	void* ptr=mem->free[mark].start;
	erase_array(mem->free,mark,mark+1,mem->nFrees,sizeof(FreeInfo));
	mem->nFrees--;
	return ptr;
}
void* memmng_alloc_4k(int size)
{
	size=(size+0xfff)&~0xfff;
	void* ptr=memmng_alloc(MEMMNG_ADDR,size);
	return ptr;
}
int memmng_free_4k(void* ptr,int size)
{
	if(ptr == (void*)0)return 0;
	size=(size+0xfff)&~0xfff;
	return memmng_free(MEMMNG_ADDR,ptr,size);
}

namespace mem
{
	void dump_memory(char *ptr,int len)
	{
		char str[256];
		for(int i=0;i<len;i++)
		{
			if((i&15)==0)
			{
				sprintf(str,"%08x:",(int)ptr+i);
				DEBUG_PRINT(str);
			}
			sprintf(str,"%02x%s",((char*)ptr)[i]&0xff,((i&15)==15)?"\n":" ");
			DEBUG_PRINT(str);
		}
	}
}
int dump_memlist(FreeInfo * fi,int len)
{
	int i;
	MemMng* mem=MEMMNG_ADDR;
	int dumplen=(len>mem->nFrees)?mem->nFrees:len;
	for(i=0;i<dumplen;i++)
	{
		fi[len]=mem->free[i];
	}
	return mem->nFrees;
}


