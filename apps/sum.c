#include <io.h>


int KMain(void)
{
	int sum=0;
	int i;
	int fd=kos_open("test.bin",1);
	char teststr[]="teststr@sum.hrb\n";
	kos_write(STDOUT,(void*)teststr,sizeof(teststr));
	return 0;
	if(fd<0)return -1;
	int size=kos_size(fd);
	if(size < 0)return -2;
	for(i=0;i<size;i++)
	{
		char ch;
		kos_read(fd,&ch,1);
		sum*=0xff;
		sum+= ch & 0xff;
	}
	//kos_write(STDOUT,(void*)&sum,sizeof(sum));
	return 0;
}
