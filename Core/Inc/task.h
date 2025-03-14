#include "stm32f4xx.h"

#ifndef TASK_H_
#define TASK_H_

/* Task states */
#define TASK_READY 0
#define TASK_RUNNING 1
#define TASK_BLOCKED 2

#define STACK_SIZE  1024

/* Task control blocks */
typedef struct {
    uint32_t *stack_ptr;    /* Stack pointer */
    uint32_t stack[STACK_SIZE]; /* Task stack */
    uint32_t state;         /* Task state */
} TCB_t;

extern TCB_t TCB_1, TCB_2;
extern TCB_t *current_task;

void init_task(TCB_t *task, void (*function)(void));

#endif