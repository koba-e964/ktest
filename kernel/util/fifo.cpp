#include "../all.h"
void init_fifo8(Fifo8* fi,char* buf,int buflen)
{
	if(buflen<=0)
	{
		return;
	}
	fi->buf=buf;
	fi->len=buflen;
	fi->wpos=0;
	fi->rpos=0;
}
int put_fifo8(Fifo8* fi,char dat)
{
	if((fi->rpos-fi->wpos+fi->len)%fi->len==1)
		return -1;
	io_cli();
	fi->buf[fi->wpos]=dat;
	++(fi->wpos);
	if(fi->wpos>=fi->len)
		fi->wpos=0;
	io_sti();
	return 0;
}
int get_fifo8(_inout Fifo8* fi)
{
	char out;
	if(fi->rpos==fi->wpos)
		return -1;
	out=fi->buf[fi->rpos];
	++(fi->rpos);
	if(fi->rpos>=fi->len)
		fi->rpos=0;
	return out&0xff;
}
int status_fifo8(Fifo8* fi)
{
	return (fi->wpos-fi->rpos+fi->len)%fi->len;
}


void init_fifo32(Fifo32* fi,i32* buf,int buflen)
{
	if(buflen<=0)
	{
		return;
	}
	fi->buf=buf;
	fi->len=buflen;
	fi->wpos=0;
	fi->rpos=0;
}
int put_fifo32(Fifo32* fi,i32 dat)
{
	if((fi->rpos-fi->wpos+fi->len)%fi->len==1)
		return -1;
	io_cli();
	fi->buf[fi->wpos]=dat;
	++(fi->wpos);
	if(fi->wpos>=fi->len)
		fi->wpos=0;
	io_sti();
	return 0;
}
i32 get_fifo32(Fifo32* fi)
{
	i32 out;
	if(fi->rpos==fi->wpos)
		return -1;
	io_cli();
	out=fi->buf[fi->rpos];
	++(fi->rpos);
	if(fi->rpos>=fi->len)
		fi->rpos=0;
	io_sti();
	return out;
}
int status_fifo32(Fifo32* fi)
{
	return (fi->wpos-fi->rpos+fi->len)%fi->len;
}

