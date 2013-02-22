#include "proc.h"
#include <stdio.h>

extern ThreadCtl *ptr_thctl;
namespace proc
{
	using namespace util::macros;
	ProcCtl::ProcCtl(void){}//dummy:it does nothing
	void ProcCtl::ctor(void)
	{
		DEBUG_PRINT("ProcCtl::ctor\n");
		using namespace util;
		for(int i=0;i<sizeArray(pid_tbl);i++)
		{
			pid_tbl[i]=-1;
		}
		ProcInfo pi={-1,0};
		for(int i=0;i<sizeArray(procs);i++)
		{
			procs[i]=pi;
		}
	}
	int ProcCtl::create(int thid,int stdio[3])
	{
		int prid=-1;//find blank in procs ,-1 for invaild proc_id
		{
			for(int i=0;i<sizeArray(procs);i++)
			{
				if(procs[i].starttime==-1)
				{
					prid=i;
					break;
				}
			}
		}
		if(prid==-1)//Not found
		{
			return -1;
		}
		int ctime=timerctl.count;
		if(ctime==-1)ctime=-2;
		procs[prid].starttime=ctime;
		procs[prid].pd=mem::talloc<ProcData>();
		for(int i=0;i<3;i++)procs[prid].pd->fd_tbl[i]=stdio[i];
		for(int i=3;i<sizeArray(procs[prid].pd->fd_tbl);i++)
		{
			procs[prid].pd->fd_tbl[i]=-1;//null
		}
		pid_tbl[thid]=prid;
		return prid;
	}
	int ProcCtl::start_app(const char *filename)
	{
		using namespace fs;
		using namespace io::iomng;
		//parse
		char name[8+1]={0},ext[3+1]={0};
		{
			fat12::nameParse(filename,name,ext);
		}
		FAT12Info *app=fat12img->getInfo(0x2600,224,name,ext);

		char str[0x100];
		sprintf(str,"app:name=%s,name=%s,",name,ext);
		DEBUG_PRINT(str);
		if(app==(FAT12Info*)0)return -1;
		int size=app->size;
		sprintf(str,"size=%x\n",size);
		DEBUG_PRINT(str);
		char *buf=(char*)memmng_alloc_4k(size);
		tmpCode::readFileAll(app,size,buf);
		int imgsize=((int*)buf)[0];
		int imgcodelen=imgsize,imgdatalen=imgsize;//‰ž‹}ˆ’u
		char *imgcodebuf=(char*)memmng_alloc_4k(imgcodelen);
		char *imgdatabuf= (char*)memmng_alloc_4k(imgdatalen);
		//Loading..
		int initesp=((int*)buf)[3];
		int datasize=((int*)buf)[4];
		int dataoffset=((int*)buf)[5];
		int heapaddr=((int*)buf)[8];
		{
			memcpy(imgcodebuf + 0,(const char*)buf + 0,dataoffset-0);//.text
			memcpy(imgdatabuf + initesp,buf + dataoffset,datasize);//.data
			
		}
		int entry=0x1b;
		sprintf(str,"app.entry=%08x\n",entry);
		DEBUG_PRINT(str);
		int app_th;
		app_th=thread_alloc();
		{
			const int LDT_BIT=0x04;
			int ldt_code=0*8 + LDT_BIT;
			int ldt_data=1*8 + LDT_BIT;
			ThreadInfo* ti=&ptr_thctl->threads[app_th];
			ti->tss.eflags=0x202;//if=1
			ti->tss.eip=(i32)(entry);
			ti->tss.ldtr=task::LDTInGDT(app_th) * 8;
			ti->tss.iomap=0x40000000;
			
			ti->tss.esp=(int)(initesp);
			ti->tss.es=ldt_data;
			ti->tss.cs=ldt_code;
			ti->tss.ss=ldt_data;
			ti->tss.ds=ldt_data;
			ti->tss.fs=ldt_data;
			ti->tss.gs=ldt_data;
		
	
			ti->state=THREAD_RUNNING;
			ti->argv=0;
			ti->level=0;
			set_segmdesc(ti->ldt + 0,0xffff,(int)imgcodebuf,AR_CODE32_ER + 0x00 );
			set_segmdesc(ti->ldt + 1,0xffff,(int)imgdatabuf,AR_DATA32_RW + 0x00 );
		}
		thread_add(app_th);
		thread_suspend(app_th);
		sprintf(str,"Application : app_thid=%d\n",app_th);
		DEBUG_PRINT(str);
		memmng_free_4k(buf,size);
		sprintf(str,"start_app:screen=%d\n",screen);
		DEBUG_PRINT(str);
		int stdio[3]={-1,screen,screen};
		int prid=this->create(app_th,stdio);
		this->procs[prid].pd->init(imgcodelen,imgcodebuf,imgdatalen,imgdatabuf);
		{
			int efl=load_eflags();
			io_cli();
			thread_resume(app_th);
			dump_threadlist();
			store_eflags(efl);
		}
		return 0;
	}
	void ProcCtl::init(void)
	{
		procctl = mem::talloc<ProcCtl>();
		char str[0x100];
		sprintf(str,"init:procctl=%08x,sizeof(ProcCtl)=%08x\n",procctl,sizeof(ProcCtl));
		DEBUG_PRINT(str);
		procctl->ctor();
	}
	ProcData *ProcCtl::current(void)const
	{
		int prid=pid_tbl[thread_current()];
		ProcData *pd=this->procs[prid].pd;
		char str[0x100];
		sprintf(str,"ProcCtl::current pid_tbl=%08x,pd=%08x,prid=%d\n",pid_tbl,pd,prid);
		DEBUG_PRINT(str);
		return pd;
	}
	void ProcData::init(int codelen,void *code,int detalen,void *data)
	{
		this->codelen=codelen;
		this->code=code;
		this->datalen=datalen;
		this->data=data;
	}
	int ProcData::open(const char *filename,int attr)
	{
		char str[0x100];
		sprintf(str,"ProcData::open(%s,%x)\n",filename,attr);
		DEBUG_PRINT(str);
		char name[8+1],ext[3+1];
		fs::fat12::nameParse(filename,name,ext);
		return -1;//null descriptor
	}
	int ProcData::write(int fd,void *buffer,int length)
	{
		using namespace io::iomng;
		char str[0x100];
		int dev_id=this->fd_tbl[fd];
		sprintf(str,"ProcData::write this=%08x,fd_tbl=%08x,fd=%d,length=%x,dev_id=%x\n",this,this->fd_tbl,fd,length,dev_id);
		DEBUG_PRINT(str);
		RWData data={length,buffer};
		return iomng->access(dev_id,WRITE,&data);
	}
}
