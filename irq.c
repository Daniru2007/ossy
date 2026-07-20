#include <stdint.h>
#include "pic.h"

struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

void irq_handler(struct registers regs) {
    /* For now, only the timer (IRQ0 -> interrupt 32) is wired up.
       Every IRQ handler MUST send an EOI or the PIC will refuse
       to send any further interrupts of that type ever again. */

    pic_send_eoi(regs.int_no - 32);
}
