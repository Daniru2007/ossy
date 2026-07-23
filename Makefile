CC = clang
TARGET = i686-elf
CFLAGS = -target $(TARGET) -ffreestanding -O2 -Wall -Wextra -std=gnu99 -mno-sse -mno-sse2 -mno-mmx -mno-3dnow -msoft-float
LDFLAGS = -T linker.ld -nostdlib

all: kernel.bin

boot.o: boot.s
	clang -target $(TARGET) -c boot.s -o boot.o

gdt_flush.o: gdt_flush.s
	clang -target $(TARGET) -c gdt_flush.s -o gdt_flush.o

idt_flush.o: idt_flush.s
	clang -target $(TARGET) -c idt_flush.s -o idt_flush.o

isr.o: isr.s
	clang -target $(TARGET) -c isr.s -o isr.o

irq.o: irq.s
	clang -target $(TARGET) -c irq.s -o irq.o

gdt.o: gdt.c gdt.h
	$(CC) $(CFLAGS) -c gdt.c -o gdt.o

idt.o: idt.c idt.h
	$(CC) $(CFLAGS) -c idt.c -o idt.o

isr_c.o: isr.c
	$(CC) $(CFLAGS) -c isr.c -o isr_c.o

irq_c.o: irq.c pic.h
	$(CC) $(CFLAGS) -c irq.c -o irq_c.o

pic.o: pic.c pic.h
	$(CC) $(CFLAGS) -c pic.c -o pic.o

pit.o: pit.c pit.h io.h
	$(CC) $(CFLAGS) -c pit.c -o pit.o

pmm.o: pmm.c pmm.h multiboot.h
	$(CC) $(CFLAGS) -c pmm.c -o pmm.o

kernel.o: kernel.c gdt.h idt.h pic.h pit.h pmm.h multiboot.h
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.bin: boot.o gdt_flush.o idt_flush.o isr.o irq.o gdt.o idt.o isr_c.o irq_c.o pic.o pit.o pmm.o kernel.o linker.ld
	ld.lld $(LDFLAGS) -o kernel.bin boot.o gdt_flush.o idt_flush.o isr.o irq.o gdt.o idt.o isr_c.o irq_c.o pic.o pit.o pmm.o kernel.o

clean:
	rm -f *.o kernel.bin
