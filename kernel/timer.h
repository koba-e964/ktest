#ifndef TIMER_H_43122bdb
#define TIMER_H_43122bdb

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

#include "./util/fifo.h"
#define TIMERCTL_MAX 500

typedef int (*Handler)(int arg0,int arg1);

typedef struct{
	int id;//-1...unused
	u32 flags;
	u32 remain;//残り時間
	u32 original;//もともと
	Handler proc;
	int arg0,arg1;
}Timer;
typedef struct
{
	u32 count;//経過時間
	int uniq_id;
	Fifo32 notifyIDs;//時間が来たタイマー
	int nTimer;
	Timer timer[TIMERCTL_MAX];
}TimerCtl;


global TimerCtl timerctl;
void init_pit(void);
extern "C"
void inthandler20(int*);


#define SET_TIMER_PERIODIC 0x1
int set_timer(u32 timeout,u32 flags);
int set_timer_proc(u32 timeout,u32 flags,Handler proc,int a0,int a1);

int get_notifying_timerID(void);

#endif
