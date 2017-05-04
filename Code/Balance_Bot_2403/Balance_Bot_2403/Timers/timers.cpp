/*
* Project Name: Balance_Bot_2403
* File Name: timers.cpp
*
* Created: 04-Dec-16 7:56:40 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for handling Timers
*
* Functions: timer1_init(), start_timer1(), timer3_init(), start_timer3(),
* timer4_init(), start_timer4(), epoch()
*
* Global Variables: time_ms, time_sec
*/

#include <avr/interrupt.h>
#include "timers.h"

volatile unsigned long int time_ms = 0;
volatile unsigned long int time_sec = 0;

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
void timer1_init()
{
	TCCR1B = 0x00; 		// Stop Timer
	TCNT1  = 0xFB80;	// 0.02s
	OCR1A  = 0x0000; 	// Output Compare Register (OCR) - Not used
	OCR1B  = 0x0000; 	// Output Compare Register (OCR) - Not used
	OCR1C  = 0x0000; 	// Output Compare Register (OCR) - Not used
	ICR1   = 0x0000; 	// Input Capture Register (ICR)  - Not used
	TCCR1A = 0x00;
	TCCR1C = 0x00;
}

/**********************************
Function name	:	start_timer1
Functionality	:	Start timer 1
Arguments		:	None
Return Value	:	None
Example Call	:	start_timer1()
***********************************/
void start_timer1()
{
	TCCR1B = 0x04; 		// Prescaler 256 1-0-0
	TIMSK1 = 0x01;		// Enable Timer Overflow Interrupt
}

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
void timer3_init()
{
	TCCR3B = 0x00; 		// Stop Timer
	TCNT3  = 0xFF70;	// 0.01s
	OCR3A  = 0x0000; 	// Output Compare Register (OCR) - Not used
	OCR3B  = 0x0000; 	// Output Compare Register (OCR) - Not used
	OCR3C  = 0x0000; 	// Output Compare Register (OCR) - Not used
	ICR3   = 0x0000; 	// Input Capture Register (ICR)  - Not used
	TCCR3A = 0x00;
	TCCR3C = 0x00;
}

/**********************************
Function name	:	start_timer3
Functionality	:	Start timer 3
Arguments		:	None
Return Value	:	None
Example Call	:	start_timer3()
***********************************/
void start_timer3()
{
	TCCR3B = 0x05; 		// Prescaler 1024 1-0-1
	TIMSK3 = 0x01;		// Enable Timer Overflow Interrupt
}

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
void timer4_init()
{
	TCCR4B = 0x00; 		// Stop Timer
	TCNT4  = 0xC667;	// 0.0009999593097s (~0.001s)
	OCR4A  = 0x0000; 	// Output Compare Register (OCR) - Not used
	OCR4B  = 0x0000; 	// Output Compare Register (OCR) - Not used
	OCR4C  = 0x0000; 	// Output Compare Register (OCR) - Not used
	ICR4   = 0x0000; 	// Input Capture Register (ICR)  - Not used
	TCCR4A = 0x00;
	TCCR4C = 0x00;
}

/**********************************
Function name	:	start_timer4
Functionality	:	Start timer 4
Arguments		:	None
Return Value	:	None
Example Call	:	start_timer4()
***********************************/
void start_timer4()
{
	TCCR4B = 0x01; 		// Prescaler None 0-0-1
	TIMSK4 = 0x01;		// Enable Timer Overflow Interrupt
}

/**********************************
Function name	:	ISR(TIMER4_OVF_vect)
Functionality	:	ISR for program clock
Arguments		:	Timer 4 overflow vector
Return Value	:	None
Example Call	:	Called automatically
***********************************/
ISR(TIMER4_OVF_vect)
{
	TCNT4 = 0xC667;		// Reload counter value
	time_ms++;			// Increment ms value
	
	if (time_ms>=1000)
	{
		time_ms = 0;	// Reset ms value
		time_sec++;		// Increments seconds value
	}
}

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
unsigned long epoch()
{
	unsigned long elapsed_time;
	elapsed_time = time_sec*1000 + time_ms;
	return elapsed_time;
}