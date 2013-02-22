include ./objs.def

TOOLPATH = ../../z_tools/
LTOOL    = ../../k_tool/
INCPATH  = $(TOOLPATH)haribote

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
CC1X     = $(TOOLPATH)cc1plus -I$(INCPATH) -Os -Wall -quiet -DCPP_MODE
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = ./haribote.rul
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = cp
DEL      = rm
LISTDIR=listfile/
# デフォルト動作

default :
	$(MAKE) haribote.sys

# ファイル生成規則
.PHONY : listdir
listdir:
	if test -f $(LISTDIR);then rm $(LISTDIR);fi
	if test ! -e $(LISTDIR);then mkdir $(LISTDIR);fi
	if test ! -e $(LISTDIR)/util; then mkdir $(LISTDIR)/util;fi
	if test ! -e $(LISTDIR)/device; then mkdir $(LISTDIR)/device;fi
	if test ! -e $(LISTDIR)/fs; then mkdir $(LISTDIR)/fs;fi
	if test ! -e $(LISTDIR)/io; then mkdir $(LISTDIR)/io;fi
	if test ! -e $(LISTDIR)/proc; then mkdir $(LISTDIR)/proc;fi
rm-listdir:
	-rm $(LISTDIR) -r

ipl10.bin : ipl10.nas Makefile
	$(MAKE) listdir
	$(NASK) ipl10.nas ipl10.bin $(LISTDIR)/ipl10.lst

asmhead.bin : asmhead.nas Makefile
	$(MAKE) listdir
	$(NASK) asmhead.nas asmhead.bin $(LISTDIR)/asmhead.lst

hankaku.bin : hankaku.def Makefile
	$(MAKEFONT) hankaku.def hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku
bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK) || rm bootpack.bim
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0
	$(LTOOL)sum.exe bootpack.hrb bpsum.bin

haribote.sys : asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb >haribote.sys


# 一般規則

%.gas : %.c $(HEADERS)
	$(CC1) -O2 -o $*.gas $*.c

%.gas : %.cpp $(HEADERS)
	$(CC1X) -O2 -o $*.gas $*.cpp

#%.nas : %.gas
#	-$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas
	$(NASK) $*.nas $*.obj $(LISTDIR)/$*.lst
%.obj : %.gas
	$(TOOLPATH)as.exe -a $*.gas -o $*.obj >$(LISTDIR)$*.lst
#For Test
%.test : %.cpp %.h
	g++ -o $*.exe $*.cpp -DTEST
	$*.exe

# コマンド


crun : img Makefile
	$(COPY) haribote.img ../z_tools/qemu/fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

.PHONY : clean
clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys
	$(MAKE) rm-listdir
	$(MAKE) -C device clean
	$(MAKE) -C util clean

