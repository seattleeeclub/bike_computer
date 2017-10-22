/*
LedTask Definition
*/

///////////////////////////////////////////////////////////////////////
#include "LedTask.h"
#include "gpio_driver.h"

QueueHandle_t LedQueue;

////////////////////////////////////////////////////
//LedTask_Init
//Build the queue and task
void LedTask_Init()
{
    LED_DisplayNone();

    //FreeRTOS Tasks
    xTaskCreate(LedTask, (const portCHAR *)"led", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    //FreeRTOS Queue - LedMessage Structs
    LedQueue = xQueueCreate(10, sizeof(LedMessage));
}



//////////////////////////////////////////////////////
//LedTask
//Receiver Task - receives messages that 
//were forwarded on by the rx task  defined in main
void LedTask(void *pvParameters)
{
    LedMessage msg;

    for(;;)
    {
        if (pdPASS == xQueueReceive(LedQueue, &msg, portMAX_DELAY))
        {
            switch(msg.signal)
            {
                case SIG_LED_DISPLAY_RED:
                {
                    LED_DisplayRed();
                    break;
                }
                case SIG_LED_DISPLAY_BLUE:
                {
                    LED_DisplayBlue();
                    break;
                }
                case SIG_LED_DISPLAY_GREEN:
                {
                    LED_DisplayGreen();
                    break;
                }
                case SIG_LED_DISPLAY_PURPLE:
                {
                    LED_DisplayPurple();
                    break;
                }
                case SIG_LED_DISPLAY_YELLOW:
                {
                    LED_DisplayYellow();
                    break;
                }
                case SIG_LED_DISPLAY_WHITE:
                {
                    LED_DisplayWhite();
                    break;
                }
                case SIG_LED_DISPLAY_NONE:
                {
                    LED_DisplayNone();
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    //clean exit - delete this task if ever makes it here
    vTaskDelete(NULL);
}



