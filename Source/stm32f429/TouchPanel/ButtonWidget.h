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
//////////////////////////////////////////////////////////

#ifndef __BUTTON_WIDGET_H
#define __BUTTON_WIDGET_H

#include <stdio.h>
#include <stdint.h>


#define DEFAULT_BUTTON_HEIGHT			80
#define DEFAULT_BUTTON_WIDTH			80

typedef enum
{
	BUTTON_STATE_OFF,
	BUTTON_STATE_ON,
}ButtonState_t;

typedef struct
{
	uint16_t index;					//0, 1, 2...
	ButtonState_t btnState;			//BUTTON_STATE_OFF, _ON
	uint16_t xPosition;				//offset from 0,0
	uint16_t yPosition;				//offset from 0,0
	uint16_t xSize;					//width of detected area
	uint16_t ySize;					//height of detected area
	uint16_t offColor;				//off color
	uint16_t onColor;				//on color
	void (*buttonHandler)(void);	//function to run

}ButtonWidget_t;


/////////////////////////////////////////////////
//Button Handler Functions - prototypes
void btnFunction1(void);
void btnFunction2(void);
void btnFunction3(void);
void btnFunction4(void);


void ButtonWidget_init(void);
void ButtonWidget_DrawButtons(uint16_t layer);
int ButtonWidget_GetButtonIndexFromPosition(uint16_t xPos, uint16_t yPos);
ButtonWidget_t ButtonWidget_GetButtonFromIndex(int index);
ButtonWidget_t ButtonWidget_GetButtonFromPosition(uint16_t xPos, uint16_t yPos);


void ButtonWidget_SetButtonState(ButtonWidget_t button, ButtonState_t state);
ButtonState_t ButtonWidget_GetButtonState(ButtonWidget_t button);
void ButtonWidget_SetButtonColor(ButtonWidget_t button, uint16_t color);



#endif
