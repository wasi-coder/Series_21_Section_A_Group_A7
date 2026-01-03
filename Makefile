# Makefile for kacchiOS
CC = gcc
LD = ld
AS = as

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdinc \
         -fno-builtin -fno-stack-protector -I.
ASFLAGS = --32
LDFLAGS = -m elf_i386

# UPDATED: Added memory.o, process.o, and switch.o
OBJS = boot.o kernel.o serial.o string.o memory.o process.o switch.o

all: kernel.elf

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -T link.ld -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

run: kernel.elf
	qemu-system-i386 -kernel kernel.elf -m 64M -serial stdio -display none

run-vga: kernel.elf
	qemu-system-i386 -kernel kernel.elf -m 64M -serial mon:stdio

clean:
	rm -f *.o kernel.elf

.PHONY: all run run-vga clean