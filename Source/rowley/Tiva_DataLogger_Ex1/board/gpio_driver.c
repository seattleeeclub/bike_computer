///////////////////////////////////////////////////////////////////
/*
GPIO Driver function definitions
Configure LED and button pins
*/
//////////////////////////////////////////////////////////////////

#include "gpio_driver.h"
#include "Priorities.h"     //hardware interrupts
#include "ButtonTask.h"     //posting to button task

void GPIO_init()
{
       //configure ports - leds and buttons
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlDelay(3);		//about 320ns
    
    //configure ports - card cs, cmd, and rst pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    
    SysCtlDelay(3);		//about 320ns
                     
    //LEDs
    GPIOPinTypeGPIOOutput(LEDbase, LEDred|LEDblue|LEDgreen);
    GPIOPinWrite(LEDbase, LEDred | LEDblue | LEDgreen, 0x00);		//no matches = off

    //Control lines - PA5, PA6, PA7
    GPIOPinTypeGPIOOutput(CONTROL_PORT, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(CONTROL_PORT, GPIO_PIN_6);
    GPIOPinTypeGPIOOutput(CONTROL_PORT, GPIO_PIN_7);
 
    //set initial states
    GPIOPinWrite(CONTROL_PORT, CARD_CS_PIN, CARD_CS_PIN);       //high
    GPIOPinWrite(CONTROL_PORT, LCD_CMD_PIN, 0x00);              //low
    GPIOPinWrite(CONTROL_PORT, LCD_RESET_PIN, LCD_RESET_PIN);   //high 
  
}


////////////////////////////////////////////////////
//Button_init(void)
//Configure buttons on PF0 and PF4
//init buttons 1 and 2
void Button_init(void)
{
    //GPIOF enable
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);  // Init PF4 as input
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


    //set the priority for all port F.  If we are using RTOS, 
    //the max priority we can use is 5, the min is 7
    //RTOS consumes priorities 0 to 5.  if not using
    //RTOS, we can go all the way down to 0.  Priorities
    //are stored in the  top 3 bits
    IntPrioritySet(INT_GPIOF_TM4C123, HARDWARE_PRIORITY_LOW);

    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4);        // Disable interrupt for PF4 (in case it was enabled)
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);      // Clear pending interrupts for PF4
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);             // Configure PF4 for falling edge trigger
    GPIOIntRegister(GPIO_PORTF_BASE, Button_ISR);     // Register our handler function for port F

    //if you comment this out it won't work
//    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);       //i don't think you need this if you call intRegister...  leave it here anyway

    //PF0 - this one requires some unlocking
    //config
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    HWREG    (GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_M;

    //interrupts
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE, Button_ISR);
//    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);       //i don't think you need this if you call intRegister...  leave it here anyway


    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);       //i don't think you need this if you call intRegister...  leave it here anyway
}


////////////////////////////////////////////////////
//Button_External_Init(void)
//Configure buttons on PA2, PA3, and PA4
//Configure these as interrupts
void ExternalButton_init(void)
{
    //GPIOA enable
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);		//about 320ns

    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

    //PA2, PA3, PA4, pullup  enabled
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //disable interrupts
    GPIOIntDisable(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

    //clear pending interrupts
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_FALLING_EDGE);

    //set the priority of the interrupt - low - all portA
    IntPrioritySet(INT_GPIOA_TM4C123, HARDWARE_PRIORITY_LOW);

    //register handler functions for port a buttons
    GPIOIntRegister(GPIO_PORTA_BASE, ExternalButton_ISR);

    //I think this can be commented out
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);

}


/////////////////////////////////////////////////////
//ExternalButton_ISR(void)
//Interrupt routine for buttons on PA2, PA3, PA4
//If using FreeRTOS, posts a message to the ButtonTask
//user buttons are aligned on the right side of the device
//USB port facing up, board buttons aligned along the bottom.
//
void ExternalButton_ISR(void)
{
    ButtonMessage msg;
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    //PA2
    if (GPIOIntStatus(GPIO_PORTA_BASE, false) & GPIO_PIN_2)
    {
        //clear the interrupt - required or it won't leave.
        GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2);

        //post to button task
        msg.signal = SIG_BUTTON_UP;

        //post the message
        xQueueSendFromISR(ButtonQueue, &msg, &xHigherPriorityTaskWoken) != pdPASS;
    }

    //PA3
    else if (GPIOIntStatus(GPIO_PORTA_BASE, false) & GPIO_PIN_3)
    {
        //clear the interrupt - required or it won't leave.
        GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_3);

        //post to button task
        msg.signal = SIG_BUTTON_SELECT;

        //post the message
        xQueueSendFromISR(ButtonQueue, &msg, &xHigherPriorityTaskWoken) != pdPASS;

    }

    //PA4
    else if (GPIOIntStatus(GPIO_PORTA_BASE, false) & GPIO_PIN_4)
    {

        //clear the interrupt - required or it won't leave.
        GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_4);

        //post to button task
        msg.signal = SIG_BUTTON_DOWN;

        //post the message
        xQueueSendFromISR(ButtonQueue, &msg, &xHigherPriorityTaskWoken) != pdPASS;

    }

    //else, unknown interrupt source - clear all flags
    else
    {
        GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 |  GPIO_PIN_4);
    }
}







/////////////////////////////////////////////////////
//Button_ISR(void)
//Interrupt routine for buttons SW1 and SW2
//on PF4 and PF0.  Both interrupts are registered
//to the same function.  I tried doing separate functions
//but it didn't work. I'm wondering if these are on a 
//shared EXTI line.
//
//If using FreeRTOS, posts a message to the button task
//left and right - oriented such that usb port is 
//facing up, buttons along the bottom.
//
void Button_ISR(void)
{
    ButtonMessage msg;
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    //PF0
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0)
    {
        //clear the interrupt - required or it won't leave.
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);

        //post to button task
        msg.signal = SIG_BUTTON_RIGHT;

        //post the message
        xQueueSendFromISR(ButtonQueue, &msg, &xHigherPriorityTaskWoken) != pdPASS;


    }

    //PF4
    else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4)
    {
        //clear the interrupt - required or it won't leave.
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

        //post to button task
        msg.signal = SIG_BUTTON_LEFT;

        //post the message
        xQueueSendFromISR(ButtonQueue, &msg, &xHigherPriorityTaskWoken) != pdPASS;

    }

    //else, unknown interrupt source - clear all flags
    else
    {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    }
}





void LED_ToggleRed(void)
{
    uint8_t value = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) & GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~value);
}

void LED_ToggleGreen(void)
{
    uint8_t value = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) & GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, ~value);
}

void LED_ToggleBlue(void)
{
    uint8_t value = (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) & GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~value);
}

void LED_SetRed(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
}

void LED_SetGreen(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xFF);
}

void LED_SetBlue(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xFF);
}

void LED_ClearRed(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
}

void LED_ClearGreen(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
}

void LED_ClearBlue(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
}

void LED_DisplayRed(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
}

void LED_DisplayGreen(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
}

void LED_DisplayBlue(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_2);
}

void LED_DisplayWhite(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

void LED_DisplayNone(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
}

void LED_DisplayPurple(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1|GPIO_PIN_2);
}

void LED_DisplayYellow(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1|GPIO_PIN_3);
}







