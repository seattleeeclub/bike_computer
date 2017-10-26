///////////////////////////////////////////////////////
/*
Dana Olcott 2017

Touch Panel Controller Files for the touch screen
component to the lcd on the stm32f429 discovery
board.

Touch screen uses the the STM811 controller ic
and is wired up to the I2C3 peripheral.


For now, make somethinng simple to read the product code

Next, 
enable the interrupt pin (EXTI line 15)
read the x and y position from the touch panel


*/
//////////////////////////////////////////////////////////

#ifndef __TOUCH_PANEL_H
#define __TOUCH_PANEL_H


#include "i2c.h"


//address
#define TOUCH_PANEL_ADDRESS		0x82

#define TOUCH_PANEL_CHIP_ID		((uint16_t)0x0811)
#define TOUCH_PANEL_ID_VER		((uint8_t)0x03)


//register defines
#define TOUCH_PANEL_CHIP_ID_REG		0x00		//2 bytes 0x0811
#define TOUCH_PANEL_ID_VER_REG		0x02		//1 byte 0x03



int TouchPanel_init(void);
uint8_t TouchPanel_readReg1Byte(uint16_t reg);
uint16_t TouchPanel_readReg2Bytes(uint16_t reg);
void TouchPanel_writeReg1Byte(uint16_t reg, uint8_t value);
void TouchPanel_writeReg2Bytes(uint16_t reg, uint16_t value);

uint16_t TouchPanel_readChipID(void);
uint8_t TouchPanel_readIDVersion(void);




#endif
