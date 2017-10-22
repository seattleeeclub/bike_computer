/*
ComTask Definition
*/

///////////////////////////////////////////////////////////////////////
#include "ComTask.h"
#include "uart_driver.h"

QueueHandle_t ComQueue;
ComMessage comMessage;
uint16_t comRxIndex;

////////////////////////////////////////////////////
//ComTask_Init
//Build the queue and task
void ComTask_Init()
{
    //FreeRTOS Tasks
    xTaskCreate(ComTask, (const portCHAR *)"com", (2*configMINIMAL_STACK_SIZE), NULL, 1, NULL);

    //FreeRTOS Queue - ComMessage Structs
    ComQueue = xQueueCreate(10, sizeof(ComMessage));

    //reset / init the comMessage
    comMessage.size = 0x00;
    comMessage.port = PORT_UART_0;
    memset(comMessage.buf, 0x00, COM_MESSAGE_BUFFER_SIZE);

    comRxIndex = 0;
}



//////////////////////////////////////////////////////
//ComTask
//Receiver Task - receives messages posted from 
//uart isr.  Message contains a size, message buffer
//and the source of the message.  Process and execute the
//command.
//
void ComTask(void *pvParameters)
{
    ComMessage msg;

    for(;;)
    {
        if (pdPASS == xQueueReceive(ComQueue, &msg, portMAX_DELAY))
        {
            uint16_t size = msg.size;
            if (size > 0)
            {
                //pass it to the uart_process command function
                if ((msg.port == PORT_UART_0) || (msg.port == PORT_UART_1))
                {
                    uart_processCommand(msg.port, msg.buf, msg.size);
                }
                else
                {
                    //invalid port
                }
            }
        }
    }

    //clean exit
    vTaskDelete(NULL);
}



