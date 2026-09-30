#include <assert.h>
#include "stm32f7xx_hal.h"
#include "clock.h"

static TIM_HandleTypeDef hal_timebase;

void clock_rcc_init(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;
    RCC_OscInitStruct.PLL.PLLR = 2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        assert(0);
    }

    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        assert(0);
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        assert(0);
    }
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    RCC_ClkInitTypeDef    clkconfig;
    uint32_t              uwTimclock, uwAPB1Prescaler = 0U;

    uint32_t              uwPrescalerValue = 0U;
    uint32_t              pFLatency;
    HAL_StatusTypeDef     status;

    /* Enable TIM6 clock */
    __HAL_RCC_TIM6_CLK_ENABLE();

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;

    /* Compute TIM6 clock */
    if (uwAPB1Prescaler == RCC_HCLK_DIV1)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
    }

    /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

    /* Initialize TIM6 */
    hal_timebase.Instance = TIM6;

    /* Initialize TIMx peripheral as follow:
    + Period = [(TIM6CLK/1000) - 1]. to have a (1/1000) s time base.
    + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
    + ClockDivision = 0
    + Counter direction = Up
    */
    hal_timebase.Init.Period = (1000000U / 1000U) - 1U;
    hal_timebase.Init.Prescaler = uwPrescalerValue;
    hal_timebase.Init.ClockDivision = 0;
    hal_timebase.Init.CounterMode = TIM_COUNTERMODE_UP;
    hal_timebase.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    status = HAL_TIM_Base_Init(&hal_timebase);

    if (status == HAL_OK)
    {
        /* Start the TIM time Base generation in interrupt mode */
        status = HAL_TIM_Base_Start_IT(&hal_timebase);

        if (status == HAL_OK)
        {
            /* Enable the TIM6 global Interrupt */
            HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

            /* Configure the SysTick IRQ priority */
            if (TickPriority < (1UL << __NVIC_PRIO_BITS))
            {
                /* Configure the TIM IRQ priority */
                HAL_NVIC_SetPriority(TIM6_DAC_IRQn, TickPriority, 0U);
                uwTickPrio = TickPriority;
            }
            else
            {
                status = HAL_ERROR;
            }
        }
    }

    /* Return function status */
    return status;
}

void HAL_SuspendTick(void)
{

    /* Disable TIM1 update Interrupt */
    __HAL_TIM_DISABLE_IT(&hal_timebase, TIM_IT_UPDATE);
}

void HAL_ResumeTick(void)
{

    /* Enable TIM1 Update interrupt */
    __HAL_TIM_ENABLE_IT(&hal_timebase, TIM_IT_UPDATE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{

    if (htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
}

void TIM6_DAC_IRQHandler(void)
{

    HAL_TIM_IRQHandler(&hal_timebase);
}

void bsp_high_res_tim_init(void)
{

    /* To support unified clock.h interface
     * TODO: implement this function for F7 */
}

uint32_t bsp_high_res_tim_get(void)
{

    /* To support unified clock.h interface
     * TODO: implement this function for F7 */
    return 0;
}
