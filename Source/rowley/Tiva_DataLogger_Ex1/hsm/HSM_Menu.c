///////////////////////////////////////////////
/*
HSM Menu

example how to transition state
 simplehsm_transition_state(&hsmMenu, MenuState_MyState);

*/
//////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>

#include "HSM_Menu.h"
#include "uart_driver.h"
#include "console.h"            //port defintions

/////////////////////////////////////////////////////
//state machine - declared as extern so the button task
//can access it (up, down, left, right, select)
//
simplehsm_t hsmMenu;

HSM_MenuItem_t hsmMenuItem;     //for display various menu items
DisplayItem_t displayItem;      //display item none, speed, position
DataLogState_t loggerState;     //data logger state for enter/exit
uint8_t fileNameBuffer[FILE_NAME_BUFFER_LENGTH];           //buffer for loading log file name "log34.txt"
uint8_t fileIndex;              //file index - combine to build a file name

void HSM_Menu_Init(void)
{
    hsmMenuItem = MENU_ITEM_LOG_FILE;           //set on entry into menu on state
    displayItem = DISPLAY_ITEM_SPEED;            //set on entry into menu on state
    loggerState = DATA_LOG_OFF;                 //data logger off
    memset(fileNameBuffer, 0x00, FILE_NAME_BUFFER_LENGTH);
    fileIndex = 0;

    simplehsm_initialize(&hsmMenu, MenuState_TopState);
}


void HSM_MenuItemIncrease(void)
{
    if (hsmMenuItem < MENU_ITEM_DISPLAY_POSITION)
        hsmMenuItem++;
    else
        hsmMenuItem = MENU_ITEM_LOG_FILE;
}

void HSM_MenuItemDecrease(void)
{
    if (hsmMenuItem > MENU_ITEM_LOG_FILE)
        hsmMenuItem--;
    else
        hsmMenuItem = MENU_ITEM_DISPLAY_POSITION;
}



void HSM_Menu_DisplayMenuItem(HSM_MenuItem_t item)
{
    uint8_t buf[30];
    LCD_Clear(BLACK);

    switch(item)
    {
        case MENU_ITEM_LOG_FILE:
        {
            int n = sprintf(buf, "Log File Name");
            LCD_DrawString88KernLength(3, 1, buf, n);
            
            break;
        }
        case MENU_ITEM_LOG_ON:
        {
            int n = sprintf(buf, "Enable Data Logger");
            LCD_DrawString88KernLength(3, 1, buf, n);

            break;
        }
        case MENU_ITEM_LOG_OFF:
        {
            int n = sprintf(buf, "Disable Data Logger");
            LCD_DrawString88KernLength(3, 1, buf, n);

            break;
        }
        case MENU_ITEM_DISPLAY_SPEED:
        {
            int n = sprintf(buf, "Display Speed");
            LCD_DrawString88KernLength(3, 1, buf, n);

            break;
        }
        case MENU_ITEM_DISPLAY_POSITION:
        {
            int n = sprintf(buf, "Display Position");
            LCD_DrawString88KernLength(3, 1, buf, n);

            break;
        }
    }
}


///////////////////////////////////////////
//MenuState_TopState
//All States are nested in TopState
stnext MenuState_TopState(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            uart_print(PORT_UART_0, "MenuState_TopState: SIG_ENTRY\r\n");
            return stnone;
        case SIG_INIT:
            simplehsm_init_transition_state(&hsmMenu, MenuState_Off);
            return stnone;
        case SIG_EXIT:
            uart_print(PORT_UART_0, "MenuState_TopState: SIG_EXIT\r\n");
            return stnone;
    }

    return stnone;
}


////////////////////////////////////////
//MenuState_Off
//Display Task should be showing something
//other than none, the display task is showing
//something
stnext MenuState_Off(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_ENTRY\r\n");

            //resume display item
            LCD_Clear(BLACK);                           //clear any text
            DisplayTask_SetDisplayItem(displayItem);    //resume the display item
            DisplayTask_SetDataLog(loggerState);        //resume the log state

            return stnone;
        case SIG_INIT:
            return stnone;
        case SIG_EXIT:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_EXIT\r\n");
            return stnone;
        case SIG_HSM_MENU_OFF:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_OFF\r\n");
            return stnone;
        case SIG_HSM_MENU_ON:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_ON\r\n");
            return stnone;
        case SIG_HSM_MENU_UP:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_UP\r\n");
            return stnone;
        case SIG_HSM_MENU_DOWN:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_DOWN\r\n");
            return stnone;
        case SIG_HSM_MENU_LEFT:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_LEFT\r\n");
            return stnone;
        case SIG_HSM_MENU_RIGHT:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_RIGHT\r\n");
            return stnone;
        case SIG_HSM_MENU_SELECT:
            uart_print(PORT_UART_0, "MenuState_Off: SIG_HSM_MENU_SELECT\r\n");

            simplehsm_transition_state(&hsmMenu, MenuState_On);

            return stnone;
    }

    return (stnext)MenuState_TopState;
}






////////////////////////////////////////////////
//MenuState_On
//Display a list of menu items.  Display task
//should be either suspended or in a mode where
//although it might be running, it does not 
//produce any change to the display content.
//
stnext MenuState_On(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            uart_print(PORT_UART_0, "MenuState_On: SIG_ENTRY\r\n");

            //disable the current display item
            displayItem = DisplayTask_GetDisplayItem();     //store previous item
            DisplayTask_SetDisplayItem(DISPLAY_ITEM_NONE);  //disable the display

            //stop the data collection, record state before menu on
            loggerState = DisplayTask_GetDataLogState();
            DisplayTask_DisableDataLog();

            LCD_Clear(BLACK);            
            hsmMenuItem = MENU_ITEM_LOG_FILE;
            HSM_Menu_DisplayMenuItem(hsmMenuItem);

            return stnone;
        case SIG_INIT:
            return stnone;
        case SIG_EXIT:
            uart_print(PORT_UART_0, "MenuState_On: SIG_EXIT\r\n");
            return stnone;
        case SIG_HSM_MENU_OFF:
            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_OFF\r\n");
            return stnone;
        case SIG_HSM_MENU_ON:
            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_ON\r\n");
            return stnone;
        case SIG_HSM_MENU_UP:
            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_UP\r\n");
            
            HSM_MenuItemIncrease();
            HSM_Menu_DisplayMenuItem(hsmMenuItem);

            return stnone;
        case SIG_HSM_MENU_DOWN:

            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_DOWN\r\n");

            HSM_MenuItemDecrease();
            HSM_Menu_DisplayMenuItem(hsmMenuItem);

            return stnone;
        case SIG_HSM_MENU_LEFT:
            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_LEFT\r\n");
            return stnone;
        case SIG_HSM_MENU_RIGHT:
            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_RIGHT\r\n");
            return stnone;
        case SIG_HSM_MENU_SELECT:
            uart_print(PORT_UART_0, "MenuState_On: SIG_HSM_MENU_SELECT\r\n");

            switch(hsmMenuItem)
            {
                case MENU_ITEM_LOG_FILE:
                {
                    //transition into a display the current log
                    //file state, where up/down change index tagged to end of file name
                    simplehsm_transition_state(&hsmMenu, MenuState_LogFile);                    
                    break;
                }
                case MENU_ITEM_LOG_ON:
                {
                    //set the variable, enables/disables on entry into menu off
                    loggerState = DATA_LOG_ON;
                    simplehsm_transition_state(&hsmMenu, MenuState_Off);
                    break;
                }
                case MENU_ITEM_LOG_OFF:
                {
                    //set the variable, enables/disables on entry into menu off
                    loggerState = DATA_LOG_OFF;
                    simplehsm_transition_state(&hsmMenu, MenuState_Off);
                    break;
                }
                case MENU_ITEM_DISPLAY_SPEED:
                {
                    //set to display speed
                    displayItem = DISPLAY_ITEM_SPEED;
                    simplehsm_transition_state(&hsmMenu, MenuState_Off);
                    break;
                }
                case MENU_ITEM_DISPLAY_POSITION:
                {
                    displayItem = DISPLAY_ITEM_POSITION;
                    simplehsm_transition_state(&hsmMenu, MenuState_Off);
                    break;
                }
            }

            return stnone;
    }

    return (stnext)MenuState_TopState;
}



///////////////////////////////////////////
//Display The Current log file.  Up/down
//change the title on the log  file
//select transitions to off
stnext MenuState_LogFile(int signal, void* param)
{
    uint8_t buf[20];
    int n;
    uint8_t length;

    switch (signal)
    {
        case SIG_ENTRY:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_ENTRY\r\n");

            //resume display item
            LCD_Clear(BLACK);                           //clear any text

            //get the log file name - current
            length = DisplayTask_DataLogIndexGetCurrent(fileNameBuffer);

            n = sprintf(buf, "%s", "LOG FILE:");
            LCD_DrawString88KernLength(2, 1, buf, n);
            LCD_DrawString88KernLength(4, 1, fileNameBuffer, length);

            return stnone;
        case SIG_INIT:
            return stnone;
        case SIG_EXIT:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_EXIT\r\n");

            return stnone;
        case SIG_HSM_MENU_OFF:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_OFF\r\n");
            return stnone;
        case SIG_HSM_MENU_ON:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_ON\r\n");
            return stnone;
        case SIG_HSM_MENU_UP:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_UP\r\n");

            //increase the log file number and set in display task
            length = DisplayTask_DataLogIndexIncrease(fileNameBuffer);

            //display it on the screen
            n = sprintf(buf, "%s", "LOG FILE ->");
            LCD_DrawString88KernLength(2, 1, buf, n);

            n = sprintf(buf, "%s  ", fileNameBuffer);
            LCD_DrawString88KernLength(4, 1, buf, n);

            return stnone;
        case SIG_HSM_MENU_DOWN:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_DOWN\r\n");

            //decrease the log file number and set in display task
            length = DisplayTask_DataLogIndexDecrease(fileNameBuffer);

            //display it on the screen
            n = sprintf(buf, "%s", "LOG FILE:");
            LCD_DrawString88KernLength(2, 1, buf, n);
            LCD_DrawString88KernLength(4, 1, fileNameBuffer, length);

            return stnone;
        case SIG_HSM_MENU_LEFT:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_LEFT\r\n");
            return stnone;
        case SIG_HSM_MENU_RIGHT:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_RIGHT\r\n");
            return stnone;
        case SIG_HSM_MENU_SELECT:
            uart_print(PORT_UART_0, "MenuState_LogFile: SIG_HSM_MENU_SELECT\r\n");

            simplehsm_transition_state(&hsmMenu, MenuState_Off);

            return stnone;
    }

    return (stnext)MenuState_TopState;
}


