/*! \file uart.h
  \brief UART Driver for the GoodWatch
*/

//! Set to 1 if the UART is active.
extern int uartactive;

//! UART buffer length.
#define UARTBUFLEN 64
//! UART receive buffer.
extern uint8_t uart_buffer[];


//! Initializes the UART if the host is listening.
void uart_init();

//! Transmit a byte to the UART.
void uart_tx(uint8_t byte);


