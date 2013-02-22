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
		指定したsegment descriptorにおけるアドレスvaddrの実際の物理的位置を返す.
		失敗すると(void*)0が返る.
	*/
	void *getPhysAddr(SEGMENT_DESCRIPTOR *,u32 vaddr);
}

#endif
