#include <stdio.h>
#include <io.h>
#include <comm_io.h>

FILE *fopen(const char* filename,const char *attrib)
{
	if(attrib==(const char*)0)
		return (FILE*)0;
	int iattr=0;
	while(*attrib)
	{
		switch(*attrib)
		{
		case 'a':
			
		case 'w':
			iattr|=KOS_OPEN_WRITE;
			break;
		case 'r':
			break;
		case 'b':
			iattr|=KOS_OPEN_BINARY;
			break;
		case '+':
		default:
			{}
		}
		attrib++;
	}
	int fd=kos_open(filename,iattr);
	return (FILE*)(fd+FPBIAS);
}
int fclose(FILE *fp)
{
	int fd=((int)fp)-FPBIAS;
	//flush
	{}
	//close
	return kos_close(fd);
}

