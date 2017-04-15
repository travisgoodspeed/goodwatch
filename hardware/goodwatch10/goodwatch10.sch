EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:goodwatch10
LIBS:goodwatch10-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 7350 7500 0    60   ~ 0
GoodWatch Rev 10
Text Notes 10600 7650 0    60   ~ 0
10 (WIP)
Text Notes 7000 6750 0    60   ~ 0
Replacement PCB for the Casio CA53W built around the CC430F6137.\nby Travis Goodspeed
$Comp
L CA53LCD LCD1
U 1 1 58F11D4F
P 1350 4000
F 0 "LCD1" H 1150 5800 60  0000 C CNN
F 1 "CA53LCD" H 1350 4150 60  0000 C CNN
F 2 "AAgoodwatch10:ca53lcd" H 1350 4000 60  0001 C CNN
F 3 "" H 1350 4000 60  0001 C CNN
	1    1350 4000
	-1   0    0    1   
$EndComp
$Comp
L CC430F613x U1
U 1 1 58F17607
P 4350 4000
F 0 "U1" H 3900 3650 60  0000 C CNN
F 1 "CC430F613x" H 4000 4350 60  0000 C CNN
F 2 "" H 4350 4000 60  0001 C CNN
F 3 "" H 4350 4000 60  0001 C CNN
	1    4350 4000
	0    1    1    0   
$EndComp
Wire Wire Line
	2250 2250 4300 2250
Wire Wire Line
	4300 2250 4300 2800
Wire Wire Line
	2250 2350 4000 2350
Wire Wire Line
	4000 2350 4000 2800
Wire Wire Line
	2250 2450 3900 2450
Wire Wire Line
	3900 2450 3900 2800
Wire Wire Line
	2250 2550 3800 2550
Wire Wire Line
	3800 2550 3800 2800
Wire Wire Line
	2250 2650 3700 2650
Wire Wire Line
	3700 2650 3700 2800
Wire Wire Line
	2250 2750 3600 2750
Wire Wire Line
	3600 2750 3600 2800
Wire Wire Line
	2250 2850 3300 2850
Wire Wire Line
	3300 2850 3300 3250
Wire Wire Line
	2250 2950 3250 2950
Wire Wire Line
	3250 2950 3250 3350
Wire Wire Line
	3250 3350 3300 3350
Wire Wire Line
	2250 3050 3200 3050
Wire Wire Line
	3200 3050 3200 3450
Wire Wire Line
	3200 3450 3300 3450
Wire Wire Line
	2250 3150 3150 3150
Wire Wire Line
	3150 3150 3150 3550
Wire Wire Line
	3150 3550 3300 3550
Wire Wire Line
	2250 3250 3100 3250
Wire Wire Line
	3100 3250 3100 3650
Wire Wire Line
	3100 3650 3300 3650
Wire Wire Line
	2250 3350 3050 3350
Wire Wire Line
	3050 3350 3050 3750
Wire Wire Line
	3050 3750 3300 3750
Wire Wire Line
	2250 3450 3000 3450
Wire Wire Line
	3000 3450 3000 3850
Wire Wire Line
	3000 3850 3300 3850
Wire Wire Line
	2250 3550 2950 3550
Wire Wire Line
	2950 3550 2950 3950
Wire Wire Line
	2950 3950 3300 3950
Wire Wire Line
	2250 3650 2900 3650
Wire Wire Line
	2900 3650 2900 4150
Wire Wire Line
	2900 4150 3300 4150
Wire Wire Line
	2250 3750 2850 3750
Wire Wire Line
	2850 3750 2850 4250
Wire Wire Line
	2850 4250 3300 4250
Wire Wire Line
	2250 3850 2800 3850
Wire Wire Line
	2800 3850 2800 4350
Wire Wire Line
	2800 4350 3300 4350
Wire Wire Line
	2250 3950 2750 3950
Wire Wire Line
	2750 3950 2750 4450
Wire Wire Line
	2750 4450 3300 4450
Wire Wire Line
	2250 4050 2700 4050
Wire Wire Line
	2700 4050 2700 4550
Wire Wire Line
	2700 4550 3300 4550
Wire Wire Line
	2250 4150 2650 4150
Wire Wire Line
	2650 4150 2650 4650
Wire Wire Line
	2650 4650 3300 4650
Wire Wire Line
	2250 4250 2600 4250
Wire Wire Line
	2600 4250 2600 4750
Wire Wire Line
	2600 4750 3300 4750
Wire Wire Line
	2250 4350 2550 4350
Wire Wire Line
	2550 4350 2550 4800
Wire Wire Line
	2550 4800 3350 4800
Wire Wire Line
	3350 4800 3350 5200
Wire Wire Line
	3350 5200 3600 5200
Wire Wire Line
	2250 4450 2500 4450
Wire Wire Line
	2500 4450 2500 4850
Wire Wire Line
	2500 4850 3300 4850
Wire Wire Line
	3300 4850 3300 5250
Wire Wire Line
	3300 5250 3700 5250
Wire Wire Line
	3700 5250 3700 5200
Wire Wire Line
	2250 4550 2450 4550
Wire Wire Line
	2450 4550 2450 4900
Wire Wire Line
	2450 4900 3250 4900
Wire Wire Line
	3250 4900 3250 5300
Wire Wire Line
	3250 5300 3800 5300
Wire Wire Line
	3800 5300 3800 5200
$Comp
L CA53KEYPAD KPD1
U 1 1 58F233A6
P 6050 3550
F 0 "KPD1" H 6200 3150 60  0000 C CNN
F 1 "CA53KEYPAD" V 6200 3650 60  0000 C CNN
F 2 "AAgoodwatch10:ca53keypad" H 6050 3550 60  0001 C CNN
F 3 "" H 6050 3550 60  0001 C CNN
	1    6050 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3950 5850 3950
Wire Wire Line
	5400 3850 5850 3850
Wire Wire Line
	5850 3750 5400 3750
Wire Wire Line
	5400 3650 5850 3650
Wire Wire Line
	5850 3550 5400 3550
Wire Wire Line
	5400 3450 5850 3450
Wire Wire Line
	5850 3350 5400 3350
Wire Wire Line
	5850 3250 5400 3250
Wire Wire Line
	5850 3150 5300 3150
Wire Wire Line
	5300 3150 5300 2800
Wire Wire Line
	5300 2800 5100 2800
Text Notes 7500 1050 0    60   ~ 0
NOTE WELL:\n1. Both LCD and Keypad pin 1 must be on the West of the board.\n2. Some LCD segments can't be mapped due to pin limits.  Sorry.\n3. P1.5 and P1.6 double as a UART for the BSL. (!RST and TST also required.)
$Comp
L CA53BOARD BRD1
U 1 1 58F257A1
P 5100 2500
F 0 "BRD1" H 5600 2550 60  0000 C CNN
F 1 "CA53BOARD" H 5550 3200 60  0000 C CNN
F 2 "AAgoodwatch10:ca53board" H 5100 2500 60  0001 C CNN
F 3 "" H 5100 2500 60  0001 C CNN
	1    5100 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 2800 5000 2300
Wire Wire Line
	5000 2300 5250 2300
Wire Wire Line
	4900 2800 4900 2200
Wire Wire Line
	4900 2200 5250 2200
$EndSCHEMATC
