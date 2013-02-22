#ifndef UTIL_LIST_OP_H
#define UTIL_LIST_OP_H
#include "../types.h"
int erase_array(void* arraydata,int from,int to,int length,size_t sizeof_elem);
/**
*	0 -> success,-1 -> failure
*/
int insert_array(void* arraydata,int to,void* insertee,int inslength,int length,
		int maxlength,size_t sizeof_elem);
#endif
