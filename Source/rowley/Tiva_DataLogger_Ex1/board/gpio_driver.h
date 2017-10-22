#ifndef GPIO_DRIVER__H
#define GPIO_DRIVER__H

#include "main.h"

#define LEDbase         GPIO_PORTF_BASE
#define LEDred          GPIO_PIN_1
#define LEDblue         GPIO_PIN_2
#define LEDgreen        GPIO_PIN_3

//pin defines for breakout board, PA5, 6, 7
#define CONTROL_PORT    GPIO_PORTA_BASE

#define CARD_CS_PIN     GPIO_PIN_5
#define LCD_CMD_PIN     GPIO_PIN_6
#define LCD_RESET_PIN   GPIO_PIN_7


//pin defines for buttons - PA2, PA3, PA4
//Orange buttons
#define BUTTON_EXT_PORT     GPIO_PORTA_BASE
#define BUTTON_EXT_UP       GPIO_PIN_2
#define BUTTON_EXT_SELECT   GPIO_PIN_3
#define BUTTON_EXT_DOWN     GPIO_PIN_4

//left and right on board buttons with usb facing up
//buttons along the bottom
#define BUTTON_PORT         GPIO_PORTF_BASE
#define BUTTON_LEFT         GPIO_PIN_4
#define BUTTON_RIGHT        GPIO_PIN_0

void GPIO_init(void);
void Button_init(void);             //PF0 and PF4
void Button_ISR(void);              //isr for buttons

void ExternalButton_init(void);     //user buttons 3
void ExternalButton_ISR(void);      //isr for buttons

void LED_ToggleRed(void);
void LED_ToggleBlue(void);
void LED_ToggleGreen(void);

void LED_SetRed(void);
void LED_SetBlue(void);
void LED_SetGreen(void);

void LED_ClearRed(void);
void LED_ClearBlue(void);
void LED_ClearGreen(void);

void LED_DisplayRed(void);
void LED_DisplayBlue(void);
void LED_DisplayGreen(void);
void LED_DisplayWhite(void);
void LED_DisplayNone(void);
void LED_DisplayPurple(void);
void LED_DisplayYellow(void);


#endif
