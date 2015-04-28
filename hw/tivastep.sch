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
LIBS:special
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
LIBS:tivastep-cache
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
$Comp
L CONN_02X10 P?
U 1 1 553E31F3
P 1750 1950
F 0 "P?" H 1750 2500 50  0000 C CNN
F 1 "CONN_02X10" V 1750 1950 50  0000 C CNN
F 2 "" H 1750 750 60  0000 C CNN
F 3 "" H 1750 750 60  0000 C CNN
	1    1750 1950
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X10 P?
U 1 1 553E3237
P 4050 1950
F 0 "P?" H 4050 2500 50  0000 C CNN
F 1 "CONN_02X10" V 4050 1950 50  0000 C CNN
F 2 "" H 4050 750 60  0000 C CNN
F 3 "" H 4050 750 60  0000 C CNN
	1    4050 1950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P?
U 1 1 553E33F2
P 8900 3100
F 0 "P?" H 8900 3550 50  0000 C CNN
F 1 "CONN_01X08" H 8950 2600 50  0000 C CNN
F 2 "" H 8900 3100 60  0000 C CNN
F 3 "" H 8900 3100 60  0000 C CNN
	1    8900 3100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR?
U 1 1 553E34D7
P 7400 1950
F 0 "#PWR?" H 7400 2050 30  0001 C CNN
F 1 "VCC" H 7400 2050 30  0000 C CNN
F 2 "" H 7400 1950 60  0000 C CNN
F 3 "" H 7400 1950 60  0000 C CNN
	1    7400 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 2750 8700 2750
Wire Wire Line
	8600 2950 8700 2950
Connection ~ 8600 2750
Wire Wire Line
	8600 3150 8700 3150
Connection ~ 8600 2950
Text Notes 9000 2800 0    60   ~ 0
PUL+
Text Notes 9000 2900 0    60   ~ 0
PUL-
Text Notes 9000 3000 0    60   ~ 0
DIR+
Text Notes 9000 3100 0    60   ~ 0
DIR-
Text Notes 9000 3200 0    60   ~ 0
ENA+
Text Notes 9000 3300 0    60   ~ 0
ENA-
Text Notes 9000 3400 0    60   ~ 0
FLT+
Text Notes 9000 3500 0    60   ~ 0
FLT-
$Comp
L GND #PWR?
U 1 1 553E3561
P 8600 3550
F 0 "#PWR?" H 8600 3550 30  0001 C CNN
F 1 "GND" H 8600 3480 30  0001 C CNN
F 2 "" H 8600 3550 60  0000 C CNN
F 3 "" H 8600 3550 60  0000 C CNN
	1    8600 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 3450 8600 3450
Wire Wire Line
	8600 3450 8600 3550
$Comp
L R R?
U 1 1 553E3581
P 6800 2300
F 0 "R?" V 6880 2300 40  0000 C CNN
F 1 "470" V 6807 2301 40  0000 C CNN
F 2 "" V 6730 2300 30  0000 C CNN
F 3 "" H 6800 2300 30  0000 C CNN
	1    6800 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 3350 8000 3350
Wire Wire Line
	7800 3350 7800 4950
$Comp
L NPN Q?
U 1 1 553E3647
P 7100 3950
F 0 "Q?" H 7100 3800 50  0000 R CNN
F 1 "NPN" H 7100 4100 50  0000 R CNN
F 2 "" H 7100 3950 60  0000 C CNN
F 3 "" H 7100 3950 60  0000 C CNN
	1    7100 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 3250 7200 3750
$Comp
L R R?
U 1 1 553E378F
P 6650 3950
F 0 "R?" V 6730 3950 40  0000 C CNN
F 1 "8k2" V 6657 3951 40  0000 C CNN
F 2 "" V 6580 3950 30  0000 C CNN
F 3 "" H 6650 3950 30  0000 C CNN
	1    6650 3950
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 553E37F8
P 7200 4150
F 0 "#PWR?" H 7200 4150 30  0001 C CNN
F 1 "GND" H 7200 4080 30  0001 C CNN
F 2 "" H 7200 4150 60  0000 C CNN
F 3 "" H 7200 4150 60  0000 C CNN
	1    7200 4150
	1    0    0    -1  
$EndComp
$Comp
L NPN Q?
U 1 1 553E39FD
P 7300 4550
F 0 "Q?" H 7300 4400 50  0000 R CNN
F 1 "NPN" H 7300 4700 50  0000 R CNN
F 2 "" H 7300 4550 60  0000 C CNN
F 3 "" H 7300 4550 60  0000 C CNN
	1    7300 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 3050 7400 4350
$Comp
L NPN Q?
U 1 1 553E3A7D
P 7500 5150
F 0 "Q?" H 7500 5000 50  0000 R CNN
F 1 "NPN" H 7500 5300 50  0000 R CNN
F 2 "" H 7500 5150 60  0000 C CNN
F 3 "" H 7500 5150 60  0000 C CNN
	1    7500 5150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E3AE3
P 7400 4750
F 0 "#PWR?" H 7400 4750 30  0001 C CNN
F 1 "GND" H 7400 4680 30  0001 C CNN
F 2 "" H 7400 4750 60  0000 C CNN
F 3 "" H 7400 4750 60  0000 C CNN
	1    7400 4750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E3AF6
P 7600 5350
F 0 "#PWR?" H 7600 5350 30  0001 C CNN
F 1 "GND" H 7600 5280 30  0001 C CNN
F 2 "" H 7600 5350 60  0000 C CNN
F 3 "" H 7600 5350 60  0000 C CNN
	1    7600 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 2850 7600 4950
$Comp
L R R?
U 1 1 553E3B41
P 6850 4550
F 0 "R?" V 6930 4550 40  0000 C CNN
F 1 "8k2" V 6857 4551 40  0000 C CNN
F 2 "" V 6780 4550 30  0000 C CNN
F 3 "" H 6850 4550 30  0000 C CNN
	1    6850 4550
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 553E3B7C
P 7050 5150
F 0 "R?" V 7130 5150 40  0000 C CNN
F 1 "8k2" V 7057 5151 40  0000 C CNN
F 2 "" V 6980 5150 30  0000 C CNN
F 3 "" H 7050 5150 30  0000 C CNN
	1    7050 5150
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 553E3CC2
P 6450 3350
F 0 "R?" V 6530 3350 40  0000 C CNN
F 1 "8k2" V 6457 3351 40  0000 C CNN
F 2 "" V 6380 3350 30  0000 C CNN
F 3 "" H 6450 3350 30  0000 C CNN
	1    6450 3350
	0    1    1    0   
$EndComp
Connection ~ 7800 3350
Text GLabel 6100 3350 0    60   Input ~ 0
FLT
Text GLabel 6300 3950 0    60   Input ~ 0
ENA
Text GLabel 6500 4550 0    60   Input ~ 0
DIR
Text GLabel 6700 5150 0    60   Input ~ 0
PUL
Wire Wire Line
	6700 5150 6800 5150
Wire Wire Line
	6600 4550 6500 4550
Wire Wire Line
	6300 3950 6400 3950
Wire Wire Line
	6100 3350 6200 3350
$Comp
L R R?
U 1 1 553E423F
P 8250 2850
F 0 "R?" V 8150 2850 40  0000 C CNN
F 1 "18" V 8257 2851 40  0000 C CNN
F 2 "" V 8180 2850 30  0000 C CNN
F 3 "" H 8250 2850 30  0000 C CNN
	1    8250 2850
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 553E4357
P 8250 3050
F 0 "R?" V 8150 3050 40  0000 C CNN
F 1 "18" V 8257 3051 40  0000 C CNN
F 2 "" V 8180 3050 30  0000 C CNN
F 3 "" H 8250 3050 30  0000 C CNN
	1    8250 3050
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 553E4384
P 8250 3250
F 0 "R?" V 8200 3000 40  0000 C CNN
F 1 "18" V 8257 3251 40  0000 C CNN
F 2 "" V 8180 3250 30  0000 C CNN
F 3 "" H 8250 3250 30  0000 C CNN
	1    8250 3250
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 553E43B6
P 8250 3350
F 0 "R?" V 8300 3100 40  0000 C CNN
F 1 "18" V 8250 3350 40  0000 C CNN
F 2 "" V 8180 3350 30  0000 C CNN
F 3 "" H 8250 3350 30  0000 C CNN
	1    8250 3350
	0    1    1    0   
$EndComp
Wire Wire Line
	7600 2850 8000 2850
Wire Wire Line
	7400 3050 8000 3050
Wire Wire Line
	7200 3250 8000 3250
Wire Wire Line
	8500 3350 8700 3350
Wire Wire Line
	8700 3250 8500 3250
Wire Wire Line
	8500 3050 8700 3050
Wire Wire Line
	8700 2850 8500 2850
$Comp
L +3.3V #PWR?
U 1 1 553E4763
P 7100 1950
F 0 "#PWR?" H 7100 1910 30  0001 C CNN
F 1 "+3.3V" H 7100 2060 30  0000 C CNN
F 2 "" H 7100 1950 60  0000 C CNN
F 3 "" H 7100 1950 60  0000 C CNN
	1    7100 1950
	1    0    0    -1  
$EndComp
$Comp
L ZENER D?
U 1 1 553E4D66
P 7800 5150
F 0 "D?" H 7800 5250 50  0000 C CNN
F 1 "5V6" H 7800 5050 40  0000 C CNN
F 2 "" H 7800 5150 60  0000 C CNN
F 3 "" H 7800 5150 60  0000 C CNN
	1    7800 5150
	0    -1   -1   0   
$EndComp
$Comp
L ZENER D?
U 1 1 553E4EAD
P 8100 5150
F 0 "D?" H 8100 5250 50  0000 C CNN
F 1 "5V6" H 8100 5050 40  0000 C CNN
F 2 "" H 8100 5150 60  0000 C CNN
F 3 "" H 8100 5150 60  0000 C CNN
	1    8100 5150
	0    -1   -1   0   
$EndComp
$Comp
L ZENER D?
U 1 1 553E4F68
P 8400 5150
F 0 "D?" H 8400 5250 50  0000 C CNN
F 1 "5V6" H 8400 5050 40  0000 C CNN
F 2 "" H 8400 5150 60  0000 C CNN
F 3 "" H 8400 5150 60  0000 C CNN
	1    8400 5150
	0    -1   -1   0   
$EndComp
$Comp
L ZENER D?
U 1 1 553E4F96
P 8700 5150
F 0 "D?" H 8700 5250 50  0000 C CNN
F 1 "5V6" H 8700 5050 40  0000 C CNN
F 2 "" H 8700 5150 60  0000 C CNN
F 3 "" H 8700 5150 60  0000 C CNN
	1    8700 5150
	0    -1   -1   0   
$EndComp
Connection ~ 7600 4800
Connection ~ 7400 4250
Connection ~ 7200 3650
Wire Wire Line
	7200 3650 8100 3650
Wire Wire Line
	8100 3650 8100 4950
Wire Wire Line
	7400 4250 8400 4250
Wire Wire Line
	8400 4250 8400 4950
Wire Wire Line
	7600 4800 8700 4800
Wire Wire Line
	8700 4800 8700 4950
$Comp
L GND #PWR?
U 1 1 553E531B
P 7800 5350
F 0 "#PWR?" H 7800 5350 30  0001 C CNN
F 1 "GND" H 7800 5280 30  0001 C CNN
F 2 "" H 7800 5350 60  0000 C CNN
F 3 "" H 7800 5350 60  0000 C CNN
	1    7800 5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E541A
P 8100 5350
F 0 "#PWR?" H 8100 5350 30  0001 C CNN
F 1 "GND" H 8100 5280 30  0001 C CNN
F 2 "" H 8100 5350 60  0000 C CNN
F 3 "" H 8100 5350 60  0000 C CNN
	1    8100 5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E5471
P 8400 5350
F 0 "#PWR?" H 8400 5350 30  0001 C CNN
F 1 "GND" H 8400 5280 30  0001 C CNN
F 2 "" H 8400 5350 60  0000 C CNN
F 3 "" H 8400 5350 60  0000 C CNN
	1    8400 5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E548F
P 8700 5350
F 0 "#PWR?" H 8700 5350 30  0001 C CNN
F 1 "GND" H 8700 5280 30  0001 C CNN
F 2 "" H 8700 5350 60  0000 C CNN
F 3 "" H 8700 5350 60  0000 C CNN
	1    8700 5350
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 553E55F2
P 7100 2350
F 0 "C?" H 7100 2450 40  0000 L CNN
F 1 "C" H 7106 2265 40  0000 L CNN
F 2 "" H 7138 2200 30  0000 C CNN
F 3 "" H 7100 2350 60  0000 C CNN
	1    7100 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E57A9
P 7100 2550
F 0 "#PWR?" H 7100 2550 30  0001 C CNN
F 1 "GND" H 7100 2480 30  0001 C CNN
F 2 "" H 7100 2550 60  0000 C CNN
F 3 "" H 7100 2550 60  0000 C CNN
	1    7100 2550
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 553E5963
P 8100 2350
F 0 "C?" H 8100 2450 40  0000 L CNN
F 1 "C" H 8106 2265 40  0000 L CNN
F 2 "" H 8138 2200 30  0000 C CNN
F 3 "" H 8100 2350 60  0000 C CNN
	1    8100 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 2050 8600 3150
$Comp
L FUSE F?
U 1 1 553E5C01
P 7750 2050
F 0 "F?" V 7700 2200 40  0000 C CNN
F 1 "50mA PTC" V 7800 2300 40  0000 C CNN
F 2 "" H 7750 2050 60  0000 C CNN
F 3 "" H 7750 2050 60  0000 C CNN
	1    7750 2050
	-1   0    0    1   
$EndComp
$Comp
L C C?
U 1 1 553E5C83
P 7400 2350
F 0 "C?" H 7400 2450 40  0000 L CNN
F 1 "C" H 7406 2265 40  0000 L CNN
F 2 "" H 7438 2200 30  0000 C CNN
F 3 "" H 7400 2350 60  0000 C CNN
	1    7400 2350
	-1   0    0    1   
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 553E6711
P 1400 1400
F 0 "#PWR?" H 1400 1360 30  0001 C CNN
F 1 "+3.3V" H 1400 1510 30  0000 C CNN
F 2 "" H 1400 1400 60  0000 C CNN
F 3 "" H 1400 1400 60  0000 C CNN
	1    1400 1400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR?
U 1 1 553E688C
P 2100 1400
F 0 "#PWR?" H 2100 1500 30  0001 C CNN
F 1 "VCC" H 2100 1500 30  0000 C CNN
F 2 "" H 2100 1400 60  0000 C CNN
F 3 "" H 2100 1400 60  0000 C CNN
	1    2100 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553E6A4C
P 4400 2500
F 0 "#PWR?" H 4400 2500 30  0001 C CNN
F 1 "GND" H 4400 2430 30  0001 C CNN
F 2 "" H 4400 2500 60  0000 C CNN
F 3 "" H 4400 2500 60  0000 C CNN
	1    4400 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 2500 4400 1500
Wire Wire Line
	4400 1500 4300 1500
Wire Wire Line
	2000 1500 2100 1500
Wire Wire Line
	2100 1500 2100 1400
Wire Wire Line
	1400 1400 1400 1500
Wire Wire Line
	1400 1500 1500 1500
$Comp
L CONN_01X06 P?
U 1 1 553F2AC4
P 9900 3100
F 0 "P?" H 9900 3450 50  0000 C CNN
F 1 "CONN_01X06" H 9900 2700 50  0000 C CNN
F 2 "" H 9900 3100 60  0000 C CNN
F 3 "" H 9900 3100 60  0000 C CNN
	1    9900 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 2750 9450 2750
Wire Wire Line
	9450 2750 9550 2850
Wire Wire Line
	9550 2850 9700 2850
Wire Wire Line
	9300 2850 9400 2850
Wire Wire Line
	9400 2850 9500 2950
Wire Wire Line
	9500 2950 9700 2950
Wire Wire Line
	9300 3050 9700 3050
Wire Wire Line
	9300 3250 9400 3250
Wire Wire Line
	9400 3250 9500 3150
Wire Wire Line
	9500 3150 9700 3150
Wire Wire Line
	9300 3350 9450 3350
Wire Wire Line
	9450 3350 9550 3250
Wire Wire Line
	9550 3250 9700 3250
Wire Wire Line
	9700 3350 9600 3350
Wire Wire Line
	9600 3350 9500 3450
Wire Wire Line
	9500 3450 9300 3450
Text Notes 10000 3400 0    60   ~ 0
FLT-
Text Notes 10000 3300 0    60   ~ 0
FLT+
Text Notes 10000 3200 0    60   ~ 0
ENA-
Text Notes 10000 3100 0    60   ~ 0
DIR-
Text Notes 10000 3000 0    60   ~ 0
PUL-
Text Notes 10000 2900 0    60   ~ 0
PUL+/DIR+/ENA+
Text Label 8600 2600 0    60   ~ 0
PUL+/DIR+/ENA+
Text Label 9400 2750 0    60   ~ 0
PUL+/DIR+/ENA+
Text Label 8650 2850 0    60   ~ 0
PUL-
Text Label 8650 3050 0    60   ~ 0
DIR-
Text Label 8600 3250 0    60   ~ 0
ENA-
Text Label 8600 3350 0    60   ~ 0
FLT+
Wire Wire Line
	4300 2100 4600 2100
Wire Wire Line
	4300 2200 4600 2200
Wire Wire Line
	4300 2300 4600 2300
Wire Wire Line
	4300 2400 4600 2400
Text GLabel 4600 2100 2    47   Input ~ 0
FLT
Text GLabel 4600 2200 2    47   Input ~ 0
ENA
Text GLabel 4600 2300 2    47   Input ~ 0
DIR
Text GLabel 4600 2400 2    47   Input ~ 0
PUL
Wire Wire Line
	8000 2050 8600 2050
Wire Wire Line
	8100 2150 8100 2050
Connection ~ 8100 2050
Wire Wire Line
	7500 2050 7400 2050
Wire Wire Line
	7400 1950 7400 2150
$Comp
L GND #PWR?
U 1 1 553F4B48
P 7400 2550
F 0 "#PWR?" H 7400 2550 30  0001 C CNN
F 1 "GND" H 7400 2480 30  0001 C CNN
F 2 "" H 7400 2550 60  0000 C CNN
F 3 "" H 7400 2550 60  0000 C CNN
	1    7400 2550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553F4B6B
P 8100 2550
F 0 "#PWR?" H 8100 2550 30  0001 C CNN
F 1 "GND" H 8100 2480 30  0001 C CNN
F 2 "" H 8100 2550 60  0000 C CNN
F 3 "" H 8100 2550 60  0000 C CNN
	1    8100 2550
	1    0    0    -1  
$EndComp
Connection ~ 7400 2050
Wire Wire Line
	7100 1950 7100 2150
$Comp
L +3.3V #PWR?
U 1 1 553F5180
P 6800 1950
F 0 "#PWR?" H 6800 1910 30  0001 C CNN
F 1 "+3.3V" H 6800 2060 30  0000 C CNN
F 2 "" H 6800 1950 60  0000 C CNN
F 3 "" H 6800 1950 60  0000 C CNN
	1    6800 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 1950 6800 2050
Wire Wire Line
	6800 2550 6800 3350
Connection ~ 6800 3350
$Comp
L ZENER D?
U 1 1 553F5B77
P 8400 2350
F 0 "D?" H 8400 2450 50  0000 C CNN
F 1 "5V6" H 8400 2250 40  0000 C CNN
F 2 "" H 8400 2350 60  0000 C CNN
F 3 "" H 8400 2350 60  0000 C CNN
	1    8400 2350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8400 2150 8400 2050
Connection ~ 8400 2050
$Comp
L GND #PWR?
U 1 1 553F5E0D
P 8400 2550
F 0 "#PWR?" H 8400 2550 30  0001 C CNN
F 1 "GND" H 8400 2480 30  0001 C CNN
F 2 "" H 8400 2550 60  0000 C CNN
F 3 "" H 8400 2550 60  0000 C CNN
	1    8400 2550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 553F5F2B
P 2100 2500
F 0 "#PWR?" H 2100 2500 30  0001 C CNN
F 1 "GND" H 2100 2430 30  0001 C CNN
F 2 "" H 2100 2500 60  0000 C CNN
F 3 "" H 2100 2500 60  0000 C CNN
	1    2100 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 1600 2100 1600
Wire Wire Line
	2100 1600 2100 2500
Text Notes 2150 1550 0    60   ~ 0
VBUS
Text Notes 1050 1550 0    60   ~ 0
+3.3V
Text Notes 2150 1650 0    60   ~ 0
GND
Text Notes 4450 1550 0    60   ~ 0
GND
Text Notes 4850 2150 0    60   ~ 0
PB6
Text Notes 4850 2250 0    60   ~ 0
PA4
Text Notes 4850 2350 0    60   ~ 0
PA3
Text Notes 4850 2450 0    60   ~ 0
PA2
$EndSCHEMATC
