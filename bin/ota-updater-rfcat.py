#!/usr/bin/env python3

import struct
from rflib import *
from time import sleep
from argparse import ArgumentParser
from binascii import hexlify, unhexlify
from hexdump import hexdump
from time import sleep

import progressbar

GREEN  = '\u001b[32m'
BLUE   = '\u001b[34m'
RED    = '\u001b[31m'
YELLOW = '\u001b[33m'
RESET  = '\u001b[0m'

PROGRESSBAR_WIDGETS = [
    progressbar.SimpleProgress(),
    f'{YELLOW} ',
    progressbar.Bar(),
    ' ',
    progressbar.Percentage(),
    f'{RESET} ',
    progressbar.widgets.FileTransferSpeed(unit='pkt'),
    ' ',
    progressbar.Timer(),
    progressbar.ETA(),
]

PKT_TYPE_INIT   = 0
PKT_TYPE_DATA   = 1
PKT_TYPE_REBOOT = 2
PKT_TYPE_LOG    = 0xf0
PKT_TYPE_RESET  = 0xf1
PKT_TYPE_BUSY   = 0xfa
PKT_TYPE_DENIED = 0xfb
PKT_TYPE_UNK    = 0xfc
PKT_TYPE_OOO    = 0xfd
PKT_TYPE_ERR    = 0xfe
PKT_TYPE_OK     = 0xff


IHEX_TYPE_DATA = 0
IHEX_TYPE_EOF = 1
IHEX_TYPE_ENTRY_POINT = 3

class Updater:
    def __init__(self, freq):
        d = RfCat()

        d.setFreq(freq)
        d.setEnablePktCRC(True)
        d.setMdmModulation(MOD_GFSK)
        d.setMdmSyncWord(0xd391)
        d.setMdmDRate(250000)
        # d.setMdmChanSpc(200) # seems not to work ?!
        d.setMdmChanSpc(chanspc_m=0x11, chanspc_e=3)
        d.setMdmChanBW(541666)
        d.setMdmDeviatn(122953)
        # d.setRFRegister(DEVIATN, (5 << 4) | 4)
        d.setMdmSyncMode(SYNCM_30_of_32)
        d.setMdmNumPreamble(MFMCFG1_NUM_PREAMBLE_24)
        d.setMaxPower()

        d.makePktVLEN(255)
        # print(d.reprRadioConfig())

        self.dongle = d

    def listen(self):
        self.dongle.RFlisten()

    def flash(self, fn):

        writes = []

        with open(fn) as f:

            for line in f.readlines():
                line = line.strip()

                if line[0] != ':':
                    return

                count = int(line[1:3], 16)
                addr = int(line[3:7], 16)
                type = int(line[7:9], 16)
                data = unhexlify(line[9:-2])
                crc = int(line[-2:], 16)

                crc_calc = (-sum(unhexlify(line[1:-2]))) & 0x0FF

                if crc != crc_calc:
                    print(f'crc error {crc:x} != {crc_calc:x}: line', file=sys.stderr)

                if type == IHEX_TYPE_DATA:

                    if writes and addr == writes[-1][0] + len(writes[-1][1]) and len(writes[-1][1]) + len(data) <= 56:
                        writes[-1][1] += data
                    else:
                        writes += [[addr, data]]

        # writes = [writes[0]]
        pkt = self.make_init_packet(len(writes))
        # return

        if not self.send_raw(pkt, check_ok=True):
            print('Init failed')
            return

        print(f'{GREEN}Initialized: {len(writes)} packets {RESET}')

        bar = progressbar.ProgressBar(max_value=len(writes), widgets=PROGRESSBAR_WIDGETS, redirect_stdout=True)

        for nr, (addr, data) in enumerate(writes):
            bar.update(nr)

            pkt = self.make_data_packet(nr + 1, addr, data)
            # print(f'{nr+1} {addr:x} {data}')

            ret = self.send_raw(pkt)

            if not ret:
                print(f'{RED}Connection timeout / stale @ {nr + 1}/{len(writes)} ({addr:x}){RESET}')
                return False

            if ret[0] == PKT_TYPE_OK:
                # print('ok')
                continue
            if ret[0] == PKT_TYPE_OOO:
                print('Out of Order received: Trying to fix it')

                last_pkt = self.make_data_packet(nr, writes[nr-1][0], writes[nr-1][1])

                if self.send_raw(last_pkt):
                    if self.send_raw(pkt):
                        print('Fixed')
                        continue

                print('{RED}Failed{RESET}')

                return False
            elif ret[0] == PKT_TYPE_ERR:
                print('write failed, try again')

                if self.send_raw(pkt):
                    print('Fixed')
                    continue

                return False

        # bar.finish()

        print(f'{GREEN}Flashing complete{RESET}')
        return True


    def log(self):
        pkt = self.make_packet(PKT_TYPE_LOG)

        data = self.send_raw(pkt)

        if not data:
            return

        if data[0] == PKT_TYPE_UNK:
            print(f'{RED}Log command not understood: not debug build?{RESET}')
            return

        if data[0] != PKT_TYPE_OK:
            print(f'{RED}Unexpected response: {data}')
            return

        try:
            print(data[1:].decode())
        except:
            print(data[1:])

    def reset(self):
        pkt = self.make_packet(PKT_TYPE_RESET)
        return self.send_raw(pkt, check_ok=True)

    def reboot(self):
        pkt = self.make_packet(PKT_TYPE_REBOOT)
        return self.send_raw(pkt, check_ok=True)

    def send_raw(self, pkt, check_ok=False, retry=10):

        data = None

        for i in range(retry):

            self.dongle.setModeIDLE()
            self.dongle.RFxmit(pkt)

            self.dongle.setModeRX()

            try:
                data, ts = self.dongle.RFrecv(1000)
            except:
                # print('Timeout')
                continue

            break


        if i > 0:
            print(f'{YELLOW}resent={i} pkt={pkt} data={data}{RESET}')
        # print(f'resent={i} pkt={pkt} data={data}')

        if data and check_ok:
            if data[0] == PKT_TYPE_UNK:
                print(f'{RED}Command not understood{RESET}')
            elif data[0] not in [PKT_TYPE_OK, PKT_TYPE_ERR]:
                print(f'unknown data: {data}')
                return False

            return data[0] == PKT_TYPE_OK

        return data

    def make_init_packet(self, num_packets):
        data = struct.pack('H', num_packets)
        return self.make_packet(PKT_TYPE_INIT, data)

    def make_data_packet(self, num, addr, data):
        data = struct.pack('H', num) + struct.pack('H', addr) + data
        return self.make_packet(PKT_TYPE_DATA, data)

    def make_packet(self, type, data=b''):

        pkt = struct.pack('B', type)
        pkt += data

        return pkt



if __name__ == '__main__':
    p = ArgumentParser()

    p.add_argument('-L', '--listen', action='store_true', default=False, help='listen for packets')
    p.add_argument('flash', nargs='?', help='goodwatch.ihex')
    p.add_argument('-l', '--log', action='store_true', default=False, help='fetches log of updater')
    p.add_argument('-r', '--reset', action='store_true', default=False, help='resets updater on target')
    p.add_argument('-R', '--reboot', action='store_true', default=False, help='reboots updater on target')

    p.add_argument('-f', '--freq', type=int, default=433920000, help='listen for packets')

    args = p.parse_args()

    upd = Updater(args.freq)

    if args.reset:
        if upd.reset():
            print(f'{GREEN}Device reseted{RESET}')
        else:
            print(f'{RED}Device not reseted{RESET}')
            sys.exit(1)

    if args.listen:
        upd.listen()
        sys.exit(0)

    if args.log:
        upd.log()

    if args.flash:
        upd.flash(args.flash)

    if args.reboot and not args.flash:
        if upd.reboot():
            print(f'{GREEN}Device rebooted{RESET}')
        else:
            print(f'{RED}Device not rebooted{RESET}')
            sys.exit(1)

