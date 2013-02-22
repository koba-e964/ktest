#ifndef IO_IOMNG_H_36bd893f
#define IO_IOMNG_H_36bd893f

#include "../types.h"
#include "../global.h"

namespace io{ namespace iomng{
	typedef int (*IOProc)(int num,void *argv);
	struct DeviceData
	{
		i64 timestamp;//-1:void
		IOProc proc;
	};
	const int DEV_MAX=0x1000;
	global class IOManager
	{
	private:
		DeviceData devdat[DEV_MAX];
	public:
		void ctor(void);
		static void init(void);
		int add(IOProc ioproc);
		int access(int dev_id,int num,void *data);
	}*iomng;
	typedef struct{
		int len;
		void *buf;
	}RWData;
	//request number
	const int READ=1,WRITE=2;
//global variables
	global int screen;
}}//namespace io::iomng
#endif
