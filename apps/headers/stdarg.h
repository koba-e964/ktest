#ifndef STDARG_H_ad4b7120
#define STDARG_H_ad4b7120
#include <stdlib.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C"{
#endif

typedef struct va_list
{
	int *current;
}va_list;
#define _INTSIZEOF(n) ((sizeof(n)+sizeof(int)-1)/sizeof(int))
#define va_start(list,lastarg) (void)((list).current=((int*)&(lastarg)+_INTSIZEOF(lastarg)))
#define va_arg(list,data_type) (*(data_type*)va_arg_0(&(list),_INTSIZEOF(datatype)))
EXT_C_BEG
	void *va_arg_0(va_list *list,size_t isiz);
EXT_C_END
#define va_end(list) (void)((list).current=(int*)0)
#ifdef _cplusplus
}//extern "C"
#endif

#endif
