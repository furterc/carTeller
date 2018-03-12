/*
 * spi_device.cpp
 *
 *  Created on: 17 Jan 2018
 *      Author: christo
 */

#include "spi_device.h"

void cSpiDevice::csHigh()
{
	mNssPin->set();
}

void cSpiDevice::csLow()
{
	mNssPin->reset();
}

HAL_StatusTypeDef cSpiDevice::writeEnable()
{
	csLow();
	HAL_StatusTypeDef status = mSPI->writeOpCode(SPI_OPCODE_WRITE_ENABLE);
	csHigh();

	return status;
}

HAL_StatusTypeDef cSpiDevice::writeDisable()
{
	csLow();
	HAL_StatusTypeDef status = mSPI->writeOpCode(SPI_OPCODE_WRITE_DISABLE);
	csHigh();

	return status;
}

cSpiDevice::cSpiDevice(cSPI *spi, cOutput *nssPin)
{
	mSPI = spi;
	mNssPin = nssPin;
}

cSpiDevice::~cSpiDevice()
{
	// TODO Auto-generated destructor stub
}

HAL_StatusTypeDef cSpiDevice::write(uint32_t address, uint8_t *txData, uint8_t len)
{
	writeEnable();
	csLow();

//	printf("spi wrote %d bytes @ 0x%08X\r", len, (unsigned int)address);
	HAL_StatusTypeDef status = mSPI->writeOpCodeAt(address, SPI_OPCODE_WRITE);

	if (status == HAL_OK)
		mSPI->write(txData, len);

	csHigh();
	writeDisable();

	return status;
}

HAL_StatusTypeDef cSpiDevice::read(uint32_t address, uint8_t *rxData, uint8_t len)
{
	csLow();

	HAL_StatusTypeDef status = mSPI->writeOpCodeAt(address, SPI_OPCODE_READ);

	if (status == HAL_OK)
		status = mSPI->read(rxData, len);

	csHigh();

	return status;
}

HAL_StatusTypeDef cSpiDevice::erase(uint16_t address, uint8_t blockSizekB)
{
	writeEnable();
	csLow();

	uint8_t opCode = SPI_OPCODE_ERASE_64KB;
	if (blockSizekB <= 4)
		opCode = SPI_OPCODE_ERASE_4KB;
	else if (blockSizekB <= 32)
		opCode = SPI_OPCODE_ERASE_32KB;

	HAL_StatusTypeDef status = mSPI->writeOpCodeAt(address, opCode);

	csHigh();
	writeDisable();

	return status;
}

HAL_StatusTypeDef cSpiDevice::chipErase()
{
	writeEnable();
	csLow();

	HAL_StatusTypeDef status = mSPI->writeOpCode(SPI_OPCODE_CHIP_ERASE);

	csHigh();
	writeDisable();

	return status;
}

HAL_StatusTypeDef cSpiDevice::readId(uint8_t *data, uint8_t len)
{
	if (len > 3)
		return HAL_ERROR;

	csLow();

	HAL_StatusTypeDef status = mSPI->readOpCode(SPI_OPCODE_READ_ID, data, len);

	csHigh();

	return status;
}
