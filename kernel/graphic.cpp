// graphic.c

#include "bootpack.h"


// ------------------------------------------------------------------
// パケットデータを設定する関数（RGBで設定）
// ------------------------------------------------------------------
unsigned char *init_palette(void)
{
  // 色情報をRGBで持っておく
  static unsigned char table_rgb[16 * 3] = {
    0x00, 0x00, 0x00,  //  0:黒色
    0xff, 0x00, 0x00,  //  1:赤色
    0x00, 0xff, 0x00,  //  2:緑色
    0xff, 0xff, 0x00,  //  3:黄色
    0x00, 0x00, 0xff,  //  4:青色
    0xff, 0x00, 0xff,  //  5:紫色
    0x00, 0xff, 0xff,  //  6:水色
    0xff, 0xff, 0xff,  //  7:白色
    0xc6, 0xc6, 0xc6,  //  8:灰色
    0x84, 0x00, 0x00,  //  9:赤色（暗い）
    0x00, 0x84, 0x00,  // 10:緑色（暗い）
    0x84, 0x84, 0x00,  // 11:黄色（暗い）
    0x00, 0x00, 0x84,  // 12:青色（暗い）
    0x84, 0x00, 0x84,  // 13:紫色（暗い）
    0x00, 0x84, 0x84,  // 14:水色（暗い）
    0x84, 0x84, 0x84   // 15:灰色（暗い）
  };
  return table_rgb;
}


// ------------------------------------------------------------------
// VRAMにパレットをセットする関数
// この関数の詳しい解説は http://community.osdev.info/?VGA を見るべし
// ------------------------------------------------------------------
void set_palette(int start, int end, const unsigned char *rgb)
{
  //int eflags = io_load_eflags();// 現在のフラグレジスタを記憶
  io_cli();                       // 許可フラグを0にして割り込み禁止にする
  io_out8(0x03c8, start);
  while(start <= end) {
    io_out8(0x03c9, rgb[0] / 4);  // 16色の情報を設定
    io_out8(0x03c9, rgb[1] / 4);  // 1つの色はRGBで3バイトなので、
    io_out8(0x03c9, rgb[2] / 4);  // このように3回使って設定する
    rgb += 3;
    start++;
  }
  io_sti();
  //io_store_eflags(eflags);      // フラグレジスタを復元
  return;
}


// ------------------------------------------------------------------
// 長方形を描く関数
// ------------------------------------------------------------------
void boxfill8(unsigned char *vram, // VRAMのアドレス
              int xsize,           // X軸のサイズ
              unsigned char c,     // 色情報
              int x0, int y0,      // 開始座標（左上の1点）
              int x1, int y1       // 終了座標（右下の1点）
             )
{
  int x, y;
  for (y = y0; y <= y1; y++) {  // VRAMのアドレスへ
    for (x = x0; x <= x1; x++)  // 1バイトずつ長方形のデータを
      vram[y * xsize + x] = c;  // 書き込んでいく
  }
}


// ------------------------------------------------------------------
// スクリーン画面を初期化する
// ------------------------------------------------------------------
void init_screen8(unsigned char *vram, int x, int y)
{
  boxfill8(vram, x, COL8_DARKWATER, 0,      0, x -  1, y - 29);
  boxfill8(vram, x, COL8_GRAY,      0, y - 28, x -  1, y - 28);
  boxfill8(vram, x, COL8_WHITE,     0, y - 27, x -  1, y - 27);
  boxfill8(vram, x, COL8_GRAY,      0, y - 26, x -  1, y -  1);

  boxfill8(vram, x, COL8_WHITE,     3, y - 24, 59, y - 24);
  boxfill8(vram, x, COL8_WHITE,     2, y - 24,  2, y -  4);
  boxfill8(vram, x, COL8_DARKGRAY,  3, y -  4, 59, y -  4);
  boxfill8(vram, x, COL8_DARKGRAY, 59, y - 23, 59, y -  5);
  boxfill8(vram, x, COL8_BLACK,     2, y -  3, 59, y -  3);
  boxfill8(vram, x, COL8_BLACK,    60, y - 24, 60, y -  3);

  boxfill8(vram, x, COL8_DARKGRAY, x - 47, y - 24, x -  4, y - 24);
  boxfill8(vram, x, COL8_DARKGRAY, x - 47, y - 23, x - 47, y -  4);
  boxfill8(vram, x, COL8_WHITE,    x - 47, y -  3, x -  4, y -  3);
  boxfill8(vram, x, COL8_WHITE,    x -  3, y - 24, x -  3, y -  3);
}


// ------------------------------------------------------------------
// 1文字出力関数
// vram = VRAM, xsize = スクリーンX軸サイズ
// x, y = 表示するスクリーン座標（x, y）
// c = 表示する文字の色
// font = 表示する文字フォントがあるアドレス
// ------------------------------------------------------------------
void putfont8(char *vram, int xsize, 
              int x, int y, char c, char *font)
{
  int i;
  for (i = 0; i < 16; i++) {
    // dはフォントのビット情報
    char d = font[i];
    // pはVRAMの描画するアドレス
    char *p = vram + (y + i) * xsize + x;
    // フォントのビット情報を元に色の点を打っていく
    if ((d & 0x80) != 0) { p[0] = c; }
    if ((d & 0x40) != 0) { p[1] = c; }
    if ((d & 0x20) != 0) { p[2] = c; }
    if ((d & 0x10) != 0) { p[3] = c; }
    if ((d & 0x08) != 0) { p[4] = c; }
    if ((d & 0x04) != 0) { p[5] = c; }
    if ((d & 0x02) != 0) { p[6] = c; }
    if ((d & 0x01) != 0) { p[7] = c; }
  }
}


// ------------------------------------------------------------------
// 文字列出力関数
// vram = VRAM, xsize = スクリーンX軸サイズ
// x, y = 表示するスクリーン座標（x, y）
// c = 表示する文字列の色
// s = 表示する文字列のアドレス
// ------------------------------------------------------------------
void putfonts8_asc(char *vram, int xsize, 
                   int x, int y, char color, unsigned char *s)
{
  // 他ファイルに定義されているグローバル変数を参照
  extern char hankaku[4096];
  for (; *s != 0x00; s++) {
    putfont8(vram, xsize, x, y, color, hankaku + *s * 16);
    x += 8;
  }
  return;
}


// ------------------------------------------------------------------
// マウスカーソルの画像を生成する
// ------------------------------------------------------------------
#define CURSOR_XSIZE 16
#define CURSOR_YSIZE 16
void init_mouse_cursor8(char *mouse, char bc)
{
  static char cursor[CURSOR_YSIZE][CURSOR_XSIZE+1] = {
    "**..............",
    "*O*.............",
    "*OO*............",
    "*OOO*...........",
    "*OOOO*..........",
    "*OOOOO*.........",
    "*OOOOOO*........",
    "*OOOOOOO*.......",
    "*OOOOO****......",
    "*OOOOOO*........",
    "*OOOO*OO*.......",
    "*OO***OO*.......",
    "*O*..*OO*.......",
    "**....*OO*......",
    "......*OO*......",
    ".......***......"
  };

  int x, y;
  for (y = 0; y < 16; y++)
    for (x = 0; x < 16; x++)
      switch (cursor[y][x])
      {
      case '*':
        mouse[y * 16 + x] = COL8_BLACK;
        break;
      case 'O':
        mouse[y * 16 + x] = COL8_WHITE;
        break;
      case '.':
        mouse[y * 16 + x] = bc;
        break;
      }
}


// ------------------------------------------------------------------
// 画像描画関数
// vram   = VRAM
// vxsize = スクリーンX軸サイズ
// pxsize, pysize = 描画する画像のサイズ（X, Y）
// px0, py0 = 描画する画像の位置（X, Y）
// buf = 描画する画像データ
// bxsize = 描画する画像のX軸サイズ
// ------------------------------------------------------------------
void putblock8_8(char *vram, int vxsize, 
                 int pxsize, int pysize, 
                 int px0, int py0, 
                 char *buf, int bxsize)
{
  int x, y;
  for (y = 0; y < pysize; y++)
    for (x = 0; x < pxsize; x++)
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
}
