#include "gdt.h"

#define GDT_ENTRIES 5

struct gdt_entry gdt[GDT_ENTRIES];
struct gdt_ptr    gp;

/* Defined in gdt_flush.s — actual asm needed to load the table into the CPU */
extern void gdt_flush(uint32_t);

/* Fills in one entry's bit-packed fields from human-readable inputs */
static void gdt_set_gate(int num, uint32_t base, uint32_t limit,
                         uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access       = access;
}

void gdt_install(void) {
    gp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gp.base  = (uint32_t)&gdt;

    /* Entry 0: Null descriptor — CPU requires this to always be zeroed */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* Entry 1: Kernel code segment — flat (base 0, limit covers all 4GB), ring 0 */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* Entry 2: Kernel data segment — flat, ring 0 */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Entry 3: User code segment — flat, ring 3 (for later, when you add user mode) */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    /* Entry 4: User data segment — flat, ring 3 */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint32_t)&gp);
}
