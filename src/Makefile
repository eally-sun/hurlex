#!Makefile
#
# --------------------------------------------------------
#
#    hurlex 这个小内核的 Makefile
#    默认使用的C语言编译器是 GCC、汇编语言编译器是 nasm
#
# --------------------------------------------------------
#

# patsubst 处理所有在 C_SOURCES 字列中的字（一列文件名），如果它的 结尾是 '.c'，就用 '.o' 把 '.c' 取代
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Iinclude
LD_FLAGS = -Tscripts/kernel.ld -melf_i386
ASM_FLAGS = -f elf

all: $(S_OBJECTS) $(C_OBJECTS) link

# The automatic variable `$<' is just the first prerequisite
.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo 编译 $< ...
	nasm $(ASM_FLAGS) $<

link:
	@echo 链接内核文件...
	ld $(LD_FLAGS) $(C_OBJECTS) $(S_OBJECTS) -o kernel

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) kernel

.PHONY:update_image
update_image:
	sudo mount floppy.img /mnt/kernel
	sudo cp kernel /mnt/kernel/kernel
	sleep 1
	sudo umount /mnt/kernel

