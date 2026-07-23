#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include "multiboot.h"

#define PAGE_SIZE 4096

void pmm_init(struct multiboot_info *mbi, uint32_t kernel_end_addr);
uint32_t pmm_alloc_frame(void);
void pmm_free_frame(uint32_t frame_addr);
uint32_t pmm_free_frame_count(void);

#endif
