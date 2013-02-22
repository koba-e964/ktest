#ifndef S71F_NASKFUNC
#define S71F_NASKFUNC
#include "../all.h"


EXTERN_C_BEGIN
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);

void io_out8(int port, int data);
void io_out16(int,int);
void io_out32(int,int);
int io_in8(int port);
int io_io16(int);
int io_in32(int);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler26(void);
void asm_inthandler2c(void);
void asm_inthandler40(void);

uint load_eflags(void);
void store_eflags(uint newflags);
uint load_cr0(void);
void store_cr0(uint);

void farjmp(int addr,short seg);
int farcall(int addr,short seg);
void task_change(int new_cs);

/**
	現在動いているタスクが、GDTの何番目に、TSSとして
	保存されるかを設定する。
	tr : the index of segment(tss)
	sample: load_tr(3*8);
*/
void load_tr(int tr);

/**
	*valが0以外である間,lockを行い,
	*valが0になったら,*val=1;return;を行う.
*/

void lock_acquire(int *val);
void lock_release(int *val);
EXTERN_C_END
#endif
