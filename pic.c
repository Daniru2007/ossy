#include "pic.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01
#define PIC_EOI   0x20

/* Raw port I/O — talks to hardware chips directly, not memory */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0); /* unused port, writing to it just burns a tiny delay */
}

void pic_remap(void) {
    /* Start initialization sequence on both PICs */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* Tell each PIC where its interrupt numbers should start */
    outb(PIC1_DATA, 32); /* master PIC: IRQ0 becomes interrupt 32 */
    io_wait();
    outb(PIC2_DATA, 40); /* slave PIC: IRQ8 becomes interrupt 40 */
    io_wait();

    /* Tell master PIC there's a slave PIC at IRQ2 */
    outb(PIC1_DATA, 4);
    io_wait();
    /* Tell slave PIC its own cascade identity */
    outb(PIC2_DATA, 2);
    io_wait();

    /* Set both PICs to 8086 mode */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /* Unmask everything for now — all IRQs enabled */
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}
