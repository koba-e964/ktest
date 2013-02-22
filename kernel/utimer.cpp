#include "utimer.h"

int uSleep_wakeup(int thid,int dummy)
{
	thread_resume(thid);
	return 0;
}

int uSleep(u32 time)
{
	if(time==0)
		return 0;
	int thid=thread_current();
	if(time != U_INFINITE)
	{
		set_timer_proc(time,0,uSleep_wakeup,thid,0);
	}
	thread_suspend(thid);
	return 0;
}
