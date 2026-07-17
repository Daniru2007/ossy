#ifndef GDT_H
#define GDT_H

#include <stdint.h>

/* One GDT entry, packed exactly as the CPU expects — no padding allowed */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

/* What the CPU's lgdt instruction actually wants: size + pointer, not the table itself */
struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void gdt_install(void);

#endif
