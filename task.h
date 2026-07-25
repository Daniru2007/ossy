#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_TASKS 4

typedef struct task {
    uint32_t esp;   /* saved stack pointer, valid only when this task isn't running */
    uint8_t  used;
} task_t;

void tasking_init(void);
int  task_create(void (*entry)(void));
void task_yield(void);

#endif
