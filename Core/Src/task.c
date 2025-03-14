#include "task.h"
#include "timer2_tick.h"

/* Task management */
TCB_t tasks[MAX_TASKS];
uint8_t num_tasks = 0;
uint8_t current_task_id = 0;
static volatile uint32_t system_ticks = 0;

static void task1(void);
static void task2(void);
static void task3(void);

void create_example_tasks(void) {
    /* Create tasks with their periods (in system ticks) and execution times */
    create_task(task1, 200, 50, "Task1");    /* 200ms period, ~50ms execution time */
    create_task(task2, 500, 100, "Task2");   /* 500ms period, ~100ms execution time */
    create_task(task3, 100, 20, "Task3");    /* 100ms period, ~20ms execution time */

    /* Start the scheduler */
    start_scheduler();
}

/* Initialize task control block */
int create_task(void (*task_func)(void), uint32_t period, uint32_t execution_time, const char *name) {
    if (num_tasks >= MAX_TASKS) {
        return 0xFF; /* No space for new task */
    }

    uint8_t task_id = num_tasks++;
    TCB_t *task = &tasks[task_id];

    /* Initialize stack with default values */
    memset(task->stack, 0, sizeof(task->stack));

    /* Set initial stack pointer to point to the end of the stack */
    task->stack_ptr = &task->stack[STACK_SIZE - 16];

    /* Set the program counter to the task function */
    task->stack[STACK_SIZE - 2] = (uint32_t)task_func; /* PC */

    /* Set the initial PSR (Program Status Register) with T-bit set (Thumb mode) */
    task->stack[STACK_SIZE - 1] = 0x01000000; /* PSR */

    /* Initialize task parameters */
    task->state = TASK_READY;
    task->period = period;
    task->execution_time = execution_time;
    task->deadline = system_ticks + period;  /* Initial deadline */
    task->task_func = task_func;

    /* Copy task name */
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0'; /* Ensure null termination */

    return task_id;
}

/* return how many ticks has passed */
uint32_t get_tick(void) {
    return system_ticks;
}

/* Find task with earliest deadline */
static int find_earliest_deadline_task(void) {
    uint8_t earliest_task = 0xFF;
    uint32_t earliest_deadline = 0xFFFFFFFF;

    for (uint8_t i = 0; i < num_tasks; i++) {
        if (tasks[i].state == TASK_READY && tasks[i].deadline < earliest_deadline) {
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

    /* Choose next task with EDF algorithm */
    schedule_next_task();

    /* If no ready task found, use idle task */
    if (current_task_id == 0xFF) {
        /* Handle idle state */
        return;
    }

    /* Set PSP to the new task's stack pointer */
    __set_PSP((uint32_t)tasks[current_task_id].stack_ptr);

    /* Update task state */
    tasks[current_task_id].state = TASK_RUNNING;

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

    /* If current task is still running, mark it as ready */
    if (current_task_id != 0xFF) {
        tasks[current_task_id].state = TASK_READY;
    }

    /* Update current task ID */
    current_task_id = next_task;
}

/* Increment tick counter */
static void tick_callback_handler(void) {
    system_ticks++;

    /* Check for tasks that need to be activated (when period is reached) */
    for (uint8_t i = 0; i < num_tasks; i++) {
        if (system_ticks >= tasks[i].deadline) {
            /* Set new deadline */
            tasks[i].deadline = system_ticks + tasks[i].period;

            /* Make task ready if it's not already */
            if (tasks[i].state == TASK_BLOCKED) {
                tasks[i].state = TASK_READY;
            }
        }
    }
}

/* Idle task - runs when no other tasks are ready */
static void idle_task(void) {
    while(1) {
        /* Low power mode could be entered here */
        printf("Idle task entered\r\n");

        /* Small delay */
        for (volatile uint32_t i = 0; i < 100000; i++);
    }
}

/* Example task 1 */
static void task1(void) {
    while(1) {
        /* Task 1 code */
        printf("Task1 started\r\n");

        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < 500000; i++);

        printf("Task1 finished\r\n");
        /* Voluntarily yield by blocking this task */
        __disable_irq();
        tasks[current_task_id].state = TASK_BLOCKED;
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
        __enable_irq();
    }
}

/* Example task 2 */
static void task2(void) {
    while(1) {
        /* Task 2 code */
        printf("Task2 started\r\n");

        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < 1000000; i++);

        printf("Task2 finished\r\n");
        /* Voluntarily yield by blocking this task */
        __disable_irq();
        tasks[current_task_id].state = TASK_BLOCKED;
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
        __enable_irq();
    }
}

/* Example task 3 - higher frequency task */
static void task3(void) {
    while(1) {
        /* Task 3 code */
        printf("Task3 started\r\n");

        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < 200000; i++);

        printf("Task3 finished\r\n");
        /* Voluntarily yield by blocking this task */
        __disable_irq();
        tasks[current_task_id].state = TASK_BLOCKED;
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
        __enable_irq();
    }
}

/* Start the scheduler */
void start_scheduler(void) {
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