#include <stdint.h>

/* Must match exactly what isr_common_stub pushes, in reverse order */
struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

static const char *exception_messages[] = {
    "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds", "Invalid Opcode",
    "No Coprocessor", "Double Fault", "Coprocessor Segment Overrun",
    "Bad TSS", "Segment Not Present", "Stack Fault",
    "General Protection Fault", "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check", "Machine Check",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved"
};

static void vga_print(const char *str) {
    static int row = 1; /* keep row 0 for the existing "Hello" message */
    volatile char *vga = (volatile char*) 0xB8000;
    int col = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        int offset = (row * 80 + col) * 2;
        vga[offset]     = str[i];
        vga[offset + 1] = 0x04; /* red on black, so it stands out */
        col++;
    }
    row++;
}

static void hex_to_str(uint32_t val, char *buf) {
    const char *digits = "0123456789ABCDEF";
    buf[0] = '0'; buf[1] = 'x';
    for (int i = 0; i < 8; i++) {
        buf[9 - i] = digits[val & 0xF];
        val >>= 4;
    }
    buf[10] = '\0';
}

void isr_handler(struct registers regs) {
    if (regs.int_no < 32) {
        vga_print("EXCEPTION:");
        vga_print(exception_messages[regs.int_no]);

        if (regs.int_no == 14) { /* Page Fault -- CR2 holds the faulting address */
            uint32_t fault_addr;
            __asm__ __volatile__ ("mov %%cr2, %0" : "=r"(fault_addr));

            char buf[16] = "Address: ";
            char hex[11];
            hex_to_str(fault_addr, hex);
            int i = 0; while (buf[i] != '\0') i++;
            int j = 0; while (hex[j] != '\0') buf[i++] = hex[j++];
            buf[i] = '\0';
            vga_print(buf);
        }
    }

    for (;;) {
        __asm__ __volatile__ ("hlt");
    }
}
