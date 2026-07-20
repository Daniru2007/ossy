#include "pit.h"
#include "io.h"

#define PIT_CHANNEL0   0x40
#define PIT_COMMAND    0x43
#define PIT_BASE_FREQ  1193182  /* fixed hardware clock speed, not your choice */

void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_BASE_FREQ / frequency;

    outb(PIT_COMMAND, 0x36);  /* channel 0, lobyte/hibyte, mode 3 (square wave) */

    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));         /* low byte */
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));  /* high byte */
}
