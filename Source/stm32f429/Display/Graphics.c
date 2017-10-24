//////////////////////////////////////////////////
/*
Dana Olcott 2015 +/-
Graphics
Function definitions for displaying text, clear
layer, set display layer, etc on the LCD

Assumes the SDRAM is initialized.  All graphics functions
manipulate memory addresses on the external SDRAM.

Note:  There is a problem with using 8bit color in that the
SDRAM is connected as 16bit bus.  Therefore, either need to
change the color depth to 16bits, or compact the frame buffer
into two pixels per write, and do a read/mod/write to put
a single pixel.


*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "Graphics.h"
#include "angle.h"
#include "fonts.h"
#include "ltdc.h"
#include "Memory.h"

uint32_t colorPalletRGB332[256] =
{
	0x000000, 0x000055, 0x0000AA, 0x0000FF,
	0x002400, 0x002455, 0x0024AA, 0x0024FF,
	0x004800, 0x004855, 0x0048AA, 0x0048FF,
	0x006D00, 0x006D55, 0x006DAA, 0x006DFF,
	0x009100, 0x009155, 0x0091AA, 0x0091FF,
	0x00B600, 0x00B655, 0x00B6AA, 0x00B6FF,
	0x00DA00, 0x00DA55, 0x00DAAA, 0x00DAFF,
	0x00FF00, 0x00FF55, 0x00FFAA, 0x00FFFF,
	0x240000, 0x240055, 0x2400AA, 0x2400FF,
	0x242400, 0x242455, 0x2424AA, 0x2424FF,
	0x244800, 0x244855, 0x2448AA, 0x2448FF,
	0x246D00, 0x246D55, 0x246DAA, 0x246DFF,
	0x249100, 0x249155, 0x2491AA, 0x2491FF,
	0x24B600, 0x24B655, 0x24B6AA, 0x24B6FF,
	0x24DA00, 0x24DA55, 0x24DAAA, 0x24DAFF,
	0x24FF00, 0x24FF55, 0x24FFAA, 0x24FFFF,
	0x480000, 0x480055, 0x4800AA, 0x4800FF,
	0x482400, 0x482455, 0x4824AA, 0x4824FF,
	0x484800, 0x484855, 0x4848AA, 0x4848FF,
	0x486D00, 0x486D55, 0x486DAA, 0x486DFF,
	0x489100, 0x489155, 0x4891AA, 0x4891FF,
	0x48B600, 0x48B655, 0x48B6AA, 0x48B6FF,
	0x48DA00, 0x48DA55, 0x48DAAA, 0x48DAFF,
	0x48FF00, 0x48FF55, 0x48FFAA, 0x48FFFF,
	0x6D0000, 0x6D0055, 0x6D00AA, 0x6D00FF,
	0x6D2400, 0x6D2455, 0x6D24AA, 0x6D24FF,
	0x6D4800, 0x6D4855, 0x6D48AA, 0x6D48FF,
	0x6D6D00, 0x6D6D55, 0x6D6DAA, 0x6D6DFF,
	0x6D9100, 0x6D9155, 0x6D91AA, 0x6D91FF,
	0x6DB600, 0x6DB655, 0x6DB6AA, 0x6DB6FF,
	0x6DDA00, 0x6DDA55, 0x6DDAAA, 0x6DDAFF,
	0x6DFF00, 0x6DFF55, 0x6DFFAA, 0x6DFFFF,
	0x910000, 0x910055, 0x9100AA, 0x9100FF,
	0x912400, 0x912455, 0x9124AA, 0x9124FF,
	0x914800, 0x914855, 0x9148AA, 0x9148FF,
	0x916D00, 0x916D55, 0x916DAA, 0x916DFF,
	0x919100, 0x919155, 0x9191AA, 0x9191FF,
	0x91B600, 0x91B655, 0x91B6AA, 0x91B6FF,
	0x91DA00, 0x91DA55, 0x91DAAA, 0x91DAFF,
	0x91FF00, 0x91FF55, 0x91FFAA, 0x91FFFF,
	0xB60000, 0xB60055, 0xB600AA, 0xB600FF,
	0xB62400, 0xB62455, 0xB624AA, 0xB624FF,
	0xB64800, 0xB64855, 0xB648AA, 0xB648FF,
	0xB66D00, 0xB66D55, 0xB66DAA, 0xB66DFF,
	0xB69100, 0xB69155, 0xB691AA, 0xB691FF,
	0xB6B600, 0xB6B655, 0xB6B6AA, 0xB6B6FF,
	0xB6DA00, 0xB6DA55, 0xB6DAAA, 0xB6DAFF,
	0xB6FF00, 0xB6FF55, 0xB6FFAA, 0xB6FFFF,
	0xDA0000, 0xDA0055, 0xDA00AA, 0xDA00FF,
	0xDA2400, 0xDA2455, 0xDA24AA, 0xDA24FF,
	0xDA4800, 0xDA4855, 0xDA48AA, 0xDA48FF,
	0xDA6D00, 0xDA6D55, 0xDA6DAA, 0xDA6DFF,
	0xDA9100, 0xDA9155, 0xDA91AA, 0xDA91FF,
	0xDAB600, 0xDAB655, 0xDAB6AA, 0xDAB6FF,
	0xDADA00, 0xDADA55, 0xDADAAA, 0xDADAFF,
	0xDAFF00, 0xDAFF55, 0xDAFFAA, 0xDAFFFF,
	0xFF0000, 0xFF0055, 0xFF00AA, 0xFF00FF,
	0xFF2400, 0xFF2455, 0xFF24AA, 0xFF24FF,
	0xFF4800, 0xFF4855, 0xFF48AA, 0xFF48FF,
	0xFF6D00, 0xFF6D55, 0xFF6DAA, 0xFF6DFF,
	0xFF9100, 0xFF9155, 0xFF91AA, 0xFF91FF,
	0xFFB600, 0xFFB655, 0xFFB6AA, 0xFFB6FF,
	0xFFDA00, 0xFFDA55, 0xFFDAAA, 0xFFDAFF,
	0xFFFF00, 0xFFFF55, 0xFFFFAA, 0xFFFFFF
};

uint32_t colorPalletRGB323[256] =
{
	0x000000, 0x000024, 0x000048, 0x00006D,
	0x000091, 0x0000B6, 0x0000DA, 0x0000FF,
	0x005500, 0x005524, 0x005548, 0x00556D,
	0x005591, 0x0055B6, 0x0055DA, 0x0055FF,
	0x00AA00, 0x00AA24, 0x00AA48, 0x00AA6D,
	0x00AA91, 0x00AAB6, 0x00AADA, 0x00AAFF,
	0x00FF00, 0x00FF24, 0x00FF48, 0x00FF6D,
	0x00FF91, 0x00FFB6, 0x00FFDA, 0x00FFFF,
	0x240000, 0x240024, 0x240048, 0x24006D,
	0x240091, 0x2400B6, 0x2400DA, 0x2400FF,
	0x245500, 0x245524, 0x245548, 0x24556D,
	0x245591, 0x2455B6, 0x2455DA, 0x2455FF,
	0x24AA00, 0x24AA24, 0x24AA48, 0x24AA6D,
	0x24AA91, 0x24AAB6, 0x24AADA, 0x24AAFF,
	0x24FF00, 0x24FF24, 0x24FF48, 0x24FF6D,
	0x24FF91, 0x24FFB6, 0x24FFDA, 0x24FFFF,
	0x480000, 0x480024, 0x480048, 0x48006D,
	0x480091, 0x4800B6, 0x4800DA, 0x4800FF,
	0x485500, 0x485524, 0x485548, 0x48556D,
	0x485591, 0x4855B6, 0x4855DA, 0x4855FF,
	0x48AA00, 0x48AA24, 0x48AA48, 0x48AA6D,
	0x48AA91, 0x48AAB6, 0x48AADA, 0x48AAFF,
	0x48FF00, 0x48FF24, 0x48FF48, 0x48FF6D,
	0x48FF91, 0x48FFB6, 0x48FFDA, 0x48FFFF,
	0x6D0000, 0x6D0024, 0x6D0048, 0x6D006D,
	0x6D0091, 0x6D00B6, 0x6D00DA, 0x6D00FF,
	0x6D5500, 0x6D5524, 0x6D5548, 0x6D556D,
	0x6D5591, 0x6D55B6, 0x6D55DA, 0x6D55FF,
	0x6DAA00, 0x6DAA24, 0x6DAA48, 0x6DAA6D,
	0x6DAA91, 0x6DAAB6, 0x6DAADA, 0x6DAAFF,
	0x6DFF00, 0x6DFF24, 0x6DFF48, 0x6DFF6D,
	0x6DFF91, 0x6DFFB6, 0x6DFFDA, 0x6DFFFF,
	0x910000, 0x910024, 0x910048, 0x91006D,
	0x910091, 0x9100B6, 0x9100DA, 0x9100FF,
	0x915500, 0x915524, 0x915548, 0x91556D,
	0x915591, 0x9155B6, 0x9155DA, 0x9155FF,
	0x91AA00, 0x91AA24, 0x91AA48, 0x91AA6D,
	0x91AA91, 0x91AAB6, 0x91AADA, 0x91AAFF,
	0x91FF00, 0x91FF24, 0x91FF48, 0x91FF6D,
	0x91FF91, 0x91FFB6, 0x91FFDA, 0x91FFFF,
	0xB60000, 0xB60024, 0xB60048, 0xB6006D,
	0xB60091, 0xB600B6, 0xB600DA, 0xB600FF,
	0xB65500, 0xB65524, 0xB65548, 0xB6556D,
	0xB65591, 0xB655B6, 0xB655DA, 0xB655FF,
	0xB6AA00, 0xB6AA24, 0xB6AA48, 0xB6AA6D,
	0xB6AA91, 0xB6AAB6, 0xB6AADA, 0xB6AAFF,
	0xB6FF00, 0xB6FF24, 0xB6FF48, 0xB6FF6D,
	0xB6FF91, 0xB6FFB6, 0xB6FFDA, 0xB6FFFF,
	0xDA0000, 0xDA0024, 0xDA0048, 0xDA006D,
	0xDA0091, 0xDA00B6, 0xDA00DA, 0xDA00FF,
	0xDA5500, 0xDA5524, 0xDA5548, 0xDA556D,
	0xDA5591, 0xDA55B6, 0xDA55DA, 0xDA55FF,
	0xDAAA00, 0xDAAA24, 0xDAAA48, 0xDAAA6D,
	0xDAAA91, 0xDAAAB6, 0xDAAADA, 0xDAAAFF,
	0xDAFF00, 0xDAFF24, 0xDAFF48, 0xDAFF6D,
	0xDAFF91, 0xDAFFB6, 0xDAFFDA, 0xDAFFFF,
	0xFF0000, 0xFF0024, 0xFF0048, 0xFF006D,
	0xFF0091, 0xFF00B6, 0xFF00DA, 0xFF00FF,
	0xFF5500, 0xFF5524, 0xFF5548, 0xFF556D,
	0xFF5591, 0xFF55B6, 0xFF55DA, 0xFF55FF,
	0xFFAA00, 0xFFAA24, 0xFFAA48, 0xFFAA6D,
	0xFFAA91, 0xFFAAB6, 0xFFAADA, 0xFFAAFF,
	0xFFFF00, 0xFFFF24, 0xFFFF48, 0xFFFF6D,
	0xFFFF91, 0xFFFFB6, 0xFFFFDA, 0xFFFFFF
};

/////////////////////////////////////////////////
//GetPalletValue
//Returns the pallet entry for a 16 bit 565 RGB color
//
//Value in the color pallet is BGR - 888 according to the text
//in Segger bitmap utility.  Funny thing is that it returns
//an 8 bit value assuming RGB - 8 bit, 332!!!
//There might be a typo in the comments in output
//related to the color pallet.
//
uint8_t LCD_GetRGB332PalletValue(uint16_t color)
{
	uint8_t red = (uint8_t)((color >> 13) & 0x07);
	uint8_t green = (uint8_t)((color >> 8) & 0x07);
	uint8_t blue = (uint8_t)((color >> 3) & 0x03);

	uint8_t finalColor = (red << 5) | ( green << 2) | (blue);
	return finalColor;
}



/////////////////////////////////////////////
//Clear page/layer in SDRAM with a color.
//Note:
//We could probably cut the number of writes
//in half if the layer used the top and bottom bytes
//
//
void LCD_Clear(uint8_t layer, uint16_t color)
{
	//get the 8 bit color value from the 16 bit value
	uint8_t color8 = LCD_GetRGB332PalletValue(color);
	uint32_t index = 0;

	uint32_t numPixels = LCD_WIDTH * LCD_HEIGHT;
	uint32_t pageOffset = layer * SDRAM_LCD_LAYER_SIZE;

	for (index = 0 ; index < numPixels ; index++)
	{
		//deref value of address = color
		*(__IO uint32_t*) (SDRAM_BASE_ADDR + SDRAM_LCD_LAYER_OFFSET + pageOffset + index) = color8;
	}
}


///////////////////////////////////////////////
//LCD_PutPixel
//Draw pixel into page located on SDRAM.
//Note:  This draws an extra byte since the SDRAM
//is configured as 16bits, need to fix this.
//
void LCD_PutPixel(uint8_t layer, uint16_t x, uint16_t y, uint16_t color)
{
    //do simple test for x and y in range
    if ((x < 0) || (x > LCD_WIDTH-1))
        return;

    if ((y < 0) || (y > LCD_HEIGHT - 1))
        return;

    uint8_t color8 = LCD_GetRGB332PalletValue(color);
    //each of these correspond to a number of elements
    //counted from the top element
    uint16_t rowOffset = y * LCD_WIDTH;
    uint16_t colOffset = x; 
    uint16_t cell = rowOffset + colOffset;

    //draw to the appropriate layer in sdram
	*(__IO uint32_t*) (SDRAM_BASE_ADDR + SDRAM_LCD_LAYER_OFFSET +
						(layer * SDRAM_LCD_LAYER_SIZE) + cell) = color8;
}

//////////////////////////////////////////////
//Breshman's eq.
//
void LCD_DrawLine(uint8_t layer, int x0, int y0, int x1, int y1, uint16_t color)
{

	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
	  LCD_PutPixel(layer, x0,y0, color);
	  if (x0==x1 && y0==y1) break;
	  e2 = err;
	  if (e2 >-dx) { err -= dy; x0 += sx; }
	  if (e2 < dy) { err += dx; y0 += sy; }
	}    
}


///////////////////////////////////////////
//LCD_DrawRadius
//Draw line function modified to start x,y,
//length and radius.  Uses angle lookup table
//to compute sin/cos.
//
void LCD_DrawRadius(uint8_t layer, int x0, int y0, int length, int angle, uint16_t color)
{
	float dx_length, dy_length;

	dx_length = length * angleList[angle].cos_val;
	dy_length = length * angleList[angle].sin_val;

	int x1 = x0 + (int)dx_length;
	int y1 = y0 + (int)dy_length;

	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
	  LCD_PutPixel(layer, x0,y0, color);
	  if (x0==x1 && y0==y1) break;
	  e2 = err;
	  if (e2 >-dx) { err -= dy; x0 += sx; }
	  if (e2 < dy) { err += dx; y0 += sy; }
	}
}


///////////////////////////////////////////////////////
//LCD_RotateBuffer
//Rotates the contents of source into destination
//at angle, about center xc, yc.
//
void LCD_RotateBuffer(uint8_t* source, uint8_t* destination, uint16_t angle, int xc, int yc)
{
	//temp values
	//computed x and y based on i, j, and coordinate system.
	int x, y, convX, convY;
	float newx, newy;

	//get the cos and sin from angle.h   
	float _cos = angleList[angle].cos_val;
	float _sin = angleList[angle].sin_val;

	//we need to iterate on each value in the source
	int16_t i, j;
	for (i = 0 ; i < LCD_HEIGHT ; i++)
	{
		for (j = 0 ; j < LCD_WIDTH ; j++)        
		{
           //get my x and y, original, and offset
           convX = j - xc;
           convY = i - yc;
           
           x = j;
           y = i;

           //compute the new x and y
           newx = convX*_cos - convY*_sin;	//compute transformed value
           newy = convX*_sin + convY*_cos;	//compute transformed value

           //add back the offsets - might have to do a rounding
           //somewhere or add 1
           
           newx += xc;			//put the offset back
           newy += yc;			//put the offset back

           if (newx < 0.0)
               newx = 0.0;
           else if (newx > (LCD_WIDTH - 1))
               newx = LCD_WIDTH - 1;
           if (newy < 0.0)
               newy = 0.0;
           else if (newy > (LCD_HEIGHT - 1))
               newy = LCD_HEIGHT - 1;


            uint16_t newRowOffset = (uint16_t)newy * LCD_WIDTH;
            uint16_t newColOffset = (uint16_t)newx;

            uint16_t value = source[newRowOffset + newColOffset];

            uint16_t cell = y*LCD_WIDTH + x;
            destination[cell] = value;
		}
	}
}





////////////////////////////////////////////////////////////
//LCD Text Functions
void LCD_DrawChar(uint8_t layer, uint8_t row, uint8_t col, uint8_t letter)
{
	uint16_t charWidth = 16;
	uint16_t charHeight = 24;

	//starting point for a single char
	uint16_t xOffset = col * charWidth;
	uint16_t yOffset = row * charHeight;

	//write the char in terms of x,y space calling put pixel
	//first char in the lookup table is ascii 32
	 uint16_t line = (uint16_t)letter - (uint16_t)32;

	//font table is 16 bits per entry, one entry per line
	//bit has to be 16 bits since we are shifting up to
	//16 times
	uint16_t p = 16, bit = 0;

	//there are 24 elements per char height
	for (uint16_t i = 0 ; i < 24 ;  i++)
	{
		//get element and test for flip top/bottom
		uint32_t temp = Ascii_16_24_Table[(line*24)+i];

	   //display flipped - read bottom up
	   if (DISP_ORIENTATION == 180)
	   {
		  temp = Ascii_16_24_Table[(line*24)+23-i];
	   }

		p = 16;      	//pixels on the line
		bit = 0;        //test if color or no color
		while (p > 0)
		{
			//font table is reading flipped left to right
//			bit = (((uint16_t)1u << (p-1) ) & temp) >> (p-1);

			//scan right to left
			bit = (((uint16_t)1u << (16-p) ) & temp) >> (16-p);

			//if the bit = 0 - it's blank
			//if the bit = 1 - it's a color
			if (bit == 1)
			{
				//draws flipped left to right
				LCD_PutPixel(layer, xOffset + 16 - p, yOffset + i, WHITE);
			}
			else
			{
				LCD_PutPixel(layer, xOffset + 16 - p, yOffset + i, BLACK);
			}

			p--;
		}
	}
}

//LCD_DrawString - Displays a string on the LCD at a
//given layer and row.  Does not support kerning
void LCD_DrawString(uint8_t layer, uint8_t row, char* output)
{
	//get the length limited by max cols
	uint8_t length = strlen(output);
	if (length > MAX_COLS)
		length = MAX_COLS;

	for (uint8_t i = 0 ; i < length ; i++)
		LCD_DrawChar(layer, row, i, output[i]);
}

void LCD_DrawStringLength(uint8_t layer, uint8_t row, char output[], uint8_t length)
{
	//get the length limited by max cols
	uint8_t size = length;
	if (length > MAX_COLS)
		size = MAX_COLS;

	for (uint8_t i = 0 ; i < size ; i++)
		LCD_DrawChar(layer, row, i, output[i]);
}

