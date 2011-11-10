CROSS = ~/cross/bin
LD = $(CROSS)/x86_64-elf-ld
CC = $(CROSS)/x86_64-elf-gcc
CXX = $(CROSS)/x86_64-elf-g++
AS = nasm

ASFLAGS = -f elf64
LDFLAGS = -nostdlib -nodefaultlibs

objects = kmain.o

all::	boot.iso

install::	/media/sf_VirtualBox/boot.iso

.PHONY:	clean
clean::
	-rm -rf image boot.iso *.o *.img *~

%.o:	%.s
	$(AS) $(ASFLAGS) -o $@ $<

image/boot/%:	%
	-mkdir -p image/boot
	cp $< $@

/media/sf_VirtualBox/%:	%
	cp $< $@

image/bootstrap.elf32:	bootstrap.ld start.o
	-mkdir image
	$(LD) $(LDFLAGS) -T bootstrap.ld -o $@ start.o

image/kernel.elf64:	kernel.ld $(objects)
	-mkdir image
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(objects)

core.img:	grub-mkimage.cfg
	grub-mkimage -p "(cd)/boot" -o core.img -O i386-pc -c grub-mkimage.cfg biosdisk iso9660 multiboot configfile

eltorito.img:	/usr/lib/grub/i386-pc/cdboot.img core.img
	-mkdir -p image/boot
	cat $^ > $@

boot.iso:	image/boot/eltorito.img image/bootstrap.elf32 image/kernel.elf64 image/boot/grub.cfg
	genisoimage -R -b boot/eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o $@ image
