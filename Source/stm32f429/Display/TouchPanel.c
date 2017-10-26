///////////////////////////////////////////////////////
/*
Dana Olcott 2017

Touch Panel Controller Files for the touch screen
component to the lcd on the stm32f429 discovery
board.

Touch screen uses the the STM811 controller ic
and is wired up to the I2C3 peripheral.


hi2c3

 I2C3 GPIO Configuration
    PC9     ------> I2C3_SDA
    PA8     ------> I2C3_SCL

For now, make somethinng simple to read the product code

Next, 
enable the interrupt pin (EXTI line 15)
read the x and y position from the touch panel

API function calls:

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

*/
//////////////////////////////////////////////////////////

#include "TouchPanel.h"
#include "i2c.h"
#include "gpio.h"



////////////////////////////////////////
//reads the chip id and id version.
//returns... 0 if ok, -1 if not
//
int TouchPanel_init(void)
{
	int ret = 0x00;

	uint16_t chipID = TouchPanel_readChipID();
	uint8_t IDVersion = TouchPanel_readIDVersion();

	if (chipID == TOUCH_PANEL_CHIP_ID)
	{
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);
		ret = -1;
	}
	if (IDVersion == TOUCH_PANEL_ID_VER)
	{
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);

	}
	else
	{
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);
		ret = -1;
	}

	return ret;

}

uint8_t TouchPanel_readReg1Byte(uint16_t reg)
{
	uint8_t value = 0x00;
	HAL_I2C_Mem_Read(&hi2c3, TOUCH_PANEL_ADDRESS, reg, 1, &value, 1, 0xFF);

	return value;
}

///////////////////////////////////////////////
//hal i2c read
//reads into msb followed by lsb
//
uint16_t TouchPanel_readReg2Bytes(uint16_t reg)
{
	uint8_t value[2] = {0x00, 0x00};
	HAL_I2C_Mem_Read(&hi2c3, TOUCH_PANEL_ADDRESS, reg, 1, value, 2, 0xFF);

	//reads into msb then lsb
	uint16_t result = ((uint16_t)value[0]) << 8;
	result |= value[1];

	return result;
}

void TouchPanel_writeReg1Byte(uint16_t reg, uint8_t value)
{
	uint8_t data = value;
	HAL_I2C_Mem_Write(&hi2c3, TOUCH_PANEL_ADDRESS, reg, 1, &data, 1, 0xFF);
}

////////////////////////////////////////////////
//write 2 bytes - write most followed by least
void TouchPanel_writeReg2Bytes(uint16_t reg, uint16_t value)
{
	uint8_t data[2] = {0x00, 0x00};
	data[0] = (value >> 8) & 0xFF;
	data[1] = value & 0xFF;

	HAL_I2C_Mem_Write(&hi2c3, TOUCH_PANEL_ADDRESS, reg, 1, data, 2, 0xFF);
}




///////////////////////////////////////
//read 2 byte value - chip id register
uint16_t TouchPanel_readChipID()
{
	uint16_t ret = TouchPanel_readReg2Bytes(TOUCH_PANEL_CHIP_ID_REG);
	return ret;
}

uint8_t TouchPanel_readIDVersion()
{
	uint8_t ret = TouchPanel_readReg1Byte(TOUCH_PANEL_ID_VER_REG);
	return ret;
}

