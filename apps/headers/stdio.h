#ifndef STDIO_H_ce7fe147
#define STDIO_H_ce7fe147

#include <stddef.h>
#include <comm_io.h>
#include <stdarg.h>
#define FPBIAS 0x8e800000
#define stdin (FILE*)(FPBIAS+STDIN)
#define stdout (FILE*)(FPBIAS+STDOUT)
#define stderr (FILE*)(FPBIAS+STDERR)
EXT_C_BEG

	typedef struct{} FILE;//dummy,FILE* <->fd
	FILE *fopen(const char *filename,const char *attrib);
	int fclose(FILE* fd);
	int printf(const char *format,...);
	int vfprintf(FILE *fp,const char *format,va_list v);
EXT_C_END
#endif
