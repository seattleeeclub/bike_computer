//////////////////////////////////////////////
/*
HSM Menu
*/
//////////////////////////////////////////////


#ifndef __HSM_MENU_H
#define __HSM_MENU_H

#include <stdio.h>

#include "simplehsm.h"


//Menu Signals
//On, Off, User Buttons (up, down, left, right, select)

typedef enum 
{
    SIG_HSM_MENU_OFF = SIG_USER,
    SIG_HSM_MENU_ON,
    SIG_HSM_MENU_UP,
    SIG_HSM_MENU_DOWN,
    SIG_HSM_MENU_LEFT,
    SIG_HSM_MENU_RIGHT,
    SIG_HSM_MENU_SELECT,
}HSM_MenuSignal_t;

//menu items
typedef enum
{
    MENU_ITEM_LOG_FILE = 0,         //make a new log file
    MENU_ITEM_LOG_ON = 1,           //enable the data logger
    MENU_ITEM_LOG_OFF = 2,          //disable data logger
    MENU_ITEM_DISPLAY_SPEED = 3,    //display speed on LCD
    MENU_ITEM_DISPLAY_POSITION = 4, //display position on LCD

}HSM_MenuItem_t;

//state machine
extern simplehsm_t hsmMenu;

//functions - init, display menu items, etc.
void HSM_Menu_Init(void);
void HSM_MenuItemIncrease(void);
void HSM_MenuItemDecrease(void);
void HSM_Menu_DisplayMenuItem(HSM_MenuItem_t item);


//states
stnext MenuState_TopState(int signal, void* param);
stnext MenuState_Off(int signal, void* param);
stnext MenuState_On(int signal, void* param);

stnext MenuState_LogFile(int signal, void* param);


#endif