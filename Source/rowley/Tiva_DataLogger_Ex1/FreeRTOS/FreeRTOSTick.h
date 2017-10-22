/*
FreeRTOSTick
Port Tick Handler for the FreeRTOS Tick
Configures Timer to interrupt at a rate of 1ms.
Maps FreeRTOS Tick to the timer.  

*/
#ifndef FREERTOS_TICK_H
#define FREERTOS_TICK_H

//timer definitions
#define RTOS_TIMER_TIMER0A      (1)
#define RTOS_TIMER_TIMER1A      (2)
#define RTOS_TIMER_TIMER2A      (3)
#define RTOS_TIMER              (RTOS_TIMER_TIMER0A)


////////////////////////////////////////
//Leave this here - it's accessed from port.c
//and need to call it in the isr
extern void xPortSysTickHandler(void);

//Prototypes
void FreeRTOSTick_Init(void);
void FreeRTOSTick_ISR(void);


#endif
