/*
Driver File for the TFT Display from Adafruit
Full color 128x160 display with SPI interface

Link to LCD Information

General - https://learn.adafruit.com/1-8-tft-display
Pinouts - https://learn.adafruit.com/1-8-tft-display/breakout-pinouts
*/

#include "lcd_driver.h"
#include "font.h"
#include "offset.h"


//////////////////////////////////////////////
//Declare Variables
uint16_t m_lcdFontColor = WHITE;
uint16_t m_lcdBackColor = BLACK;

////////////////////////////////////////////
//generic delay function, not accurate
//a value of 500 is about 14us
void LCD_Delay(uint32_t time)
{
    volatile uint32_t counter = time;
    while (counter > 0)
        counter--;    
}

void LCD_Init(void)
{
    LCD_Reset();
    LCD_Delay(50000);

    //Initialize lcd registers - from Adafruit
    //and the ST7735 controller

	LCD_WriteCommand(0x11);		//take out of sleep mode
    LCD_Delay(50000);
    
	//Frame rate - reg B1, B2, B3
	LCD_Write24(0xB1, 0x01, 0x2C, 0x2D);	//frame rate, front porch, back porch, etc
	LCD_Write24(0xB2, 0x01, 0x2C, 0x2D);	//same, but in idle mode
	LCD_Write24(0xB3, 0x01, 0x2C, 0x2D);	//same, but in partial mode, full color

	//column inversion
	LCD_Write8(0xB4, 0x07);				//frame inversion for each mode

	//power on sequence
	LCD_Write24(0xC0, 0xA2, 0x02, 0x84);	//
	LCD_Write8(0xC1, 0xC5);
	LCD_Write16(0xC2, 0x0A, 0x00);
	LCD_Write16(0xC3, 0x8A, 0x2A);
	LCD_Write16(0xC4, 0x8A, 0xEE);

	LCD_Write8(0xC5, 0x0E);				//VCOM


    //See Page 113 of the datasheet
    //LCD Display Orientation - Normal is 128x160 with the header at the 
    //bottom of the display.  Mx, My, Mv bits control how the
    //x, y, and vertical are displayed, rotated, etc.  
    //Reg 36h - bits 5, 6, 7

#ifdef DISPLAY_VERTICAL
    //vertical orientation - header at the bottom of the display
    //RGB enabled
    //my=1, mx=1, mv=0, ml=0            //header at the top of the display
    LCD_Write8(0x36, 0xC8);			//Mx, My, RBG mode
#else




    /////////////////////////////////////////////////////
    //Use this one....  
    //160x128, header on the left side of the display
    //This puts the buttons at the top, the battery connector
    //on the top.
    //requires flipping x and y in all instances
    //rotate 90 degrees - header at the left side of the display
    //my=0, mx=1, mv=1, ml=0
    //RGB enabled
    LCD_Write8(0x36, 0x68);

#endif


	//gamma sequence - a bunch of writes here
	LCD_WriteCommand(0xE0);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x1A);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x18);
	LCD_WriteData(0x2F);
	LCD_WriteData(0x28);
	LCD_WriteData(0x20);
	LCD_WriteData(0x22);
	LCD_WriteData(0x1F);
	LCD_WriteData(0x1B);
	LCD_WriteData(0x23);
	LCD_WriteData(0x37);
	LCD_WriteData(0x00);
	LCD_WriteData(0x07);
	LCD_WriteData(0x02);
	LCD_WriteData(0x10);

	LCD_WriteCommand(0xE1);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x1B);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x17);
	LCD_WriteData(0x33);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x29);
	LCD_WriteData(0x2E);
	LCD_WriteData(0x30);
	LCD_WriteData(0x30);
	LCD_WriteData(0x39);
	LCD_WriteData(0x3F);
	LCD_WriteData(0x00);
	LCD_WriteData(0x07);
	LCD_WriteData(0x03);
	LCD_WriteData(0x10);

	LCD_Write32(0x2A, 0x00, 0x00, 0x00, 0x7F);		//column address set xs = 0 and xe = 127
	LCD_Write32(0x2B, 0x00, 0x00, 0x00, 0x9F);		//row address set ys = 0, ye = 159

	LCD_Write8(0xF0, 0x01);		//enable test command
	LCD_Write8(0xF6, 0x00);		//disable ram power save mode

	LCD_Write8(0x3A, 0x05);		//65k mode


	//add writes to registers for read back project code, LCD code, etc
	//ID2 and ID3 in registers 0xD1 and 0xD2, stores values in EEPROM

	//read these back using registers 0xDB for ID2
	//and 0xDC for ID3
	LCD_Write8(0xD1, 0x55); 	//bits 6, 4, 2, 0 high (ID26, ID24, ID22, ID20)
	LCD_Write8(0xD2, 0xCC); 	//bits 7, 6, 3, 2 (ID37, ID36, ID33, ID32)

	LCD_WriteCommand(0x29);		//display on
	LCD_Clear(0x0000);				//black
           
}

void LCD_Reset(void)
{
    GPIOPinWrite(CONTROL_PORT, LCD_RESET_PIN, 0x00);            //low
    LCD_Delay(500);                                             //wait
    GPIOPinWrite(CONTROL_PORT, LCD_RESET_PIN, LCD_RESET_PIN);   //high        
}


void LCD_ChipSelect_Select(void)
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
}


void LCD_ChipSelect_Deselect(void)
{
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
}

void LCD_DataEnable(void)
{
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high
}

void LCD_CommandEnable(void)
{
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);   //cmd low
}

void LCD_WriteNoChipSelect(uint8_t data)
{
    uint32_t dummy;
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, data);                            //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           
}



/////////////////////////////////////////
//Writes
//Write Data
void LCD_WriteData(uint8_t data)
{
    uint32_t dummy;
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, data);                            //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select
}

//Write Command
void LCD_WriteCommand(uint8_t command)
{
    uint32_t dummy;
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, command);                            //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select
}

//Write Command and 1 Data Byte
void LCD_Write8(uint8_t reg, uint8_t value1)
{
    uint32_t dummy;
    //command first
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select

    //command
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, reg);                             //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    //data
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value1);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    //deselect
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select
}

//Write Command and 2 data bytes
void LCD_Write16(uint8_t reg, uint8_t value1, uint8_t value2)
{
    uint32_t dummy;

    //command first
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select

    //command
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, reg);                             //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    //data
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value1);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value2);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    //deselect
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select
}

//Write Command and 3 data bytes
void LCD_Write24(uint8_t reg, uint8_t value1, uint8_t value2, uint8_t value3)
{
    uint32_t dummy;
    //command first
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select

    //command
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, reg);                             //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    //data
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value1);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value2);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value3);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    //deselect
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select
}

//Write Command and 4 data bytes
void LCD_Write32(uint8_t reg, uint8_t value1, uint8_t value2, uint8_t value3, uint8_t value4)
{
    uint32_t dummy;

    //command first
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select

    //command
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, reg);                             //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    //data
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value1);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value2);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value3);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, value4);                          //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    //deselect
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select
}




/////////////////////////////////////////
//Read a register containing a 32bit value
//Send reg - command
//read 4 bytes using dat
uint32_t LCD_ReadReg32(uint8_t reg)
{
    uint32_t byte0 = 0xFF;
    uint32_t byte1 = 0xFF;
    uint32_t byte2 = 0xFF;
    uint32_t byte3 = 0xFF;

    uint32_t dummy;


    //command first
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select

    //send the register as a write   
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, reg);                             //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           

    //generate 32bits of clock cycles, pulling data off the fifo
    //for each 8 bits

    //switch to data mode
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high

    //read 0
    SSIDataPut(SSI3_BASE, 0xFF);                            //dummy
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &byte0);                          //byte 0

    SSIDataPut(SSI3_BASE, 0xFF);                            //dummy
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &byte1);                          //byte 0

    SSIDataPut(SSI3_BASE, 0xFF);                            //dummy
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &byte2);                          //byte 0

    SSIDataPut(SSI3_BASE, 0xFF);                            //dummy
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &byte3);                          //byte 0

    //deselect
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select

    uint32_t value = byte0 & 0xFF;
    value |= ((byte1 & 0xFF) << 8);
    value |= ((byte2 & 0xFF) << 16);
    value |= ((byte3 & 0xFF) << 24);

    return value;

}





/////////////////////////////////////////
//Read a register containing a 16bit value
//Send reg - command
//read 2 bytes using dat
uint32_t LCD_ReadReg16(uint8_t reg)
{
    uint32_t byte0, byte1, dummy;
    

    //command first
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);          //cmd low
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);        //select

    //send the register as a write   
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataPut(SSI3_BASE, reg);                             //send
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &dummy);           //clear the fifo           


    //generate 16bits of clock cycles, pulling data off the fifo
    //for each 8 bits

    //switch to data mode
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, LCD_CMD_PIN);   //data high

    //read 0
    SSIDataPut(SSI3_BASE, 0x00);                            //dummy
    while(SSIBusy(SSI3_BASE));                              //wait
    SSIDataGet(SSI3_BASE, &byte0);                          //byte 0

    SSIDataPut(SSI3_BASE, 0x00);                            //dummy
    while(SSIBusy(SSI3_BASE));                              //wait    
    SSIDataGet(SSI3_BASE, &byte1);                          //byte 0

    //deselect
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);  //select

    uint32_t value = byte0 & 0xFF;
    value |= ((byte1 & 0xFF) << 8);

    return value;

}





///////////////////////////////////////////
//Graphics
uint16_t rgb2bgr(uint16_t color)
{
    uint16_t  r, g, b, rgb;

    b = ( color>>(uint16_t)0 )  & 0x1f;
    g = ( color>>(uint16_t)5 )  & 0x3f;
    r = ( color>>(uint16_t)11 ) & 0x1f;

    rgb =  (b<<(uint16_t)11) + (g<<(uint16_t)5) + (r<<(uint16_t)0);

    return( rgb );

}

////////////////////////////////////
//LCD_Clear(COLOR)
//clears the entire screen.  sets the xstart stop
//and y start stop here as well.  I think the colors
//are written in reverse, so flip rgb to bgr

void LCD_Clear(uint16_t color)
{
	uint8_t top = (uint8_t)(rgb2bgr(color) >> 8);
	uint8_t bottom = (uint8_t)(rgb2bgr(color) & 0xFF);

	LCD_Write32(0x2A, 0x00, 0x00, 0x00, (LCD_WIDTH - 1));		//column address set xs = 0 and xe = LCD_WIDTH - 1
	LCD_Write32(0x2B, 0x00, 0x00, 0x00, (LCD_HEIGHT - 1));		//row address set ys = 0, ye = LCD_HEIGHT - 1

	int i, j;

	//set to ram write - 2c.  column and row start and stop
	//registers are assumed to be set prior to this.  probably
	//should set these again here.  might try to set just a
	//small area.
	LCD_WriteCommand(0x2C);

    //go time.... write 12x160 x 2 bytes

    LCD_ChipSelect_Select();        //select
    LCD_DataEnable();               //send data

    //y
	for (i = 160 ; i > 0 ; i--)
	{
        //x
		for (j = 128 ; j > 0 ; j--)
		{
			LCD_WriteNoChipSelect(top);
			LCD_WriteNoChipSelect(bottom);
		}
	}

    LCD_ChipSelect_Deselect();

}

void LCD_PutPixel(uint8_t x, uint8_t y, uint16_t color)
{
    //LCD_HEIGHT and WIDTH are defined based on 
    //display orientation
	//simple test for valid input x and y
	if ((x > (LCD_WIDTH - 1)) || (y > (LCD_HEIGHT - 1)))
	{
		return;
	}

	//set up the registers for x and y start and stop
	LCD_Write32(0x2A, 0x00, x, 0x00, x);		//col address, x
	LCD_Write32(0x2B, 0x00, y, 0x00, y);		//row address, y

	uint8_t top = (uint8_t)(rgb2bgr(color) >> 8);
	uint8_t bottom = (uint8_t)(rgb2bgr(color) & 0xFF);

	LCD_Write16(0x2C, top, bottom);
}



void LCD_DrawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
	  LCD_PutPixel(x0,y0, color);
	  if (x0==x1 && y0==y1) break;
	  e2 = err;
	  if (e2 >-dx) { err -= dy; x0 += sx; }
	  if (e2 < dy) { err += dx; y0 += sy; }
	}
}



///////////////////////////////////////
//LCD_DrawChar88()
//Puts a character on the display at a given row
//and column.  Rows are equally spaced among the
//height of the display. 
//Max Rows = 160 / 8 = 20
//Max Cols = 128 / 8 = 16
//
//Sets up a draw window 8x8 at row and col offset
void LCD_DrawChar88(uint8_t row, uint8_t col, uint8_t letter)
{
	if ((row > 20) || (col > 16))
	{
		return;
	}

	//set the start and stop x and y values
	uint8_t x1 = col * 8;
	uint8_t x2 = x1 + 7;
	uint8_t y1 = row * 8;
	uint8_t y2 = y1 + 7;

	LCD_Write32(0x2A, 0x00, x1, 0x00, x2);		//col address, x1 and x2
	LCD_Write32(0x2B, 0x00, y1, 0x00, y2);		//row address, y1 and y2

	LCD_WriteCommand(0x2C);		//ram set

	//get the values from the font table
    uint8_t line = letter - (uint8_t)32;
    uint8_t p = 8, bit = 0;
    uint8_t i = 0;

	uint8_t topFont = (uint8_t)(rgb2bgr(m_lcdFontColor) >> 8);
	uint8_t bottomFont = (uint8_t)(rgb2bgr(m_lcdFontColor) & 0xFF);

	uint8_t topBack = (uint8_t)(rgb2bgr(m_lcdBackColor) >> 8);
	uint8_t bottomBack = (uint8_t)(rgb2bgr(m_lcdBackColor) & 0xFF);

    //select the device, enable data
    LCD_ChipSelect_Select();
    LCD_DataEnable();

    for (i = 0 ; i < 8 ;  i++)
    {
        uint8_t temp = font_table[(line*8)+i];

        p = 8;                                           //pixels on the line
        bit = 0;                                          //test if color or no color
        while (p > 0)
        {
            bit = ((1u << (p-1)) & temp) >> (p-1);      //test each of 8 pixels on a single line

            //if the bit = 0 - it's blank               //background
            //if the bit = 1 - it's a color             //font
            if (bit == 1)
            {
    			LCD_WriteNoChipSelect(topFont);
    			LCD_WriteNoChipSelect(bottomFont);
            }
            else
            {
    			LCD_WriteNoChipSelect(topBack);
    			LCD_WriteNoChipSelect(bottomBack);
            }

            p--;
        }
    }

    LCD_ChipSelect_Deselect();
}



//////////////////////////////////////////
//LCD_DrawChar1624()
//Draws a 16x24 character listed in the font
//library, font_table1624[] at a specified row
//and column.  For this display, the max number
//of rows = 160 / 24 = 6, and the max number of
//columns = 128 / 16 = 8
//Sets up a draw window 16x24 at row and col offset
void LCD_DrawChar1624(uint8_t row, uint8_t col, uint8_t letter)
{
	if ((row > 5) || (col >8))
	{
		return;
	}

	//set the start and stop x and y values
	uint8_t x1 = col * 16;
	uint8_t x2 = x1 + 15;
	uint8_t y1 = row * 24;
	uint8_t y2 = y1 + 23;

	LCD_Write32(0x2A, 0x00, x1, 0x00, x2);		//col address, x1 and x2
	LCD_Write32(0x2B, 0x00, y1, 0x00, y2);		//row address, y1 and y2

	LCD_WriteCommand(0x2C);		//ram set

    //select the display and enable data
    LCD_ChipSelect_Select();
    LCD_DataEnable();

	//get the values from the font table
    uint8_t line = letter - (uint8_t)32;
    uint8_t p = 16, bit = 0;			//p = width
    uint8_t i = 0;

	uint8_t topFont = (uint8_t)(rgb2bgr(m_lcdFontColor) >> 8);
	uint8_t bottomFont = (uint8_t)(rgb2bgr(m_lcdFontColor) & 0xFF);

	uint8_t topBack = (uint8_t)(rgb2bgr(m_lcdBackColor) >> 8);
	uint8_t bottomBack = (uint8_t)(rgb2bgr(m_lcdBackColor) & 0xFF);

    for (i = 0 ; i < 24 ;  i++)
    {
        //text orientation
        uint16_t temp = font_table1624[(line*24)+i];

        p = 16;              //pixels on the line
        bit = 0;             //test if color or no color
        while (p > 0)
        {
            //search from RHS to LHS side since font is reversed.
            bit = (((uint16_t)1u << (16-p)) & temp) >> (16-p);

            //if the bit = 0 - it's blank
            //if the bit = 1 - it's a color
            if (bit == 1)
            {
    			LCD_WriteNoChipSelect(topFont);
    			LCD_WriteNoChipSelect(bottomFont);
            }
            else
            {
    			LCD_WriteNoChipSelect(topBack);
    			LCD_WriteNoChipSelect(bottomBack);
            }

            p--;
        }
    }

    //deselect the display
    LCD_ChipSelect_Deselect();

}



///////////////////////////////////////
//LCD_DrawChar88XY()
//Puts a character on the display at a given
//x,y location.
//
//Sets up a draw window 8x8 at row and col offset
void LCD_DrawChar88XY(uint8_t x, uint8_t y, uint8_t letter)
{
	if ((x > (LCD_WIDTH - 9)) || (y > (LCD_HEIGHT - 9)))
	{
		return;
	}

	//set the start and stop x and y values
	uint8_t x1 = x;
	uint8_t x2 = x1 + 7;
	uint8_t y1 = y;
	uint8_t y2 = y1 + 7;

	LCD_Write32(0x2A, 0x00, x1, 0x00, x2);		//col address, x1 and x2
	LCD_Write32(0x2B, 0x00, y1, 0x00, y2);		//row address, y1 and y2

	LCD_WriteCommand(0x2C);		//ram set

	//get the values from the font table
    uint8_t line = letter - (uint8_t)32;
    uint8_t p = 8, bit = 0;
    uint8_t i = 0;

	uint8_t topFont = (uint8_t)(rgb2bgr(m_lcdFontColor) >> 8);
	uint8_t bottomFont = (uint8_t)(rgb2bgr(m_lcdFontColor) & 0xFF);

	uint8_t topBack = (uint8_t)(rgb2bgr(m_lcdBackColor) >> 8);
	uint8_t bottomBack = (uint8_t)(rgb2bgr(m_lcdBackColor) & 0xFF);

    //select the device, enable data
    LCD_ChipSelect_Select();
    LCD_DataEnable();

    for (i = 0 ; i < 8 ;  i++)
    {
        uint8_t temp = font_table[(line*8)+i];

        p = 8;                                           //pixels on the line
        bit = 0;                                          //test if color or no color
        while (p > 0)
        {
            bit = ((1u << (p-1)) & temp) >> (p-1);      //test each of 8 pixels on a single line

            //if the bit = 0 - it's blank               //background
            //if the bit = 1 - it's a color             //font
            if (bit == 1)
            {
    			LCD_WriteNoChipSelect(topFont);
    			LCD_WriteNoChipSelect(bottomFont);
            }
            else
            {
    			LCD_WriteNoChipSelect(topBack);
    			LCD_WriteNoChipSelect(bottomBack);
            }

            p--;
        }
    }

    LCD_ChipSelect_Deselect();

}






//////////////////////////////////////////
//LCD_DrawChar1624XY()
//Draws a 16x24 character listed in the font
//library, font_table1624[] at a specified
//x and y offset.  Simple check to make sure it
//fits on the screen.
void LCD_DrawChar1624XY(uint8_t x, uint8_t y, uint8_t letter)
{
	if ((x > (LCD_WIDTH - 17)) || (y >(LCD_HEIGHT - 25)))
	{
		return;
	}

	//set the start and stop x and y values
	uint8_t x1 = x;
	uint8_t x2 = x1 + 15;
	uint8_t y1 = y;
	uint8_t y2 = y1 + 23;

	LCD_Write32(0x2A, 0x00, x1, 0x00, x2);		//col address, x1 and x2
	LCD_Write32(0x2B, 0x00, y1, 0x00, y2);		//row address, y1 and y2

	LCD_WriteCommand(0x2C);		//ram set


    //select the display and enable data
    LCD_ChipSelect_Select();
    LCD_DataEnable();

	//get the values from the font table
    uint8_t line = letter - (uint8_t)32;
    uint8_t p = 16, bit = 0;			//p = width
    uint8_t i = 0;

	uint8_t topFont = (uint8_t)(rgb2bgr(m_lcdFontColor) >> 8);
	uint8_t bottomFont = (uint8_t)(rgb2bgr(m_lcdFontColor) & 0xFF);

	uint8_t topBack = (uint8_t)(rgb2bgr(m_lcdBackColor) >> 8);
	uint8_t bottomBack = (uint8_t)(rgb2bgr(m_lcdBackColor) & 0xFF);

    for (i = 0 ; i < 24 ;  i++)
    {
        //text orientation
        uint16_t temp = font_table1624[(line*24)+i];

        p = 16;              //pixels on the line
        bit = 0;             //test if color or no color
        while (p > 0)
        {
        //search from RHS to LHS side since font is reversed.
            bit = (((uint16_t)1u << (16-p)) & temp) >> (16-p);

            //if the bit = 0 - it's blank
            //if the bit = 1 - it's a color
            if (bit == 1)
            {
    			LCD_WriteNoChipSelect(topFont);
    			LCD_WriteNoChipSelect(bottomFont);

            }
            else
            {
    			LCD_WriteNoChipSelect(topBack);
    			LCD_WriteNoChipSelect(bottomBack);
            }

            p--;
        }
    }

    //deselect the display
    LCD_ChipSelect_Deselect();

}








//////////////////////////////////////////
//whole string
void LCD_DrawString88(uint8_t row, char* temp)
{
	uint8_t i = 0;

	while ((temp[i] != '\0') && (i < 20))
	{
		LCD_DrawChar88(row, i, temp[i]);
		i++;
	}
}

void LCD_DrawString1624(uint8_t row, char* temp)
{
	uint8_t i = 0;
	while ((temp[i] != '\0') && (i < 9))
	{
		LCD_DrawChar1624(row, i, temp[i]);
		i++;
	}
}



void LCD_DrawString88Kern(uint8_t row, uint8_t kern, char* temp)
{
    uint8_t xoffset = 0;
	uint8_t i = 0;
    uint8_t y = row * (8 + LCD_ROW_PADDING88);

	while ((temp[i] != '\0') && (i < 20))
	{
        LCD_DrawChar88XY(xoffset, y, temp[i]);
        uint8_t offset = offsetChar88[temp[i] - 32];
        xoffset += (8 - offset + kern);
		i++;
	}
}

void LCD_DrawString1624Kern(uint8_t row, uint8_t kern, char* temp)
{
    uint8_t xoffset = 0;
	uint8_t i = 0;
    uint8_t y = row * (24 + LCD_ROW_PADDING1624);

	while ((temp[i] != '\0') && (i < 20))
	{
        LCD_DrawChar1624XY(xoffset, y, temp[i]);
        uint8_t offset = offsetChar1624[temp[i] - 32];
        xoffset += (16 - offset + kern);
		i++;
	}
}


void LCD_DrawString88KernLength(uint8_t row, uint8_t kern, char* temp, uint8_t length)
{
    uint8_t xoffset = 0;
    uint8_t y = row * (8 + LCD_ROW_PADDING88);

    for (int i = 0 ; i < length ; i++)
	{
        LCD_DrawChar88XY(xoffset, y, temp[i]);
        uint8_t offset = offsetChar88[temp[i] - 32];
        xoffset += (8 - offset + kern);
	}
}

void LCD_DrawString1624KernLength(uint8_t row, uint8_t kern, char* temp, uint8_t length)
{
    uint8_t xoffset = 0;
    uint8_t y = row * (24 + LCD_ROW_PADDING1624);

    for (int i = 0 ; i < length ; i++)
	{
        LCD_DrawChar1624XY(xoffset, y, temp[i]);
        uint8_t offset = offsetChar1624[temp[i] - 32];
        xoffset += (16 - offset + kern);
	}

}

