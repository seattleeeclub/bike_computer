///////////////////////////////////////////////////////
/*
Dana Olcott 2017



*/
//////////////////////////////////////////////////////////

#ifndef __BUTTON_WIDGET_H
#define __BUTTON_WIDGET_H

#include <stdio.h>
#include <stdint.h>

//#include "queue.h"

#define DEFAULT_BUTTON_HEIGHT			80
#define DEFAULT_BUTTON_WIDTH			80


typedef struct
{
	uint16_t index;					//0, 1, 2...
	uint16_t xPosition;				//offset from 0,0
	uint16_t yPosition;				//offset from 0,0
	uint16_t xSize;					//width of detected area
	uint16_t ySize;					//height of detected area
	void (*buttonHandler)(void);	//function to run
}ButtonWidget_t;


//declare PanelQueue here so we can see it
//extern QueueHandle_t PanelQueue;



/////////////////////////////////////////////////
//Button Handler Functions - prototypes
void btnFunction1(void);
void btnFunction2(void);
void btnFunction3(void);
void btnFunction4(void);


int ButtonWidget_GetButtonIndexFromPosition(uint16_t xPos, uint16_t yPos);
ButtonWidget_t ButtonWidget_GetButtonFromIndex(int index);
ButtonWidget_t ButtonWidget_GetButtonFromPosition(uint16_t xPos, uint16_t yPos);




#endif
