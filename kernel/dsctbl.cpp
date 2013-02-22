#include "all.h"

void init_gdtidt(void)
{
  PSEGMENT_DESCRIPTOR pgdt = (PSEGMENT_DESCRIPTOR) ADR_GDT;
  PGATE_DESCRIPTOR    pidt = (PGATE_DESCRIPTOR   ) ADR_IDT;

  int i;

  // GDT�̏�����
  // GDT�̂��ׂẴZ�O�����g�ɑ΂���
  // ���~�b�g0�A�x�[�X�A�h���X0�A�A�N�Z�X����0��ݒ�
  for (i = 0; i <= LIMIT_GDT / 8; i++)
    set_segmdesc(pgdt + i, 0, 0, 0);

  // ���~�b�g0xffffffff�A�x�[�X�A�h���X0x00000000�A�����ǂݏ����p�ɐݒ�
  // �v����ɂ��ׂẴ��������ΏۂƂȂ�
  set_segmdesc(pgdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);
  // bootpack.hrb�̂��߂̃Z�O�����g��ݒ�
  set_segmdesc(pgdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
  // GDTR���W�X�^��GDT�̃f�[�^������A�h���X������
  load_gdtr(LIMIT_GDT, ADR_GDT);

  // IDT�̏�����
  for (i = 0; i <= LIMIT_IDT / 8; i++)
    set_gatedesc(pidt + i, 0, 0, 0);
  // IDTR���W�X�^��IDT�̃f�[�^������A�h���X������
  load_idtr(LIMIT_IDT, ADR_IDT);

  // IDT�̐ݒ�
  // �L�[�{�[�h���͂ƃ}�E�X���͎��Ɋ��荞�݂𔭐�������
  // ���荞�݂����������Ƃ��ɏ��������֐���asm_inthandler21��asm_inthandler2c
  // naskfunc.nas���Q��
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
  // G_bit�̔���
  if (limit > 0xfffff) {
    ar |= GDT_G_bit;    // G_bit ON
    limit /= 0x1000; // 4KB�P�ʂɂ���
  }

  // �Z�O�����g�̃x�[�X�A�h���X�ݒ�i16 + 8 + 8 = 32bit�j
  psd->base_low  = base & 0xffff;
  psd->base_mid  = (base >> 16) & 0xff;
  psd->base_high = (base >> 24) & 0xff;

  // �Z�O�����g�̃T�C�Y�ݒ�i4 + 16 = 20bit�j
  psd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  psd->limit_low  = limit & 0xffff;

  // �Z�O�����g�����̐ݒ�
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

