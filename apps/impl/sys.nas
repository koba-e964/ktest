[format "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "sys.nas"]
global  _sys_exit,_sys_end
;int sys_exit(int result,int affix);
[SECTION .text]
_sys_end: ;void sys_end();=>sys_exit(0,0);
	push 0
	push 0
	call _sys_exit
	add esp,8
	ret
_sys_exit:
	mov eax,2
	mov ecx,[esp +4];
	mov edx,[esp + 8]
	int 0x40;
	ret

