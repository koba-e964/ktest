// graphic.c

#include "bootpack.h"


// ------------------------------------------------------------------
// �p�P�b�g�f�[�^��ݒ肷��֐��iRGB�Őݒ�j
// ------------------------------------------------------------------
unsigned char *init_palette(void)
{
  // �F����RGB�Ŏ����Ă���
  static unsigned char table_rgb[16 * 3] = {
    0x00, 0x00, 0x00,  //  0:���F
    0xff, 0x00, 0x00,  //  1:�ԐF
    0x00, 0xff, 0x00,  //  2:�ΐF
    0xff, 0xff, 0x00,  //  3:���F
    0x00, 0x00, 0xff,  //  4:�F
    0xff, 0x00, 0xff,  //  5:���F
    0x00, 0xff, 0xff,  //  6:���F
    0xff, 0xff, 0xff,  //  7:���F
    0xc6, 0xc6, 0xc6,  //  8:�D�F
    0x84, 0x00, 0x00,  //  9:�ԐF�i�Â��j
    0x00, 0x84, 0x00,  // 10:�ΐF�i�Â��j
    0x84, 0x84, 0x00,  // 11:���F�i�Â��j
    0x00, 0x00, 0x84,  // 12:�F�i�Â��j
    0x84, 0x00, 0x84,  // 13:���F�i�Â��j
    0x00, 0x84, 0x84,  // 14:���F�i�Â��j
    0x84, 0x84, 0x84   // 15:�D�F�i�Â��j
  };
  return table_rgb;
}


// ------------------------------------------------------------------
// VRAM�Ƀp���b�g���Z�b�g����֐�
// ���̊֐��̏ڂ�������� http://community.osdev.info/?VGA ������ׂ�
// ------------------------------------------------------------------
void set_palette(int start, int end, const unsigned char *rgb)
{
  //int eflags = io_load_eflags();// ���݂̃t���O���W�X�^���L��
  io_cli();                       // ���t���O��0�ɂ��Ċ��荞�݋֎~�ɂ���
  io_out8(0x03c8, start);
  while(start <= end) {
    io_out8(0x03c9, rgb[0] / 4);  // 16�F�̏���ݒ�
    io_out8(0x03c9, rgb[1] / 4);  // 1�̐F��RGB��3�o�C�g�Ȃ̂ŁA
    io_out8(0x03c9, rgb[2] / 4);  // ���̂悤��3��g���Đݒ肷��
    rgb += 3;
    start++;
  }
  io_sti();
  //io_store_eflags(eflags);      // �t���O���W�X�^�𕜌�
  return;
}


// ------------------------------------------------------------------
// �����`��`���֐�
// ------------------------------------------------------------------
void boxfill8(unsigned char *vram, // VRAM�̃A�h���X
              int xsize,           // X���̃T�C�Y
              unsigned char c,     // �F���
              int x0, int y0,      // �J�n���W�i�����1�_�j
              int x1, int y1       // �I�����W�i�E����1�_�j
             )
{
  int x, y;
  for (y = y0; y <= y1; y++) {  // VRAM�̃A�h���X��
    for (x = x0; x <= x1; x++)  // 1�o�C�g�������`�̃f�[�^��
      vram[y * xsize + x] = c;  // ��������ł���
  }
}


// ------------------------------------------------------------------
// �X�N���[����ʂ�����������
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
// 1�����o�͊֐�
// vram = VRAM, xsize = �X�N���[��X���T�C�Y
// x, y = �\������X�N���[�����W�ix, y�j
// c = �\�����镶���̐F
// font = �\�����镶���t�H���g������A�h���X
// ------------------------------------------------------------------
void putfont8(char *vram, int xsize, 
              int x, int y, char c, char *font)
{
  int i;
  for (i = 0; i < 16; i++) {
    // d�̓t�H���g�̃r�b�g���
    char d = font[i];
    // p��VRAM�̕`�悷��A�h���X
    char *p = vram + (y + i) * xsize + x;
    // �t�H���g�̃r�b�g�������ɐF�̓_��ł��Ă���
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
// ������o�͊֐�
// vram = VRAM, xsize = �X�N���[��X���T�C�Y
// x, y = �\������X�N���[�����W�ix, y�j
// c = �\�����镶����̐F
// s = �\�����镶����̃A�h���X
// ------------------------------------------------------------------
void putfonts8_asc(char *vram, int xsize, 
                   int x, int y, char color, unsigned char *s)
{
  // ���t�@�C���ɒ�`����Ă���O���[�o���ϐ����Q��
  extern char hankaku[4096];
  for (; *s != 0x00; s++) {
    putfont8(vram, xsize, x, y, color, hankaku + *s * 16);
    x += 8;
  }
  return;
}


// ------------------------------------------------------------------
// �}�E�X�J�[�\���̉摜�𐶐�����
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
// �摜�`��֐�
// vram   = VRAM
// vxsize = �X�N���[��X���T�C�Y
// pxsize, pysize = �`�悷��摜�̃T�C�Y�iX, Y�j
// px0, py0 = �`�悷��摜�̈ʒu�iX, Y�j
// buf = �`�悷��摜�f�[�^
// bxsize = �`�悷��摜��X���T�C�Y
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
