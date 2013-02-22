#ifndef DSCTBL_H
#define DSCTBL_H

#include "./x86.h"
#include "types.h"

#define ADR_BOTPAK    0x00280000
#define LIMIT_BOTPAK  0x0007ffff


void init_gdtidt(void);
void set_segmdesc(SEGMENT_DESCRIPTOR*,uint limit,int base,
			int access_right);
void set_gatedesc(PGATE_DESCRIPTOR pgd, int offset,
			int selector,int ar);

namespace dsctbl
{
	/**
		�w�肵��segment descriptor�ɂ�����A�h���Xvaddr�̎��ۂ̕����I�ʒu��Ԃ�.
		���s�����(void*)0���Ԃ�.
	*/
	void *getPhysAddr(SEGMENT_DESCRIPTOR *,u32 vaddr);
}

#endif
