#include "./all.h"
#include <stdio.h>
#define NOTIFY_BUF_MAX 0x100
static int notify_buf[NOTIFY_BUF_MAX];


static int set_timer_internal(u32,u32,Handler,int,int,int);

void init_pit(void)
{
	int i;
	io_out8(0x43,0x34);
	io_out8(0x40,0x9c);
	io_out8(0x40,0x2e);//割り込み周期:0x2e9c ->11932 ->100Hz

	timerctl.count=0;
	timerctl.uniq_id=0;
	for(i=0;i<TIMERCTL_MAX;i++)
	{
		timerctl.timer[i].id=-1;
		timerctl.timer[i].flags=0;
		timerctl.timer[i].remain=0;
		timerctl.timer[i].original=0;
		timerctl.timer[i].proc=(Handler)0;
	}
	init_fifo32(&timerctl.notifyIDs,notify_buf,NOTIFY_BUF_MAX);
	return;
}

void inthandler20(int* esp)
{
	io_out8(PIC0_OCW2,0x60);//IRQ-00(0x60)->PIC0
	timerctl.count++;
//decrement
	if(timerctl.nTimer<=0)
	{
		return;
	}
	if(timerctl.timer[0].remain>=1)
	{
		timerctl.timer[0].remain--;
	}
	while(timerctl.timer[0].remain<=0)
	{
		int note_id=timerctl.timer[0].id;
		int flags=timerctl.timer[0].flags;
		int original=timerctl.timer[0].original;
		Handler proc=timerctl.timer[0].proc;
		int arg0=timerctl.timer[0].arg0,
			arg1=timerctl.timer[0].arg1;
		erase_array(timerctl.timer,0,1,timerctl.nTimer,sizeof(Timer));
		timerctl.timer[timerctl.nTimer-1].id=-1;//clear
		timerctl.nTimer--;
		if(flags&SET_TIMER_PERIODIC)
		{
			set_timer_internal(original,flags,proc,arg0,arg1,note_id);
		}
		if(note_id!=thctl_timer)
		{
			put_fifo32(&timerctl.notifyIDs,note_id);
			if(proc != 0)
			{
				proc(arg0,arg1);
			}
		}
		else{
			thread_switch();
		}
	}
}
/**
	timeout:時間
	flags:SET_TIMER_PERIODIC ->timeout毎に知らせる。
*/
int set_timer(u32 timeout,u32 flags)
{
	return set_timer_internal(timeout,flags,(Handler)0,0,0,-1);
}

int set_timer_proc(u32 timeout,u32 flags,Handler proc,int a0,int a1)
{
	return set_timer_internal(timeout,flags,proc,a0,a1,-1);
}
/**
	timeout,flags
	prev_id:-1以外を指定した場合は、そのidでタイマーを作る。
		そうでなければ、まだ使われていないidになる。

	戻り値:成功した場合はidを、失敗した場合は-1を返す。
*/
static
int set_timer_internal(u32 timeout,u32 flags,Handler proc,int arg0,int arg1,int prev_id)
{
	int i;
	if(timerctl.nTimer>=TIMERCTL_MAX)
		return -1;
	io_cli();
	i32 left=timeout;
	int id;
	if(prev_id==-1)
	{
		id=timerctl.uniq_id;
		timerctl.uniq_id++;
	}
	else id=prev_id;
	for(i=0;i<TIMERCTL_MAX;i++)
	{
		if(timerctl.timer[i].id==-1)//unused
		{
			timerctl.timer[i].id=id;
			timerctl.timer[i].flags=flags;
			timerctl.timer[i].remain=left;
			timerctl.timer[i].original=timeout;
			timerctl.timer[i].proc=proc;
			timerctl.timer[i].arg0=arg0;
			timerctl.timer[i].arg1=arg1;
			goto end;
		}
		i32 currem=timerctl.timer[i].remain;
		if(left>=currem)
		{
			left-=currem;
			continue;
		}
		//left<currem
		timerctl.timer[i].remain-=left;
		Timer insertee={id,flags,left,timeout,proc,arg0,arg1};
		insert_array(timerctl.timer,i,&insertee,1,timerctl.nTimer,TIMERCTL_MAX,
				sizeof(Timer));
		break;
	}
end:
	timerctl.nTimer++;
	io_sti();
	return id;
}
