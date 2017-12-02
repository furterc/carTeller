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
#include "cmsis_os.h"
#include "FreeRTOSConfig.h"
#include "terminal.h"
#include "rtc.h"
#include "commands.h"
#include "stdlib.h"
#include "distance.h"
#include "nvm.h"
#include "carCheck.h"

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


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

    distance_Init();
    rtc_init();

    printf(GREEN("terminal ready\n"));

    printf(BLUE("SysClkFreq:\t%d\n"), (int)HAL_RCC_GetSysClockFreq());

    car_check_Init();

    MX_FREERTOS_Init();

    osKernelStart();

    /* Infinite loop */
    while (1)
    {
//        terminal_run();
//        distance_run();
//
//        int sample = 0;
//        if (distance_getLastSample(&sample))
//        	car_check_Run(sample);
    }
}

void pulse(uint8_t argc, char **argv)
{
	distance_pulse();
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
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
	  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.HSICalibrationValue = 16;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	//    _Error_Handler(__FILE__, __LINE__);
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
	//    _Error_Handler(__FILE__, __LINE__);
	  }

	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
	//    _Error_Handler(__FILE__, __LINE__);
	  }

	    /**Configure the Systick interrupt time
	    */
	  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	    /**Configure the Systick
	    */
	  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	  /* SysTick_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(SysTick_IRQn, 3, 0);
}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

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
void _Error_Handler(const char * file, int line)
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
