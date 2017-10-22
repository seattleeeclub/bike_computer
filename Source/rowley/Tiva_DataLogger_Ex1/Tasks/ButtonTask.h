/*
Button Task
Pupose of this task is to manage incoming
button clicks posted from button isr
*/

#ifndef BUTTON_TASK__H
#define BUTTON_TASK__H

//includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef struct
{ 
    uint16_t numClicks_up;
    uint16_t numClicks_down;
    uint16_t numClicks_left;
    uint16_t numClicks_right;
    uint16_t numClicks_select;
}ButtonStats;

typedef enum
{
	SIG_BUTTON_UP,
	SIG_BUTTON_DOWN,
	SIG_BUTTON_LEFT,
	SIG_BUTTON_RIGHT,
	SIG_BUTTON_SELECT,
}ButtonSignal_t;

typedef struct
 {
 	ButtonSignal_t signal;     //signal
 }ButtonMessage;


//free rtos queue definition

extern QueueHandle_t ButtonQueue;


//function prototypes
void ButtonTask_Init(void);
void ButtonTask(void *pvParameters);
ButtonStats ButtonTask_GetStats(void);
void ButtonTask_ClearStats(void);



#endif
