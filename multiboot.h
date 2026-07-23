#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* Only the fields up through mmap_addr are modeled -- we never read
   anything past that, so the struct doesn't need to describe it. */
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed));

/* One entry in the memory map GRUB builds for you */
struct multiboot_mmap_entry {
    uint32_t size;      /* size of this entry, NOT including this field itself */
    uint64_t addr;
    uint64_t len;
    uint32_t type;      /* 1 = usable RAM, anything else = reserved/unusable */
} __attribute__((packed));

#define MULTIBOOT_MEMORY_AVAILABLE 1

#endif
