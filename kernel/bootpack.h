#ifndef BOOTPACK_H
#define BOOTPACK_H
#include "all.h"
// BOOTINFO（ブート時の情報）

// アドレス（0x0ff0 - 0x0fff）に格納されている
typedef struct bootinfo {
  char cyls;    // ブートセクタの読み込み
  char leds;    // ブート時のLED（キーボード）の状態
  char vmode;   // ビデオモード（何ビットカラー？）
  char reserve; // 予約領域
  short scrnx, scrny; // 画面解像度
  char *vram;         // VRAM
} BOOTINFO, *PBOOTINFO,BootInfo;

// BOOTINFOが存在する場所
#define ADDR_BOOTINFO    ((BootInfo*)0x0ff0)
#define ADDR_DISKIMG     ((void*)0x100000)


/**
	lower 16 bits:req number
	upper 16 bits:parameter
*/
global Fifo32 req_fifo;//リクエストを受け取るbuffer
global uid_t lymouse;

global fs::FAT12Image *fat12img;

int sampleWH(int,int,int,int);

extern "C" void HariMain(void);
namespace tmpCode
{
	int readFileAll(fs::FAT12Info *file,int size,char *buffer);
}

#endif
