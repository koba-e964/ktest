#include <kos-lib/sb.h>
#include <stddef.h>
typedef struct StringBuilder
{
	char *buffer;
	size_t clen;
	size_t cmax;
}StringBuilder;

void sb_ctor(StringBuilder *sb,int isize)
{
	if(isize<=0)isize=50;
	sb->buffer=(char*)malloc(isize);
	sb->clen=0;
	sb->cmax=(size_t)isize;
}
void sb_dtor(StringBuilder *sb)
{
	free(sb->buffer);
	sb->buffer=(char*)0;
	sb->clen=0;
	sb->cmax=0;
}
void sb_append_len(StringBuilder *sb,const char *str,size_t len)
{
	if(sb->buffer == (char*)0)return;
	if(sb->clen+len > sb->cmax)
	{
		char *tmp=(char*)malloc((int)((sb->clen+len)*1.5));
		if(tmp==(char*)0)return;
		memcpy(tmp,sb->buffer,sb->clen);
		free(sb->buffer);
		sb->buffer=tmp;
	}
	memcpy(sb->buffer+sb->clen,str,len);
	sb->clen+=len;
}
void sb_append(StringBuilder *sb,const char *str)
{
	size_t len=strlen(str);
	sb_append_len(sb,str,len);
}


