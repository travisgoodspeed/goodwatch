#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//#include <cc430x613x.h>

#include "packet.h"
#include "debug.h"
#include "radio.h"
#include "flash.h"


#define st(x)                                           do { x } while (__LINE__ == -1)
#define ENTER_CRITICAL_SECTION(x)                       st( x = __get_interrupt_state(); __disable_interrupt(); )
#define EXIT_CRITICAL_SECTION(x)                        __set_interrupt_state(x)

#define RAM_IVT_ADDRESS                                 0x2b80

// SIMULATES FLASHING ( doesn't touch flash )
#define TEST_MODE 0

enum STATE {
	STATE_LISTEN,
	STATE_TRANSFER,
	STATE_FINISHED,
};

static enum STATE state = STATE_LISTEN;

enum PKT_TYPE {
	PKT_TYPE_INIT   = 0,
	PKT_TYPE_DATA   = 1,
	PKT_TYPE_REBOOT = 2,
	PKT_TYPE_LOG    = 0xf0,
	PKT_TYPE_RESET  = 0xf1,
	PKT_TYPE_BUSY   = 0xfa,
	PKT_TYPE_DENIED = 0xfb,
	PKT_TYPE_UNK    = 0xfc,
	PKT_TYPE_OOO    = 0xfd,
	PKT_TYPE_ERR    = 0xfe,
	PKT_TYPE_OK     = 0xff,
};

typedef struct {
	uint8_t len;

	enum PKT_TYPE type;

	union {
		struct {
			uint16_t num_packets;
		} init;

		struct {
			uint16_t nr;
			uint16_t address;
			uint8_t data[56];
		} __attribute__ ((packed)) data;

#if DEBUG
		struct {
			uint8_t data[60];
		} log;
#endif

		uint8_t raw_data[60];
	};

} __attribute__ ((packed, aligned(2))) packet_t;


static packet_t pkt = {0};

static uint16_t pkts_tot = 0;
static uint16_t pkt_num = 0;
static bool ready_to_send = true;

void packet_ack(enum PKT_TYPE type, int len){
	if (!ready_to_send)
		return;

	pkt.len = len;
	pkt.type = type;

	// packet_rxoff();
	// __delay_cycles(8500); // TODO: needed?
	ready_to_send = false;
	packet_tx((uint8_t *) &pkt, len + 1);
}

void packet_rx(uint8_t *buf, int len){

	// UCSCTL4 = SELM_3 + SELS_0 + SELA_0; // step up
	packet_rxoff();
	__delay_cycles(8500); // TODO: needed?

	packet_t *packet = (packet_t *) buf;

	enum PKT_TYPE rettype = PKT_TYPE_ERR;
	int pkt_len  = 1;

	switch(packet->type){

	case PKT_TYPE_INIT:
		if (state == STATE_LISTEN){
			L("init");
			pkts_tot = packet->init.num_packets;
			pkt_num = 0;

#if !TEST_MODE
			flash_erase();
#endif
			state = STATE_TRANSFER;
			rettype = PKT_TYPE_OK;
		} else {
			L("bsy");
			rettype = PKT_TYPE_BUSY;
		}
		break;

	case PKT_TYPE_DATA:
		if (state == STATE_TRANSFER){

			if (packet->data.nr == pkt_num + 1 && packet->data.nr <= pkts_tot){

#if !TEST_MODE
				flash_unlock();
				bool success = flash_write(packet->data.address, packet->data.data, packet->len - 5);
				flash_lock();
#else
				bool success = true;
#endif

				if (success){
					// L("written");
					rettype = PKT_TYPE_OK;
					pkt_num++;
				} else {
					L("faild");
				}

				if (packet->data.nr == pkts_tot) {
					L("done");
					rettype = PKT_TYPE_OK;
					state = STATE_FINISHED;

					// reset mcu
				}
			} else {
				L("ooo");
				rettype = PKT_TYPE_OOO;
			}
		} else {
			rettype = PKT_TYPE_DENIED;
		}
		break;

#if DEBUG
	case PKT_TYPE_LOG:
        memcpy(pkt.log.data, (char*) DEBUG_LOG_ADDRESS, 60);
        pkt_len = 60 + 1;
		rettype = PKT_TYPE_OK;
		break;
#endif

	case PKT_TYPE_RESET:
		L("rst");
		state = STATE_LISTEN;
		rettype = PKT_TYPE_OK;
		break;

	case PKT_TYPE_REBOOT:
		state = STATE_FINISHED;
		rettype = PKT_TYPE_OK;
		break;

	default:
		L("unkn");
		rettype = PKT_TYPE_UNK;
		break;

	};

	//__delay_cycles(8500); // TODO: needed?
	packet_ack(rettype, pkt_len);
}

void packet_tx_callback(){

	if (state == STATE_FINISHED){
		PMMCTL0 = PMMPW | PMMSWBOR;
	}

	ready_to_send = true;
	packet_rxon();
}


__interrupt void default_isr(){
	L("isr");
}


extern void _start();
int main() {
	L("init");

	WDTCTL = WDTPW + WDTHOLD;        // Stop watchdog timer
	P2IE = 0;                        // Disable keypad (port2)
	RTCCTL0 &= ~(RTCTEVIE | RTCAIE | RTCRDYIE); // disable rtc

	// UCSCTL4 = SELM_3 + SELS_0 + SELA_0; // step up

	uint8_t int_state;

	packet_rxoff();

	ENTER_CRITICAL_SECTION(int_state);

	// Not sure what the best strategy is, copy old ivt / set handlers to nop / clear ivt
	//memset((char *) RAM_IVT_ADDRESS, 0, 0x80)
	//memcpy((char *) RAM_IVT_ADDRESS, (char *)0xff80, 0x80); 
	static uint16_t *ram_ivt = (uint16_t *) RAM_IVT_ADDRESS;

	for (int i = 0; i < 64; i++){
		ram_ivt[i] = (uint16_t)default_isr;
	}

	ram_ivt[53] = (uint16_t) packet_isr;
	//ram_ivt[63] = (uint16_t) _start; // doesn't  work, as SYSRIVECT will get resetted

	SYSCTL |= SYSRIVECT;

	EXIT_CRITICAL_SECTION(int_state);

	packet_rxon();
	// __delay_cycles(8000);

	L("rdy");
    __bis_SR_register(LPM3_bits + GIE);        // Enter LPM3

	L("err");

	while(1);
}
