#include "./sys.h"
#include <stdio.h>
#include "../layer.h"
#include "../task.h"
#include "../io/iomng.h"

namespace proc
{
	namespace sys
	{
		int nio_proc(int low,int arg0,int arg1,int arg2)
		{
			switch(low)
			{
			case 0://kos_open
			{
				using namespace util::macros;
				int namelen=min(100,arg0);
				char filename[101];
				int result=task::loadCurrentToKernel
					(1 /*ldtnum for .data*/,(u32)arg1, namelen, filename);
				if(result==0)
				{
					filename[100]=0;//in order to guarantee that strlen(filename)<=100
				}
				else//failure
				{
					char str[0x80];
					sprintf(str,"errorcode=%08x\n",result);
					DEBUG_PRINT(str);
					return (int)0xbad51500;
				}
				int fd=proc::procctl->current()->open(filename,arg2);
				return fd;
			}
			case 2://write
			{
				void *buffer=memmng_alloc_4k(arg2);
				DEBUG_PRINT("nio_proc case 2:\n");
				task::loadCurrentToKernel(1,(u32)arg1,arg2,buffer);
				ProcData *current=proc::procctl->current();
				int res=current->write(arg0,buffer,arg2);
				memmng_free_4k(buffer,arg2);
				return res;
			}
			default:
				break;
			}
			return -1;
		}
	}//namespace sys
}//proc

extern "C" void inthandler0c(int *esp)
{
	DEBUG_PRINT("Stack Exception!\n");
	thread_terminate(thread_current());
}
extern "C" void inthandler0d(int *esp)
{
	char str[0x40];
	int efl=load_eflags();
	io_cli();
	sprintf(str,"General Protection Exception.\n@esp=%08x",(int)esp);
	//DEBUG_PRINT(str);
	//mem::dump_memory((char*)esp-0x10,0x20);
	store_eflags(efl);
	thread_terminate(thread_current());
}
extern "C" int app_syscall(int num,int a0,int a1,int a2)
{
	int efl=load_eflags();
	io_cli();//Stack‚ð‹¤—L‚µ‚Ä‚¢‚é‚Ì‚Å,“¯Žž‚ÉŽÀs‚·‚é‚Æ¢‚é
	int low=num&0xffff;
	int high=(int)((unsigned int)num>>16);
	int result=-1;
	{
		char str[64];
		sprintf(str,"ThreadID = %d \nsyscall_%x(%x,%x,%x)\n",thread_current(),num,a0,a1,a2);
		DEBUG_PRINT(str);
	}
	if(num == 2)//exit
	{
		thread_terminate(thread_current());//-------------------------------Deprecated!----->process_terminate(procctl->current())
		return -1;
	}
	switch(high)
	{
	case 2:
		result=proc::sys::nio_proc(low,a0,a1,a2);
		break;
	default:
		break;
	}
	store_eflags(efl);
	return result;
}
