#include <io.h>
#include <string.h>


#define knin(kosf,kosf64,niof) \
int kosf(int fd) {int high,low;niof(fd,&low,&high);return low;}\
i64 kosf64(int fd){int high,low;niof(fd,&low,&high);return (i64)low|((i64)high<<32);}
#define knin2(kosf,kosf64,niof) \
int kosf(int fd,int arg){return niof(fd,arg,0);}\
int kosf64(int fd,i64 arg){return niof(fd,(int)arg,(int)((unsigned long long)arg>>32));}

knin(kos_size,kos_size64,nio_size)
knin(kos_tell,kos_tell64,nio_tell)
knin2(kos_seek,kos_seek64,nio_seek)
knin2(kos_seteof,kos_seteof64,nio_seteof)


int kos_open(const char *filename,int attr)
{
	return nio_open(strlen(filename),filename,attr);
}