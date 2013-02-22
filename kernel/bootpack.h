#ifndef BOOTPACK_H
#define BOOTPACK_H
#include "all.h"
// BOOTINFO�i�u�[�g���̏��j

// �A�h���X�i0x0ff0 - 0x0fff�j�Ɋi�[����Ă���
typedef struct bootinfo {
  char cyls;    // �u�[�g�Z�N�^�̓ǂݍ���
  char leds;    // �u�[�g����LED�i�L�[�{�[�h�j�̏��
  char vmode;   // �r�f�I���[�h�i���r�b�g�J���[�H�j
  char reserve; // �\��̈�
  short scrnx, scrny; // ��ʉ𑜓x
  char *vram;         // VRAM
} BOOTINFO, *PBOOTINFO,BootInfo;

// BOOTINFO�����݂���ꏊ
#define ADDR_BOOTINFO    ((BootInfo*)0x0ff0)
#define ADDR_DISKIMG     ((void*)0x100000)


/**
	lower 16 bits:req number
	upper 16 bits:parameter
*/
global Fifo32 req_fifo;//���N�G�X�g���󂯎��buffer
global uid_t lymouse;

global fs::FAT12Image *fat12img;

int sampleWH(int,int,int,int);

extern "C" void HariMain(void);
namespace tmpCode
{
	int readFileAll(fs::FAT12Info *file,int size,char *buffer);
}

#endif
