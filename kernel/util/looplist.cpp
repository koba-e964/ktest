#include "looplist.h"
#include <stdio.h>

#ifdef TEST

int main(int,char**)
{
	using namespace util;
	LoopListCpp<int,100> list;
	list.array[3].data=5;
	list.add(3);
	list.add(5);
	list.erase();
	for(int i=0;i<list.loopLength();i++)
	{
		printf("%d:%d\n",i,*list);
		list++;
	}
	return 0;
}

#endif

