#include <stdio.h>
#include <stdarg.h>
size_t format_helper(char *buf,size_t max,size_t *nextneeded,
const char **format,va_list list);
int printf(const char *format,...)
{
	va_list list;
	int res;
	va_start(list,format);
	res=vfprintf(stdout,format,list);
	va_end(list);
	return res;
}
int vfprintf(FILE *fp,const char *format,va_list list)
{
	int ocnt=0;
	char buf[0x200];
	size_t prog;
	size_t needed;
	while(1)
	{
		prog=format_helper(buf,0x200,&needed,&format,list);
		if(prog>0)
		{
			fwrite(buf,1,prog,fp);
			ocnt+=prog;
			if(format[0]==(char)0)//format end
				break;
			continue;
		}
		if(needed==0)break;
		char *buf2=(char*)malloc(needed);
		size_t prog2=format_helper(buf2,needed,(size_t*)0,&format,list);
		//assert(prog2==needed);
		fwrite(buf2,1,prog2,fp);
		ocnt+=prog2;
	}
	return ocnt;
}
