/*! \file monitor.h
  \brief Debugging commands, often received over UART.
*/


//! Handle a monitor command.
int monitor_handle(uint8_t *buffer, int len);

