#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_PRESENT  0x1
#define PAGE_WRITABLE 0x2

void paging_init(void);
void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);

#endif
