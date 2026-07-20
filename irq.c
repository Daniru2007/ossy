#include <stdint.h>
#include "pic.h"

struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

static volatile uint32_t tick_count = 0;

/* No sprintf available -- convert a small uint to a string by hand */
static void uint_to_str(uint32_t val, char *buf) {
    char tmp[12];
    int i = 0;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (val > 0) {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    int j = 0;
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

static void vga_print_at(int row, const char *str) {
    volatile char *vga = (volatile char*) 0xB8000;
    int col = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        int offset = (row * 80 + col) * 2;
        vga[offset]     = str[i];
        vga[offset + 1] = 0x02; /* green on black */
        col++;
    }
}

void irq_handler(struct registers regs) {
    if (regs.int_no == 32) {  /* IRQ0 = the timer, remapped to interrupt 32 */
        tick_count++;

        if (tick_count % 100 == 0) {  /* at 100Hz, this is once per second */
            char buf[16] = "Ticks: ";
            char num[12];
            uint_to_str(tick_count, num);

            int i = 0;
            while (buf[i] != '\0') i++;
            int j = 0;
            while (num[j] != '\0') buf[i++] = num[j++];
            buf[i] = '\0';

            vga_print_at(2, buf);
        }
    }

    pic_send_eoi(regs.int_no - 32);
}
