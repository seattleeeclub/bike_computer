#ifndef UART_DRIVER__H
#define UART_DRIVER__H

#include "main.h"
#include "console.h"

#define UARTbase 	GPIO_PORTA_BASE
#define UART_txPin	GPIO_PIN_0
#define UART_rxPin	GPIO_PIN_1

void uart0_init(void);
void uart1_init(void);

void uart_print(PortType_t port, char* buf);
void uart_printArray(PortType_t port, uint8_t* buffer, uint32_t size);

void uart0_interruptHandler(void);
void uart1_interruptHandler(void);

void uart_processCommand(PortType_t port, uint8_t *buffer, uint32_t length);

//predefined uart0 isr and uart1 isr - defined in TM4C123.vec
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);




#endif
