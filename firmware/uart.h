/*! \file uart.h
  \brief UART Driver for the GoodWatch
*/

//! Set to 1 if the UART is active.
extern int uartactive;

//! Initializes the UART if the host is listening.
void uart_init();

//! Transmit a byte to the UART.
void uart_tx(uint8_t byte);


