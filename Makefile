# Cross-compiling (e.g., on Mac OS X)
#TOOLPREFIX = i386-jos-elf-

# Using native tools (e.g., on X86 Linux)
TOOLPREFIX = 

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-builtin -Wall -MD -ggdb -m32
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m32
# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)

leios.img: bootblock
	dd if=/dev/zero of=leios.img count=10000
	dd if=boot/bootblock of=leios.img conv=notrunc

bootblock: boot/bootasm.S boot/bootmain.c
	$(CC) $(CFLAGS) -O -nostdinc -I. -c boot/bootmain.c -o boot/bootmain.o
	$(CC) $(CFLAGS) -nostdinc -I. -c boot/bootasm.S -o boot/bootasm.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o boot/bootblock.o boot/bootasm.o boot/bootmain.o
	$(OBJDUMP) -S boot/bootblock.o > boot/bootblock.asm
	$(OBJCOPY) -S -O binary boot/bootblock.o boot/bootblock.out
	./boot/sign.pl boot/bootblock.out boot/bootblock


clean: 
	rm leios.img && cd boot && rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*~ *.o *.d *.asm *.sym vectors.S parport.out \
	bootblock bootblock.out kernel fs.img mkfs \
	$(UPROGS)

# run in emulators
qemu: leios.img
	qemu -parallel stdio -hda leios.img

