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
#include "stm32l0xx_hal_conf.h"
#include "terminal.h"
#include "rtc.h"
#include "commands.h"
#include <stdlib.h>
#include <string.h>
#include "nvm.h"
#include "car_check.h"
#include "spi.h"
#include "car_wash.h"

#include "output.h"
#include "timer_ic.h"
#include "ultra_s_sensor.h"
#include "hw.h"

cHw HW = cHw();
cSPI spi1 = cSPI();


cUltraSSensor *distanceSensor = 0;

uint8_t triggerDistance;
uint8_t triggerTime;

int main(void)
{
    /* MCU Configuration----------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    HW.SystemClock_Config();

    /* Initialize the terminal */
    terminal_init();

    /* Initialize all configured peripherals */
    HW.Gpio_Init();

    rtc_init();

    printf("Flash size   : %d kB\n", HW.GetFlashSize());
    printf("System Clock : %dHz\n\r", (int)HAL_RCC_GetSysClockFreq());

    spi1.init(SPI1, (uint32_t)10000000);

	memcpy(&triggerDistance, nvm_get_CarDistance(),1);
	memcpy(&triggerTime, nvm_get_CarTime(),1);

    TimerIc.init();
    cTimerIc *timer = &TimerIc;

    cOutput *triggers[SENSOR_COUNT];
    cUltraSSensor *sensors[2];
    cCarCheck *carcheckers[2];

    uint8_t i = 0;

    triggers[i] = new cOutput(GPIOB, GPIO_PIN_0);
    sensors[i] = new cUltraSSensor(timer, triggers[i], TIM_CHANNEL_2);
    carcheckers[i] = new cCarCheck(triggerDistance, triggerTime, i);

    i++;

    triggers[i] = new cOutput(GPIOA, GPIO_PIN_10);
    sensors[i] = new cUltraSSensor(timer, triggers[i], TIM_CHANNEL_3);
    carcheckers[1] = new cCarCheck(triggerDistance, triggerTime, i);

    /* Infinite loop */
    while (1)
    {
    	terminal_run();

    	static uint8_t idx = 0;
    	bool timerRunning = distanceSensor->run();

    	if (!timerRunning)
    	{
    		//select next sample count
    		if(idx++ == 1)
    			idx = 0;

    		distanceSensor = sensors[idx];
    		distanceSensor->sample();
    	}

    	uint32_t s = distanceSensor->getLastSample();
    	if(s)
    	{
    		carcheckers[idx]->run(s);
    	}
    }
}

void carDistance(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
    	printf("trigDistance: %dcm\n", triggerDistance);
    }
    else if (argc == 2)
    {
    	uint8_t data = atoi(argv[1]);
    	if (nvm_set_CarDistance(&data) != 1)
    		printf("'n fokken gemors\n");

    	triggerDistance = data;
    	printf("trigDistance: %dcm\n", triggerDistance);
    }
}
sTermEntry_t carDistEntry =
{ "cd", "Set/Get the car trigger distance", carDistance };

void carTime(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
    	printf("trigTime: %dcm\n", triggerDistance);
    }
    else if (argc == 2)
    {
    	uint8_t data = atoi(argv[1]);
    	if (nvm_set_CarTime(&data) != 1)
    		printf("'n fokken gemors\n");

    	triggerTime = data;
    	printf("trigTime: %dcm\n", triggerTime);
    }
}
sTermEntry_t carTimeEntry =
{ "ct", "Set/Get the car trigger time", carTime };

void distance_debug(uint8_t argc, char **argv)
{
	if (argc != 2)
	{
		printf("n - debug lvl enabled \n\r0 - debug disabled\n");
		return;
	}

	uint8_t lvl = atoi(argv[1]);

	if (lvl > 3)
		lvl = 3;

	if (lvl < 0)
		lvl = 0;

	distanceSensor->setDebug(lvl);
}

sTermEntry_t ddebugEntry =
{ "dd", "distanceDebug", distance_debug };

void spiTry(uint8_t argc, char **argv)
{
	uint8_t data[3];
	spi1.readId(data, 3);

	printf("spi id: 0x%02X 0x%02X 0x%02X\n", data[0], data[1], data[2]);
}

sTermEntry_t spiEntry =
{ "s", "spishit", spiTry};

void RspiTry(uint8_t argc, char **argv)
{
	uint8_t cnt = atoi(argv[1]);

	printf("read %d bytes from 0x1000\b", cnt);

	uint8_t data[50];

	spi1.read(0x0, data, cnt);

	for(int i=0; i < cnt; i++)
		printf("data %X\n", data[i]);
}

sTermEntry_t readspiEntry =
{ "sr", "spishitread", RspiTry};

void WspiTry(uint8_t argc, char **argv)

{
	uint8_t data[4] = { 0xFF, 0x01, 0x00, 0x04};
	spi1.write(0x0, data, 4);
}

sTermEntry_t writespiEntry =
{ "sw", "spishitwrite", WspiTry};

void EspiTry(uint8_t argc, char **argv)
{
	spi1.erase(0x0, 4);
}

sTermEntry_t erasespiEntry =
{ "se", "spi erase", EspiTry};


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



void _Error_Handler(const char * file, int line)
{
    printf(RED("Probleem: %s\t:%d\n"), file, line);
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
