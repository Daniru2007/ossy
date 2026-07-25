.global switch_task
.type switch_task, @function

/* void switch_task(uint32_t *old_esp_store, uint32_t new_esp); */
switch_task:
    mov 4(%esp), %eax    /* first argument: where to SAVE the current esp */
    mov 8(%esp), %edx    /* second argument: the esp to SWITCH TO */

    push %ebp
    push %ebx
    push %esi
    push %edi

    mov %esp, (%eax)     /* save fully-set-up current stack pointer */
    mov %edx, %esp       /* jump onto the other task's stack */

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp

    ret                  /* returns into whatever that stack says to return to */
