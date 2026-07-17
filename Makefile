CC = clang
TARGET = i686-elf
CFLAGS = -target $(TARGET) -ffreestanding -O2 -Wall -Wextra -std=gnu99 -mno-sse -mno-sse2 -mno-mmx -mno-3dnow -msoft-float
LDFLAGS = -T linker.ld -nostdlib

all: kernel.bin

boot.o: boot.s
	clang -target $(TARGET) -c boot.s -o boot.o

gdt_flush.o: gdt_flush.s
	clang -target $(TARGET) -c gdt_flush.s -o gdt_flush.o

gdt.o: gdt.c gdt.h
	$(CC) $(CFLAGS) -c gdt.c -o gdt.o

kernel.o: kernel.c gdt.h
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.bin: boot.o gdt_flush.o gdt.o kernel.o linker.ld
	ld.lld $(LDFLAGS) -o kernel.bin boot.o gdt_flush.o gdt.o kernel.o

clean:
	rm -f *.o kernel.bin
