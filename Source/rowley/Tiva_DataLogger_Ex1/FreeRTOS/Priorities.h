/*
Priorities.h
Assumes 8 total, 5 of which are reserved for the rtos
and 3 are for the hardware.  hardware level priorities
are 5 to 7, 7 being the lowest.
*/

#ifndef PRIORITIES_H
#define PRIORITIES_H

//RTOS
#define RTOS_PRIORITY_IDLE              (0)
#define RTOS_PRIORITY_LOW               (1)
#define RTOS_PRIORITY_MED               (2)
#define RTOS_PRIORITY_HIGH              (3)
#define RTOS_PRIORITY_REALTIME          (4)

//Hardware
#define HARDWARE_PRIORITY_LOW           ((uint8_t)(7 << 5))
#define HARDWARE_PRIORITY_MED           ((uint8_t)(6 << 5))
#define HARDWARE_PRIORITY_HIGH          ((uint8_t)(5 << 5))

#endif

