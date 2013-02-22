#ifndef PROC_PROC_H_451da30f
#define PROC_PROC_H_451da30f
#include "../task.h"
#include "../all.h"

namespace proc
{
	const int FD_MAX=0x400;
	typedef struct ProcData
	{
		int fd_tbl[FD_MAX];//index of the handle(io::iomng::DeviceData*)
		int fd_attr[FD_MAX];
		int codelen;
		void *code;
		int datalen;
		void *data;
		void init(int codelen,void* code,int datalen,void *data);
		/**
			@retval file descriptor
		*/
		int open(const char *filename,int attrib);
		int write(int fd,void *ptr,int len);
	}ProcData;	
	class ProcCtl
	{
	private:
		static const int PROCESS_MAX=100;
		//using namespace util;
		int pid_tbl[THREAD_MAX];//‚Ç‚Ìthread‚ª‚Ç‚Ìprocess‚É‘®‚·‚é‚©
		typedef struct ProcInfo
		{
			int starttime;//-1 -> not used
			ProcData *pd;
		}ProcInfo;
		ProcInfo procs[PROCESS_MAX];
	public:
		ProcCtl(void);
		void ctor(void);
		int create(int threadId,int stdio[3]);
		int start_app(const char* filename);
		int destroy(int procId,int result);
		static void init(void);
		ProcData *current(void)const;
	};
	global ProcCtl *procctl;//linker‚ÌŽd—l‚Åctor‚ªŒÄ‚Î‚ê‚È‚¢
}//namespace proc

#endif
