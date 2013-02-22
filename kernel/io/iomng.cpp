#include "iomng.h"
#include "../util/macros.h"
#include "../mem.h"
#include "scr_cons.h"
#include <stdio.h>
namespace io{ namespace iomng{
	using namespace util::macros;
	void IOManager::ctor(void)
	{
		for(int i=0;i<sizeArray(devdat);++i)
		{
			devdat[i].timestamp=-1;
			devdat[i].proc=(IOProc)(void*)0;
		}
	}
	int IOManager::add(IOProc ioproc)
	{
		int id=-1;
		for(int i=0;i<sizeArray(devdat);i++)
		{
			if(devdat[i].timestamp==-1)
			{
				id=i;
				break;
			}
		}
		if(id==-1)return -1;
		i64 now=timerctl.count;
		devdat[id].timestamp=now;
		devdat[id].proc=ioproc;
		return id;
	}
	void IOManager::init(void)
	{
		iomng=mem::talloc<IOManager>();
		iomng->ctor();
		screen=iomng->add(io::scr_cons::output);
	}
	int IOManager::access(int dev_id,int num,void *data)
	{
		char str[0x100];
		sprintf(str, "IOManager::access,dev_id=%08x,num=%d\n",dev_id,num);
		DEBUG_PRINT(str);
		if(dev_id<0||dev_id>=sizeArray(this->devdat))return -1;
		return (*(this->devdat[dev_id].proc))(num,data);
	}
}}
