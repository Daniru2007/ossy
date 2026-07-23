#include "pmm.h"

#define MAX_FRAMES   1048576              /* supports tracking up to 4GB of RAM */
#define BITMAP_SIZE  (MAX_FRAMES / 8)      /* 1 bit per frame -> 128KB bitmap */

static uint8_t bitmap[BITMAP_SIZE];
static uint32_t highest_frame = 0;         /* how much of the bitmap is actually meaningful */

static void set_bit(uint32_t frame) {
    bitmap[frame / 8] |= (1 << (frame % 8));
}

static void clear_bit(uint32_t frame) {
    bitmap[frame / 8] &= ~(1 << (frame % 8));
}

static int test_bit(uint32_t frame) {
    return bitmap[frame / 8] & (1 << (frame % 8));
}

static void mark_region_free(uint32_t addr, uint32_t len) {
    uint32_t start_frame = addr / PAGE_SIZE;
    uint32_t end_frame   = (addr + len) / PAGE_SIZE;

    for (uint32_t f = start_frame; f < end_frame && f < MAX_FRAMES; f++) {
        clear_bit(f);
        if (f > highest_frame) highest_frame = f;
    }
}

static void mark_region_used(uint32_t addr, uint32_t len) {
    uint32_t start_frame = addr / PAGE_SIZE;
    uint32_t end_frame   = (addr + len + PAGE_SIZE - 1) / PAGE_SIZE; /* round up */

    for (uint32_t f = start_frame; f < end_frame && f < MAX_FRAMES; f++) {
        set_bit(f);
    }
}

void pmm_init(struct multiboot_info *mbi, uint32_t kernel_end_addr) {
    /* Step 1: assume everything is used/reserved until proven otherwise */
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0xFF;
    }

    /* Step 2: walk GRUB's memory map, freeing every region marked "available".
       Bit 6 of mbi->flags tells us whether GRUB actually filled in
       mmap_addr/mmap_length -- check before trusting them. */
    if (!(mbi->flags & (1 << 6))) {
        return; /* no memory map available -- everything stays marked used */
    }

    struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)mbi->mmap_addr;
    uint32_t end = mbi->mmap_addr + mbi->mmap_length;

    while ((uint32_t)entry < end) {
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            /* addr/len are 64-bit in the spec; truncate to 32-bit since
               this kernel doesn't address beyond 4GB anyway */
            mark_region_free((uint32_t)entry->addr, (uint32_t)entry->len);
        }
        entry = (struct multiboot_mmap_entry *)((uint32_t)entry + entry->size + 4);
    }

    /* Step 3: reclaim the kernel's own memory, and the first 1MB (BIOS/legacy
       area), regardless of what the memory map says -- we must never hand
       out memory we're already occupying, or GRUB's leftover data structures */
    mark_region_used(0, kernel_end_addr);
}

uint32_t pmm_alloc_frame(void) {
    for (uint32_t f = 0; f <= highest_frame; f++) {
        if (!test_bit(f)) {
            set_bit(f);
            return f * PAGE_SIZE;
        }
    }
    return 0; /* out of memory -- 0 is never a valid frame since it's reserved */
}

void pmm_free_frame(uint32_t frame_addr) {
    clear_bit(frame_addr / PAGE_SIZE);
}

uint32_t pmm_free_frame_count(void) {
    uint32_t count = 0;
    for (uint32_t f = 0; f <= highest_frame; f++) {
        if (!test_bit(f)) count++;
    }
    return count;
}
