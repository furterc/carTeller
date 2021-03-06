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
#include <stdlib.h>
#include <string.h>

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_conf.h"

#include "main.h"
#include "hw.h"
#include "terminal.h"
#include "commands.h"

#include "nvm.h"
#include "rtc.h"
#include "output.h"

#include "timer_ic.h"
#include "ultra_s_sensor.h"

#include "log.h"
#include "spi.h"
#include "spi_device.h"
#include "cir_flash_map.h"
#include "car_check.h"
#include "car_wash.h"

// hardware
cHw HW = cHw();

// SPI
cSPI spi = cSPI();

// SPI Flash
cOutput spiNss = cOutput(GPIOA, GPIO_PIN_4);
cSpiDevice spiFlash= cSpiDevice(&spi, &spiNss);
cCirFlashMap cirFlash = cCirFlashMap(0x010000, 8);

// Log
cLog log = cLog(&spiFlash, &cirFlash, 1);

cOutput *triggers[SENSOR_COUNT];
cUltraSSensor *sensors[SENSOR_COUNT];
cCarCheck *carcheckers[SENSOR_COUNT];

nvm_config_t nvm;
//currentSensor
cUltraSSensor *distanceSensor = 0;

void resetSource()
{
	if (READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF))
		printf("Reset        : Low-power\n");
	if (READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF))
		printf("Reset        : Window watchdog\n");
	if (READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF))
		printf("Reset        : Independent Window watchdog\n");
	if (READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF))
		printf("Reset        : Software reset\n");
	if (READ_BIT(RCC->CSR, RCC_CSR_PORRSTF))
		printf("Reset        : Power on\n");
//   if(READ_BIT(RCC->CSR, RCC_CSR_PINRSTF))
//      printf("Reset        : Pin\n");
	if (READ_BIT(RCC->CSR, RCC_CSR_FWRSTF))
		printf("Reset        : Firewall\n");
	if (READ_BIT(RCC->CSR, RCC_CSR_OBLRSTF))
		printf("Reset        : Options bytes load\n");

	SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}

int main(void)
{
	/* MCU Configuration----------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock and refresh register */
	HW.SystemClock_Config();
	SystemCoreClockUpdate();

	/* Initialize the terminal */
	terminal_init();

	/* Initialize all configured peripherals */
	HW.Gpio_Init();

	/* Clear the screen */
	printf("%c%s", 0x1B, "[2J");

	/* Print some device data */
	printf(GREEN("Welcome to the Car Wash Counter\n"));
	printf("Flash size   : %d kB\n", HW.GetFlashSize());
	printf("System Clock : %dHz\n\r", (int) HAL_RCC_GetSysClockFreq());
	resetSource();

    /* initialize the RTC */
    printf("RTC          : ");
    if(cRTC::getInstance()->init() == HAL_OK)
    {
        RTC_TimeTypeDef time;
        cRTC::getInstance()->getTime(&time);
	    RTC_DateTypeDef date;
	    cRTC::getInstance()->getDate(&date);

	    printf(GREEN("Set\n"));
	    printf("RTC Time     : %d:%02d:%02d\n", time.Hours, time.Minutes, time.Seconds);
	    printf("RTC Date     : %d/%d/20%02d\n", date.Date, date.Month, date.Year);

    }
    else if(cRTC::getInstance()->init() == HAL_TIMEOUT)
        printf(RED("NOT SET\n"));
    else
        printf(RED("Failed\n"));

	/* initialize the SPI */
    printf("SPI          : ");
	if(spi.init(SPI1, (uint32_t) 8000000) == HAL_OK)
	    printf(GREEN("initialized\n"));
	else
	    printf(RED("failed\n"));

	/* initialize the log */
	printf("LOG          : ");
	if (log.init() == HAL_OK)
	{
		printf(GREEN("initialized\n"));
		log.printHeadTail();
	}
	else
		printf(RED("failed\n"));

	nvm_getConfig(&nvm);
	show_nvm();

	/* initialize the timerIc */
	TimerIc.init();
	cTimerIc *timer = &TimerIc;

	/* initialize sensor 0 */
	triggers[0]     = new cOutput(GPIOB, GPIO_PIN_0);
	sensors[0]      = new cUltraSSensor(timer, triggers[0], TIM_CHANNEL_2);
	carcheckers[0]  = new cCarCheck(nvm.triggerDistance, nvm.triggerTime, 0);

	/* initialize sensor 1 */
//	triggers[1]     = new cOutput(GPIOA, GPIO_PIN_10);
//	sensors[1]      = new cUltraSSensor(timer, triggers[1], TIM_CHANNEL_3);
//	carcheckers[1]  = new cCarCheck(nvm->triggerDistance, nvm->triggerDistance, 1);

	printf(GREEN("$: "));
	uint8_t idx = 0;
	/* Infinite loop */
	while (1)
	{
		terminal_run();

		// cycle through the sensors
		bool timerRunning = distanceSensor->run();

		if (!timerRunning)
		{
			//select next sample count
//			if (idx++ == 1)
//				idx = 0;

			distanceSensor = sensors[idx];
			distanceSensor->sample();
		}

		uint32_t sample = distanceSensor->getLastSample();
		if (sample)
		{
			if (carcheckers[idx]->run(sample))
			{
				cCarWash *gewasdeKar = carcheckers[idx]->getCarWash();
				sCarwashObject_t carWashObj;
				gewasdeKar->getObject(&carWashObj);

				RTC_TimeTypeDef time;
				cRTC::getInstance()->getTime(&time);
				printf(MAGENTA("Bay %d end  : %02d:%02d:%02d\n"), carWashObj.bayNumber,time.Hours, time.Minutes, time.Seconds);
				printf(MAGENTA("Bay %d dur  : %02d:%02d\n"), carWashObj.bayNumber, carWashObj.duration_minute ,carWashObj.duration_second);

				if(carWashObj.duration_minute < nvm.minimumTime)
				    printf(YELLOW("%ds too short\n"), carWashObj.duration_second);
				else
				    log.addWashEntry(&carWashObj);

				delete (gewasdeKar);
			}
		}
	}
}

void distance_debug(uint8_t argc, char **argv)
{
	if (argc == 2)
	{
		uint8_t disable = atoi(argv[1]);
		if (!disable)
		{
			for (uint8_t idx = 0; idx < SENSOR_COUNT; idx++)
				sensors[idx]->setDebug(0);
			printf(CYAN("disabled debug\n"));
			return;
		}
	}

	if (argc != 3)
	{
		printf("n - debug lvl enabled \n\r0 - debug disabled\n");
		return;
	}

	uint8_t channel = atoi(argv[1]);
	uint8_t lvl = atoi(argv[2]);

	if (channel > SENSOR_COUNT)
	{
		printf(RED("unknown channel\n"));
		return;
	}

	if (lvl > 3)
		lvl = 3;

	if (lvl < 0)
		lvl = 0;

	sensors[channel]->setDebug(lvl);
	printf("set chan %d to dbg level %d\n", channel, lvl);
}

sTermEntry_t ddebugEntry =
{ "dd", "distanceDebug", distance_debug };

void spiTry(uint8_t argc, char **argv)
{
	uint8_t data[3];
	spiFlash.readId(data, 3);

	printf("SPI Device ID: 0x%02X%02X%02X\n", data[0], data[1], data[2]);
	log.printHeadTail();
}

sTermEntry_t spiEntry =
{ "s", "spiID", spiTry };

void RspiTry(uint8_t argc, char **argv)
{
	if(argc == 1)
	{
		printf("try to take a dump\n");
		log.dumpLog();
		return;
	}

	uint32_t sector = atoi(argv[1]);
	uint32_t startAddr = 0;
	for(uint8_t idx = 0; idx < sector; idx++)
	    startAddr += 0x010000;

	uint32_t cnt = atoi(argv[2]);

	printf("read %d entries from sector %d @ 0x%08X\n", (unsigned int)cnt, (unsigned int)sector, (unsigned int)startAddr);

	uint8_t data[16];
	for (uint32_t idx = 0; idx<cnt; idx++)
	{
		spiFlash.read(startAddr, data, 16);

		printf("data @ 0x%08X: ", (unsigned int)startAddr);
		for (int i = 0; i < 16; i++)
			printf(" 0x%02X", data[i]);
		printf("\n");
		startAddr += 0x10;
	}
}

sTermEntry_t readspiEntry =
{ "sr", "spishitread", RspiTry };

void WspiTry(uint8_t argc, char **argv)
{
    uint32_t sector = atoi(argv[1]);
    uint32_t startAddr = 0;
    for(uint8_t idx = 0; idx < sector; idx++)
        startAddr += 0x010000;

    uint32_t cnt = atoi(argv[2]);

    printf("write %d dummy entries at sector %d @ 0x%08X\n", (unsigned int)cnt, (unsigned int)sector, (unsigned int)startAddr);
    sCarwashObject_t dummy;
    memset(&dummy, 0xFF, sizeof(sCarwashObject_t));
    dummy.bayNumber = 0xAA;
    dummy.date_year = 0x11;

    log.addWashEntryAt(startAddr, &dummy);
    for (uint32_t idx = 0; idx<cnt; idx++)
    {
        log.addWashEntry(&dummy);
        startAddr += 0x10;
    }
}
sTermEntry_t writespiEntry = { "sw", "spishitwrite", WspiTry };

void EspiTry(uint8_t argc, char **argv)
{
//	printf("washAddr: 0x%08X\n", log.getWashDataAddress());

	log.eraseDevice();
//	spi1.erase(0x0, 4);
}

sTermEntry_t erasespiEntry =
{ "se", "spi erase", EspiTry };

void ack(uint8_t argc, char **argv)
{
    if(argc > 2)
        return;

    int count = atoi(argv[1]);
    if (count < 0)
    {
        printf(RED("count < 0"));
        return;
    }

    printf("ack %d entries\n", count);
    log.ackEntries(count);


}

sTermEntry_t ackEntry =
{ "ack", "acknowledge @ sector with count", ack };


void rtcSetGet(uint8_t argc, char **argv)
{
	RTC_TimeTypeDef time;
	cRTC::getInstance()->getTime(&time);

	if (argc == 1)
	{
		printf("Time: %d:%02d:%02d\n", time.Hours, time.Minutes, time.Seconds);
	}
	else if (argc == 3)
	{
		time.Hours = atoi(argv[1]);
		time.Minutes = atoi(argv[2]);
		time.Seconds = 0;

		cRTC::getInstance()->setTime(time);
		printf("Set Time: %d:%d:%d\n", time.Hours, time.Minutes, time.Seconds);
	}
}
sTermEntry_t rtcEntry =
{ "t", "Set/Get the rtc", rtcSetGet };

void dateSetGet(uint8_t argc, char **argv)
{

	RTC_DateTypeDef date;
	cRTC::getInstance()->getDate(&date);

	if (argc == 1)
	{
		printf("Date: %d/%d/20%02d\n", date.Date, date.Month, date.Year);
	}
	else if (argc == 4)
	{
		date.Date = atoi(argv[1]);
		date.Month = atoi(argv[2]);
		date.Year = atoi(argv[3]);

		cRTC::getInstance()->setDate(date);
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
