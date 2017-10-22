/*
Driver File for the TFT Display from Adafruit
Full color 128x160 display with SPI interface

Link to LCD Information

General - https://learn.adafruit.com/1-8-tft-display
Pinouts - https://learn.adafruit.com/1-8-tft-display/breakout-pinouts
*/


#ifndef LCD_DRIVER__H
#define LCD_DRIVER__H

#include "main.h"
#include <stdint.h>

/////////////////////////////////////////////
//Defines - Size, registers, etc
//

//Display Orientation
//If defined, then header is on the bottom, else 
//it's on the left side

//#define DISPLAY_VERTICAL        1

#ifdef DISPLAY_VERTICAL
#define LCD_WIDTH           ((uint8_t)128)
#define LCD_HEIGHT          ((uint8_t)160)
#else
#define LCD_WIDTH           ((uint8_t)160)
#define LCD_HEIGHT          ((uint8_t)128)
#endif

//row padding
#define LCD_ROW_PADDING88     ((uint16_t)4)
#define LCD_ROW_PADDING1624     ((uint16_t)6)

////////////////////
//color defines
/* LCD color */
#define BLACK                   0x0000
#define NAVY                    0x000F
#define DGREEN                  0x03E0
#define DCYAN                   0x03EF
#define MAROON                  0x7800
#define PURPLE                  0x780F
#define OLIVE                   0x7BE0
#define GREY                    0xF7DE
#define LGRAY                   0xC618
#define DGRAY                   0x7BEF
#define BLUE                    0x001F
#define BLUE2                   0x051F
#define GREEN                   0x07E0
#define CYAN                    0x07FF
#define RED                     0xF800
#define MAGENTA                 0xF81F
#define YELLOW                  0xFFE0
#define WHITE                   0xFFFF

#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define PI					3.14159265359


////////////////////////////////////////////
//Prototypes
void LCD_Delay(uint32_t time);
void LCD_Init(void);
void LCD_Reset(void);


/////////////////////////////////////////
//Writes
void LCD_ChipSelect_Select(void);
void LCD_ChipSelect_Deselect(void);
void LCD_WriteNoChipSelect(uint8_t data);
void LCD_DataEnable(void);
void LCD_CommandEnable(void);

void LCD_WriteData(uint8_t data);
void LCD_WriteCommand(uint8_t command);
void LCD_Write8(uint8_t, uint8_t);								//command + 1 data
void LCD_Write16(uint8_t, uint8_t, uint8_t);					//command + 2 data
void LCD_Write24(uint8_t, uint8_t, uint8_t, uint8_t);			//command + 3 data
void LCD_Write32(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);	//command + 4 data

//reads
uint32_t LCD_ReadReg32(uint8_t reg);
uint32_t LCD_ReadReg16(uint8_t reg);

///////////////////////////////////////////
//Graphics
uint16_t rgb2bgr(uint16_t color);
void LCD_Clear(uint16_t color);
void LCD_PutPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_DrawLine(int x0, int y0, int x1, int y1, uint16_t color);

void LCD_DrawChar88(uint8_t row, uint8_t col, uint8_t letter);
void LCD_DrawChar88XY(uint8_t x, uint8_t y, uint8_t letter);
void LCD_DrawChar1624(uint8_t row, uint8_t col, uint8_t letter);
void LCD_DrawChar1624XY(uint8_t x, uint8_t y, uint8_t letter);

void LCD_DrawString88(uint8_t row, char* temp);
void LCD_DrawString1624(uint8_t row, char* temp);

void LCD_DrawString88Kern(uint8_t row, uint8_t kern, char* temp);
void LCD_DrawString1624Kern(uint8_t row, uint8_t kern, char* temp);

void LCD_DrawString88KernLength(uint8_t row, uint8_t kern, char* temp, uint8_t length);
void LCD_DrawString1624KernLength(uint8_t row, uint8_t kern, char* temp, uint8_t length);

#endif
