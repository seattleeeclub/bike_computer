/*
RunTimeStats
*/


#include "RunTimeStats.h"
#include "FreeRTOS.h"

//#include "main.h"

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"        //gpio
#include "inc/hw_timer.h"       //pwm, timer!!
#include "inc/hw_types.h"       //?


#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"    //countdown timer


long long llInitialRunTimeCounterValue = 0LL;
long long llTicksPerHundedthMillisecond = 800LL;


///////////////////////////////////////////////////////
//Not used but found as a way of setting the count
//on a timer.  
void TimerValueSet(uint32_t ulBase, uint32_t ulTimer, uint32_t ulValue)
{
    if(ulTimer & TIMER_A)
    {
        (*(uint32_t*)(HWREG(ulBase + TIMER_O_TAILR))) = ulValue;
    }

    if(ulTimer & TIMER_B)
    {
        (*(uint32_t*)(HWREG(ulBase + TIMER_O_TBILR))) = ulValue;
    }
}
 

//////////////////////////////////////////
//See page 141 in the programming manual, 
//setting the systick register count value
//STCURRENT - contains the current value of the systick
void SysTickValueSet(uint32_t value)
{
    uint32_t base = 0xE000E000;
    uint32_t offset = 0x018;
    uint32_t reg = base | offset;

    //temporarily disable the systick
    SysTickDisable();

    
    //cast to a pointer, deref it, and assign it.
    (*((volatile uint32_t*)reg)) = value;

    uint32_t counter = SysTickValueGet();

    //Enable the systick
    SysTickEnable();

    counter = SysTickValueGet();

}


//////////////////////////////////
//SysTick timer value should get reset
//on interrupt, so use this as a way of setting it
//
void RunTimeStatsInt(void)
{

}

////////////////////////////////////
//RunTimeStats
//Init the systick timer to track runtime stats
void RunTimeStats_Init(void)
{
    SysTickPeriodSet(80000000);      //rolls over at 100ms 8 ticks per ms - throws off the delays

    IntPrioritySet(FAULT_SYSTICK, (7 << 5));    //set to the lowest priority 7
    SysTickIntRegister(RunTimeStatsInt);
    SysTickIntEnable();
    SysTickEnable();
}




/*-----------------------------------------------------------*/
//////////////////////////////////////////////
//Set up a timer for run time stats
//for this case, let's not do anything since we'll
//use htim1 as the timebase

void vConfigureTimerForRunTimeStats( void )
{
     //set the global extern variable ticks per 100th ms
    llTicksPerHundedthMillisecond = 80;

    //clear the system clock counter
    SysTickValueSet(0);

    //assign
    llInitialRunTimeCounterValue = SysTickValueGet();

}

/*-----------------------------------------------------------*/
unsigned long ulGetRunTimeCounterValue( void )
{
    unsigned long liCurrentCount;
    unsigned long ulReturn;

    liCurrentCount = SysTickValueGet();

    //systick is a down counter
	ulReturn = ( unsigned long ) ( (llInitialRunTimeCounterValue - liCurrentCount ) / llTicksPerHundedthMillisecond );

	return ulReturn;
}
/*-----------------------------------------------------------*/
