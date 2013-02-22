#ifndef K756_KEYBOARD
#define K756_KEYBOARD
#include "../all.h"
#define PORT_KEYDAT             0x0060
#define PORT_KEYSTA             0x0064
#define PORT_KEYCMD             0x0064
#define KEYSTA_SEND_NOTREADY    0x02
#define KEYCMD_WRITE_MODE       0x60
#define KBC_MODE                0x47



global Fifo8 key_fifo;
void wait_kbc_sendready(void);

void init_keyboard(void);


/*
	on keyboard interruption
*/
extern "C"
void inthandler21(int*);


//[ 1 , KEY_CHAR ):普通の文字
#define KEY_CHAR 0x100

//区間　[ KEY_NONCHAR , 0x10000 ) は、文字にあらず、制御コード
#define KEY_NONCHAR 0x8000

#define KEY_LSHIFT 0x802a
#define KEY_RSHIFT 0x8036
#define KEY_ESC    0x8001
#define KEY_CTRL   0x801d
#define KEY_ALT    0x8038
#define KEY_LALT   KEY_ALT
#define KEY_BS     0x8881
#define KEY_TAB    0x800f

//[KEY_10 , KEY_10_END ) :10-key (0 - 9 )
#define KEY_10     0x6000
#define KEY_10_END (KEY_10+0x100)


#define KEY_F1 0x803b
#define KEY_F2 0x803c
#define KEY_F3 0x803d
#define KEY_F4 0x803e
#define KEY_F5 0x803f
#define KEY_F6 0x8040
#define KEY_F7 0x8041
#define KEY_F8 0x8042
#define KEY_F9 0x8043
#define KEY_F10 0x8044
#define KEY_F11 0x8057
#define KEY_F12 0x8058


#define KEY_SYS 0x9000

#define KEY_SYS_ENTER 0x901c
#define KEY_SYS_CTRL 0x901d
#define KEY_SYS_ALT  0x9038

#define KEY_SYS_UP    0x9048
#define KEY_SYS_LEFT  0x904b
#define KEY_SYS_RIGHT 0x904d
#define KEY_SYS_DOWN  0x9050

#define KEY_UNDEF 0xffff

int kb_getmappedchar(int keycode,int shift);
/*
	e0,e1などのコードを出すキイの判定。
*/
int kb_getmappedsys(int syscode,int keycode);

int kb_event(void);

#endif
