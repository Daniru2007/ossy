/* kernel.c */
#include "gdt.h"
#include "idt.h"
#include "pic.h"

void kernel_main(void) {
    gdt_install();
    idt_install();
    pic_remap();
    __asm__ __volatile__ ("sti");   /* actually enable interrupts */
    const char *str = "Hello from my kernel!";
    volatile char *vga = (volatile char*) 0xB8000; // VGA text buffer, direct memory-mapped I/O

    for (int i = 0; str[i] != '\0'; i++) {
        vga[i*2]     = str[i];
        vga[i*2 + 1] = 0x07; // light grey on black
    }
}
