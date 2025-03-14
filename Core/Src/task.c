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

/* Function to switch context between tasks */
static void context_switch(void) {
    /* Save current task's context */
    __asm volatile (
        "MRS R0, PSP\n"           /* Get current PSP value */
        "STMDB R0!, {R4-R11}\n"   /* Save R4-R11 to stack */
        "MSR PSP, R0\n"           /* Update PSP */
    );

    /* Store current stack pointer */
    current_task->stack_ptr = (uint32_t *)__get_PSP();

    /* Choose next task (simple toggle between the two tasks) */
    if (current_task == &TCB_1) {
        current_task = &TCB_2;
    } else {
        current_task = &TCB_1;
    }

    /* Set PSP to the new task's stack pointer */
    __set_PSP((uint32_t)current_task->stack_ptr);

    /* Restore context of the new task */
    __asm volatile (
        "MRS R0, PSP\n"           /* Get current PSP value */
        "LDMIA R0!, {R4-R11}\n"   /* Restore R4-R11 from stack */
        "MSR PSP, R0\n"           /* Update PSP */
    );

    /* Return using EXC_RETURN value to ensure proper exception handling */
    __asm volatile (
        "BX LR\n"
    );
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
    /* Disable interrupts */
    __disable_irq();

    /* Perform context switch */
    context_switch();

    /* Enable interrupts */
    __enable_irq();
}