#if 0

#include "./all.h"
#include <string.h>
#include <stdio.h>
#define XCHAR 40
#define YCHAR 11
static int gl_xpos=0;
static int gl_ypos=0;
void print(char* str)
{
	//int xpos=gl_xpos,ypos=gl_ypos;
	static char conbuf[XCHAR*YCHAR];
	BOOTINFO* binfo=(BOOTINFO*)ADDR_BOOTINFO;
	extern char hankaku[];
	while((*str)!='\0')
	{
		switch(*str)
		{
		case '\r':
			gl_xpos=0;
			break;
		case '\n':
			gl_xpos=0;
			++gl_ypos;
			if(gl_ypos>=YCHAR)
			{
				int i,j,k;
				for(k=0;k<YCHAR-1;k++)
				{
					for(i=0;i<XCHAR;i++)
					{
						conbuf[k*XCHAR+i]=conbuf[k*XCHAR+XCHAR+i];
					}
				}
				for(i=0;i<XCHAR;i++)
					conbuf[YCHAR*XCHAR-XCHAR+i]='\0';
				init_screen8(binfo->vram,binfo->scrnx,binfo->scrny);
				for(i=0;i<YCHAR;i++)
					for(j=0;j<XCHAR;j++)
						putfont8(binfo->vram,binfo->scrnx,j<<3,i<<4,COL8_RED,hankaku+conbuf[i*XCHAR+j]*16);
				gl_ypos--;
			}
			break;
		default:
			//putfonts8_asc(binfo->vram,binfo->scrnx,gl_xpos<<3,gl_ypos<<4,COL8_RED,str);
			boxfill8(binfo->vram,binfo->scrnx,COL8_DARKWATER,gl_xpos<<3,gl_ypos<<4,(gl_xpos<<3)+7,(gl_ypos<<4)+15);
			putfont8(binfo->vram,binfo->scrnx,gl_xpos<<3,gl_ypos<<4,COL8_RED,hankaku+(*str)*16);
			conbuf[gl_ypos*XCHAR+gl_xpos]=*str;
			++gl_xpos;
			if(gl_xpos>=XCHAR)
			{
				print("\n");
			}
		}
		++str;
	}
}
void printi(int i)
{
	char str[0x100];
	sprintf(str,"%d",i);
	print(str);
}
#endif //#if 0
