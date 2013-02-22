int KMain(void);
#include "../headers/sys.h"
void KStartup(void)
{
	int res=KMain();
	sys_exit(res,0);
}
