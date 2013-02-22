#ifndef K756_FIFO
#define K756_FIFO
#include "../types.h"
#include "./macros.h"
typedef struct
{
	char* buf;
	int wpos,rpos;
	int len;
}Fifo8;

void init_fifo8(_out Fifo8*,_out char* buf,int buflen);
int put_fifo8(_inout Fifo8*,char);
int get_fifo8(_inout Fifo8*);
int status_fifo8(_in Fifo8*);//Fifo‚É‚½‚Ü‚Â‚Ä‚î‚édata—Ê

typedef struct
{
	i32* buf;
	int wpos,rpos;
	int len;
}Fifo32;

void init_fifo32(_out Fifo32*,_out i32* buf, int buflen);
int put_fifo32(_inout Fifo32* , i32);
i32 get_fifo32(_inout Fifo32*);
int status_fifo32(_in Fifo32*);//Fifo‚É‚½‚Ü‚Â‚Ä‚î‚édata—Ê



#endif
