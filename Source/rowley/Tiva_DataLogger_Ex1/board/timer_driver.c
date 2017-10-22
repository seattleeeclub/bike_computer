/*
Timer Driver
A simple countdown timer with interrupt
Tick rate is 1000hz.  
This is used for SD card timeout functions

*/


#include "timer_driver.h"
#include "Priorities.h"
//#include "sdcard_driver.h"        //comment out for now

//////////////////////////////////////////////
//Variables
uint32_t mTimer1Value = 0x00;

/////////////////////////////////////////////////
void Timer_init()
{

    uint32_t timer1APeriod = (SysCtlClockGet() / 1000) - 1;      // period - SysCtlClockGet() divided by x, gives count for x Hz period
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);               // enable the timer peripherals
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);            // timer1A configured 32 bit periodic
    TimerLoadSet(TIMER1_BASE, TIMER_A, timer1APeriod);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);            // interrupt setup for the timer1A timeouts
    IntPrioritySet(INT_TIMER1A, HARDWARE_PRIORITY_MED);
    IntEnable(INT_TIMER1A);                                     // interrupt enabled for the timer1A
    TimerEnable(TIMER1_BASE, TIMER_A);                          // enable the timer1A

    Timer_SetTimer1(0x00);

}

void Timer_ISR()
{
    //clear the interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    //decrement the timer value, continuous rollover
    mTimer1Value--;

    //function call to the sdcard driver file

//    disk_timerproc();

}


///////////////////////////////////////////
//Timer1 value - continuous countdown timer that
//that is always running.  To use, simply
//set the value using _SetTimer1(value)
//and poll in a while loop for a timeout
//Intended to be used with sd card functions
void Timer_SetTimer1(uint32_t timeout)
{
    mTimer1Value = timeout;
}


//////////////////////////////
//Current Value on the timeout
uint32_t Timer_GetTimer1()
{
    return mTimer1Value;
}



/////////////////////////////////////
//IRQ handler for Timer 1A
//Function is prototyped in TM4C123.vec
//Call Timer_ISR
//
void TIMER1A_IRQHandler(void)
{
    Timer_ISR();
}
