/*
FreeRTOSTick Function Defintions

COnfigures a timer to timeout with interrupt
at rate of 1ms.  Registers FreeRTOS port tick
function as the ISR.  ISR function is prototyped 
and defined in port.c.  Declared as extern here
so we can see it.

Make a switch so we can use the systick or one of the 
timers.

For now, assume use Timer
*/


#include "main.h"

#include "FreeRTOSTick.h"
#include "Priorities.h"

/////////////////////////////////////////
//Init the timer that controls the port
//tick for RTOS.  For now, assume we're using
//Timer 1A.
//Note, this will need to get changed when
//we add the sd card
//
//Note:  Apparently you can't simply register the xPortSysTickHandler
//function in with the time interrupt directly and avoid definition of
//Timer1A ISR.  It keeps going into a hardfault.  so, remove the register
//function and run the timeout timer as usual.
//
void FreeRTOSTick_Init(void)
{
#if (RTOS_TIMER == RTOS_TIMER_TIMER0A)
    uint32_t timer0APeriod = (SysCtlClockGet() / 1000) - 1;      // period - SysCtlClockGet() divided by x, gives count for x Hz period
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);               // enable the timer peripherals
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);            // timer0A configured XX bit periodic
    TimerLoadSet(TIMER0_BASE, TIMER_A, timer0APeriod);
//    TimerIntRegister(TIMER0_BASE, TIMER_TIMA_TIMEOUT, xPortSysTickHandler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);            // interrupt setup for the timer0A timeouts
    IntPrioritySet(INT_TIMER0A, HARDWARE_PRIORITY_LOW);          //set timeout to lowest priority
    IntEnable(INT_TIMER0A);                                     // interrupt enabled for the timer0A
    TimerEnable(TIMER0_BASE, TIMER_A);                          // enable the timer0A

#elif (RTOS_TIMER == RTOS_TIMER_TIMER1A)
    uint32_t timer1APeriod = (SysCtlClockGet() / 1000) - 1;      // period - SysCtlClockGet() divided by x, gives count for x Hz period
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);               // enable the timer peripherals
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);            // timer1A configured 32 bit periodic
    TimerLoadSet(TIMER1_BASE, TIMER_A, timer1APeriod);
//    TimerIntRegister(TIMER1_BASE, TIMER_TIMA_TIMEOUT, xPortSysTickHandler);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);            // interrupt setup for the timer1A timeouts
    IntPrioritySet(INT_TIMER1A, HARDWARE_PRIORITY_LOW);          //set timeout to lowest priority
    IntEnable(INT_TIMER1A);                                     // interrupt enabled for the timer1A
    TimerEnable(TIMER1_BASE, TIMER_A);                          // enable the timer1A

#elif (RTOS_TIMER == RTOS_TIMER_TIMER2A)
    uint32_t timer2APeriod = (SysCtlClockGet() / 1000) - 1;      // period - SysCtlClockGet() divided by x, gives count for x Hz period
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);               // enable the timer peripherals
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);            // timer2A configured XX bit periodic
    TimerLoadSet(TIMER2_BASE, TIMER_A, timer2APeriod);
//    TimerIntRegister(TIMER2_BASE, TIMER_TIMA_TIMEOUT, xPortSysTickHandler);
    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);            // interrupt setup for the timer2A timeouts
    IntPrioritySet(INT_TIMER2A, HARDWARE_PRIORITY_LOW);          //set timeout to lowest priority
    IntEnable(INT_TIMER2A);                                     // interrupt enabled for the timer2A
    TimerEnable(TIMER2_BASE, TIMER_A);                          // enable the timer2A
#else
#error "RTOS_TIMER type needs to be defined.  See FreeRTOSTick.h"
#endif

}



////////////////////////////////////////////
//FreeRTOSTick_ISR
//Handles the  xPortSysTickHandler() in port.c
//Clears the appropriate flag and calls
//the port tick handler
//
void FreeRTOSTick_ISR(void)
{
    //clear the appropriate interrupt flag
    //based on which timer was defined
#if (RTOS_TIMER == RTOS_TIMER_TIMER0A)
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
#elif (RTOS_TIMER == RTOS_TIMER_TIMER1A)
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
#elif (RTOS_TIMER == RTOS_TIMER_TIMER2A)
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
#else
#error "RTOS_TIMER type needs to be defined.  See FreeRTOSTick.h"
#endif

    //call rtos port tick function
    xPortSysTickHandler();
}


/////////////////////////////////////////////////
//IRQ Handler functions  prototyped in TM4C123.vec
//Define the appropriate IRQ handler function
//based on which timer the RTOS is using
//
#if (RTOS_TIMER == RTOS_TIMER_TIMER0A)
void TIMER0A_IRQHandler(void)
{
    FreeRTOSTick_ISR();
}
#elif (RTOS_TIMER == RTOS_TIMER_TIMER1A)
void TIMER1A_IRQHandler(void)
{
    FreeRTOSTick_ISR();
}
#elif (RTOS_TIMER == RTOS_TIMER_TIMER2A)
void TIMER2A_IRQHandler(void)
{
    FreeRTOSTick_ISR();
}
#else
#error "RTOS_TIMER type needs to be defined.  See FreeRTOSTick.h"
#endif

