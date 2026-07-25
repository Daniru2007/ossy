#include "task.h"
#include "pmm.h"

extern void switch_task(uint32_t *old_esp_store, uint32_t new_esp);

static task_t tasks[MAX_TASKS];
static int current_task = 0;
static int task_count = 1; /* task 0 = kernel_main itself, always exists */

void tasking_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].used = 0;
    }
    tasks[0].used = 1; /* task 0 represents whatever's calling tasking_init */
    current_task = 0;
    task_count = 1;
}

int task_create(void (*entry)(void)) {
    if (task_count >= MAX_TASKS) return -1;

    int id = task_count++;

    /* A fresh 4KB frame to serve as this task's own stack.
       NOTE: must land within the identity-mapped first 4MB, since
       map_page() can't yet create mappings for brand-new page tables --
       true for early allocations right after kernel_end, which is fine here. */
    uint32_t stack_frame = pmm_alloc_frame();
    uint32_t stack_top   = stack_frame + PAGE_SIZE;

    /* Build the stack to look exactly like one that already went through
       switch_task's four `push` instructions, so the final `ret` in
       switch_task lands on `entry` the first time this task ever runs. */
    uint32_t *stack = (uint32_t *)stack_top;
    *(--stack) = (uint32_t)entry; /* return address for switch_task's `ret` */
    *(--stack) = 0; /* ebp */
    *(--stack) = 0; /* ebx */
    *(--stack) = 0; /* esi */
    *(--stack) = 0; /* edi */

    tasks[id].esp  = (uint32_t)stack;
    tasks[id].used = 1;

    return id;
}

void task_yield(void) {
    int next = current_task;
    for (int i = 0; i < MAX_TASKS; i++) {
        next = (next + 1) % MAX_TASKS;
        if (tasks[next].used) break;
    }

    if (next == current_task) return; /* nothing else to switch to */

    int prev = current_task;
    current_task = next;
    switch_task(&tasks[prev].esp, tasks[next].esp);
}
