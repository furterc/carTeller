/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "terminal.h"
//#include "distance.h"
#include "carCheck.h"
#include "rtc.h"
/* USER CODE BEGIN Includes */

//#include "output.h"
//#include "distance_sensor.h"
//#include "ic_timer.h"
///* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId samplerTaskHandle;
osThreadId terminalTaskHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartSamplerTask(void const * argument);
void StartTerminalTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void HAL_Delay(__IO uint32_t Delay)
{
	osDelay(Delay);
}
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */
#include "output.h"
#include "distance_sensor.h"
#include "ic_timer.h"

cDistanceSensor *distPrt = 0;
//cDistanceSensor *distPrt = 0;

void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */

	cOutput trig = cOutput(GPIOB, GPIO_PIN_0);
	printf(GREEN("ps: %p mTrigger\n"), &trig);

	cDistanceSensor dist = cDistanceSensor(&trig, 400, 2);
	distPrt = &dist;
	IcTimer.init();
	IcTimer.initSensor(2, &dist);

	printf("p: dist\t\t: 0x%p\n", &dist);
	printf("p: disttrggrr1\t: 0x%p\n", dist.getTrigger());
	printf("p: disttrggrr2\t: 0x%p\n", distPrt->getTrigger());
	printf("hi\n");

//	dist.pulse();

//	distPrt->pulse();

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	osThreadDef(samplerTask, StartSamplerTask, osPriorityNormal, 0, 256);
	samplerTaskHandle = osThreadCreate(osThread(samplerTask), NULL);
	osThreadDef(terminalTask, StartTerminalTask, osPriorityNormal, 0, 384);
	terminalTaskHandle = osThreadCreate(osThread(terminalTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
	/* Infinite loop */
	for (;;)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		osDelay(100);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		osDelay(100);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		osDelay(100);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		osDelay(800);
	}
}

void StartSamplerTask(void const * argument)
{
	/* Infinite loop */
	for (;;)
	{
		printf("p: distptr\t: 0x%p\n", distPrt);
		printf("p: disttrigger\t: 0x%p\n", distPrt->getTrigger());
		osDelay(1000);
//		printf("osTick: %d\n", (int)osKernelSysTick());

//		if(!distPrt)
//			return;
//
//		distPrt->run();

//		uint16_t tmp = distPrt->getLastSample();


//		if (tmp)
//			printf("dist: %d\n", tmp);
//		distance_run();

		int sample = 0;
//		if (distance_getLastSample(&sample))
//			car_check_Run(sample);
	}
}

/* StartDefaultTask function */
void StartTerminalTask(void const * argument)
{
	/* Infinite loop */
	for (;;)
	{
		terminal_run();
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
