/*
Button Task
Pupose of this task is to manage incoming
button clicks posted from button isr

*/

///////////////////////////////////////////////////////////////////////
#include "ButtonTask.h"
#include "gpio_driver.h"
#include "DisplayTask.h"
#include "HSM_Menu.h"           //for posting to hsm Menu
QueueHandle_t ButtonQueue;

//button clicks
uint16_t mNumClicks_Up = 0x00;
uint16_t mNumClicks_Down = 0x00;
uint16_t mNumClicks_Left = 0x00;
uint16_t mNumClicks_Right = 0x00;
uint16_t mNumClicks_Select = 0x00;


////////////////////////////////////////////////////
//ButtonTask_Init
//Build the queue and task
void ButtonTask_Init()
{
    ButtonTask_ClearStats();

    //FreeRTOS Tasks
    xTaskCreate(ButtonTask, (const portCHAR *)"button",
        2*configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    //FreeRTOS Queue - Queue of ButtonMessage structs
    ButtonQueue = xQueueCreate(6, sizeof(ButtonMessage));

}



//////////////////////////////////////////////////////
//ButtonTask
//Receiver Task - receives messages from user buttons
//located on board and external buttons
//Button presses are passed into the menu state machine
void ButtonTask(void *pvParameters)
{
    ButtonMessage msg;

    for(;;)
    {
        if (pdPASS == xQueueReceive(ButtonQueue, &msg, portMAX_DELAY))
        {
            //sit for a few ms
            vTaskDelay(10);

            //re-read the button based on the signal
            //if it's valid, then do something                        
            switch(msg.signal)
            {
                case SIG_BUTTON_UP:     //PA2
                {
                    if (!(GPIOPinRead(BUTTON_EXT_PORT, BUTTON_EXT_UP) & BUTTON_EXT_UP))
                    {
                        mNumClicks_Up++;
                        simplehsm_signal_current_state(&hsmMenu, SIG_HSM_MENU_UP, NULL);
                    }

                    break;
                }
                case SIG_BUTTON_DOWN:   //PA4
                {
                    if (!(GPIOPinRead(BUTTON_EXT_PORT, BUTTON_EXT_DOWN) & BUTTON_EXT_DOWN))
                    {
                        mNumClicks_Down++;
                        simplehsm_signal_current_state(&hsmMenu, SIG_HSM_MENU_DOWN, NULL);
                    }
                    break;
                }
                case SIG_BUTTON_LEFT:   //PF4
                {
                    if (!(GPIOPinRead(BUTTON_PORT, BUTTON_LEFT) & BUTTON_LEFT))
                    {
                        mNumClicks_Left++;
                        simplehsm_signal_current_state(&hsmMenu, SIG_HSM_MENU_LEFT, NULL);
                    }
                    break;
                }
                case SIG_BUTTON_RIGHT:  //PF0
                {
                    if (!(GPIOPinRead(BUTTON_PORT, BUTTON_RIGHT) & BUTTON_RIGHT))
                    {
                        mNumClicks_Right++;
                        simplehsm_signal_current_state(&hsmMenu, SIG_HSM_MENU_RIGHT, NULL);
                    }
                    break;
                }
                case SIG_BUTTON_SELECT: //PA3
                {
                    if (!(GPIOPinRead(BUTTON_EXT_PORT, BUTTON_EXT_SELECT) & BUTTON_EXT_SELECT))
                    {
                        mNumClicks_Select++;
                        simplehsm_signal_current_state(&hsmMenu, SIG_HSM_MENU_SELECT, NULL);
                    }

                    break;
                }                
            }
        }
    }

    //clean exit
    vTaskDelete(NULL);
}

//////////////////////////////////////////
ButtonStats ButtonTask_GetStats(void)
{
    ButtonStats stats;
    stats.numClicks_up = mNumClicks_Up;
    stats.numClicks_down = mNumClicks_Down;
    stats.numClicks_left = mNumClicks_Left;
    stats.numClicks_right = mNumClicks_Right;
    stats.numClicks_select = mNumClicks_Select;
    return stats;
}


////////////////////////////////////////
void ButtonTask_ClearStats(void)
{
    mNumClicks_Up = 0x00;
    mNumClicks_Down = 0x00;
    mNumClicks_Left = 0x00;
    mNumClicks_Right = 0x00;
    mNumClicks_Select = 0x00;
}

