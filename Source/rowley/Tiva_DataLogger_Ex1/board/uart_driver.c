/*
Configures the UART0 peripheral on PA0 and PA1
on the Tiva Launchpad.  Configured with interrupt handler,
all commands / data terminated with \n to 
trigger the command process function.


*/

#include "uart_driver.h"
#include "console.h"
#include "Priorities.h"



//set up a a double buffer for flip flop
#define RX_BUFFER_SIZE    128
//uart0 - built in usb connection
uint32_t uart0RxIndex = 0x00;
uint8_t uart0activeRxBuffer = 1;
uint8_t uart0rxBuffer1[RX_BUFFER_SIZE];
uint8_t uart0rxBuffer2[RX_BUFFER_SIZE];

//uart1 - gps input
uint32_t uart1RxIndex = 0x00;
uint8_t uart1activeRxBuffer = 1;
uint8_t uart1rxBuffer1[RX_BUFFER_SIZE];
uint8_t uart1rxBuffer2[RX_BUFFER_SIZE];


void uart0_init()
{
    //configure ports, use uart0 on pa0 and pa1    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);    
    SysCtlDelay(3);		

    //pin configure
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //config the uart
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, 
        (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

    //config the fifos
    //1_8 = 1/8 full, or triggers when it sees 2 characters.
    //if you disable the fifo, it triggers
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX4_8, UART_FIFO_RX1_8);
//    UARTFIFOEnable(UART0_BASE);
    UARTFIFODisable(UART0_BASE);

    
    //interrupt handler register
    IntMasterEnable();
    IntPrioritySet(INT_UART0, HARDWARE_PRIORITY_MED);
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX);

    //variables
    uart0RxIndex = 0x00;
    uart0activeRxBuffer = 1;
    memset(uart0rxBuffer1, 0x00, RX_BUFFER_SIZE);
    memset(uart0rxBuffer2, 0x00, RX_BUFFER_SIZE);    

}


void uart1_init()
{
    //configure ports, use uart1 on PC4 and PC5 (rx,tx)    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);    
    SysCtlDelay(3);		

    //pin configure
    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //configure the uart1 - 9600
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, 
        (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

    //config the fifos
    //1_8 = 1/8 full, or triggers when it sees 2 characters.
    //if you disable the fifo, it triggers
    UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX1_8);
//    UARTFIFOEnable(UART1_BASE);
    UARTFIFODisable(UART1_BASE);
    
    //interrupt handler register
    IntMasterEnable();
    IntPrioritySet(INT_UART1, HARDWARE_PRIORITY_MED);
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX);

    //variables
    uart1RxIndex = 0x00;
    uart1activeRxBuffer = 1;
    memset(uart1rxBuffer1, 0x00, RX_BUFFER_SIZE);
    memset(uart1rxBuffer2, 0x00, RX_BUFFER_SIZE);    

}



void uart_print(PortType_t port, char* buf)
{
    uint32_t counter = 0x00;
    while(buf[counter] != '\0')
    {
        if (port == PORT_UART_0)
        {
            UARTCharPut(UART0_BASE, buf[counter]);
        }
        else if (port == PORT_UART_1)
        {
            UARTCharPut(UART1_BASE, buf[counter]);
        }

        counter++;
    }
}


void uart_printArray(PortType_t port, uint8_t* buffer, uint32_t size)
{
    for (int i = 0 ; i < size ; i++)
    {
        if (port == PORT_UART_0)
        {
            UARTCharPut(UART0_BASE, buffer[i]);
        }

        else if (port == PORT_UART_1)
        {
            UARTCharPut(UART1_BASE, buffer[i]);
        }
    }
}


///////////////////////////////////////
//prototyped in TM4C123.vec.  This should also
//be defined in the startup files.  You'd need to 
//do this differently in keil or ccs.  funny that 
//it takes several its to get this to trigger.  
//might have to set the fifo to a smaller value?

void UART0_IRQHandler(void)
{
  uart0_interruptHandler();
}


///////////////////////////////////////
//prototyped in TM4C123.vec.  This should also
//be defined in the startup files.  You'd need to 
//do this differently in keil or ccs.  funny that 
//it takes several its to get this to trigger.  
//might have to set the fifo to a smaller value?

void UART1_IRQHandler(void)
{
  uart1_interruptHandler();
}



void uart0_interruptHandler(void)
{
    unsigned long status;

    status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, status);

    //read the chars from the fifo - the fifo is
    //disabled, so it fires everytime.  
    //get the char and put it in an array
    //for processing
    while (UARTCharsAvail(UART0_BASE))
    {
        char c = UARTCharGet(UART0_BASE);

        if ((uart0RxIndex < RX_BUFFER_SIZE - 1) && (c > 0))
        {
            if (uart0activeRxBuffer == 1)
                uart0rxBuffer1[uart0RxIndex] = c;
            else
                uart0rxBuffer2[uart0RxIndex] = c;

            uart0RxIndex++;

            //eval the char
            if (c == '\n')
            {
                 if (uart0activeRxBuffer == 1)
                 {
                    uart0rxBuffer1[uart0RxIndex] = '\0';
                    uart_processCommand(PORT_UART_0, uart0rxBuffer1, uart0RxIndex);
                    uart0activeRxBuffer = 2;           //flip
                    uart0RxIndex = 0x00;               //reset
                 }
                 else
                 {
                    uart0rxBuffer2[uart0RxIndex] = '\0';
                    uart_processCommand(PORT_UART_0, uart0rxBuffer2, uart0RxIndex);
                    uart0activeRxBuffer = 1;           //flip
                    uart0RxIndex = 0x00;               //reset
                 }
            }
        }

        //else, char over run, reset everything
        else
        {
            uart0RxIndex = 0x00;
            memset(uart0rxBuffer1, 0x00, RX_BUFFER_SIZE);
            memset(uart0rxBuffer2, 0x00, RX_BUFFER_SIZE);
            uart0activeRxBuffer = 1;
        }   
    }
}


///////////////////////////////////////////
//UART1 on PC4 and PC5
void uart1_interruptHandler(void)
{
    unsigned long status;

    status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, status);

    //read the chars from the fifo - the fifo is
    //disabled, so it fires everytime.  
    //get the char and put it in an array
    //for processing
    while (UARTCharsAvail(UART1_BASE))
    {
        char c = UARTCharGet(UART1_BASE);

        if ((uart1RxIndex < RX_BUFFER_SIZE - 1) && (c > 0))
        {
            if (uart1activeRxBuffer == 1)
                uart1rxBuffer1[uart1RxIndex] = c;
            else
                uart1rxBuffer2[uart1RxIndex] = c;

            uart1RxIndex++;

            //eval the char
            if (c == '\n')
            {
                 if (uart1activeRxBuffer == 1)
                 {
                    uart1rxBuffer1[uart1RxIndex] = '\0';
                    uart_processCommand(PORT_UART_1, uart1rxBuffer1, uart1RxIndex);
                    uart1activeRxBuffer = 2;           //flip
                    uart1RxIndex = 0x00;               //reset
                 }
                 else
                 {
                    uart1rxBuffer2[uart1RxIndex] = '\0';
                    uart_processCommand(PORT_UART_1, uart1rxBuffer2, uart1RxIndex);
                    uart1activeRxBuffer = 1;           //flip
                    uart1RxIndex = 0x00;               //reset
                 }
            }
        }

        //else, char over run, reset everything
        else
        {
            uart1RxIndex = 0x00;
            memset(uart1rxBuffer1, 0x00, RX_BUFFER_SIZE);
            memset(uart1rxBuffer2, 0x00, RX_BUFFER_SIZE);
            uart1activeRxBuffer = 1;
        }   
    }
}

////////////////////////////////////////////////////////////
//Pass the source into the function, port
//so it knows where it came from.  pointer and a length
//
void uart_processCommand(PortType_t port, uint8_t *buffer, uint32_t length)
{
    //command line variables
    int argc;
    char *argv[ARG_BUFFER_SIZE];

    //load the received command string into cmdBuffer
    //remove all end of line chars
    memset(argv, 0, sizeof(argv));
    for (int i = 0 ; i < length ; i++)
    {
            char c = (char)buffer[i];
            //test for a new line or return
            if ((c == '\r') || (c == '\n'))
                c = 0x00;

            cmdBuffer[i] = c;
    }

    //split up into arguments
    ParseArgs(&argc, argv);

    if (argc > 0)
    {
        //usd - route to the cli
        if (port == PORT_UART_0)
            ExecCmd(port, argc, argv);             

        //port1 - gps.  Route only $GPRMC header
        //Reuse the cli command parser and handler functions
        else if (port == PORT_UART_1)
        {
            //exe only the header command
            if (!strcmp(argv[0], "$GPRMC"))
            {
                ExecCmd(port, argc, argv);             
            }
        }

        else
        {
            //error - don't know how it got here.
        }
    }
}

