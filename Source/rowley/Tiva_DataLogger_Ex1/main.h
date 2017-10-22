#ifndef MAIN__H
#define MAIN__H


#define PART_TM4C123GH6PM


//__NO_SYSTEM_INIT


#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"     //uart driver
#include "inc/hw_gpio.h"     //gpio
#include "inc/hw_timer.h"    //pwm, timer!!
#include "inc/hw_ssi.h"    //spi
//#include "inc/hw_i2c.h"    //i2c light sensor
#include "inc/hw_types.h"    //?


#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"     //uart driver.
//#include "driverlib/udma.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"    //countdown timer
#include "driverlib/ssi.h"
//#include "driverlib/i2c.h"    //light sensor.
//#include "driverlib/pwm.h"    //pwm module
//#include "driverlib/adc.h"    //adc

#include <string.h>


//user defined driver files - board levels
#include "gpio_driver.h"
#include "uart_driver.h"
#include "timer_driver.h"
#include "spi_driver.h"
#include "lcd_driver.h"
#include "console.h"

#include "sdcard_driver.h"
//sensor driver files
#include "gpsGP20U7.h"


//FreeRTOS includes - user
#include "FreeRTOSHook.h"
#include "FreeRTOSTick.h"
#include "Priorities.h"
#include "RunTimeStats.h"

//FreeRTOS includes - Core
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//FreeRTOS Task includes
#include "LedTask.h"
#include "ComTask.h"
#include "ButtonTask.h"
#include "DisplayTask.h"

//state machine includes
#include "HSM_Menu.h"



#endif
