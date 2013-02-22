#include "../all.h"
int find_array(void* listdata,void* key,int len,size_t sizeof_elem);
int erase_array(void* listdata,int fromIndex,int toIndex,int len,size_t sizeof_elem)
{
	if(fromIndex<0 || fromIndex>=toIndex || toIndex>len || sizeof_elem<=0)
		return 0;
	char* dst=(char*)listdata+sizeof_elem*fromIndex;
	k_memmov(dst+sizeof_elem*(toIndex-fromIndex),dst,(len-toIndex)*sizeof_elem);
	//Should listdata[toIndex---->len] be filled with 0?
	return toIndex-fromIndex;
}
int insert_array(void* array,int into,void* insertee,int ins_len,int length,int max_len,size_t sizeof_elem)
{
	if(into<0 || ins_len<0 || length<0 || ins_len+length>max_len)
		return -1;
	char* src=(char*)array+sizeof_elem*into;
	char* dst=src+sizeof_elem*ins_len;
	k_memmov(src,dst,sizeof_elem*(length-into));
	k_memmov(insertee,src,sizeof_elem*ins_len);
	return 0;
}

#if 0
int util_array_op_test(void)
{
	char str[]="0123456789abcdefghijklmn";
	erase_array(str,4,sizeof(str)-1,sizeof(str)/sizeof(char),sizeof(char));
	return (str[4]=='n')?0:-14;
}
#endif
