#include "all.h"

void init_gdtidt(void)
{
  PSEGMENT_DESCRIPTOR pgdt = (PSEGMENT_DESCRIPTOR) ADR_GDT;
  PGATE_DESCRIPTOR    pidt = (PGATE_DESCRIPTOR   ) ADR_IDT;

  int i;

  // GDTの初期化
  // GDTのすべてのセグメントに対して
  // リミット0、ベースアドレス0、アクセス属性0を設定
  for (i = 0; i <= LIMIT_GDT / 8; i++)
    set_segmdesc(pgdt + i, 0, 0, 0);

  // リミット0xffffffff、ベースアドレス0x00000000、属性読み書き用に設定
  // 要するにすべてのメモリが対象となる
  set_segmdesc(pgdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);
  // bootpack.hrbのためのセグメントを設定
  set_segmdesc(pgdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
  // GDTRレジスタにGDTのデータがあるアドレスを入れる
  load_gdtr(LIMIT_GDT, ADR_GDT);

  // IDTの初期化
  for (i = 0; i <= LIMIT_IDT / 8; i++)
    set_gatedesc(pidt + i, 0, 0, 0);
  // IDTRレジスタにIDTのデータがあるアドレスを入れる
  load_idtr(LIMIT_IDT, ADR_IDT);

  // IDTの設定
  // キーボード入力とマウス入力時に割り込みを発生させる
  // 割り込みが発生したときに処理される関数はasm_inthandler21とasm_inthandler2c
  // naskfunc.nasを参照
	set_gatedesc(pidt + 0x0c, (int) asm_inthandler0c, 2 * 8, AR_INTGATE32);
	set_gatedesc(pidt + 0x0d, (int) asm_inthandler0d, 2 * 8, AR_INTGATE32);
	set_gatedesc(pidt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	set_gatedesc(pidt + 0x26, (int) asm_inthandler26, 2 * 8, AR_INTGATE32);
	set_gatedesc(pidt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);
	set_gatedesc(pidt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	set_gatedesc(pidt + 0x40, (int) asm_inthandler40, 2 * 8, AR_INTGATE32);
}
void set_segmdesc(PSEGMENT_DESCRIPTOR psd, unsigned int limit, int base, int ar)
{
#define GDT_G_bit 0x8000
  // G_bitの判定
  if (limit > 0xfffff) {
    ar |= GDT_G_bit;    // G_bit ON
    limit /= 0x1000; // 4KB単位にする
  }

  // セグメントのベースアドレス設定（16 + 8 + 8 = 32bit）
  psd->base_low  = base & 0xffff;
  psd->base_mid  = (base >> 16) & 0xff;
  psd->base_high = (base >> 24) & 0xff;

  // セグメントのサイズ設定（4 + 16 = 20bit）
  psd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  psd->limit_low  = limit & 0xffff;

  // セグメント属性の設定
  psd->access_right = ar & 0xff;
}

void set_gatedesc(PGATE_DESCRIPTOR pgd, int offset, int selector, int ar)
{
  pgd->offset_low  = offset & 0xffff;
  pgd->offset_high = (offset >> 16) & 0xffff;

  pgd->selector     = selector;
  pgd->dw_count     = (ar >> 8) & 0xff;
  pgd->access_right = ar & 0xff;
}
namespace dsctbl
{
	void *getPhysAddr(SEGMENT_DESCRIPTOR *segd,u32 vaddr)
	{
		u32 base=(segd->base_low&0xffff)|((segd->base_mid&0xff)<<16)|
			((segd->base_high&0xff)<<24);
		u32 limit=(segd->limit_low&0xffff)|((segd->limit_high&0x0f)<<16);
		int gra=(segd->limit_high&0xf0)&(GDT_G_bit>>8);
		if(gra)//Granularity-bit
		{
			limit*=0x1000;
			limit+=0xfff;
		}
		if(vaddr>limit)//error : out of range
		{
			return (void*)0;
		}
		return (void*)(base+vaddr);
	}
}

