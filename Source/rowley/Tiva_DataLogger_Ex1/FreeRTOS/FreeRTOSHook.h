/*
FreeRTOSHook

Function prototypes/definitions for stack overflow
and malloc failed.  See FreeRTOSConfig.h to check
if these are to be included in the project

*/
#ifndef FREERTOS_HOOK_H
#define FREERTOS_HOOK_H

//#include "main.h"

#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook(void);
#endif


#if (configCHECK_FOR_STACK_OVERFLOW == 1)
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
#endif







#endif
