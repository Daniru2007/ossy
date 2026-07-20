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

void isr_handler(struct registers regs) {
    if (regs.int_no < 32) {
        vga_print("EXCEPTION:");
        vga_print(exception_messages[regs.int_no]);
    }

    for (;;) {
        __asm__ __volatile__ ("hlt");
    }
}
