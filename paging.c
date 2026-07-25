#include "paging.h"
#include "pmm.h"

/* Must be 4KB-aligned -- the hardware requires this, not a style choice */
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
    uint32_t dir_index   = virt_addr >> 22;          /* top 10 bits: which page table */
    uint32_t table_index = (virt_addr >> 12) & 0x3FF; /* next 10 bits: which entry in it */

    /* NOTE: this simplified version assumes the relevant page table already
       exists (true only for the identity-mapped region set up below).
       A general-purpose version would allocate a new page table via
       pmm_alloc_frame() here if page_directory[dir_index] isn't present yet. */
    uint32_t *table = (uint32_t *)(page_directory[dir_index] & 0xFFFFF000);
    table[table_index] = (phys_addr & 0xFFFFF000) | flags;
}

void paging_init(void) {
    /* Step 1: mark every directory entry "not present" -- nothing mapped yet */
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002; /* not present, writable, supervisor */
    }

    /* Step 2: identity-map the first 4MB -- virtual address == physical address.
       This keeps your kernel, GDT, IDT, stack, and VGA buffer all working
       at the exact addresses they already use, once paging turns on. */
    for (int i = 0; i < 1024; i++) {
        uint32_t phys = i * 0x1000; /* each entry covers one 4KB page */
        first_page_table[i] = phys | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* Step 3: point directory entry 0 at that table (covers 0 - 4MB) */
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;

    /* Step 4: tell the CPU where the page directory lives (CR3),
       then flip the paging-enable bit in CR0 -- this is the actual
       instant paging turns on for real */
    __asm__ __volatile__ (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        : "r"(page_directory)
        : "eax"
    );
}
