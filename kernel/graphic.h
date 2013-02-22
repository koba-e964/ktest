#ifndef GRAPHIC__H_K
#define GRAPHIC__H_K
#include "types.h"

//colours
#define COL8_BLACK      0  // 黒色
#define COL8_RED        1  // 赤色
#define COL8_GREEN      2  // 緑色
#define COL8_YELLOW     3  // 黄色
#define COL8_BLUE       4  // 青色
#define COL8_PURPLE     5  // 紫色
#define COL8_WATER      6  // 水色
#define COL8_WHITE      7  // 白色
#define COL8_GRAY       8  // 灰色
#define COL8_DARKRED    9  // 赤色（暗い）
#define COL8_DARKGREEN  10 // 緑色（暗い）
#define COL8_DARKYELLOW 11 // 黄色（暗い）
#define COL8_DARKBLUE   12 // 青色（暗い）
#define COL8_DARKPURPLE 13 // 紫色（暗い）
#define COL8_DARKWATER  14 // 水色（暗い）
#define COL8_DARKGRAY   15 // 灰色（暗い）

unsigned char *init_palette(void);
void set_palette(int start, int end, const uchar *rgb);
void boxfill8(uchar *vram, int xsize, uchar c, 
              int x0, int y0, int x1, int y1);

void init_screen8(char *vram, int x, int y);

void putfont8(char *vram, int xsize, 
    int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, 
    int x, int y, char c, uchar *s);

void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
    int pysize, int px0, int py0, char *buf, int bxsize);

#define CURSOR_XSIZE 16
#define CURSOR_YSIZE 16
#endif
