/*
* Project Name: Balance_Bot_2403
* File Name: motors.cpp
*
* Created: 01-Jan-17 9:09:52 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for Motors and Encoders
*
* Functions: motor_pin_config, encoder_pin_config, set_motor_PWM,
* set_motor_pin, set_motor_mode, drive_motor, update_motors,
* left_encoder_interrupt, right_encoder_interrupt, motors_init
*
* Global Variables: left_encoder_count, right_encoder_count
*/

// Define parameters for Pin Change Interrupts Library
#define NO_PORTA_PINCHANGES
#define NO_PORTB_PINCHANGES
#define NO_PORTJ_PINCHANGES

#include <Arduino.h>
#include "../Support/digitalWriteFast.h"
#include "motors.h"

// Global variables
volatile float left_encoder_count = 0;
volatile float right_encoder_count = 0;

/**********************************
Function name	:	motor_pin_config
Functionality	:	To configure the motor pins
Arguments		:	None
Return Value	:	None
Example Call	:	motor_pin_config()
***********************************/
void motor_pin_config()
{
	// Set as output pins and initial state LOW
	DDRH  = DDRH | 0x30;	// PH5 - Motor A1
	PORTH = PORTH & 0xCF;	// PH4 - Motor A2
	
	DDRB  = DDRB | 0x60;	// PB5 - Motor B1
	PORTB = PORTB & 0x9F;	// PB6 - Motor B2
	
	DDRL  = DDRL | 0x18;	// PL3 - Enable A
	PORTL = PORTL & 0xE7;	// PL4 - Enable B
}

/**********************************
Function name	:	encoder_pin_config()
Functionality	:	To configure the quadrature encoder pins
					Encoder 1 - Channel A - PD2 - INT2
					Encoder 1 - Channel B - PD3 - INT3
					Encoder 2 - Channel A - PE4 - INT4
					Encoder 2 - Channel B - PE5 - INT5
Arguments		:	None
Return Value	:	None
Example Call	:	encoder_pin_config
***********************************/
void encoder_pin_config()
{
	// Set as input pins and enable internal pull-up
	pinMode(ENCA1, INPUT_PULLUP); // Encoder 1 - Channel A - PD2 - INT2
	pinMode(ENCA2, INPUT_PULLUP); // Encoder 1 - Channel B - PD3 - INT3
	
	pinMode(ENCB1, INPUT_PULLUP); // Encoder 2 - Channel A - PE4 - INT4
	pinMode(ENCB2, INPUT_PULLUP); // Encoder 2 - Channel B - PE5 - INT5
	
	// Attach interrupts for the encoder input pins
	attachInterrupt(digitalPinToInterrupt(ENCA1), left_encoder_interrupt, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENCB1), right_encoder_interrupt, CHANGE);
}

/**********************************
Function name	:	set_motor_PWM
Functionality	:	To set the PWM value for controlling the motor speed
Arguments		:	Motor type (LEFT/RIGHT), PWM motor speed to set
Return Value	:	None
Example Call	:	set_motor_PWM(LEFT, 255)
***********************************/
void set_motor_PWM(int motor, unsigned char motor_speed)
{
	if (motor==LEFT)  analogWrite(EA, motor_speed);
	if (motor==RIGHT) analogWrite(EB, motor_speed);
}

/**********************************
Function name	:	set_motor_pin
Functionality	:	To set the direction of the motors
Arguments		:	Motor type (LEFT/RIGHT), Pin 1 logic value, Pin 2 logic value
Return Value	:	None
Example Call	:	set_motor_pin(LEFT, HIGH, LOW)
***********************************/
void set_motor_pin(int motor, bool pin1, bool pin2)
{
	if (motor==LEFT)  PORTH = (pin1 << 5) | (pin2 << 4);
	if (motor==RIGHT) PORTB = (pin2 << 5) | (pin1 << 6);
}

/**********************************
Function name	:	set_motor_mode
Functionality	:	To set the direction/motion logic of the motors
Arguments		:	Motor type (LEFT/RIGHT), Direction value
Return Value	:	None
Example Call	:	set_motor_mode(LEFT, FORWARD)
***********************************/
void set_motor_mode(int motor, int mode)
{
	if (mode==FORWARD)	set_motor_pin(motor, LOW, HIGH);
	if (mode==BACK)		set_motor_pin(motor, HIGH, LOW);
	if (mode==COAST)	set_motor_pin(motor, LOW, LOW);
	if (mode==BRAKE)	set_motor_pin(motor, HIGH, HIGH);
}

/**********************************
Function name	:	drive_motor
Functionality	:	To set the direction/motion and speed of the robot based on the PWM value
Arguments		:	Motor type (LEFT/RIGHT), PWM Value, Minimum PWM value for motor
Return Value	:	None
Example Call	:	drive_motor(LEFT, 150, 80)
***********************************/
void drive_motor(int motor, float PWM_value, float min_value)
{
	// Coast the motors
	if (PWM_value == 0)
	{
		set_motor_PWM(motor, 0);
		set_motor_mode(motor, COAST);
	}
	
	// Move the robot forward
	else if (PWM_value > 0)
	{
		PWM_value = map(PWM_value, 0, 255, min_value, 255);		// Map the PWM values
		set_motor_PWM(motor, (unsigned char)PWM_value);			// Set motor speed
		set_motor_mode(motor, FORWARD);							// Set motor direction
	}
	
	// Move the robot back
	else if (PWM_value < 0)
	{
		PWM_value = map(PWM_value, 0, -255, min_value, 255);	// Map the PWM values
		set_motor_PWM(motor, (unsigned char)PWM_value);			// Set motor speed
		set_motor_mode(motor, BACK);							// Set motor direction
	}
}

/**********************************
Function name	:	update_motors
Functionality	:	To update the speed and direction od motors based on the PID values computed
Arguments		:	PID value computed, Left/Right rotation offset values
Return Value	:	None
Example Call	:	update_motors(150, 10, -10)
***********************************/
void update_motors(float PID_output, float left_offset, float right_offset)
{
	float left_PWM=0, right_PWM=0;
	
	// Add rotation offsets and constrain the output
	left_PWM = constrain(PID_output + left_offset, -255, 255);
	right_PWM = constrain(PID_output + right_offset, -255, 255);
	
	// Drive the motors
	drive_motor(LEFT, left_PWM, LEFT_PWM_MIN);
	drive_motor(RIGHT, right_PWM, RIGHT_PWM_MIN);
}

//                           _______         _______
//               Pin1 ______|       |_______|       |______ Pin1
// Positive <--          _______         _______         __       --> Negative
//               Pin2 __|       |_______|       |_______|   Pin2

/**********************************
Function name	:	left_encoder_interrupt
Functionality	:	To handle interrupt for left encoder channel A
Arguments		:	None
Return Value	:	None
Example Call	:	Called automatically
***********************************/
void left_encoder_interrupt()
{
	int state = digitalReadFast(ENCA1);
	if(digitalReadFast(ENCA2)) 
	state ? left_encoder_count-- : left_encoder_count++;
	else 
	state ? left_encoder_count++ : left_encoder_count--;
}

/**********************************
Function name	:	right_encoder_interrupt
Functionality	:	To handle interrupt for right encoder channel A
Arguments		:	None
Return Value	:	None
Example Call	:	Called automatically
***********************************/
void right_encoder_interrupt()
{
	int state = digitalReadFast(ENCB1);
	if(digitalReadFast(ENCB2)) 
	state ? right_encoder_count++ : right_encoder_count--;
	else 
	state ? right_encoder_count-- : right_encoder_count++;
}

/**********************************
Function name	:	motors_init
Functionality	:	To initiate the motors and encoders
Arguments		:	None
Return Value	:	None
Example Call	:	motors_init()
***********************************/
void motors_init()
{
	motor_pin_config();
	encoder_pin_config();
}