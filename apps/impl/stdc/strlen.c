#include <string.h>
#include <sys/types.h>

size_t strlen(const char *s)
{
	int len=0;
	while(*s)
	{
		len++;
		s++;
	}
	return len;
}
