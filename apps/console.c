//#include "ktest/io.h"



int kmain(void)
{
	void* stdin;
	while(1)
	{
		int nchar=getline(stdin,0,(char*)0);
		char *buf=malloc(nchar+1);
		getline(stdin,nchar,buf);
		//...
	}
	return 0;
}
