#ifndef IO_H_52af
#define IO_H_52af
#include <comm_io.h>
#include <sys/types.h>
#define native(sysnum) /**/
native(0) int nio_open(int namelen,const char *filename,int attr);
native(1) int kos_read(int fd,char *buf,int len);
native(2) int kos_write(int fd,const char *buf,int len);
native(3) int nio_seek(int fd,int posLow,int posHigh);
native(4) int nio_tell(int fd,int *fsizLow,int *fsizHigh);
native(5) int nio_size(int fd,int *fsizLow,int *fsizHigh);
native(6) int nio_seteof(int fd,int posLow,int posHigh);
native(7) int kos_close(int fd);
int kos_open(const char *filename,int attr);
int kos_seek(int fd,int pos);
int kos_seek64(int fd,i64 pos);
int kos_size(int fd);
i64 kos_size64(int fd);
int kos_seteof(int fd,int pos);
int kos_seteof64(int fd,i64 pos);
#endif
