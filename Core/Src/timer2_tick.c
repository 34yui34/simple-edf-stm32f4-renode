#include "main.h"
#include "timer2_tick.h"

static TIM_HandleTypeDef htim2;

void timer2_tick_init(void) {
    /* Initialized tick interrupt at 1ms interval */
    uint32_t uwPrescalerValue = (uint32_t)((SystemCoreClock /2) / 1000000) - 1;

    htim2.Instance = TIM2;
    htim2.Init.Period = 1000 - 1;
    htim2.Init.Prescaler = uwPrescalerValue;
    htim2.Init.ClockDivision = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        /* Initialization Error */
        Error_Handler();
    }

    /* Start the TIM Base generation in interrupt mode */
    /* Start Channel1 */
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
        /* Starting Error */
        Error_Handler();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    /* Enable peripherals and GPIO Clocks */
    /* TIMx Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* Configure the NVIC for TIMx */
    /* Set Interrupt Group Priority */
    /* HAL_NVIC_SetPriority(TIMx_IRQn, 4, 0); */
    HAL_NVIC_SetPriority(TIM2_IRQn, 4, 0);

    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  //  printf("Timer2 Interrupt!\n");
}