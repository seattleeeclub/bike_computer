/*
DisplayTask
*/

#ifndef DISPLAY_TASK__H
#define DISPLAY_TASK__H

//includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define LOG_FILE_STRING         ((const uint8_t*)"LOG")
#define FILE_NAME_BUFFER_LENGTH     20
#define FILE_NAME_INDEX_MIN         0
#define FILE_NAME_INDEX_MAX         10
#define FILE_NAME_INDEX_DEFAULT     FILE_NAME_INDEX_MIN


//DisplayItem - none, speed, position...
typedef enum 
{
	DISPLAY_ITEM_NONE = 0,
	DISPLAY_ITEM_SPEED = 1,
	DISPLAY_ITEM_POSITION = 2,

}DisplayItem_t;

typedef enum
{
    DATA_LOG_OFF,
    DATA_LOG_ON,

}DataLogState_t;

//function prototypes
void DisplayTask_Init(void);
void DisplayTask_SetDisplayItem(DisplayItem_t item);
DisplayItem_t DisplayTask_GetDisplayItem(void);
void DisplayTask_DisplayItemIncrease(void);
void DisplayTask_DisplayItemDecrease(void);

void DisplayTask_EnableDataLog(void);
void DisplayTask_DisableDataLog(void);
void DisplayTask_SetDataLog(DataLogState_t state);
DataLogState_t DisplayTask_GetDataLogState(void);

//increase/decrease the data log file, return the index, and load a buffer with file name
uint8_t DisplayTask_DataLogIndexGetCurrent(uint8_t* buffer);
uint8_t DisplayTask_DataLogIndexIncrease(uint8_t* buffer);
uint8_t DisplayTask_DataLogIndexDecrease(uint8_t* buffer);

void DisplayTask(void *pvParameters);



#endif
