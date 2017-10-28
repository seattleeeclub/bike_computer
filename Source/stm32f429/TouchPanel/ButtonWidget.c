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

/////////////////////////////////////////////////////
//buttonWidgetList Definition
//index, x, y, sizeX, sizeY, functionToRun
const ButtonWidget_t buttonWidgetList[] =
{
	{0, 	0,		0, 		80, 	80,		btnFunction1},
	{1, 	80,		0, 		80, 	80,		btnFunction2},
	{2, 	160,	0, 		80, 	80,		btnFunction3},
	{3, 	0,		80,		240, 	240,	btnFunction4},		//remainder

	//use 0xFF as end of table id
	{(int)0xFF, 	(int)NULL, 	(int)NULL, 	(int)NULL, 	(int)NULL,	NULL},

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
	ButtonWidget_t button;

	return button;
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


