#ifndef PROC_SYS_H_7b5265c4
#define PROC_SYS_H_7b5265c4
namespace proc
{
	namespace sys
	{
		int syscall(int num,int a,int b,int c);
	}
}//proc

extern "C" int app_syscall(int numbre,int arg0,int arg1,int arg2);
#endif

