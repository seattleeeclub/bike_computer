#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include "main.h"


void Timer_init(void);

void Timer_ISR(void);

void Timer_SetTimer1(uint32_t timeout);
uint32_t Timer_GetTimer1(void);

#endif
