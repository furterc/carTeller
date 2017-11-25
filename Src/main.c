/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2017 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "terminal.h"
#include "rtc.h"
#include "commands.h"
#include "stdlib.h"

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* Private function prototypes -----------------------------------------------*/

int main(void)
{
    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize the terminal */
    terminal_init();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM2_Init();

    rtc_init();

    printf(GREEN("me ready\n"));

    printf("%s\n", __FILE__);
    _Error_Handler(__FILE__, __LINE__);

    printf("HCLKFreq:\t%d\n", (int)HAL_RCC_GetSysClockFreq());
    printf("HCLKFreq:\t%lu\n", HAL_RCC_GetHCLKFreq());
    /* Infinite loop */
    printf("TIM2_CR1: 0x%04X\n", (int)READ_REG(TIM2->CR1));
    printf("TIM2_DIER: 0x%04X\n", (int)READ_REG(TIM2->DIER));
    printf("TIM2_ARR: 0x%04X\n", (int)READ_REG(TIM2->ARR));
    printf("TIM2_SMCR: 0x%04X\n", (int)READ_REG(TIM2->SMCR));
    printf("TIM2_CCMR: 0x%04X\n", (int)READ_REG(TIM2->CCMR1));
    printf("TIM2_EGR: 0x%04X\n", (int)READ_REG(TIM2->EGR));
    printf("TIM2_CCER: 0x%04X\n", (int)READ_REG(TIM2->CCER));
    while (1)
    {
        terminal_run();

    }
}

uint16_t atime[2];

void tim_pulse();

void pulse(uint8_t argc, char **argv)
{
	tim_pulse();
}

sTermEntry_t pulseEntry =
{ "p", "pulsit", pulse};

void rtcSetGet(uint8_t argc, char **argv)
{
    RTC_TimeTypeDef time;
    rtc_getTime(&time);

    if (argc == 1)
    {
        printf("Time: %d:%d:%d\n", time.Hours, time.Minutes, time.Seconds);
    }
    else if (argc == 3)
    {
        time.Hours = atoi(argv[1]);
        time.Minutes = atoi(argv[2]);
        time.Seconds = 0;

        rtc_setTime(time);
        printf("Set Time: %d:%d:%d\n", time.Hours, time.Minutes, time.Seconds);
    }
}
sTermEntry_t rtcEntry =
{ "t", "Set/Get the rtc", rtcSetGet };

void dateSetGet(uint8_t argc, char **argv)
{

	RTC_DateTypeDef date;
	rtc_getDate(&date);

	if (argc == 1)
	{
		printf("Date: %d/%d/20%02d\n", date.Date, date.Month, date.Year);
	}
	else if (argc == 4)
	{
		date.Date = atoi(argv[1]);
		date.Month = atoi(argv[2]);
		date.Year = atoi(argv[3]);

		rtc_setDate(date);
		printf("Set Date: %d/%d/20%02d\n", date.Date, date.Month, date.Year);
	}
}
sTermEntry_t dateEntry =
{ "d", "Set/Get the date", dateSetGet };


void tim_pulse()
{
	HAL_TIM_Base_Start(&htim2);

	int distance = atime[1] - atime[0];
	distance /= 116;
	printf("distance: %d\n", distance);
	//    printf("atime: %d %d %d\n", atime[0] , atime[1], atime[1] - atime[0]);


	// capture on rising edge
	TIM2->CCER &= ~(1 << 5);

	TIM2->CNT = 0;
	printf("cnt: %d\n", (int)TIM2->CNT);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
	HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_RESET);
	printf("cnt: %d\n", (int)TIM2->CNT);
	//printf("i pulsed it\n");
}
/** System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_IC_InitTypeDef sConfigIC;

    htim2.Instance = TIM2;
    htim2.Init.Prescaler =0x10;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFF;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    HAL_TIM_Base_Start(&htim2);

    printf("TIM2_CR1: 0x%02X\n", (int)READ_REG(TIM2->CR1));
//    __HAL_TIM_ENABLE(&htim2);
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, 0x1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE()
    ;
    __HAL_RCC_GPIOH_CLK_ENABLE()
    ;
    __HAL_RCC_GPIOA_CLK_ENABLE()
    ;

    __HAL_RCC_GPIOB_CLK_ENABLE()
        ;

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : LD2_Pin */
    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : P1_Pin */
    GPIO_InitStruct.Pin = P1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(P1_GPIO_Port, &GPIO_InitStruct);

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    printf(RED("Probleem: %s\t:%d\n"), file, line);
    //    while (1)
//    {
//    }
    /* USER CODE END Error_Handler_Debug */
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
