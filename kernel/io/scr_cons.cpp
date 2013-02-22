#include "scr_cons.h"
#include "../layer.h"
#include "iomng.h"
namespace io{ namespace scr_cons{
	using namespace io::iomng;
	int output(int num,void *ptr)
	{
		switch(num)
		{
		case iomng::WRITE:
		{
			RWData *data=(RWData*)ptr;
			DEBUG_PRINT((char*)data->buf);
			return data->len;
		}
		default:
			return 0;
		}
	}
	int print(const char* str)
	{
		RWData data={strlen(str),(char*)str};
		return output(iomng::WRITE,(void*)&data);
	}
}}//namespace io.scr_cons;
