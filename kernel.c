#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "pmm.h"
#include "multiboot.h"
#include "paging.h"
#include "task.h"

extern uint32_t kernel_end;  /* provided by linker.ld */

static void uint_to_str(uint32_t val, char *buf) {
    char tmp[12];
    int i = 0;
    if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    while (val > 0) { tmp[i++] = '0' + (val % 10); val /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

static void vga_print_at(int row, const char *str, uint8_t color) {
    volatile char *vga = (volatile char*) 0xB8000;
    int col = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        int offset = (row * 80 + col) * 2;
        vga[offset]     = str[i];
        vga[offset + 1] = color;
        col++;
    }
}

static void task_a(void) {
    volatile char *vga = (volatile char*) 0xB8000;
    uint32_t counter = 0;
    while (1) {
        int row = 4;
        char c = 'A' + (counter % 26);
        vga[(row * 80) * 2]     = c;
        vga[(row * 80) * 2 + 1] = 0x0E;
        counter++;
        for (volatile int i = 0; i < 3000000; i++); /* crude delay so it's visible */
        task_yield();
    }
}

static void task_b(void) {
    volatile char *vga = (volatile char*) 0xB8000;
    uint32_t counter = 0;
    while (1) {
        int row = 5;
        char c = 'a' + (counter % 26);
        vga[(row * 80) * 2]     = c;
        vga[(row * 80) * 2 + 1] = 0x0D;
        counter++;
        for (volatile int i = 0; i < 3000000; i++);
        task_yield();
    }
}

void kernel_main(uint32_t magic, uint32_t mbi_addr) {
    gdt_install();
    idt_install();
    pic_remap();
    pit_init(100);
    paging_init();
    vga_print_at(3, "Paging enabled.", 0x0B);
    __asm__ __volatile__ ("sti");

    vga_print_at(0, "Hello from my kernel!", 0x07);

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        vga_print_at(1, "Multiboot magic mismatch!", 0x04);
    } else {
        struct multiboot_info *mbi = (struct multiboot_info *)mbi_addr;
        pmm_init(mbi, (uint32_t)&kernel_end);

        char buf[32] = "Free frames: ";
        char num[12];
        uint_to_str(pmm_free_frame_count(), num);
        int i = 0;
        while (buf[i] != '\0') i++;
        int j = 0;
        while (num[j] != '\0') buf[i++] = num[j++];
        buf[i] = '\0';
        vga_print_at(1, buf, 0x0A);
    }

    tasking_init();
    task_create(task_a);
    task_create(task_b);

    while (1) {
        task_yield();
    }
}
