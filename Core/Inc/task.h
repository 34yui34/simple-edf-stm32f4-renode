#include "stm32f4xx.h"

#ifndef TASK_H_
#define TASK_H_

/* Task states */
#define TASK_READY 0
#define TASK_RUNNING 1
#define TASK_BLOCKED 2

#define STACK_SIZE  1024

/* Maximum number of tasks */
#define MAX_TASKS 10

/* Task control block */
typedef struct {
    uint32_t *stack_ptr;         /* Stack pointer */
    uint32_t stack[STACK_SIZE];  /* Task stack */
    uint32_t state;              /* Task state */
    uint32_t period;             /* Task period in system ticks */
    uint32_t deadline;           /* Absolute deadline */
    uint32_t execution_time;     /* Worst-case execution time */
    void (*task_func)(void);     /* Task function pointer */
    char name[256];              /* Task name for debugging */
} TCB_t;

int create_task(void (*task_func)(void), uint32_t period, uint32_t execution_time, const char *name);
uint32_t get_tick(void);
void create_example_tasks(void);

#endif