/*
 * spi.cpp
 *
 *  Created on: 07 Dec 2017
 *      Author: christo
 */

#include "spi.h"

uint32_t cSPI::SpiFrequency( uint32_t hz )
{
	uint32_t divisor = 0;
	uint32_t SysClkTmp = SystemCoreClock;
	uint32_t baudRate;

	while( SysClkTmp > hz)
	{
		divisor++;
		SysClkTmp= ( SysClkTmp >> 1);

		if (divisor >= 7)
			break;
	}

	baudRate =((( divisor & 0x4 ) == 0 )? 0x0 : SPI_CR1_BR_2  )|
			((( divisor & 0x2 ) == 0 )? 0x0 : SPI_CR1_BR_1  )|
			((( divisor & 0x1 ) == 0 )? 0x0 : SPI_CR1_BR_0  );

	return baudRate;
}

void cSPI::csHigh()
{
	if(hspi.Instance == SPI1)
		HAL_GPIO_WritePin(SPI1_GPIO_NSS_PORT, SPI1_GPIO_NSS_PIN, GPIO_PIN_SET);
}

void cSPI::csLow()
{
	if(hspi.Instance == SPI1)
		HAL_GPIO_WritePin(SPI1_GPIO_NSS_PORT, SPI1_GPIO_NSS_PIN, GPIO_PIN_RESET);
}

HAL_StatusTypeDef cSPI::writeEnable()
{
	csLow();

	uint8_t opCode = 0x06;
	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, ( uint8_t* ) &opCode,  1, HAL_MAX_DELAY);

	csHigh();

	return status;
}

HAL_StatusTypeDef cSPI::writeDisable()
{
	csLow();

	uint8_t opCode = 0x04;
	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, ( uint8_t* ) &opCode,  1, HAL_MAX_DELAY);

	csHigh();

	return status;
}

cSPI::cSPI()
{

}

cSPI::~cSPI()
{

}

void cSPI::init(SPI_TypeDef *spiNum, uint32_t frequency)
{
	hspi.Instance = spiNum;
	hspi.Init.Mode = SPI_MODE_MASTER;
	hspi.Init.Direction = SPI_DIRECTION_2LINES;
	hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi.Init.NSS = SPI_NSS_SOFT;
	hspi.Init.BaudRatePrescaler = SpiFrequency( frequency );;
	hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi.Init.CRCPolynomial = 7;
	if (HAL_SPI_Init(&hspi) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	printf("spi device init\n");
}

HAL_StatusTypeDef cSPI::readId(uint8_t *data, uint8_t len)
{
	if (len > 3)
		return HAL_ERROR;

	csLow();

	uint8_t opCode = 0x9F;
	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, ( uint8_t* ) &opCode,  1, HAL_MAX_DELAY);

	if (status == HAL_OK)
		status = HAL_SPI_Receive(&hspi,( uint8_t* ) data, 3, HAL_MAX_DELAY);

	csHigh();

	return status;
}

HAL_StatusTypeDef cSPI::write(uint32_t address, uint8_t *data, uint8_t len)
{
	if (address > SPI1_MAX_ADDRESS)
		return HAL_ERROR;

	//transmit write enable
	writeEnable();
	HAL_Delay(1);

	csLow();

	uint8_t opCode = 0x02;
	uint8_t addressBytes[3];
	addressBytes[0] = address & 0xFF;
	addressBytes[1] = (address >> 8) & 0xFF;
	addressBytes[2] = (address >> 16) & 0xFF;

	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, &opCode,  1, HAL_MAX_DELAY);

	if (status == HAL_OK)
		status = HAL_SPI_Transmit(&hspi, addressBytes,  3, HAL_MAX_DELAY);

	if (status == HAL_OK)
		status = HAL_SPI_Transmit(&hspi, data,  len, HAL_MAX_DELAY);

	csHigh();

	HAL_Delay(1);
	writeDisable();

	return HAL_OK;
}

HAL_StatusTypeDef cSPI::read(uint32_t address, uint8_t *rxData, uint8_t len)
{
	csLow();

	uint8_t opCode = 0x03;
	uint8_t addressBytes[3];
	addressBytes[0] = address & 0xFF;
	addressBytes[1] = (address >> 8) & 0xFF;
	addressBytes[2] = (address >> 16) & 0xFF;

	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, &opCode,  1, HAL_MAX_DELAY);

	if (status == HAL_OK)
		HAL_SPI_Transmit(&hspi, addressBytes,  3, HAL_MAX_DELAY);

	if (status == HAL_OK)
		HAL_SPI_Receive(&hspi,( uint8_t* ) rxData, len, HAL_MAX_DELAY);

	csHigh();

	return HAL_OK;
}

HAL_StatusTypeDef cSPI::erase(uint16_t address, uint8_t blockSizekB)
{
	writeEnable();
	HAL_Delay(1);

	csLow();

	//set opCode for max 64kb erase
	uint8_t opCode = 0xD8;

	if(blockSizekB < 4)
		opCode = 0x20;
	else if(blockSizekB < 32)
		opCode = 0x52;

	uint8_t addressBytes[3];
	addressBytes[0] = address & 0xFF;
	addressBytes[1] = (address >> 8) & 0xFF;
	addressBytes[2] = (address >> 16) & 0xFF;

	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, &opCode,  1, HAL_MAX_DELAY);

	if (status == HAL_OK)
		status = HAL_SPI_Transmit(&hspi, addressBytes,  3, HAL_MAX_DELAY);

	csHigh();

	HAL_Delay(1);
	writeDisable();

	return HAL_OK;
}

HAL_StatusTypeDef cSPI::chipErase(uint16_t address, uint8_t blockSizekB)
{
	writeEnable();
	HAL_Delay(1);

	csLow();

	uint8_t opCode = 0x60;

	uint8_t addressBytes[3];
	addressBytes[0] = address & 0xFF;
	addressBytes[1] = (address >> 8) & 0xFF;
	addressBytes[2] = (address >> 16) & 0xFF;

	HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi, &opCode,  1, HAL_MAX_DELAY);

	if (status == HAL_OK)
		status = HAL_SPI_Transmit(&hspi, addressBytes,  3, HAL_MAX_DELAY);

	csHigh();

	HAL_Delay(1);
	writeDisable();

	return HAL_OK;
}


void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();

		GPIO_InitStruct.Pin = SPI1_GPIO_NSS_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SPI1_GPIO_NSS_PORT, &GPIO_InitStruct);

		HAL_GPIO_WritePin(SPI1_GPIO_NSS_PORT, SPI1_GPIO_NSS_PIN, GPIO_PIN_SET);

		GPIO_InitStruct.Pin = SPI1_GPIO_MISO_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = SPI1_GPIO_AF;
		HAL_GPIO_Init(SPI1_GPIO_MISO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SPI1_GPIO_MOSI_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = SPI1_GPIO_AF;
		HAL_GPIO_Init(SPI1_GPIO_MOSI_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SPI1_GPIO_SCK_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = SPI1_GPIO_AF;
		HAL_GPIO_Init(SPI1_GPIO_SCK_PORT, &GPIO_InitStruct);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

	if(spiHandle->Instance==SPI1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		HAL_GPIO_DeInit(SPI1_GPIO_NSS_PORT, SPI1_GPIO_NSS_PIN);
		HAL_GPIO_DeInit(SPI1_GPIO_MISO_PORT, SPI1_GPIO_MISO_PIN);
		HAL_GPIO_DeInit(SPI1_GPIO_MOSI_PORT, SPI1_GPIO_MOSI_PIN);
		HAL_GPIO_DeInit(SPI1_GPIO_SCK_PORT, SPI1_GPIO_SCK_PIN);
	}
}
