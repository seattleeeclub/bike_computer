/*
RunTimeStats
Configures a timer to run without out interrupt
Uses RTOS API runtime stats functions
*/
#ifndef RUNTIMESTATS_H
#define RUNTIMESTATS_H

#include "FreeRTOS.h"



////////////////////////////////////////////////
//Recommended in FreeRTOS website
//
/* Variables used in the creation of the run time stats time base.  Run time 
stats record how much time each task spends in the Running state. */
extern long long llInitialRunTimeCounterValue;
extern long long llTicksPerHundedthMillisecond;


/*-----------------------------------------------------------*/
void RunTimeStats_Init(void);                       //called from main
void vConfigureTimerForRunTimeStats( void );        //RTOS call
unsigned long ulGetRunTimeCounterValue( void );     //RTOS call


#endif
