#include <stdarg.h>

void *va_arg_0(va_list *list,size_t siz)
{
	void *ret=(void*)list->current;
	list->current+=siz;
	return ret;
}
