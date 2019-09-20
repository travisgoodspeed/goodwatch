EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "GoodWatch"
Date ""
Rev "30"
Comp ""
Comment1 "by Travis Goodspeed"
Comment2 "the CC430F6137.  Tuned for either 70cm (<500MHz) or 33cm (<1GHz)."
Comment3 "Replacement PCB for the Casio 3208 watch module (CA53 or CA506) built around"
Comment4 ""
$EndDescr
$Comp
L goodwatch31:CA53LCD LCD1
U 1 1 58F11D4F
P 1350 3900
F 0 "LCD1" H 1150 5700 60  0000 C CNN
F 1 "CA53LCD" H 1350 4050 60  0000 C CNN
F 2 "goodwatch31:ca53lcd" H 1350 3900 60  0001 C CNN
F 3 "" H 1350 3900 60  0001 C CNN
F 4 "YES" H 1350 3900 50  0001 C CNN "DNP"
	1    1350 3900
	-1   0    0    1   
$EndComp
$Comp
L goodwatch31:CC430F613x U1
U 1 1 58F17607
P 4350 4000
F 0 "U1" H 3900 3650 60  0000 C CNN
F 1 "CC430F613x" H 4000 4350 60  0000 C CNN
F 2 "goodwatch31:CC430F613x" H 4350 4000 60  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/cc430f6143.pdf" H 4350 4000 60  0001 C CNN
F 4 "ic-mcu-cc430f613x" H 0   0   50  0001 C CNN "Key"
F 5 "Texas Instruments" H 4350 4000 50  0001 C CNN "Manufacturer"
F 6 "CC430F6147IRGCR" H 4350 4000 50  0001 C CNN "MFN"
	1    4350 4000
	0    1    1    0   
$EndComp
$Comp
L goodwatch31:CA53KEYPAD KPD1
U 1 1 58F233A6
P 6050 3550
F 0 "KPD1" H 6200 3150 60  0000 C CNN
F 1 "CA53KEYPAD" V 6200 3650 60  0000 C CNN
F 2 "goodwatch31:ca53keypad" H 6050 3550 60  0001 C CNN
F 3 "" H 6050 3550 60  0001 C CNN
F 4 "YES" H 6050 3550 50  0001 C CNN "DNP"
	1    6050 3550
	1    0    0    1   
$EndComp
Text Notes 7450 1100 0    60   ~ 0
NOTE WELL:\n1. Both LCD and Keypad pin 1 must be on the West of the board.\n2. Some LCD segments can't be mapped due to pin limits.  Sorry.\n3. P1.5 and P1.6 double as a UART for the BSL. (!RST and TST also required.)\n4. See pages 86 and 88 of CC430F6137 datasheet for component values.\n5. Manufacture with 0.6mm thickness. 
$Comp
L goodwatch31:CA53BOARD BRD1
U 1 1 58F257A1
P 5350 2500
F 0 "BRD1" H 5850 3100 60  0000 C CNN
F 1 "CA53BOARD" V 6050 2650 60  0000 C CNN
F 2 "goodwatch31:ca53board" H 5350 2500 60  0001 C CNN
F 3 "" H 5350 2500 60  0001 C CNN
F 4 "YES" H 5350 2500 50  0001 C CNN "DNP"
	1    5350 2500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 58F26311
P 4350 4400
F 0 "#PWR01" H 4350 4150 50  0001 C CNN
F 1 "GND" H 4350 4250 50  0000 C CNN
F 2 "" H 4350 4400 50  0000 C CNN
F 3 "" H 4350 4400 50  0000 C CNN
	1    4350 4400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 58F26936
P 5150 2100
F 0 "#PWR02" H 5150 1850 50  0001 C CNN
F 1 "GND" H 5150 1950 50  0000 C CNN
F 2 "" H 5150 2100 50  0000 C CNN
F 3 "" H 5150 2100 50  0000 C CNN
	1    5150 2100
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR03
U 1 1 58F269B9
P 5500 1850
F 0 "#PWR03" H 5500 1700 50  0001 C CNN
F 1 "VCC" H 5500 2000 50  0000 C CNN
F 2 "" H 5500 1850 50  0000 C CNN
F 3 "" H 5500 1850 50  0000 C CNN
	1    5500 1850
	1    0    0    -1  
$EndComp
NoConn ~ 2250 2650
NoConn ~ 2250 2550
NoConn ~ 2250 2450
NoConn ~ 2250 2350
NoConn ~ 2250 2250
$Comp
L power:VCC #PWR04
U 1 1 58F27AAC
P 4100 2450
F 0 "#PWR04" H 4100 2300 50  0001 C CNN
F 1 "VCC" H 4100 2600 50  0000 C CNN
F 2 "" H 4100 2450 50  0000 C CNN
F 3 "" H 4100 2450 50  0000 C CNN
	1    4100 2450
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR05
U 1 1 58F27BEB
P 6500 4050
F 0 "#PWR05" H 6500 3900 50  0001 C CNN
F 1 "VCC" H 6500 4200 50  0000 C CNN
F 2 "" H 6500 4050 50  0000 C CNN
F 3 "" H 6500 4050 50  0000 C CNN
	1    6500 4050
	1    0    0    -1  
$EndComp
Text Label 5450 4550 0    60   ~ 0
!RST
Text Label 5700 4650 2    60   ~ 0
TST
$Comp
L Device:Crystal_Small Y1
U 1 1 58F2817E
P 5700 4150
F 0 "Y1" H 5900 4200 50  0000 C CNN
F 1 "32.768kHz" H 6050 4100 50  0000 C CNN
F 2 "Crystal:Crystal_SMD_3215-2Pin_3.2x1.5mm" V 6800 4150 50  0000 C CNN
F 3 "https://abracon.com/Resonators/ABS07-120-32.768kHz-T.pdf" H 5700 4150 50  0001 C CNN
F 4 "xtal3215-32_768khz" H 0   0   50  0001 C CNN "Key"
F 5 "Abracon" H 0   0   50  0001 C CNN "Manufacturer"
F 6 "ABS07-120-32.768KHZ-T" H 0   0   50  0001 C CNN "MFN"
	1    5700 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 58F2840C
P 5700 4350
F 0 "#PWR06" H 5700 4100 50  0001 C CNN
F 1 "GND" H 5700 4200 50  0000 C CNN
F 2 "" H 5700 4350 50  0000 C CNN
F 3 "" H 5700 4350 50  0000 C CNN
	1    5700 4350
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR07
U 1 1 58F2873C
P 3200 4050
F 0 "#PWR07" H 3200 3900 50  0001 C CNN
F 1 "VCC" H 3200 4200 50  0000 C CNN
F 2 "" H 3200 4050 50  0000 C CNN
F 3 "" H 3200 4050 50  0000 C CNN
	1    3200 4050
	0    -1   -1   0   
$EndComp
NoConn ~ 2250 5550
NoConn ~ 2250 5150
NoConn ~ 5100 5200
NoConn ~ 5400 4750
$Comp
L Device:C C1
U 1 1 58F295CB
P 7200 3200
F 0 "C1" H 7225 3300 50  0000 L CNN
F 1 "47uF" H 7225 3100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" V 7000 3100 50  0000 C CNN
F 3 "" H 7200 3200 50  0000 C CNN
F 4 "cap-cer-0603-47u" H 0   0   50  0001 C CNN "Key"
	1    7200 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 58F297AD
P 7450 3200
F 0 "C2" H 7475 3300 50  0000 L CNN
F 1 "0.1uF" H 7475 3100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" V 8000 3100 50  0000 C CNN
F 3 "" H 7450 3200 50  0000 C CNN
F 4 "cap-cer-0402-03high-100n" H 0   0   50  0001 C CNN "Key"
F 5 "Use flat versions that are not higher than 0.33mm." H 0   0   50  0001 C CNN "Notes"
	1    7450 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 58F297DF
P 7700 3200
F 0 "C3" H 7725 3300 50  0000 L CNN
F 1 "0.1uF" H 7725 3100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" V 8100 3100 50  0000 C CNN
F 3 "" H 7700 3200 50  0000 C CNN
F 4 "cap-cer-0402-03high-100n" H 0   0   50  0001 C CNN "Key"
F 5 "Use flat versions that are not higher than 0.33mm." H 0   0   50  0001 C CNN "Notes"
	1    7700 3200
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR08
U 1 1 58F2986E
P 7450 2900
F 0 "#PWR08" H 7450 2750 50  0001 C CNN
F 1 "VCC" H 7450 3050 50  0000 C CNN
F 2 "" H 7450 2900 50  0000 C CNN
F 3 "" H 7450 2900 50  0000 C CNN
	1    7450 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 58F29894
P 7450 3500
F 0 "#PWR09" H 7450 3250 50  0001 C CNN
F 1 "GND" H 7450 3350 50  0000 C CNN
F 2 "" H 7450 3500 50  0000 C CNN
F 3 "" H 7450 3500 50  0000 C CNN
	1    7450 3500
	1    0    0    -1  
$EndComp
Text Label 2700 5250 2    60   ~ 0
COM2
Text Label 2700 5350 2    60   ~ 0
COM1
Text Label 2700 5450 2    60   ~ 0
COM0
Text Label 4500 2450 3    60   ~ 0
COM2
Text Label 4600 2450 3    60   ~ 0
COM1
Text Label 4700 2450 3    60   ~ 0
COM0
Wire Wire Line
	5000 2800 5000 2300
Wire Wire Line
	5000 2300 5500 2300
Wire Wire Line
	4900 2800 4900 2200
Wire Wire Line
	4900 2200 5500 2200
Wire Wire Line
	4350 4300 4350 4400
Wire Wire Line
	5850 3450 5400 3450
Wire Wire Line
	5400 3350 5850 3350
Wire Wire Line
	5850 3250 5400 3250
Wire Wire Line
	5850 3150 5250 3150
Wire Wire Line
	5250 3150 5250 2800
Wire Wire Line
	5250 2800 5100 2800
Wire Wire Line
	5500 2100 5150 2100
Wire Wire Line
	5500 2000 5500 1850
Wire Wire Line
	3800 5200 3800 5500
Wire Wire Line
	3800 5500 2950 5500
Wire Wire Line
	2950 5500 2950 5050
Wire Wire Line
	2950 5050 2250 5050
Wire Wire Line
	3700 5200 3700 5450
Wire Wire Line
	3700 5450 3000 5450
Wire Wire Line
	3000 5450 3000 4950
Wire Wire Line
	3000 4950 2250 4950
Wire Wire Line
	3600 5200 3600 5400
Wire Wire Line
	3600 5400 3050 5400
Wire Wire Line
	3050 5400 3050 4850
Wire Wire Line
	3050 4850 2250 4850
Wire Wire Line
	3300 4750 2250 4750
Wire Wire Line
	3300 4650 2250 4650
Wire Wire Line
	3300 4550 2250 4550
Wire Wire Line
	3300 4450 2250 4450
Wire Wire Line
	3300 4350 2250 4350
Wire Wire Line
	3300 4250 2250 4250
Wire Wire Line
	2250 4150 3300 4150
Wire Wire Line
	4100 2450 4100 2800
Wire Wire Line
	5400 4050 6500 4050
Wire Wire Line
	5400 4450 6500 4450
Wire Wire Line
	6500 4050 6500 4450
Connection ~ 6500 4050
Wire Wire Line
	5400 4550 5750 4550
Connection ~ 6500 4450
Wire Wire Line
	5400 4650 5700 4650
Wire Wire Line
	5400 4150 5600 4150
Wire Wire Line
	5400 4250 5800 4250
Wire Wire Line
	5800 4250 5800 4150
Wire Wire Line
	5400 4350 5700 4350
Wire Wire Line
	3200 4050 3300 4050
Wire Wire Line
	3300 3950 3000 3950
Wire Wire Line
	3000 3950 3000 4050
Wire Wire Line
	3000 4050 2250 4050
Wire Wire Line
	3300 3850 2950 3850
Wire Wire Line
	2950 3850 2950 3950
Wire Wire Line
	2950 3950 2250 3950
Wire Wire Line
	3300 3750 2900 3750
Wire Wire Line
	2900 3750 2900 3850
Wire Wire Line
	2900 3850 2250 3850
Wire Wire Line
	3300 3650 2850 3650
Wire Wire Line
	2850 3650 2850 3750
Wire Wire Line
	2850 3750 2250 3750
Wire Wire Line
	3300 3550 2800 3550
Wire Wire Line
	2800 3550 2800 3650
Wire Wire Line
	2800 3650 2250 3650
Wire Wire Line
	3300 3450 2750 3450
Wire Wire Line
	2750 3450 2750 3550
Wire Wire Line
	2750 3550 2250 3550
Wire Wire Line
	3300 3350 2700 3350
Wire Wire Line
	2700 3350 2700 3450
Wire Wire Line
	2700 3450 2250 3450
Wire Wire Line
	3300 3250 2650 3250
Wire Wire Line
	2650 3250 2650 3350
Wire Wire Line
	2650 3350 2250 3350
Wire Wire Line
	3600 2800 3250 2800
Wire Wire Line
	3250 2800 3250 3200
Wire Wire Line
	3250 3200 2600 3200
Wire Wire Line
	2600 3200 2600 3250
Wire Wire Line
	2600 3250 2250 3250
Wire Wire Line
	3700 2800 3700 2750
Wire Wire Line
	3700 2750 3200 2750
Wire Wire Line
	3200 2750 3200 3150
Wire Wire Line
	3200 3150 2250 3150
Wire Wire Line
	3800 2800 3800 2700
Wire Wire Line
	3800 2700 3150 2700
Wire Wire Line
	3150 2700 3150 3050
Wire Wire Line
	3150 3050 2250 3050
Wire Wire Line
	3900 2800 3900 2650
Wire Wire Line
	3900 2650 3100 2650
Wire Wire Line
	3100 2650 3100 2950
Wire Wire Line
	3100 2950 2250 2950
Wire Wire Line
	4000 2800 4000 2600
Wire Wire Line
	4000 2600 3050 2600
Wire Wire Line
	3050 2600 3050 2850
Wire Wire Line
	3050 2850 2250 2850
Wire Wire Line
	4300 2800 4300 2200
Wire Wire Line
	4300 2200 3000 2200
Wire Wire Line
	3000 2200 3000 2750
Wire Wire Line
	3000 2750 2250 2750
Wire Wire Line
	7450 3350 7450 3400
Wire Wire Line
	7450 2900 7450 3000
Wire Wire Line
	7200 3050 7200 3000
Wire Wire Line
	7200 3000 7450 3000
Connection ~ 7450 3000
Wire Wire Line
	7700 3000 7700 3050
Wire Wire Line
	7700 3400 7700 3350
Wire Wire Line
	7200 3400 7450 3400
Connection ~ 7450 3400
Wire Wire Line
	7200 3350 7200 3400
Wire Wire Line
	4800 2800 4800 2450
Wire Wire Line
	4700 2800 4700 2450
Wire Wire Line
	4600 2800 4600 2450
Wire Wire Line
	4500 2800 4500 2450
Wire Wire Line
	2250 5250 2700 5250
Wire Wire Line
	2250 5350 2700 5350
Wire Wire Line
	2250 5450 2700 5450
Wire Wire Line
	4200 2450 4200 2800
Wire Wire Line
	5400 3550 5850 3550
Wire Wire Line
	5850 3650 5400 3650
Wire Wire Line
	5400 3750 5850 3750
Wire Wire Line
	5850 3850 5400 3850
Wire Wire Line
	4400 2800 4400 2150
Wire Wire Line
	4400 2150 2250 2150
$Comp
L Device:C C4
U 1 1 59A8837E
P 8550 3250
F 0 "C4" H 8575 3350 50  0000 L CNN
F 1 "10uF" H 8575 3150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" V 8800 3200 50  0000 C CNN
F 3 "" H 8550 3250 50  0000 C CNN
F 4 "cap-cer-0402-10u" H 0   0   50  0001 C CNN "Key"
	1    8550 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 59A88921
P 8550 3500
F 0 "#PWR010" H 8550 3250 50  0001 C CNN
F 1 "GND" H 8550 3350 50  0000 C CNN
F 2 "" H 8550 3500 50  0000 C CNN
F 3 "" H 8550 3500 50  0000 C CNN
	1    8550 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 3500 8550 3400
Wire Wire Line
	8550 3100 8550 2750
Text Label 4800 2450 3    60   ~ 0
LCDCAP
Text Label 8550 2750 3    60   ~ 0
LCDCAP
$Comp
L Device:C C5
U 1 1 59A89285
P 9050 3250
F 0 "C5" H 9075 3350 50  0000 L CNN
F 1 "0.47uF" H 9075 3150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" V 9350 3200 50  0000 C CNN
F 3 "" H 9050 3250 50  0000 C CNN
F 4 "cap-cer-0402-470n" H 0   0   50  0001 C CNN "Key"
	1    9050 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 59A89316
P 9050 3500
F 0 "#PWR011" H 9050 3250 50  0001 C CNN
F 1 "GND" H 9050 3350 50  0000 C CNN
F 2 "" H 9050 3500 50  0000 C CNN
F 3 "" H 9050 3500 50  0000 C CNN
	1    9050 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 3400 9050 3500
Wire Wire Line
	9050 3100 9050 2750
Text Label 9050 2750 3    60   ~ 0
VCORE
Text Label 4200 2450 3    60   ~ 0
VCORE
Wire Wire Line
	5500 2400 5300 2400
Wire Wire Line
	5300 2500 5500 2500
Text Label 5300 2400 0    60   ~ 0
TST
Text Label 5300 2500 0    60   ~ 0
!RST
NoConn ~ 5000 5200
NoConn ~ 4900 5200
Wire Wire Line
	4300 5200 4300 5600
Wire Wire Line
	4400 5200 4400 5600
Text Label 4300 5600 1    60   ~ 0
RF_P
Text Label 4400 5600 1    60   ~ 0
RF_N
$Comp
L goodwatch31:YageoS432 A1
U 1 1 59D2F54A
P 2800 6750
F 0 "A1" H 2800 7150 60  0000 C CNN
F 1 "YageoS432" V 2650 6850 60  0000 C CNN
F 2 "goodwatch31:YageoS432" H 2800 6750 60  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/An_SMD_UHF_433_1204_0.pdf" H 2800 6750 60  0001 C CNN
F 4 "Optional: only used without watch body for the RF version" H 2800 6750 50  0001 C CNN "Notes"
F 5 "YES" H 2800 6750 50  0001 C CNN "DNP"
F 6 "ant-smd-yageos432" H 0   0   50  0001 C CNN "Key"
F 7 "Yaego" H 0   0   50  0001 C CNN "Manufacturer"
F 8 "ANT1204F002R0433A" H 0   0   50  0001 C CNN "MFN"
	1    2800 6750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 59D2FA2B
P 4700 5600
F 0 "R2" V 4600 5600 50  0000 C CNN
F 1 "56k" V 4700 5600 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4630 5600 50  0001 C CNN
F 3 "" H 4700 5600 50  0000 C CNN
F 4 "res-0402-56k-1%" H 4700 5600 50  0001 C CNN "Key"
F 5 "Optional: only needed for the RF version" H 0   0   50  0001 C CNN "Notes"
	1    4700 5600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 59D2FD8F
P 4700 5800
F 0 "#PWR012" H 4700 5550 50  0001 C CNN
F 1 "GND" H 4700 5650 50  0000 C CNN
F 2 "" H 4700 5800 50  0000 C CNN
F 3 "" H 4700 5800 50  0000 C CNN
	1    4700 5800
	1    0    0    -1  
$EndComp
Text Notes 4700 5800 2    60   ~ 0
1%
Wire Wire Line
	4700 5200 4700 5450
Wire Wire Line
	4700 5800 4700 5750
Wire Wire Line
	4100 5200 4100 5250
Wire Wire Line
	4100 5250 4200 5250
Wire Wire Line
	4200 5200 4200 5250
Connection ~ 4200 5250
Wire Wire Line
	4500 5200 4500 5250
Connection ~ 4500 5250
Wire Wire Line
	4600 5200 4600 5250
Connection ~ 4600 5250
Wire Wire Line
	4800 5200 4800 5250
Connection ~ 4800 5250
$Comp
L goodwatch31:FA128 Y2
U 1 1 59D3CB4A
P 2150 1000
F 0 "Y2" H 2150 800 60  0000 C CNN
F 1 "FA128 26MHz" H 2150 1150 60  0000 C CNN
F 2 "goodwatch31:FA128" H 2150 1000 60  0001 C CNN
F 3 "https://support.epson.biz/td/api/doc_check.php?dl=brief_FA-128_en.pdf" H 2150 1000 60  0001 C CNN
F 4 "xtal-fa128-26mhz" H 0   0   50  0001 C CNN "Key"
F 5 "FA-128 26.0000MF10Z-W5" H 0   0   50  0001 C CNN "MFN"
F 6 "EPSON" H 0   0   50  0001 C CNN "Manufacturer"
F 7 "Optional: only needed for the RF version" H 0   0   50  0001 C CNN "Notes"
	1    2150 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 950  1600 950 
Wire Wire Line
	2550 1100 2750 1100
Text Label 1600 950  0    60   ~ 0
GND
Text Label 2750 1100 2    60   ~ 0
GND
Wire Wire Line
	3900 5200 3900 5600
Wire Wire Line
	4000 5200 4000 5600
Text Label 3900 5600 1    60   ~ 0
RF_XIN
Text Label 4000 5600 1    60   ~ 0
RF_XOUT
Wire Wire Line
	2550 950  2950 950 
Wire Wire Line
	1750 1100 1400 1100
Text Label 2550 950  0    60   ~ 0
RF_XOUT
Text Label 1400 1100 0    60   ~ 0
RF_XIN
$Comp
L Device:C C6
U 1 1 59D3F241
P 1400 1250
F 0 "C6" H 1425 1350 50  0000 L CNN
F 1 "12pF" H 1425 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2050 1800 50  0000 C CNN
F 3 "" H 1400 1250 50  0000 C CNN
F 4 "cap-cer-0402-12p" H 0   0   50  0001 C CNN "Key"
F 5 "Optional: only needed for the RF version" H 0   0   50  0001 C CNN "Notes"
	1    1400 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C7
U 1 1 59D3F397
P 2950 1250
F 0 "C7" H 2975 1350 50  0000 L CNN
F 1 "12pF" H 2975 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2050 1900 50  0000 C CNN
F 3 "" H 2950 1250 50  0000 C CNN
F 4 "cap-cer-0402-12p" H 0   0   50  0001 C CNN "Key"
F 5 "Optional: only needed for the RF version" H 0   0   50  0001 C CNN "Notes"
	1    2950 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 950  2950 1100
$Comp
L power:GND #PWR013
U 1 1 59D3F5C9
P 2150 1500
F 0 "#PWR013" H 2150 1250 50  0001 C CNN
F 1 "GND" H 2150 1350 50  0000 C CNN
F 2 "" H 2150 1500 50  0000 C CNN
F 3 "" H 2150 1500 50  0000 C CNN
	1    2150 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 1400 2150 1400
Wire Wire Line
	2150 1500 2150 1400
Connection ~ 2150 1400
$Comp
L Device:R R1
U 1 1 59DB970E
P 5900 4550
F 0 "R1" V 5980 4550 50  0000 C CNN
F 1 "75k" V 5900 4550 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6100 4750 50  0000 C CNN
F 3 "" H 5900 4550 50  0000 C CNN
F 4 "res-0402-75k" H 0   0   50  0001 C CNN "Key"
	1    5900 4550
	0    1    1    0   
$EndComp
Wire Wire Line
	6500 4550 6050 4550
NoConn ~ 5850 3950
$Comp
L Device:L_Small L1
U 1 1 59E2854B
P 5750 6100
F 0 "L1" H 5780 6140 50  0000 L CNN
F 1 "Ferrite 1k" H 5780 6060 50  0000 L CNN
F 2 "Inductor_SMD:L_0402_1005Metric" V 5700 5700 50  0000 C CNN
F 3 "" H 5750 6100 50  0000 C CNN
F 4 "Optional: only needed for the RF version" H 100 850 50  0001 C CNN "Notes"
F 5 "ferrite-0402-1k" H 100 850 50  0001 C CNN "Key"
	1    5750 6100
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR014
U 1 1 59E287B3
P 5950 5950
F 0 "#PWR014" H 5950 5800 50  0001 C CNN
F 1 "VCC" H 5950 6100 50  0000 C CNN
F 2 "" H 5950 5950 50  0000 C CNN
F 3 "" H 5950 5950 50  0000 C CNN
	1    5950 5950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 59E28B92
P 5050 6250
F 0 "C8" H 5075 6350 50  0000 L CNN
F 1 "0.1uF" V 5200 6100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3900 5450 50  0000 C CNN
F 3 "" H 5050 6250 50  0000 C CNN
F 4 "cap-cer-0402-03high-100n" H -100 800 50  0001 C CNN "Key"
F 5 "Use flat versions that are not higher than 0.33mm. Optional: only needed for the RF version" H -100 800 50  0001 C CNN "Notes"
	1    5050 6250
	-1   0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 59E28F57
P 5450 6250
F 0 "C9" H 5475 6350 50  0000 L CNN
F 1 "0.1uF" V 5300 6100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 4500 5600 50  0000 C CNN
F 3 "" H 5450 6250 50  0000 C CNN
F 4 "cap-cer-0402-03high-100n" H 100 800 50  0001 C CNN "Key"
F 5 "Use flat versions that are not higher than 0.33mm. Optional: only needed for the RF version" H 100 800 50  0001 C CNN "Notes"
	1    5450 6250
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 59E2945F
P 5050 6500
F 0 "#PWR015" H 5050 6250 50  0001 C CNN
F 1 "GND" H 5050 6350 50  0000 C CNN
F 2 "" H 5050 6500 50  0000 C CNN
F 3 "" H 5050 6500 50  0000 C CNN
	1    5050 6500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 59E295B4
P 5450 6500
F 0 "#PWR016" H 5450 6250 50  0001 C CNN
F 1 "GND" H 5450 6350 50  0000 C CNN
F 2 "" H 5450 6500 50  0000 C CNN
F 3 "" H 5450 6500 50  0000 C CNN
	1    5450 6500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 59FA28EB
P 5200 2600
F 0 "#PWR017" H 5200 2350 50  0001 C CNN
F 1 "GND" H 5200 2450 50  0000 C CNN
F 2 "" H 5200 2600 50  0000 C CNN
F 3 "" H 5200 2600 50  0000 C CNN
	1    5200 2600
	0    1    1    0   
$EndComp
Wire Wire Line
	5500 2600 5200 2600
Wire Wire Line
	5300 2700 5500 2700
Text Label 5300 2700 0    60   ~ 0
BUZZ
Wire Wire Line
	5400 3950 5650 3950
Text Label 5650 3950 2    60   ~ 0
BUZZ
Wire Notes Line
	8500 1500 9200 1500
Wire Notes Line
	9200 1500 9200 2200
Wire Notes Line
	9200 2200 8500 2200
Wire Notes Line
	8500 2200 8500 1500
Wire Notes Line
	8500 2150 8550 2200
Wire Notes Line
	9150 2200 9200 2150
Wire Notes Line
	9200 1550 9150 1500
Wire Notes Line
	8550 1500 8500 1550
Wire Notes Line
	8650 2050 8650 1800
Wire Notes Line
	8650 1800 8900 1800
Wire Notes Line
	8900 1800 8900 2050
Wire Notes Line
	8900 2050 8650 2050
Wire Notes Line
	9100 2150 9100 2300
Wire Notes Line
	8600 2150 8600 2300
Wire Notes Line
	7950 2100 8550 2100
Wire Notes Line
	9150 2100 10150 2100
Wire Notes Line
	9150 1850 10150 1850
Wire Notes Line
	8800 1550 8800 1350
Wire Notes Line
	8800 1350 8950 1350
Text Notes 8800 1350 0    60   ~ 0
VCC
Text Notes 9100 2300 0    60   ~ 0
GND
Wire Notes Line
	9100 2300 9300 2300
Wire Notes Line
	8600 2300 9050 2300
Text Notes 8600 2300 0    60   ~ 0
RTS (TST)
Text Notes 8450 2100 2    60   ~ 0
DTR (!RST)
Text Notes 9250 2100 0    60   ~ 0
Device TX (Host RX)
Text Notes 9250 1850 0    60   ~ 0
Device RX (Host TX)
Wire Wire Line
	6500 4450 6500 4550
Wire Wire Line
	7450 3000 7450 3050
Wire Wire Line
	7450 3000 7700 3000
Wire Wire Line
	7450 3400 7450 3500
Wire Wire Line
	7450 3400 7700 3400
Wire Wire Line
	4200 5250 4500 5250
Wire Wire Line
	4500 5250 4600 5250
Wire Wire Line
	4600 5250 4800 5250
Wire Wire Line
	2150 1400 2950 1400
$Comp
L goodwatch31:0915LP15B02 U2
U 1 1 5B64268F
P 3850 7100
F 0 "U2" H 4150 7350 50  0000 L CNN
F 1 "0915LP15B02" H 4100 7450 50  0000 L CNN
F 2 "goodwatch31:LP15" H 3850 7100 50  0001 C CNN
F 3 "https://www.mouser.com/ds/2/611/LP_Filter_0500LP15A500_v2-258166.pdf, https://www.mouser.com/datasheet/2/611/JTI_Low-Pass-0915LP15B026_8-04-515344.pdf" H 3850 7100 50  0001 C CNN
F 4 "filter-0915lp15b02, filter-0915lp15b02" H 0   0   50  0001 C CNN "Key"
F 5 "Optional: only needed for the RF version; two options 500MHz/915MHz" H 0   0   50  0001 C CNN "Notes"
F 6 "0500LP15A500, 0915LP15B02" H 0   0   50  0001 C CNN "MFN"
F 7 "Johanson Technology Inc." H 0   0   50  0001 C CNN "Manufacturer"
	1    3850 7100
	1    0    0    -1  
$EndComp
$Comp
L goodwatch31:B0310J50100AH U3
U 1 1 5B642963
P 4850 7300
F 0 "U3" H 4850 7725 50  0000 C CNN
F 1 "B0310J50100AHF" H 4850 7634 50  0000 C CNN
F 2 "goodwatch31:B0310J50100AHF" H 4850 7300 50  0001 C CNN
F 3 "https://cdn.anaren.com/product-documents/Xinger/BalunTransformers/B0310J50100AHF/B0310J50100AHF_DataSheet(Rev_A).pdf" H 4850 7300 50  0001 C CNN
F 4 "balun-b0310j50100ahf" H 0   0   50  0001 C CNN "Key"
F 5 "Optional: only needed for the RF version" H 0   0   50  0001 C CNN "Notes"
F 6 "B0310J50100AH" H 0   0   50  0001 C CNN "MFN"
F 7 "Anaren" H 0   0   50  0001 C CNN "Manufacturer"
	1    4850 7300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C10
U 1 1 5B642CDA
P 3050 7100
F 0 "C10" H 3075 7200 50  0000 L CNN
F 1 "220pF" H 3075 7000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" V 3400 7000 50  0000 C CNN
F 3 "" H 3050 7100 50  0000 C CNN
F 4 "cap-cer-0402-220p" H 0   0   50  0001 C CNN "Key"
F 5 "Optional: only needed for the RF version" H 0   0   50  0001 C CNN "Notes"
	1    3050 7100
	0    1    1    0   
$EndComp
Wire Wire Line
	4450 7100 4200 7100
Wire Wire Line
	4450 7200 4200 7200
Wire Wire Line
	5250 7300 5550 7300
Wire Wire Line
	4450 7300 4200 7300
Wire Wire Line
	3500 7100 3200 7100
Text Label 4200 7200 0    60   ~ 0
GND
Text Label 4200 7300 0    60   ~ 0
RF_N
Text Label 5550 7300 2    60   ~ 0
RF_P
Wire Wire Line
	3750 6750 3750 6550
Wire Wire Line
	3950 6750 3950 6550
Wire Wire Line
	3750 7450 3750 7650
Wire Wire Line
	3950 7450 3950 7650
Text Label 3750 6550 3    60   ~ 0
GND
Text Label 3950 6550 3    60   ~ 0
GND
Text Label 3750 7650 1    60   ~ 0
GND
Text Label 3950 7650 1    60   ~ 0
GND
$Comp
L Connector:Conn_Coaxial J1
U 1 1 5B8D41DD
P 2350 6550
F 0 "J1" V 2450 6550 50  0000 L CNN
F 1 "ANT_U.FL" H 2150 6700 50  0000 L CNN
F 2 "Connector_Coaxial:U.FL_Molex_MCRF_73412-0110_Vertical" H 2350 6550 50  0001 C CNN
F 3 "https://www.molex.com/pdm_docs/sd/734120110_sd.pdf" H 2350 6550 50  0001 C CNN
F 4 "Optional: only used without watch body for the RF version" H 2350 6550 50  0001 C CNN "Notes"
F 5 "YES" H 2350 6550 50  0001 C CNN "DNP"
F 6 "conn-umcc" H 0   0   50  0001 C CNN "Key"
F 7 "0734120110" H 0   0   50  0001 C CNN "MFN"
F 8 "Molex" H 0   0   50  0001 C CNN "Manufacturer"
	1    2350 6550
	0    1    -1   0   
$EndComp
Wire Wire Line
	2800 7000 2800 7100
Wire Wire Line
	2800 7100 2900 7100
Wire Wire Line
	2800 7100 2350 7100
Wire Wire Line
	2350 7100 2350 6750
Connection ~ 2800 7100
$Comp
L power:GND #PWR0101
U 1 1 5B8FCF2F
P 2050 7200
F 0 "#PWR0101" H 2050 6950 50  0001 C CNN
F 1 "GND" H 2050 7050 50  0000 C CNN
F 2 "" H 2050 7200 50  0000 C CNN
F 3 "" H 2050 7200 50  0000 C CNN
	1    2050 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 6550 2050 6550
Wire Wire Line
	2050 6550 2050 7200
Text Label 5100 6100 0    60   ~ 0
RFVCC
Wire Wire Line
	4800 6100 5050 6100
Wire Wire Line
	4800 5250 4800 6100
Wire Wire Line
	5050 6100 5450 6100
Connection ~ 5050 6100
Wire Wire Line
	5650 6100 5450 6100
Connection ~ 5450 6100
Wire Wire Line
	5850 6100 5950 6100
Wire Wire Line
	5950 6100 5950 5950
$EndSCHEMATC
