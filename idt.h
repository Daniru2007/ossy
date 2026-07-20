#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* One IDT entry — describes where one interrupt's handler lives */
struct idt_entry {
    uint16_t base_low;     /* handler address, bits 0-15 */
    uint16_t selector;     /* which GDT code segment to use when jumping there */
    uint8_t  always0;      /* unused, must be zero */
    uint8_t  flags;        /* type + privilege level + present bit */
    uint16_t base_high;    /* handler address, bits 16-31 */
} __attribute__((packed));

/* Same idea as gdt_ptr — size + location of the whole table, for lidt */
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_install(void);

#endif
