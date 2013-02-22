TOOLPATH = ../z_tools/
KERNEL = ./kernel/
EDIMG    = $(TOOLPATH)edimg.exe


haribote.img : kernel apps Makefile imgrule.def
	$(EDIMG) @imgrule.def

.PHONY:kernel
kernel:
	make -C $(KERNEL) ipl10.bin haribote.sys

.PHONY:apps
apps:
	make -C ./apps/ all


clean:
	make -C $(KERNEL) clean
install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img
src_only : clean
	-rm haribote.img

img :
	$(MAKE) haribote.img 

crun : img Makefile
	$(COPY) haribote.img ../z_tools/qemu/fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

run: img Makefile
	$(TOOLPATH)qemu/qemu.exe -L $(TOOLPATH)qemu/ -m 32 -localtime -std-vga -fda haribote.img

