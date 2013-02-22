#ifndef DEVICE_MOUSE
#define DEVICE_MOUSE
#include "../all.h"

#define KEYCMD_SENDTO_MOUSE    0xd4
#define MOUSECMD_ENABLE        0xf4

enum MouseButtons
{
	MOUSE_L=1,
	MOUSE_R=2,
	MOUSE_C=4,
	MOUSE_ALL=0x07,
};

global Fifo8 mouse_fifo;
typedef struct _tagMouseBuf
{
	char buf[3],phase;
	int x,y,btn;
}MouseBuf;
void enable_mouse(void);
int mouse_decode(MouseBuf* , unsigned char data);

int mouse_initdraw(void);

/*
On mouse interruption
*/
extern "C"
void inthandler2c(int*);

/**
	mouse‚Ìevent‚ğˆ—‚·‚é‚½‚ß‚Ég‚¤B
*/
int mouse_event(void);

int mouse_motion(int mx,int my);
int mouse_click(int mx,int my,int flags);
int mouse_raise(int mx,int my,int flags);
#endif
