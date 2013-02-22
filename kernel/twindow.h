#ifndef TWINDOW_H
#define TWINDOW_H

#include "types.h"
#include "global.h"

enum WindowStyleFlags
{
	WSF_MAX=1,
	WSF_MIN=2,
	WSF_NORMAL=3,
	
};
enum WindowInfoConst
{
	MIN_WIDTH=30,
	MIN_HEIGHT=30,
	MAX_WINDOWS=0x100,
	MAX_CHILDREN=0x50,
	MAX_TITLELEN=100,
};

enum WindowMessages
{
	WH_INIT=0,
	WH_SENDDATA,//�f�[�^���󂯎��
	MSG_CREATE=0x100,
	MSG_DESTROY,
	MSG_CURSOR,
	MSG_LDOWN,
	MSG_RDOWN,
	MSG_LUP,
	MSG_RUP,
	MSG_MOVE,
	MSG_REPAINT,//�`���������,arg0:relx| (rely<<16),arg1:width|(height<<16)
};


typedef struct
{
	int x;
	int y;
}Position;
typedef struct{
	int x,y;
	int width,height;
}Region;

typedef int (*WindowHandler)(int handle,int msg,int arg0,int arg1);

typedef struct
{
	Region reg;
	WindowHandler wh;
	int parent;
}WindowInfo;


typedef struct InternWindowInfo
{
	int x,y,width,height;
	u32 flags;
	char title[MAX_TITLELEN];
	//int *bitmap;//not used, deprecated , should be 0
	WindowHandler wh;
	int parent;//handle of parent window. 0 iff it is root.
	struct InternWindowInfo *children[MAX_CHILDREN];
	/* A component is a child window, so its info is stored in winctl.iwi . */

}InternWindowInfo;

typedef struct
{
	uid_t winlay;
	int nWindows;
	InternWindowInfo iwi[MAX_WINDOWS];//0����nWindows-1�܂Ŗ��܂��Ă���K�v�͂Ȃ�
	int order[MAX_WINDOWS];//handle���ォ����ׂ�����
}WindowCtl;

global WindowCtl winctl;

void window_init(void);

/**
	@retval winID if successful
		-1 otherwise
	
*/
int t_CreateWindow(const char* title,const WindowInfo* wi);
int t_SetTitle(int handle,const char* title);
int t_MoveWindow(int handle,Position pos);
/**
	window��component��\��t����B
	wi�Ŏw�肳�ꂽ���W�́Ahandle��window����݂����΍��W�ł͂Ȃ��A
	��΍��W�ł���B(window�̈ړ��̎���ԂɂȂ邪�A��������ۂ���
	�d�����Ȃ��B)
	window�̘g�O�ɂ��\��t������B
	@argv cmpID:�ݒ肷��Ƃ��̒l�ɂȂ�ׂ��߂��l��component ID �Ƃ��đI�΂��B
		-1�̎��͓K���Ɍ��߂���B
	@retval ���ۂ�component ID 
*/
int createComponent(int cmpID,const WindowInfo *wi);

/**
	@retval 0 if successful
		<0 otherwise
*/
int t_DestroyWindow(int handle);

/**
	handle�������Ă���Window��WindowProc�ɑ���B
*/
int t_SendMessage(int handle,int msg,int arg0,int arg1);

int getWindow(int xpos,int ypos);

int getWindowInfo(int handle,WindowInfo *wi);

#endif

