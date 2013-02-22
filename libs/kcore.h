#ifndef KCORE_H
#define KCORE_H

typedef void* Handle;
Handle core_start_process(const char* filename,u32 searchType,
			const SPAttrib *spa);
int core_kill_process(Handle target,u32 anxiety);

int core_exit(int exitCode,int affix);
int core_terminate(int ex,int af);

Handle core_open(const char* fileName,u32 attrib);
int core_close(Handle);

#endif
