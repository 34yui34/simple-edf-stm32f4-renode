/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "main.h"
#include "uart1_logger.h"
#include "timer2_tick.h"
#include <stdio.h>
#include <stdbool.h>

// #define RUN_NORMAL_SCHELUDABLE_EDF
// #define RUN_CONCURRENT_SCHELUDABLE_EDF
#define RUN_UNSCHELUDABLE_TASKSET_EDF

#if defined(RUN_NORMAL_SCHELUDABLE_EDF) + defined(RUN_CONCURRENT_SCHELUDABLE_EDF) + defined(RUN_UNSCHELUDABLE_TASKSET_EDF) != 1
#error Define exactly one of RUN_NORMAL_SCHELUDABLE_EDF, RUN_CONCURRENT_SCHELUDABLE_EDF, RUN_UNSCHELUDABLE_TASKSET_EDF
#endif

#define LOOP_PER_5_TICKS 90000

void SystemClock_Config(void);

#ifdef RUN_NORMAL_SCHELUDABLE_EDF
/* Example task 1 */
static void task1(void) {
    while(1) {
        /* Task 1 code */
        printf("\r\n+++++++++++++++++++++++ Task1 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*10; i++);
        printf("\r\n++++++++++++++++++++++ Task1 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}

/* Example task 2 */
static void task2(void) {
    while(1) {
        /* Task 2 code */
        printf("\r\n+++++++++++++++++++++++ Task2 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*20; i++);
        printf("\r\n++++++++++++++++++++++ Task2 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}

/* Example task 3 - higher frequency task */
static void task3(void) {
    while(1) {
        /* Task 3 code */
        printf("\r\n+++++++++++++++++++++++ Task3 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*4; i++);
        printf("\r\n++++++++++++++++++++++ Task3 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}
#endif  /* RUN_NORMAL_SCHELUDABLE_EDF */

#ifdef RUN_CONCURRENT_SCHELUDABLE_EDF
/* Example task 1 */
static void task1(void) {
    while(1) {
        /* Task 1 code */
        printf("\r\n+++++++++++++++++++++++ Task1 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*100; i++);
        printf("\r\n++++++++++++++++++++++ Task1 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}

/* Example task 2 */
static void task2(void) {
    while(1) {
        /* Task 2 code */
        printf("\r\n+++++++++++++++++++++++ Task2 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*100; i++);
        printf("\r\n++++++++++++++++++++++ Task2 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}

/* Example task 3 - higher frequency task */
static void task3(void) {
    while(1) {
        /* Task 3 code */
        printf("\r\n+++++++++++++++++++++++ Task3 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*100; i++);
        printf("\r\n++++++++++++++++++++++ Task3 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}
#endif  /* RUN_CONCURRENT_SCHELUDABLE_EDF */

#ifdef RUN_UNSCHELUDABLE_TASKSET_EDF
/* Example task 1 */
static void task1(void) {
    while(1) {
        /* Task 1 code */
        printf("\r\n+++++++++++++++++++++++ Task1 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*60; i++);
        printf("\r\n++++++++++++++++++++++ Task1 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}

/* Example task 2 */
static void task2(void) {
    while(1) {
        /* Task 2 code */
        printf("\r\n+++++++++++++++++++++++ Task2 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*70; i++);
        printf("\r\n++++++++++++++++++++++ Task2 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}

/* Example task 3 - higher frequency task */
static void task3(void) {
    while(1) {
        /* Task 3 code */
        printf("\r\n+++++++++++++++++++++++ Task3 started at tick %u +++++++++++++++++++++++\r\n", get_tick());
        /* Simulate work with delay */
        for (volatile uint32_t i = 0; i < LOOP_PER_5_TICKS*100; i++);
        printf("\r\n++++++++++++++++++++++ Task3 finished at tick %u ++++++++++++++++++++++\r\n", get_tick());
        /* Yield as task is finished for current period */
        task_yield();
    }
}
#endif  /* RUN_UNSCHELUDABLE_TASKSET_EDF */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Disable interrupt first */
    __disable_irq();

    NVIC_SetPriority(PendSV_IRQn, 0xFF);

    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    uart1_logger_init();
    timer2_tick_init();

#ifdef RUN_NORMAL_SCHELUDABLE_EDF
    printf("Start: run RUN_NORMAL_SCHELUDABLE_EDF program\r\n");
    /* Create tasks with their periods (in system ticks) and execution times */
    create_task(task1, 2000, 50, 150, "Task1");    /* 2000 ticks period, ~50 ticks execution time, 150 ticks relative deadline */
    create_task(task2, 5000, 100, 250, "Task2");   /* 5000 ticks period, ~100 ticks execution time, 250 ticks relative deadline */
    create_task(task3, 100, 20, 50, "Task3");      /* 100 ticks period, ~20 ticks execution time, 50 ticks relative deadline */
#endif /* RUN_NORMAL_SCHELUDABLE_EDF */

#ifdef RUN_CONCURRENT_SCHELUDABLE_EDF
    printf("Start: run RUN_CONCURRENT_SCHELUDABLE_EDF program\r\n");
    /* Create tasks with their periods (in system ticks) and execution times */
    create_task(task1, 1600, 500, 1600, "Task1");    /* 1600 ticks period, ~500 ticks execution time, 530 ticks relative deadline */
    create_task(task2, 1600, 500, 1600, "Task2");    /* 1600 ticks period, ~500 ticks execution time, 530 ticks relative deadline */
    create_task(task3, 1600, 500, 1600, "Task3");    /* 1600 ticks period, ~500 ticks execution time, 530 ticks relative deadline */
#endif /* RUN_CONCURRENT_SCHELUDABLE_EDF */

#ifdef RUN_UNSCHELUDABLE_TASKSET_EDF
    printf("Start: run RUN_UNSCHELUDABLE_TASKSET_EDF program\r\n");
    /* Create tasks with their periods (in system ticks) and execution times */
    create_task(task1, 500, 300, 500, "Task1");    /* 500 ticks period, ~300 ticks execution time, 500 ticks relative deadline */
    create_task(task2, 700, 350, 700, "Task2");    /* 700 ticks period, ~350 ticks execution time, 700 ticks relative deadline */
    create_task(task3, 2000, 500, 2000, "Task3");  /* 2000 ticks period, ~500 ticks execution time, 2000 ticks relative deadline */
#endif /* RUN_UNSCHELUDABLE_TASKSET_EDF */

    /* Start the scheduler */
    start_scheduler();
    /* Should not get here! */
    assert_param(false);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        assert_param(false);
    }
    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        assert_param(false);
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    __disable_irq();
    printf("assert failed: at %s, line %u\r\n", file, line);
    while(1);
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
