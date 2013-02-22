#ifndef TASK_H_60da_4ffb_8c92_bdd8
#define TASK_H_60da_4ffb_8c92_bdd8

#include "./x86.h"
#include "./util/bilist.h"
#include "./util/looplist.h"
#include "./util/array.h"
#include "./util/macros.h"
#include "./types.h"
#include "./global.h"
enum {
	THREAD_MAX=500,
	LEVEL_MAX=10,
};


enum ThreadState{
THREAD_SUSPENDED=1,
THREAD_RUNNING,
THREAD_ABSENT, //���݂��Ȃ�
};
enum
{
	THREAD_LDTS=2,//Thread����ldt�̐�
};
#define szint sizeof(int)
typedef struct
{
	int current;
	BiList* list;
	BiList* suspended;
}LevelInfo;
#undef szint

typedef struct {
	TSS32 tss;
	enum ThreadState state;
	SEGMENT_DESCRIPTOR ldt[THREAD_LDTS];
	//int sel;//Selector =thid+TASK_GDT0
	void* argv;
	int level;
}ThreadInfo;
typedef struct
{
	u32 interval;
	int nThread;
	util::LimArray<ThreadInfo,THREAD_MAX> threads;
	/**
		The list of working threads.
	*/
	util::LoopListCpp<char,THREAD_MAX> list;
	char threads_used[LSB_ALIGN(THREAD_MAX,8)/8];
	util::LimArray<bool,THREAD_MAX> suspended;
	LevelInfo levels;//level0�̂�,not used
}ThreadCtl;


int threadctl_init(void);

int thread_create(int (*proc)(void*),void* argv,int level,u32 flags);
int thread_alloc(void);//�K�v��TSS�̐ݒ�͎����ōs���B
void thread_add(int thid);//���O��TSS��ݒ肵����ɌĂԁB
bool thread_exists(int thid);
int thread_suspend(int thid);
int thread_resume(int thid);
int thread_terminate(int thid);


int thread_current(void);

int thread_switch(void);

enum{
	TASK_GDT0=3,
	TASK_BOOT=3,
};//gdt��3�Ԗڂ��task�p��gdt��ݒ肷

global int thctl_timer GLOBAL_INIT (-1);

namespace task
{
	inline int LDTInGDT(int tss_id)
	{
		return TASK_GDT0 + THREAD_MAX + tss_id;
	}
	int loadCurrentToKernel(int ldtnum,u32 procmem,int len,void *dest);
	/**
		thid�Ŏ������X���b�h�̃�������Ԃ�procmem�ɂ����镔���̃���������len�̒��������A
		dest(kernel memory)�ɃR�s�[����.
		�Ȃ�,�����v���Z�X�ł��X���b�h���Ⴆ�΃�������Ԃ͈قȂ�ꍇ������.
		@retval ����:0,���s:���̒l
	*/
	int loadThreadToKernel(int thid,int ldtnum,u32 procmem,int len,void *dest);
}

// for dump

int dump_thctl(void);


/**
	thctl->level[0]->
	list,suspended��dump����
*/
void dump_threadlist(void);

#endif
