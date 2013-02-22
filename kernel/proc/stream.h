#ifndef PROC_STREAM_H_e03b4c6f
#define PROC_STREAM_H_e03b4c6f
namespace proc
{
	namespace stream
	{
		int write(int streamId,int length,char *data);
		int read(int streamId ,int length,char *data);
		
	}
}
#endif
