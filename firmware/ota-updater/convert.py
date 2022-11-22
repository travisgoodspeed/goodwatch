#!/usr/bin/env python3

import sys
from binascii import hexlify, unhexlify
import struct
from argparse import ArgumentParser


GREEN  = '\u001b[32m'
BLUE   = '\u001b[34m'
RED    = '\u001b[31m'
YELLOW = '\u001b[33m'
RESET  = '\u001b[0m'

MERGE_SECTIONS_MAX_BYTES = 16

TYPE_DATA = 0
TYPE_EOF = 1
TYPE_ENTRY_POINT = 3


def parse_ihex(filename, ignore_isr=False):
    ihex = {}

    cur_addr = 0
    cur_data = b''

    total_bytes = 0
    segments = []


    for line in open(filename):
        line = line.strip()

        if not line.startswith(':'):
            print(f'line error: {line}', file=sys.stderr)
            break

        count = int(line[1:3], 16)
        addr = int(line[3:7], 16)
        type = int(line[7:9], 16)
        data = unhexlify(line[9:-2])
        crc = int(line[-2:], 16)

        crc_calc = (-sum(unhexlify(line[1:-2]))) & 0x0FF

        # print(f'{type:x} {addr:x} {count} {hexlify(data)} {crc:x}')

        if len(data) != count:
            print(f'{RED}count error {count} != {len(data)}:{RESET} {line}', file=sys.stderr)
            break

        if crc != crc_calc:
            print(f'{RED}crc error {crc} != {crc_calc}:{RESET} line', file=sys.stderr)

        if type == TYPE_EOF:
            break

        if type == TYPE_ENTRY_POINT:
            print(f'{YELLOW}entry point:{RESET} 0x{struct.unpack(">I", data)[0]:x}', file=sys.stderr)
            continue

        if type != TYPE_DATA:
            print(f'{RED}unhandled type ({type}):{RESET} {line}', file=sys.stderr)
            continue

        if addr > cur_addr + len(cur_data) + MERGE_SECTIONS_MAX_BYTES:

            if cur_addr and (not ignore_isr or cur_addr < 0xFF80):
                segments += [(cur_addr, cur_data)]
                total_bytes += len(cur_data)

            cur_data = b''
            cur_addr = addr

        cur_data += (addr - cur_addr - len(cur_data)) * b'\x00' + data


    if cur_data and (not ignore_isr or cur_addr < 0xFF80):
        segments += [(cur_addr, cur_data)]
        total_bytes += len(cur_data)

    ihex['total_bytes'] = total_bytes
    ihex['segments'] = segments

    return ihex


if __name__ == '__main__':
    p = ArgumentParser()
    p.add_argument('file')
    p.add_argument('-c', '--c-output', action='store_true')
    p.add_argument('-t', '--txt-output', action='store_true')
    p.add_argument('-b', '--bin-output')

    p.add_argument('-i', '--ignore-isr', action='store_true', default=False)

    args = p.parse_args()

    ihex = parse_ihex(args.file, args.ignore_isr)

    print(f'{YELLOW}total:{RESET} {ihex["total_bytes"]} bytes', file=sys.stderr)

    if args.c_output:
        for addr, data in ihex['segments']:
            print(f'static const seg_{addr:x}[] = {{')

            for i in range(0, len(data), 16):
                print('\t' + ', '.join('0x%02X' % c for c in data[i:i+16]) + ',')

            print('}\n')

    if args.txt_output:
        for addr, data in ihex['segments']:

            print(f'@{addr:04X}')

            for i in range(0, len(data), 16):
                print(' '.join('%02X' % c for c in data[i:i+16]))

    if args.bin_output:
        with open(args.bin_output, 'wb+') as f:

            for addr, data in ihex['segments']:
                f.seek(addr)
                f.write(data)

