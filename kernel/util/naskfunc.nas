; naskfunc
; TAB=4

[FORMAT "WCOFF"]        ; �I�u�W�F�N�g�t�@�C������郂�[�h  
[INSTRSET "i486p"]      ; 486�̖��߂܂Ŏg�������Ƃ����L�q
[BITS 32]               ; 32�r�b�g���[�h�p�̋@�B�����点��
[FILE "naskfunc.nas"]   ; �\�[�X�t�@�C�������

    GLOBAL  _io_hlt, _io_cli, _io_sti,_io_stihlt, _io_out8
    GLOBAL  _load_eflags, _store_eflags,_load_cr0,_store_cr0

    GLOBAL  _load_gdtr, _load_idtr
    GLOBAL  _asm_inthandler0c , _asm_inthandler0d , _asm_inthandler20,_asm_inthandler21, _asm_inthandler26, _asm_inthandler2c , _asm_inthandler40
    global  _io_out16,_io_out32,_io_in8,_io_in16,_io_in32

    EXTERN _inthandler0c, _inthandler0d , _inthandler20 , _inthandler21, _inthandler26 , _inthandler2c , _app_syscall

	global _farjmp,_farcall
	global _task_change
	global _load_tr
	global _lock_acquire, _lock_release
[SECTION .text]

_io_hlt:  ;; void io_hlt(void);
    HLT   ;; CPU�̋x�~����
    RET   ;; �������[�v�̏ꍇ�͕K����������邱��

_io_cli:  ;; void io_cli(void);
    CLI   ;; ���荞�݋���
    RET

_io_sti:  ;; void io_sti(void);
    STI   ;; ���荞�݋���
    RET
_io_stihlt: ; void io_stihlt(void)
	sti
	hlt
	ret

_io_out8:              ;; void io_out8(int port, int data);
    MOV    EDX,[ESP+4] ;; �������ݐ�|�[�g
    MOV    AL,[ESP+8]  ;; �������ރf�[�^
    OUT    DX,AL
    RET
_io_out16:
	mov edx,[esp+4]
	mov eax,[esp+8]
	out dx,ax
	ret
_io_out32:
	mov edx,[esp+4]
	mov eax,[esp+8]
	out dx,eax
	ret
_io_in8:	;int io_in8(int port);
	mov edx,[esp+4]
	xor eax,eax
	in al,dx
	ret
_io_in16:	;int io_in16(int port);
	mov edx,[esp+4]
	xor eax,eax
	in ax,dx
	ret
_io_in32:	;int io_in32(int port);
	mov edx,[esp+4]
	in eax,dx
	ret

_io_load_eflags:       ;; int io_load_eflags(void);
    PUSHFD             ;; �t���O���W�X�^���X�^�b�N�֊i�[
    POP    EAX         ;; ������EAX���W�X�^�Ɏ��o��
    RET                ;; EAX���W�X�^�ɓ����Ă���f�[�^���߂�l�H

_io_store_eflags:      ;; void io_store_eflags(int eflags);
    MOV    EAX,[ESP+4] ;; ESP+4�Ɉ�����eflags�������Ă���炵��
    PUSH   EAX         ;; ������eflags���X�^�b�N�ɓ����
    POPFD              ;; �����ăt���O���W�X�^�Ɏ��o��
    RET


_load_gdtr:            ;; void load_gdtr(int limit, int addr);
    MOV    AX,[ESP+4]  ;; GDTR���W�X�^��48�r�b�g�i6�o�C�g�j�ŁA
    MOV    [ESP+6],AX  ;; ����16�r�b�g��GDT�̃��~�b�g�l�iGDT�̃T�C�Y�̈ӁH�j
    LGDT  [ESP+6]      ;; ���32�r�b�g��GDT�̃A�h���X������A
    RET                ;; limit(4byte), addr(4byte)�̈�������
                       ;; ���܂��v�Z���ă��W�X�^�ɏ�������ł���

_load_idtr:            ;; void load_idtr(int limit, int addr);
    MOV    AX,[ESP+4]  ;; GDT�̏ꍇ�͓���
    MOV    [ESP+6],AX  ;; �������ł�LIDT���߂��g������
    LIDT  [ESP+6]
    RET

_asm_inthandler0c:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,0x08
		mov		ss,ax
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; INT 0x0c �ł��A���ꂪ�K�v
	label:
		sti
		hlt
		jmp label
		IRETD

_asm_inthandler0d:
		;STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,0x08
		mov		ss,ax
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0d
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; INT 0x0d �ł́A���ꂪ�K�v
		IRETD
	
_asm_inthandler20:
    PUSH  ES           ;; ES��DS���X�^�b�N��
    PUSH  DS           ;; �����PUSHAD�Ŋ�{�I��8���W�X�^���X�^�b�N��
    PUSHAD             ;; push eax, ecx, edx, ebx, esi, edi, esp, ebp
    MOV   EAX,ESP
    PUSH  EAX
    MOV   AX,SS        ;; DS = ES = SS
    MOV   DS,AX        ;; C����̃R�[�h��DS, ES, SS�����ׂē����Z�O�����g�łȂ����
    MOV   ES,AX        ;; ���܂����s���Ă���Ȃ��i���ނ�C����`�j�B
    CALL  _inthandler20
    POP   EAX
    POPAD              ;; ���̊��荞�ݏ��������s���ꂽ���
    POP   DS           ;; ���s�����O�Ƃ܂������������W�X�^�̏�Ԃ�
    POP   ES           ;; ���Ă����Ȃ���΂Ȃ�Ȃ��̂ŁA���̂悤�ɂ킴�킴�߂��Ă���
    IRETD
 
_asm_inthandler21:     ;; �L�[�{�[�h���荞�ݎ��Ɏ��s�����
    PUSH  ES           ;; ES��DS���X�^�b�N��
    PUSH  DS           ;; �����PUSHAD�Ŋ�{�I��8���W�X�^���X�^�b�N��
    PUSHAD             ;; push eax, ecx, edx, ebx, esi, edi, esp, ebp
    MOV   EAX,ESP
    PUSH  EAX
    MOV   AX,SS        ;; DS = ES = SS
    MOV   DS,AX        ;; C����̃R�[�h��DS, ES, SS�����ׂē����Z�O�����g�łȂ����
    MOV   ES,AX        ;; ���܂����s���Ă���Ȃ��i���ނ�C����`�j�B
    CALL  _inthandler21
    POP   EAX
    POPAD              ;; ���̊��荞�ݏ��������s���ꂽ���
    POP   DS           ;; ���s�����O�Ƃ܂������������W�X�^�̏�Ԃ�
    POP   ES           ;; ���Ă����Ȃ���΂Ȃ�Ȃ��̂ŁA���̂悤�ɂ킴�킴�߂��Ă���
    IRETD
_asm_inthandler26:     ;; �L�[�{�[�h���荞�ݎ��Ɏ��s�����
    PUSH  ES           ;; ES��DS���X�^�b�N��
    PUSH  DS           ;; �����PUSHAD�Ŋ�{�I��8���W�X�^���X�^�b�N��
    PUSHAD             ;; push eax, ecx, edx, ebx, esi, edi, esp, ebp
    MOV   EAX,ESP
    PUSH  EAX
    MOV   AX,SS        ;; DS = ES = SS
    MOV   DS,AX        ;; C����̃R�[�h��DS, ES, SS�����ׂē����Z�O�����g�łȂ����
    MOV   ES,AX        ;; ���܂����s���Ă���Ȃ��i���ނ�C����`�j�B
    CALL  _inthandler26
    POP   EAX
    POPAD              ;; ���̊��荞�ݏ��������s���ꂽ���
    POP   DS           ;; ���s�����O�Ƃ܂������������W�X�^�̏�Ԃ�
    POP   ES           ;; ���Ă����Ȃ���΂Ȃ�Ȃ��̂ŁA���̂悤�ɂ킴�킴�߂��Ă���
    IRETD

_asm_inthandler2c:     ;; �}�E�X���荞�ݎ��Ɏ��s�����
    PUSH  ES           ;; ���̑��̃R�[�h��_asm_inthandler21�Ɠ���
    PUSH  DS
    PUSHAD
    MOV    EAX,ESP
    PUSH  EAX
    MOV    AX,SS
    MOV    DS,AX
    MOV    ES,AX
    CALL  _inthandler2c
    POP    EAX
    POPAD
    POP    DS
    POP    ES
    IRETD
_asm_inthandler40:     ;; Application SystemCall (eax=num,ecx=a0,edx=a1,ebx=a3)
    PUSH  ES           ;; ES��DS���X�^�b�N��
    PUSH  DS           ;; �����PUSHAD�Ŋ�{�I��8���W�X�^���X�^�b�N��
    push esi
    push edi
    push ebp             ;; push esi, edi, esp, ebp
     mov   si,ss
     MOV   di,08        ;; DS = ES = SS = 8 ; kernel data
     mov   ss,di
      mov   ebp,esp
      mov   esp,0x0026b000 ;;free space,size=0x3000(12KiB)
      MOV   DS,di        ;; C����̃R�[�h��DS, ES, SS�����ׂē����Z�O�����g�łȂ����
      MOV   ES,di        ;; ���܂����s���Ă���Ȃ��i���ނ�C����`�j�B
      push esi           ;; old ss
      push ebp           ;; old esp
       push ebx
       push edx
       push ecx
       push eax
       CALL  _app_syscall
       add  esp,16
      pop  ebp
      pop  esi
      mov  esp,ebp
     mov  ss,si         ;;return to the application
    pop ebp
    pop edi
    pop esi            
    POP   DS           
    POP   ES           
    IRETD
 	
_load_eflags:	;uint load_eflags(void);
	pushfd
	pop eax
	ret
_store_eflags:	;void store_eflags(uint newflags);
	mov eax,[4+esp]
	push eax
	popfd
	ret
_load_cr0:
	mov eax,cr0
	ret
_store_cr0:	;void store_cr0(uint);
	mov eax,[4+esp]
	mov cr0,eax
	ret
_farjmp: ; void farjmp(int proc,int new_cs)
	jmp far [4+esp]
	ret
_farcall:
	call far [4+esp]
	ret
_task_change ; void task_change(int new_cs)
	push dword [4+esp]
	push 0
	jmp far [esp]
	add esp,8
	ret

_load_tr:
	ltr [4+esp]
	ret
_lock_acquire: ; void lock_acquire(int *val);
	mov ecx,[4+esp]
	mov eax,1
loop262:
	xchg eax,[ecx] ; atomically xchg 1 with *val
	test eax,eax
	jne loop262
;end of loop262
	ret

_lock_release: ;void lock_release(int *val);
	mov ecx,[4+esp]
	mov dword[ecx],0
	ret

