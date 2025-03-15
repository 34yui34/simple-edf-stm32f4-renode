#include "task.h"
#include "timer2_tick.h"
#include "main.h"
#include <stdbool.h>

/* Task management */
TCB_t tasks[MAX_TASKS];
uint8_t num_tasks = 0;
uint8_t current_task_id = 0;
bool first_context_switch = true;
static volatile uint32_t system_ticks = 0;

static void idle_task_func(void);
static const char* get_task_state_str(uint8_t task_id);

/* Initialize task control block */
int create_task(void (*task_func)(void), uint32_t period, uint32_t execution_time, uint32_t deadline_period, const char *name) {
    if (num_tasks >= MAX_TASKS) {
        return 0xFF; /* No space for new task */
    }

    uint8_t task_id = num_tasks++;
    TCB_t *task = &tasks[task_id];

    /* Initialize stack with default values */
    memset(task->stack, 0, sizeof(task->stack));

    /* Set initial stack pointer to point to the end of the stack */
    task->stack_ptr = &task->stack[STACK_SIZE - 16];

    /* Set up initial stack frame */
    task->stack[STACK_SIZE - 1] = 0x01000000;      /* PSR (T-bit set for Thumb mode) */
    task->stack[STACK_SIZE - 2] = (uint32_t)task_func;  /* PC */
    task->stack[STACK_SIZE - 3] = 0xFFFFFFFF;      /* LR (dummy return address) */
    task->stack[STACK_SIZE - 4] = 0;               /* R12 */
    task->stack[STACK_SIZE - 5] = 0;               /* R3 */
    task->stack[STACK_SIZE - 6] = 0;               /* R2 */
    task->stack[STACK_SIZE - 7] = 0;               /* R1 */
    task->stack[STACK_SIZE - 8] = 0;               /* R0 */
    task->stack[STACK_SIZE - 9] = 0;               /* R11 */
    task->stack[STACK_SIZE - 10] = 0;              /* R10 */
    task->stack[STACK_SIZE - 11] = 0;              /* R9 */
    task->stack[STACK_SIZE - 12] = 0;              /* R8 */
    task->stack[STACK_SIZE - 13] = 0;              /* R7 */
    task->stack[STACK_SIZE - 14] = 0;              /* R6 */
    task->stack[STACK_SIZE - 15] = 0;              /* R5 */
    task->stack[STACK_SIZE - 16] = 0;              /* R4 */

    /* Initialize task parameters */
    task->state = TASK_READY;
    task->period = period;
    task->deadline_period = deadline_period;
    task->execution_time = execution_time;
    if (deadline_period == 0xFFFFFFFF)
        task->deadline = deadline_period;
    else
        task->deadline = system_ticks + deadline_period;  /* Initial deadline */
    task->wait_time = 0;    /* Start executing immediately */
    task->task_func = task_func;

    /* Copy task name */
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0'; /* Ensure null termination */

    printf("\r\n*** Create task: %s ***\r\n", task->name);
    printf("\t- Current ticks %u,\r\n", system_ticks);
    printf("\t- state %u,\r\n", task->state);
    printf("\t- period %u,\r\n", task->period);
    printf("\t- xc time %u,\r\n", task->execution_time);
    printf("\t- deadline at %u\r\n", task->deadline);

    return task_id;
}

/* return how many ticks has passed */
uint32_t get_tick(void) {
    return system_ticks;
}

/* After task finished executing for one period, should call yield */
/* This will put the current task into blocked state until next execution period comes */
void task_yield(void) {
    /* Voluntarily yield by blocking this task */
    __disable_irq();
    tasks[current_task_id].deadline = system_ticks + tasks[current_task_id].period + tasks[current_task_id].deadline_period;
    tasks[current_task_id].wait_time = tasks[current_task_id].period;
    tasks[current_task_id].state = TASK_BLOCKED;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __enable_irq();
}

/* Find task with earliest deadline */
static int find_earliest_deadline_task(void) {
    uint8_t earliest_task = 0xFF;
    uint32_t earliest_deadline = 0xFFFFFFFF;

    for (uint8_t i = 0; i < num_tasks; i++) {
        if (tasks[i].state != TASK_BLOCKED && tasks[i].deadline <= earliest_deadline) {
            earliest_deadline = tasks[i].deadline;
            earliest_task = i;
        }
    }

    return earliest_task;
}

/* Function to switch context between tasks */
static void context_switch(void) {
    /* Save current task's context if a task is running */
    if (current_task_id != 0xFF) {
        __asm volatile (
            "MRS R0, PSP\n"           /* Get current PSP value */
            "STMDB R0!, {R4-R11}\n"   /* Save R4-R11 to stack */
            "MSR PSP, R0\n"           /* Update PSP */
        );

        /* Store current stack pointer */
        tasks[current_task_id].stack_ptr = (uint32_t *)__get_PSP();
    }

    uint8_t prev_task_id = current_task_id;
    if (tasks[prev_task_id].state != TASK_BLOCKED) {
        tasks[prev_task_id].state = TASK_READY;
    }

    /* Choose next task with EDF algorithm */
    schedule_next_task();

    /* If no ready task found, use idle task */
    if (current_task_id == 0xFF) {
        /* Handle idle state */
        return;
    }

    /* Update current task state to running */
    tasks[current_task_id].state = TASK_RUNNING;

    if (!first_context_switch) {
        /* Don't output this during first context switch */
        if (current_task_id != prev_task_id) {
            printf("\r\n========= task %s swapped out for task %s at ticks %u =========\r\n",
                    tasks[prev_task_id].name,
                    tasks[current_task_id].name,
                    system_ticks
            );
        }

        DEBUG_LOG("\r\n");
        for (uint8_t i = 0; i < num_tasks; i++) {
            DEBUG_LOG("*** %s state is %s ***\r\n", tasks[i].name, get_task_state_str(i));
        }
        DEBUG_LOG("### Schedule task: %s ###\r\n", tasks[current_task_id].name);
        DEBUG_LOG("\t- ticks until deadline %u\r\n", tasks[current_task_id].deadline - system_ticks);
        DEBUG_LOG("\r\n");
    }
    else {
        /* Reset first context switch flag */
        first_context_switch = false;
    }

    /* Set PSP to the new task's stack pointer */
    __set_PSP((uint32_t)tasks[current_task_id].stack_ptr);

    /* Restore context of the new task */
    __asm volatile (
        "MRS R0, PSP\n"           /* Get current PSP value */
        "LDMIA R0!, {R4-R11}\n"   /* Restore R4-R11 from stack */
        "MSR PSP, R0\n"           /* Update PSP */
    );
}

/* Schedule the next task using EDF */
void schedule_next_task(void) {
    /* Find task with earliest deadline */
    uint8_t next_task = find_earliest_deadline_task();

    /* Update current task ID */
    current_task_id = next_task;
}

/* Increment tick counter */
static void tick_callback_handler(void) {
    system_ticks++;

    /* Check for tasks that need to be activated (when period is reached) */
    for (uint8_t i = 0; i < num_tasks; i++) {
        if (tasks[i].deadline != 0xFFFFFFFF && system_ticks >= tasks[i].deadline) {
            /* If task cannot achieve deadline, assert */
            printf("\r\n!!!!! Task %s cannot meet deadline of %d ticks !!!!!\r\n",
                    tasks[i].name,
                    tasks[i].deadline);
            assert_param(false);
        }
        if (tasks[i].wait_time > 0) {
            tasks[i].wait_time = tasks[i].wait_time - 1;
        }
        if (tasks[i].wait_time == 0) {
            tasks[i].state = TASK_READY;
        }
    }

    /* Trigger context switch */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/* Idle task - runs when no other tasks are ready */
static void idle_task_func(void) {
    /* Low power mode could be entered here */
    while(1) {
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < 100000; i++);
        __disable_irq();
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
        __enable_irq();
    }
}

static const char* get_task_state_str(uint8_t task_id) {
    if (tasks[task_id].state == TASK_BLOCKED) {
        return "BLOCKED";
    }
    else if (tasks[task_id].state == TASK_READY) {
        return "READY";
    }
    else {
        return "RUNNING";
    }

    assert_param(false);
    return NULL;
}

/* Start the scheduler */
void start_scheduler(void) {
    /* Set up idle task */
    create_task(idle_task_func, 0xFFFFFFFF, 0, 0xFFFFFFFF, "IdleTask");

    printf("\r\n########################## EDF Scheduler Started ##########################\r\n");

    /* Set up tick interrupt callback */
    timer2_set_tick_callback(tick_callback_handler);

    /* Configure system to use Process Stack for exceptions handlers */
    __set_CONTROL(__get_CONTROL() | 0x02);

    /* Start with no current task */
    current_task_id = 0xFF;

    /* Force context switch to start first task */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

    /* Enable interrupts */
    __enable_irq();

    /* Should never reach here */
    while(1);
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