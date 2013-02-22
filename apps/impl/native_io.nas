[format "WCOFF"]
[instrset "i486p"]
[bits 32]
[file "native_io.nas"]

GLOBAL  _nio_open,_kos_write, _kos_read, _nio_seek, _nio_tell,_nio_size, _nio_seteof , _kos_close


[section .text}
_nio_open: ;native int nio_open(int namelen,const char *filename,int attr);
	mov eax,0x20000
	jmp common_nio_syscall
_kos_read: ; native int kos_fread(int fd,char *buf,int len);
	mov eax,0x20001
	jmp common_nio_syscall
_kos_write: ;native int kos_write(int fd,const char *buf,int len);
	mov eax,0x20002
	jmp common_nio_syscall
_nio_seek: ;native int nio_seek(int fd,int *posLow,int *posHigh);
	mov eax,0x20003
	jmp common_nio_syscall
_nio_tell: ;native int nio_tell(int fd,int *fsizeLow,int *fsizHigh);
	mov eax,0x20004
	jmp common_nio_syscall
_nio_size: 
	mov eax,0x20005
	jmp common_nio_syscall
_nio_seteof:
	mov eax,0x20006
	jmp common_nio_syscall
_kos_close:
	mov eax,0x20007
	jmp common_nio_syscall

common_nio_syscall:
	mov ecx,[esp+4]
	mov edx,[esp+8]
	mov ebx,[esp+0x0c]
	int 0x40
	ret

