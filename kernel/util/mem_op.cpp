#include "mem_op.h"
int k_memmov(const void* src_p,void* dst_p,size_t len)
{
	const char* src=(const char*)src_p;
	char* dst=(char*)dst_p;
	if(src==dst)
	{
		return len;
	}
	if(dst<src||src+len<=dst)
	{//copy forward
		int i;
		for(i=0;i<(int)len;i++)
		{
			dst[i]=src[i];
		}	
	}
	else
	{//copy backward
		int i;
		for(i=len-1;i>=0;i--)
		{
			dst[i]=src[i];
		}
	}
	return len;
}
