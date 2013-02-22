[format "WCOFF"]
[instrset "i486p"]
[bits 32]
[file "klib.nas"]

global __alloca
global _memcpy

[section .text]

__alloca:
	sub eax,4
	sub esp,eax
	jmp [esp+eax]

_memcpy: ;void *memcpy( void *str1 , const void *str2 , size_t len );
	push ebx
	mov eax,[esp+8]
	mov ecx,[esp+0x0c]
	mov edx,[esp+0x10]
L1:	test edx,0x80000000
	jne L2
	mov BL, BYTE [ECX]
	mov byte [eax], bl
	dec edx
	inc eax
	inc ecx
	jmp L1
L2:	mov eax,[esp+8]
	pop ebx
	ret

