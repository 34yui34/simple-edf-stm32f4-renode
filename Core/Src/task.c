#include "task.h"

TCB_t TCB_1, TCB_2;
TCB_t *current_task;

/* Initialize the task control block */
void init_task(TCB_t *task, void (*function)(void)) {
    /* Initialize stack with default values */
    memset(task->stack, 0, sizeof(task->stack));

    /* Set initial stack pointer to point to the end of the stack */
    task->stack_ptr = &task->stack[STACK_SIZE - 16];

    /* Set the program counter to the task function */
    task->stack[STACK_SIZE - 2] = (uint32_t)function; /* PC */

    /* Set the initial PSR (Program Status Register) with T-bit set (Thumb mode) */
    task->stack[STACK_SIZE - 1] = 0x01000000; /* PSR */

    /* Initial state is ready */
    task->state = TASK_READY;
}