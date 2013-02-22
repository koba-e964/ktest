INCPATH = -I../headers/ -I../../comm_inc/
ZTOOLS=../../../z_tools/

include ./objs.def


default:stdc/kclib00.lib Makefile

%.obj : %.nas Makefile
	$(ZTOOLS)nask.exe $*.nas $*.obj $*.lst

%.obj : %.gas Makefile
	$(ZTOOLS)/as.exe -a  -o $*.obj $*.gas >$*.lst

%.gas : %.c Makefile
	$(ZTOOLS)/cc1.exe -Os -O2 -Wall $(INCPATH) -quiet  -o $*.gas $*.c

stdc/kclib00.lib:$(CLIBS) Makefile
	$(ZTOOLS)/golib00.exe $(CLIBS) out:$@