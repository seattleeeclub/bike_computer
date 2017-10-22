/*
LED Task

*/

#ifndef LED_TASK__H
#define LED_TASK__H

//includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



typedef enum
{
	SIG_LED_DISPLAY_RED,
	SIG_LED_DISPLAY_GREEN,
	SIG_LED_DISPLAY_BLUE,
    SIG_LED_DISPLAY_PURPLE,
    SIG_LED_DISPLAY_YELLOW,
    SIG_LED_DISPLAY_WHITE,
    SIG_LED_DISPLAY_NONE,
}LedSignal_t;

typedef struct
{
    LedSignal_t signal;     //signal
}LedMessage;


//free rtos queue definition

extern QueueHandle_t LedQueue;


//function prototypes
void LedTask_Init(void);
void LedTask(void *pvParameters);



#endif
