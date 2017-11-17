///////////////////////////////////////////////////////
/*
Dana Olcott 2017

ButtonWidget Controller files for the touch panel
component to the stm32f429 development board.

How it (hopefully) works:
Create a ButtonWidget type struct that contains
all parameters that describe the button displayed
on the screen.  Start out Including:

- 	size x, y, x and y offset - size/location of the button
- 	function pointer for a function to run when 
	the button is pressed.
-	buttons can't overlap

Allocate an array of button type widgets a head of time,
with a function call to make a new button, update number
of buttons, etc.

From the isr reading position data,
get the index from x,y
call the function associated with the index
using the following:

(*buttonWidgetList[index].buttonHandler)();

*/
//////////////////////////////////////////////

#include "cmsis_os.h"		//delay

#include "ButtonWidget.h"

#include "gpio.h"
#include "Graphics.h"		//color, LCD functions
#include "Memory.h"

/////////////////////////////////////////////////////
//buttonWidgetList Definition
//index, x, y, sizeX, sizeY, functionToRun
//state - off/on.  toggle the state changes the on/off color
//
ButtonWidget_t buttonWidgetList[] =
{
	{0,		BUTTON_STATE_OFF, 	0,		0, 		80, 	80,		WHITE,		BLUE,		btnFunction1},
	{1, 	BUTTON_STATE_OFF,	80,		0, 		80, 	80,		WHITE,		RED, 		btnFunction2},
	{2, 	BUTTON_STATE_OFF,	160,	0, 		80, 	80,		WHITE, 		GREEN,		btnFunction3},
	{3, 	BUTTON_STATE_OFF,	0,		80,		240, 	240,	WHITE, 		YELLOW,		btnFunction4},		//remainder

	//use 0xFF as end of table id
	{(int)0xFF, (int)NULL, 		(int)NULL, 	(int)NULL, 	(int)NULL, 	(int)NULL,	(int)NULL,	(int)NULL},

};



void btnFunction1(void)
{
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
}

void btnFunction2(void)
{
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
}

void btnFunction3(void)
{
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
	osDelay(100);
}



void btnFunction4(void)
{
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
	osDelay(100);
	HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin);
	osDelay(100);

}


/////////////////////////////////////////
//Draw buttons on a specific layer
//LCD_DrawBox(x, y, sizeX, sizeY, color)
//for each button.
//enable and disable the layer using the
//user button.
void ButtonWidget_init(void)
{
	ButtonWidget_DrawButtons(10);
}

void ButtonWidget_DrawButtons(uint16_t layer)
{

	uint16_t index = 0;

	while (buttonWidgetList[index].index < 0xFF)
	{

		LCD_DrawBox(layer,
					buttonWidgetList[index].xPosition,
					buttonWidgetList[index].yPosition,
					buttonWidgetList[index].xSize,
					buttonWidgetList[index].ySize,
					buttonWidgetList[index].offColor);
		index++;
	}

	HAL_LTDC_SetAddress(&hltdc, (uint32_t)SDRAM_LCD_LAYER_10, 1);

}

int ButtonWidget_GetButtonIndexFromPosition(uint16_t xPos, uint16_t yPos)
{
	int index = 0;
	uint16_t xStart, xStop, yStart, yStop = 0;

	while (buttonWidgetList[index].index < 0xFF)
	{
		xStart = buttonWidgetList[index].xPosition;
		xStop = xStart + buttonWidgetList[index].xSize;
		yStart = buttonWidgetList[index].yPosition;
		yStop = yStart + buttonWidgetList[index].ySize;

		if ((xPos >= xStart) && (xPos <= xStop) && (yPos >= yStart) && (yPos <= yStop))
		{
			return index;
		}

		index++;
	}

	return -1;
}


ButtonWidget_t ButtonWidget_GetButtonFromIndex(int index)
{
	return buttonWidgetList[index];
}


ButtonWidget_t ButtonWidget_GetButtonFromPosition(uint16_t x, uint16_t y)
{
	int index = ButtonWidget_GetButtonIndexFromPosition(x, y);

	if (index >= 0)
		return buttonWidgetList[index];
	else
	{
		return buttonWidgetList[0];
	}
}


///////////////////////////////////////////////////
//Set ButtonState_t state of typedef ButtonWidget_t
//to BUTTONS_STATE_ON / _OFF, sets the appropriate
//color based on the state.
void ButtonWidget_SetButtonState(ButtonWidget_t button, ButtonState_t state)
{
	if (state == BUTTON_STATE_OFF)
	{
		button.btnState = state;
		ButtonWidget_SetButtonColor(button, button.offColor);
	}
	else if (state == BUTTON_STATE_ON)
	{
		button.btnState = state;
		ButtonWidget_SetButtonColor(button, button.onColor);
	}
}


ButtonState_t ButtonWidget_GetButtonState(ButtonWidget_t button)
{
	return button.btnState;
}


////////////////////////////////////////////////
//draw box at coordinates of button filled with
//color.  valid colors are off or on colors
void ButtonWidget_SetButtonColor(ButtonWidget_t button, uint16_t color)
{
	if ((color == button.onColor) || (color == button.offColor))
	{
		LCD_DrawBox(1,
					button.xPosition,
					button.yPosition,
					button.xSize,
					button.ySize,
					color);
	}
}


