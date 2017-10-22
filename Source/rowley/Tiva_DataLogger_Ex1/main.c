/*
9/6/17
Tiva Launchpad FreeRTOS Implementation Project

The purpose of this project is to make a starting
point for other FreeRTOS Projects.  Contains a 
simple gpio_driver file that inits leds and
user buttons with interrupts.  

Board Peripherals initialized:

3 leds                  PF1 - PF3
2 user buttons          PF0 and PF4

FreeRTOS Configuration:
3 tasks running at different frequencies
Priority of all tasks set to 1
Hardware level interrupt priorities:
    SysTick = 7 - minimum
    GPIOF = 5 - maximum

Map the systick interrupt handler function to the port tick

port tick isr function declared as extern where the isr
function is registered so the compiler can see it
(ie, avoid changing port.c)

RTOS port is from GCC CM3.  CM4F does not work and gives
strange compile errors

Set the priority of all hardware interrupts
using the #defines in hw_ints.h

*/

///////////////////////////////////////////////////////////////////////
#include "main.h"


////////////////////////////////////////////////////////////////////////
//required for assert functions
void __error__(char *pcFilename, uint32_t ui32Line)
{
  //my error function
}


int
main(void)
{
    //configure clocks and systick
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    //configure board level items
    GPIO_init();            //LEDs     
    Button_init();          //PF0 and PF4 configured as interrupts
    ExternalButton_init();  //PA2, 3, 4 - orange user buttons
    Timer_init();           //1000hz timer used for sdcard_driver - NOTE: CHECK TO MAKE SURE NO CONFLICT WITH RTOS TICK
    SPI_init();             //SPI3, CS LCD and CS SDCARD
    LCD_Init();

    uart0_init();            //configure uart for cli
    uart1_init();            //configure uart for gps
    InitConsole();          //configure the cli console commands
    GPS_init();             //init the GPS fifo
    SD_Init();              //init SD card, put sd card idle state and mount drive

    //configure the timer for rtos tick
    FreeRTOSTick_Init();

    //configure systick timer for runtimestats
    RunTimeStats_Init();

    //FreeRTOS Tasks - Initialize tasks, queues, etc.
    LedTask_Init();
    ComTask_Init();
    ButtonTask_Init();
    DisplayTask_Init();

    //hsm init - start any state machines
    HSM_Menu_Init();

    //start the scheduler - never returns from here
    vTaskStartScheduler();

    return 0;
}





/////////////////////////////////////////////////
//Hook Functions
//Can't figure out how to get these seen and out 
//of main, so leave them here.  Remove the FreeRTOSHook.h/.c functions
//
//
#if (configUSE_MALLOC_FAILED_HOOK == 1)

////////////////////////////////////////////////////
//Hook functions - malloc failed
//
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */

	while(1){};

}
#endif


#if (configCHECK_FOR_STACK_OVERFLOW == 1)

///////////////////////////////////////////////////
//Hook functions - stack overflow
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	while(1){};

}



#endif
