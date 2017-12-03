/*! \file packet.h
  \brief Packet handling library.
*/

#include<stdint.h>

//! Length of the packet buffer.
#define PACKETLEN 61

//! Receive packet buffer.
extern uint8_t rxbuffer[];
//! Transmit packet buffer.
extern uint8_t txbuffer[];



//! Switch to receiving packets.
void packet_rxon();

//! Stop receiving packets.
void packet_rxoff();

//! Transmit a packet.
void packet_tx(uint8_t *buffer, uint8_t length);

