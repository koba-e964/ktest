; naskfunc
; TAB=4

[FORMAT "WCOFF"]        ; オブジェクトファイルを作るモード  
[INSTRSET "i486p"]      ; 486の命令まで使いたいという記述
[BITS 32]               ; 32ビットモード用の機械語を作らせる
[FILE "naskfunc.nas"]   ; ソースファイル名情報

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
    HLT   ;; CPUの休止命令
    RET   ;; 無限ループの場合は必ずこれを入れること

_io_cli:  ;; void io_cli(void);
    CLI   ;; 割り込み拒否
    RET

_io_sti:  ;; void io_sti(void);
    STI   ;; 割り込み許可
    RET
_io_stihlt: ; void io_stihlt(void)
	sti
	hlt
	ret

_io_out8:              ;; void io_out8(int port, int data);
    MOV    EDX,[ESP+4] ;; 書き込み先ポート
    MOV    AL,[ESP+8]  ;; 書き込むデータ
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
    PUSHFD             ;; フラグレジスタをスタックへ格納
    POP    EAX         ;; そしてEAXレジスタに取り出す
    RET                ;; EAXレジスタに入っているデータが戻り値？

_io_store_eflags:      ;; void io_store_eflags(int eflags);
    MOV    EAX,[ESP+4] ;; ESP+4に引数のeflagsが入っているらしい
    PUSH   EAX         ;; 引数のeflagsをスタックに入れる
    POPFD              ;; そしてフラグレジスタに取り出す
    RET


_load_gdtr:            ;; void load_gdtr(int limit, int addr);
    MOV    AX,[ESP+4]  ;; GDTRレジスタは48ビット（6バイト）で、
    MOV    [ESP+6],AX  ;; 下位16ビットがGDTのリミット値（GDTのサイズの意？）
    LGDT  [ESP+6]      ;; 上位32ビットがGDTのアドレスだから、
    RET                ;; limit(4byte), addr(4byte)の引数から
                       ;; うまく計算してレジスタに書き込んでいる

_load_idtr:            ;; void load_idtr(int limit, int addr);
    MOV    AX,[ESP+4]  ;; GDTの場合は同じ
    MOV    [ESP+6],AX  ;; こっちではLIDT命令を使うだけ
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
		ADD		ESP,4			; INT 0x0c でも、これが必要
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
		ADD		ESP,4			; INT 0x0d では、これが必要
		IRETD
	
_asm_inthandler20:
    PUSH  ES           ;; ESとDSをスタックへ
    PUSH  DS           ;; さらにPUSHADで基本的な8レジスタをスタックへ
    PUSHAD             ;; push eax, ecx, edx, ebx, esi, edi, esp, ebp
    MOV   EAX,ESP
    PUSH  EAX
    MOV   AX,SS        ;; DS = ES = SS
    MOV   DS,AX        ;; C言語のコードはDS, ES, SSがすべて同じセグメントでなければ
    MOV   ES,AX        ;; うまく実行してくれない（頼むぜC言語～）。
    CALL  _inthandler20
    POP   EAX
    POPAD              ;; この割り込み処理が実行された後は
    POP   DS           ;; 実行される前とまったく同じレジスタの状態に
    POP   ES           ;; しておかなければならないので、このようにわざわざ戻している
    IRETD
 
_asm_inthandler21:     ;; キーボード割り込み時に実行される
    PUSH  ES           ;; ESとDSをスタックへ
    PUSH  DS           ;; さらにPUSHADで基本的な8レジスタをスタックへ
    PUSHAD             ;; push eax, ecx, edx, ebx, esi, edi, esp, ebp
    MOV   EAX,ESP
    PUSH  EAX
    MOV   AX,SS        ;; DS = ES = SS
    MOV   DS,AX        ;; C言語のコードはDS, ES, SSがすべて同じセグメントでなければ
    MOV   ES,AX        ;; うまく実行してくれない（頼むぜC言語～）。
    CALL  _inthandler21
    POP   EAX
    POPAD              ;; この割り込み処理が実行された後は
    POP   DS           ;; 実行される前とまったく同じレジスタの状態に
    POP   ES           ;; しておかなければならないので、このようにわざわざ戻している
    IRETD
_asm_inthandler26:     ;; キーボード割り込み時に実行される
    PUSH  ES           ;; ESとDSをスタックへ
    PUSH  DS           ;; さらにPUSHADで基本的な8レジスタをスタックへ
    PUSHAD             ;; push eax, ecx, edx, ebx, esi, edi, esp, ebp
    MOV   EAX,ESP
    PUSH  EAX
    MOV   AX,SS        ;; DS = ES = SS
    MOV   DS,AX        ;; C言語のコードはDS, ES, SSがすべて同じセグメントでなければ
    MOV   ES,AX        ;; うまく実行してくれない（頼むぜC言語～）。
    CALL  _inthandler26
    POP   EAX
    POPAD              ;; この割り込み処理が実行された後は
    POP   DS           ;; 実行される前とまったく同じレジスタの状態に
    POP   ES           ;; しておかなければならないので、このようにわざわざ戻している
    IRETD

_asm_inthandler2c:     ;; マウス割り込み時に実行される
    PUSH  ES           ;; その他のコードは_asm_inthandler21と同じ
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
    PUSH  ES           ;; ESとDSをスタックへ
    PUSH  DS           ;; さらにPUSHADで基本的な8レジスタをスタックへ
    push esi
    push edi
    push ebp             ;; push esi, edi, esp, ebp
     mov   si,ss
     MOV   di,08        ;; DS = ES = SS = 8 ; kernel data
     mov   ss,di
      mov   ebp,esp
      mov   esp,0x0026b000 ;;free space,size=0x3000(12KiB)
      MOV   DS,di        ;; C言語のコードはDS, ES, SSがすべて同じセグメントでなければ
      MOV   ES,di        ;; うまく実行してくれない（頼むぜC言語～）。
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

