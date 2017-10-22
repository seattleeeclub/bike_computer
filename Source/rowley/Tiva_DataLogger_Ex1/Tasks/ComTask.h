/*
ComTask
Handles incoming communications from uart
*/

#ifndef COM_TASK__H
#define COM_TASK__H

//includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "console.h"        //PortType_t port


//defines
#define COM_MESSAGE_BUFFER_SIZE     64


typedef struct
 {
 	uint16_t size;
 	uint8_t buf[COM_MESSAGE_BUFFER_SIZE];
 	PortType_t port;
 }ComMessage;




//free rtos queue definition
extern QueueHandle_t ComQueue;

//variables for commanager, indices, etc
extern ComMessage comMessage;
extern uint16_t comRxIndex;




//function prototypes
void ComTask_Init(void);
void ComTask(void *pvParameters);



#endif
