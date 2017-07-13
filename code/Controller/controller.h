/*
* Project Name: Balance_Bot_2403
* File Name: controller.h
*
* Created: 13-Feb-17 8:25:05 PM
* Author: Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for the Joystick Controller
*/

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

// Structure to handle the various inputs from the controller
typedef struct JoystickController
{
	int x_position;		// Joystick X-Axis ADC Value
	int y_position;		// Joystick Y-Axis ADC Value
	
	bool button_1;		// Push Button 1 Value
	bool button_2;		// Push Button 2 Value
	bool button_3;		// Push Button 3 Value
	bool button_4;		// Push Button 4 Value
	
	// De-bounce time for the push buttons
	unsigned long b1_time;
	unsigned long b2_time;
	unsigned long b3_time;
	unsigned long b4_time;
};

extern JoystickController joystick;


// Function Declarations

/**********************************
Function name	:	get_joystick_zone
Functionality	:	To map the Joystick ADC input values to 5 zones (±2, ±1, 0)
Arguments		:	Low and High ADC bytes, Offset ADC value
Return Value	:	Zone value
Example Call	:	get_joystick_zone(255, 10, 22)
***********************************/
int get_joystick_zone(unsigned char low_byte, unsigned char high_byte, int offset);

/**********************************
Function name	:	read_joystick
Functionality	:	To read the data from the controller XBee Module and store it
Arguments		:	None
Return Value	:	None
Example Call	:	read_joystick()
***********************************/
void read_joystick();

#endif