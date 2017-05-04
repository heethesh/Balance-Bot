/*
* Project Name: Balance_Bot_2403
* File Name: timers.h
*
* Created: 04-Dec-16 7:56:40 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for handling Timers
*/

#ifndef TIMERS_H_
#define TIMERS_H_

// Function Declarations

/**********************************
Function name	:	timer1_init
Functionality	:	TIMER1 Initialize - Prescaler: 256
					WGM: 0 Normal, TOP=0xFFFF, BOTTOM=0xFB80
					Desired value: 50Hz
					Actual value:  50Hz (0%)
Arguments		:	None
Return Value	:	None
Example Call	:	timer1_init()
***********************************/
void timer1_init();

/**********************************
Function name	:	timer3_init
Functionality	:	TIMER3 Initialize - Prescaler: 1024
					WGM: 0 Normal, TOP=0xFFFF, BOTTOM=0xFD30
					Desired value: 100Hz
					Actual value:  100Hz (0%)
Arguments		:	None
Return Value	:	None
Example Call	:	timer3_init()
***********************************/
void timer3_init();

/**********************************
Function name	:	timer4_init
Functionality	:	TIMER4 Initialize - Prescaler: None
					WGM: 0 Normal, TOP=0xFFFF, BOTTOM=0xC667
					Desired value: 1000Hz
					Actual value:  1000.040692Hz (0.00406%)
Arguments		:	None
Return Value	:	None
Example Call	:	timer4_init()
***********************************/
void timer4_init();

/**********************************
Function name	:	start_timer1
Functionality	:	Start timer 1
Arguments		:	None
Return Value	:	None
Example Call	:	start_timer1()
***********************************/
void start_timer1();

/**********************************
Function name	:	start_timer3
Functionality	:	Start timer 3
Arguments		:	None
Return Value	:	None
Example Call	:	start_timer3()
***********************************/
void start_timer3();

/**********************************
Function name	:	start_timer4
Functionality	:	Start timer 4
Arguments		:	None
Return Value	:	None
Example Call	:	start_timer4()
***********************************/
void start_timer4();

/**********************************
Function name	:	epoch
Functionality	:	To keep program time from the start in ms
					NOTE: The function millis() provided in the Arduino library was found
					to have an offset of ~4ms when visualized in XCOS, SCILAB. Instead, the
					epoch() function will be used to keep track of the program time in ms.
Arguments		:	None
Return Value	:	Current program time
Example Call	:	epoch()
***********************************/
unsigned long epoch();

#endif