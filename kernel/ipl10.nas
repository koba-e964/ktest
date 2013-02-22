; haribote-ipl_
; TAB=4

CYLS  EQU    10        ;; �ǂݍ��ރV�����_���̐ݒ�

    ORG    0x7c00      ;; ���̃v���O�����̓ǂݍ��݈ʒu
    JMP    entry       ;; �{�̃R�[�h�փW�����v

;; �ȉ��͕W���I��FAT12�t�H�[�}�b�g�t���b�s�[�f�B�X�N�̂��߂̋L�q

    nop         ; NULL
    DB    "TestBOOT"   ; �u�[�g�Z�N�^�̖��O�����R�ɏ����Ă悢�i8�o�C�g�j
    DW    512          ; 1�Z�N�^�̑傫���i512�ɂ��Ȃ���΂����Ȃ��j
    DB    1            ; �N���X�^�̑傫���i1�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
    DW    1            ; FAT���ǂ�����n�܂邩�i���ʂ�1�Z�N�^�ڂ���ɂ���j
    DB    2            ; FAT�̌��i2�ɂ��Ȃ���΂����Ȃ��j
    DW    224          ; ���[�g�f�B���N�g���̈�̑傫���i���ʂ�224�G���g���ɂ���j
    DW    2880         ; ���̃h���C�u�̑傫���i2880�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
    DB    0xf0         ; ���f�B�A�̃^�C�v�i0xf0�ɂ��Ȃ���΂����Ȃ��j
    DW    9            ; FAT�̈�̒����i9�Z�N�^�ɂ��Ȃ���΂����Ȃ��j
    DW    18           ; 1�g���b�N�ɂ����̃Z�N�^�����邩�i18�ɂ��Ȃ���΂����Ȃ��j
    DW    2            ; �w�b�h�̐��i2�ɂ��Ȃ���΂����Ȃ��j
    DD    0            ; �p�[�e�B�V�������g���ĂȂ��̂ł����͕K��0
    DD    2880         ; ���̃h���C�u�傫����������x����
    DB    0,0,0x29     ; �悭�킩��Ȃ����ǂ��̒l�ɂ��Ă����Ƃ����炵��
    DD    0xc3fa31b8   ; ���Ԃ�{�����[���V���A���ԍ�
    DB    "TestOS_03c0"; �f�B�X�N�̖��O�i11�o�C�g�j
    DB    "FAT12   "   ; �t�H�[�}�b�g�̖��O�i8�o�C�g�j
    RESB  18           ; �Ƃ肠����18�o�C�g�����Ă���


;; -- START ---------------------------------------------------------
;; ------------------------------------------------------------------

entry:

    XOR    AX,AX     ;; �Z�O�����g�̏�����
    MOV    SS,AX     ;; SS = 0x00
    MOV    SP,0x7c00 ;; SP = 0x7c00
    MOV    DS,AX     ;; DS = 0x00

    MOV    AX,0x0820 ;; �f�B�X�N�ǂݍ��݂̐ݒ�
    MOV    ES,AX     ;; ES = 0x0820
    XOR    CH,CH     ;; �V�����_
    XOR    DH,DH     ;; �w�b�h
    MOV    CL,2      ;; �Z�N�^

    MOV    AX, msg_start ;; ipl10.nas�̊J�n
    CALL   print         ;; �f�o�b�O���̏o��

;; ------------------------------------
;; �f�B�X�N����ǂݍ���
;; ------------------------------------

    MOV    AX, msg_read ;; �ǂݍ��݊J�n
    CALL   print        ;; �f�o�b�O���̏o��

readloop:

    XOR    SI,SI     ;; �ǂݍ��ݎ��s�񐔂𐔂��郌�W�X�^

retry:

    MOV    AH,0x02   ;; �f�B�X�N�ǂݍ��݁iBIOS�j
    MOV    AL,1      ;; 1�Z�N�^
    XOR    BX,BX     ;; �o�b�t�@�iES:BX�j
    XOR    DL,DL     ;; A�h���C�u
    INT    0x13      ;; BIOS CALL
    JNC    next      ;; �����Ȃ�next�֐i��

    INC    SI        ;; �ǂݍ��ݎ��s�񐔂��C���N�������g
    CMP    SI,5      ;; SI��5���r
    JAE    error     ;; �ǂݍ��ݎ��s�񐔂�5�ȏ�Ȃ�error�֐i��

    XOR    AH,AH     ;; �h���C�u�̃��Z�b�g�iBIOS�j
    XOR    DL,DL     ;; A�h���C�u
    INT    0x13      ;; BIOS CALL
    JMP    retry     ;; �ă`�������W���A���̂��`

next:

    MOV    AX,ES     ;; ES:BX�����̃Z�N�^��ǂݍ��ނ��߂̃������A�h���X�ɂ��邽��
    ADD    AX,0x0020 ;; ES��512(0x200)�����Z����̂����A�Z�O�����g��
    MOV    ES,AX     ;; �萔��n�����߂͖����̂ŁA3�̖��߂Ŏ������Ă���
    INC    CL        ;; �Z�N�^���C���N�������g
    CMP    CL,18     ;; CL��18���r
    JBE    readloop  ;; �ǂݍ��݃Z�N�^����18�ȉ��Ȃ�readloop�֐i��

    MOV    CL,1      ;; 18�Z�N�^�ǂݏI�����̂ōĂ�1�ɖ߂�
    ADD    DH,1      ;; �w�b�_���C���N�������g�i���͗�������ǂށj
    CMP    DH,2      ;; DH��2���r
    JB     readloop  ;; �܂����ʓǂݏI����ĂȂ��Ȃ�readloop�֐i��

    XOR    DH,DH     ;; ���ʓǂݏI�����̂ōĂуw�b�_��0�ɖ߂�
    ADD    CH,1      ;; �V�����_���C���N�������g
    CMP    CH,CYLS   ;; CH��CYLS���r
    JB     readloop  ;; �ǂݍ��񂾃V�����_����
                     ;; �ǂݍ��ݗ\��̃V�����_����菬�����Ȃ�readloop�֐i��

    MOV    AX, msg_ok ;; �ǂݍ��݊���
    CALL   print      ;; �f�o�b�O���̏o��


;; ------------------------------------
;; �ǂݍ��݃f�[�^�̎��s�iharibote.sys�j
;; ------------------------------------

    MOV    AX, msg_exec ;; �ǂݍ��݃f�[�^�̎��s
    CALL   print        ;; �f�o�b�O���̏o��

    MOV    [0x0ff0],CH  ;; IPL���ǂ��܂œǂ񂾂̂�������
    JMP    0xc200


;; ------------------------------------
;; ������̏o��
;; ------------------------------------

print:

    PUSHA
    MOV    SI,AX     ;; SI�֏o�͂��镶����A�h���X������
putloop:
    MOV    AL,[SI]   ;; 1�����ǂݍ���
    CMP    AL,0      ;; �ǂݍ��񂾕�����0x00�Ȃ�
    JE     putfin    ;; �����̏I�[�Ɣ��f����putfin�֐i��
    MOV    AH,0x0e   ;; �ꕶ���\���t�@���N�V����
    MOV    BX,15     ;; �J���[�R�[�h
    INT    0x10      ;; BIOS CALL
    INC    SI        ;; SI�����̕����̃A�h���X�ɂ���
    JMP    putloop
putfin:
    POPA
    ret

msg_start: DB    0x0a, "ipl10.nas", 0x0a, 0
msg_read:  DB    "  reading disk to memory ... ", 0
msg_exec:  DB    "  jump to reading data", 0
msg_ok:    DB    "ok!", 0x0a, 0
msg_error: DB    "error!", 0x0a, 0


;; ------------------------------------
;; �G���[���̏���
;; ------------------------------------

error:

    MOV    AX, msg_error ;; �ǂݍ��݃f�[�^�̎��s
    CALL   print         ;; �f�o�b�O���̏o��

    XOR    AX,AX
    MOV    ES,AX     ;; ES = 0x00

fin:

    HLT              ;; ��������܂�CPU���~������
    JMP    fin       ;; �������[�v
                     ;; �ǂ��܂ł��������[�v�A�I���Ȃ��f�G�ȃ��[�v�i�d�C�O���[�v�j

;; ------------------------------------------------------------------
;; ----------------------------------------------------------- EOF --


RESB  0x7dfe-$   ;; 0x7dfe�܂ł�0x00�Ŗ��߂閽��
DB    0x55, 0xaa ;; �Ō��2�o�C�g�͌Œ�l

