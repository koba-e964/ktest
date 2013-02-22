#include "all.h"
#include <stdio.h>

ThreadCtl* ptr_thctl;
#define thctl (*ptr_thctl)
#define SWITCH_INTERVAL 3UL

int mt_tr;

int threadctl_init(void)
{	
	SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *) ADR_GDT;

	int id=set_timer(SWITCH_INTERVAL, SET_TIMER_PERIODIC);
	
	ptr_thctl=mem::talloc<ThreadCtl>();

	thctl.interval=SWITCH_INTERVAL;
	int i=0;
	thctl.threads.ctor(ThreadInfo(),THREAD_MAX);
	thctl.list.ctor();
	thctl.suspended.ctor (false,THREAD_MAX);
	for(i=0;i<THREAD_MAX;i++)
	{
		ThreadInfo *thinfo=&thctl.threads[i];
		//thinfo->sel=(TASK_GDT0 + i)<<3;
		thinfo->tss.ldtr=task::LDTInGDT(i) * 8;
		set_segmdesc(gdt+TASK_GDT0+i,sizeof(TSS32)-1,
				(i32)&thinfo->tss,AR_TSS32);
		set_segmdesc(gdt + task::LDTInGDT(i),(sizeof(SEGMENT_DESCRIPTOR))*THREAD_LDTS-1,
				(i32)thinfo->ldt,AR_LDT);
	}
	thctl.threads_used[0]=0x1;//thread numbre 0(cs=3*8)は使われている
	for(i=1;i<arraylen(thctl.threads_used);i++)
	{
		thctl.threads_used[i]=0;
	}
	for(int i=1;i<THREAD_MAX;i++)
	{
		thctl.threads[i].state=THREAD_ABSENT;
	}
	thctl_timer=id;
	mt_tr=TASK_BOOT;
	thctl.list.add(0);//add thread 0(cs=3)
	return id;
}

/**
	Deprecated!!
		1つしかtaskを作れない
	flags: ignored
*/
int thread_create(int(*proc)(void*),void* argv,int level,u32 flags)
{
	int thnum=thread_alloc();
	{
		ThreadInfo* ti=&thctl.threads[thnum];
		ti->tss.eflags=0x202;//if=1
		ti->tss.eip=(i32)proc;
		ti->tss.ldtr=0;
		ti->tss.iomap=0x40000000;
		
		ti->tss.esp=(i32)memmng_alloc_4k(0x10000)+0x10000
				-8;//argv用の空き

		ti->tss.es=1*8;
		ti->tss.cs=2*8;
		ti->tss.ss=1*8;
		ti->tss.ds=1*8;
		ti->tss.fs=1*8;
		ti->tss.gs=1*8;
	

		ti->state=THREAD_RUNNING;
		int i;
		for(i=0;i<THREAD_LDTS;i++)
			memset(&ti->ldt[i],0,sizeof(SEGMENT_DESCRIPTOR));

		ti->argv=argv;
		((void**)(ti->tss.esp))[1]=argv;//stackに積む
		ti->level=level;
	}
	thread_add(thnum);
	return thnum;
}
int thread_alloc(void)
{
	int thnum;
	{//あいているthctl.threads[*]を見つけ、thnumを決定する
		thnum=find_bit_in_array(thctl.threads_used,0,THREAD_MAX);
		if(thnum==-1)
			return -1;//failure
		thctl.threads_used[thnum/8]|=1<<(thnum&7);//1にする
	}
	return thnum;
}
void thread_add(int thid)
{
	thctl.list.add(thid);
	thctl.nThread++;
}
bool thread_exists(int thid)
{
	return (thctl.threads_used[thid/8]&(1<<(thid%8)))!=0;
}
int thread_switch_byid(int thid)
{
	char str[256];
	int mt_tr=thid+TASK_GDT0;
	sprintf(str," mt_tr=%08x\n",mt_tr);
	//DEBUG_PRINT(str);
	task_change(mt_tr*8);
	return 0;
}
int thread_suspend(int thid)
{
	char buf[256];
	io_cli();
	//DEBUG_PRINT("thread_suspend111 ");
	//dump_threadlist();
	int current=thctl.list.currentIndex();
	if(!thread_exists(thid))
		return -1;
	if(thctl.threads[thid].state != THREAD_RUNNING)
	{
		return -2;
	}
	if(thctl.list.loopLength()==1)//スレッド0個の危機
		return -3;
	thctl.list.erase(thid);
	thctl.suspended[thid]=true;
	thctl.threads[thid].state=THREAD_SUSPENDED;
	//DEBUG_PRINT("thread_suspend124 ");
	//dump_threadlist();
	io_sti();
	if(current==thid)
	{
		thread_switch_byid(thctl.list.currentIndex());
	}
	return 0;
}
int thread_resume(int thid)
{
	io_cli();
	if(!thread_exists(thid))
	{
		io_sti();
		return -1;
	}
	thctl.list.add(thid);
	if(thctl.suspended[thid])
	{
		thctl.suspended[thid]=false;
	}
	thctl.threads[thid].state=THREAD_RUNNING;
	io_sti();
	return 0;
}
int thread_terminate(int thid)
{
	DEBUG_PRINT("thread_terminate:\n");
	bool bSelf = thid == thread_current();
	if(! thread_exists(thid))
	{
		return -1;//error
	}
	if(thctl.list.loopLength() <= 1 && bSelf)//If the thread is terminated,there will be no thread.
		return -3;//error
	if(thctl.threads[thid].state == THREAD_RUNNING)
		thctl.list.erase(thid);
	if(thctl.threads[thid].state == THREAD_SUSPENDED)
		thctl.suspended[thid]=false;
	thctl.threads_used[thid/8] &= 0xff ^ (1 << (thid % 8 ));
	thctl.threads[thid].state=THREAD_ABSENT;
	if(bSelf)
	{
		thread_switch_byid(thctl.list.currentIndex());
	}
}

int thread_current(void)
{
	return thctl.list.currentIndex();
}

int thread_switch(void)
{
	char str[256];
	//DEBUG_PRINT("thread_switch() : begin\n");
	int cur=thctl.list.currentIndex();
	thctl.list++;
	int after=thctl.list.currentIndex();
		sprintf(str,"thread_switch(void):cur=%d,after=%d\n",cur,after);
	if(after == cur)//There is only 1 thread.
		return -1;
	thread_switch_byid(after);
	//DEBUG_PRINT("thread_switch() : end\n");
	return 0;
}

namespace task
{
	int loadCurrentToKernel(int ldtnum,u32 procmem,int len,void *dest)
	{
		int thid=thread_current();
		return loadThreadToKernel(thid,ldtnum,procmem,len,dest);
	}
	int loadThreadToKernel(int thid,int ldtnum,u32 procmem,int len,void *dest)
	{
		using namespace dsctbl;
		if(0>ldtnum&&ldtnum>=THREAD_LDTS)
		{
			return (int)0xbad1d400;
		}
		SEGMENT_DESCRIPTOR *sd=ptr_thctl->threads[thid].ldt+ldtnum;
		const void *phys=getPhysAddr(sd,procmem);
		const void *physend=getPhysAddr(sd,procmem+len-1);
		if(phys==(void*)0 || physend==(void*)0)//getPhysAddr failed.
		{
			return 0xbadadd01;
		}
		//It is assumed that segment(kernel.data).base==0 and that segment(thid.data) is continuous.
		memcpy(dest,phys,len);
		return 0;
	}
}



#define reg_dump(regname,reg) reg_dump_00(regname"=%08x\n",thctl.threads[i].tss.reg)

void reg_dump_00(const char* format,int reg)
{
	char str[256];
	sprintf(str,format,reg);
	DEBUG_PRINT(str);
}
int dump_thctl(void)
{
	int i=0;
	char str[256];
	sprintf(str,"thctl:\n  interval:%d\n  nThread:%d\n",thctl.interval,
			thctl.nThread);
	DEBUG_PRINT(str);
	for(i=0;i<2;i++)
	{
		sprintf(str,"thread[%d]:\n",i);DEBUG_PRINT(str);
		DEBUG_PRINT("  tss:\n");

		
		reg_dump("eax",eax);
		reg_dump("ecx",ecx);
		reg_dump("edx",edx);
		reg_dump("ebx",ebx);
		reg_dump("esp",esp);
		reg_dump("ebp",ebp);
		reg_dump("esi",esi);
		reg_dump("edi",edi);
		reg_dump("eip",eip);
		reg_dump("es ",es);
		reg_dump("cs ",cs);
		reg_dump("ss ",ss);
		reg_dump("ds ",ds);
		reg_dump("fs ",fs);
		reg_dump("gs ",gs);
		
	}
	return 0;
}
void dump_threadlist(void)
{
	char buf[256];
	util::LoopListCpp<char,THREAD_MAX> *list=&thctl.list;
	DEBUG_PRINT("tasks:");
	int l=list->loopLength();
	sprintf(buf,"%d threads:",l);
	DEBUG_PRINT(buf);
	int cur=list->currentIndex();
	for(int i=0;i<l;i++)
	{
		sprintf(buf,"%d%s",i, i == l-1 ? "" : "->");
		DEBUG_PRINT(buf);
		cur=list->array[cur].next - list->array;
	}
	DEBUG_PRINT("\n");
}


