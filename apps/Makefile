INCPATH = -Iheaders/ -I../comm_inc/
ZTOOLS=../../z_tools/
all : sum.hrb



%.obj : %.nas Makefile
	$(ZTOOLS)nask.exe $*.nas $*.obj $*.lst

%.obj : %.gas Makefile
	$(ZTOOLS)/as.exe -a  -o $*.obj $*.gas >$*.lst

%.gas : %.c Makefile
	$(ZTOOLS)/cc1.exe -Os -Wall  $(INCPATH) -quiet  -o $*.gas $*.c

sum.bim : sum.obj  deplib
	$(ZTOOLS)obj2bim.exe @./kapp.rul out:sum.bim stack:1k map:sum.map sum.obj || rm sum.bim

sum.hrb : sum.bim 
	$(ZTOOLS)bim2hrb.exe sum.bim sum.hrb 0
deplib:
	make -C impl