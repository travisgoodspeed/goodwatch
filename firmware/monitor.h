/*! \file monitor.h
  \brief Debugging commands, often received over UART.
*/


//! Handle a monitor command.
int monitor_handle(uint8_t *buffer, int len);

//! Handles packet arrival, like an application would.
void monitor_packetrx(uint8_t *packet, int len);
