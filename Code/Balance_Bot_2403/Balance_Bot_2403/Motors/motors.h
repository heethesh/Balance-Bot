/*
* Project Name: Balance_Bot_2403
* File Name: motors.h
*
* Created: 01-Jan-17 9:09:52 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for Motors and Encoders
*/

#include <Arduino.h>

#ifndef MOTORS_H_
#define MOTORS_H_

// Define Pin Numbers (Arduino Mega)
#define MA1		8
#define MA2		7
#define EA		46
#define MB1		11
#define MB2		12
#define EB		45
#define ENCA1	19
#define ENCA2	18
#define ENCB1	2
#define ENCB2	3

// Define Operating Modes
#define COAST	0
#define FORWARD	1
#define BACK	2
#define LEFT	3
#define RIGHT	4
#define BRAKE	5

// Define Minimum PWM Values for Motors
#define LEFT_PWM_MIN 35
#define RIGHT_PWM_MIN 42

extern volatile float left_encoder_count;
extern volatile float right_encoder_count;


// Function Declarations

/**********************************
Function name	:	motor_pin_config
Functionality	:	To configure the motor pins
Arguments		:	None
Return Value	:	None
Example Call	:	motor_pin_config()
***********************************/
void motor_pin_config();

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
void encoder_pin_config();

/**********************************
Function name	:	set_motor_PWM
Functionality	:	To set the PWM value for controlling the motor speed
Arguments		:	Motor type (LEFT/RIGHT), PWM motor speed to set
Return Value	:	None
Example Call	:	set_motor_PWM(LEFT, 255)
***********************************/
void set_motor_PWM(int motor, unsigned char motor_speed);

/**********************************
Function name	:	set_motor_pin
Functionality	:	To set the direction of the motors
Arguments		:	Motor type (LEFT/RIGHT), Pin 1 logic value, Pin 2 logic value
Return Value	:	None
Example Call	:	set_motor_pin(LEFT, HIGH, LOW)
***********************************/
void set_motor_pin(int motor, bool pin1, bool pin2);

/**********************************
Function name	:	set_motor_mode
Functionality	:	To set the direction/motion logic of the motors
Arguments		:	Motor type (LEFT/RIGHT), Direction value
Return Value	:	None
Example Call	:	set_motor_mode(LEFT, FORWARD)
***********************************/
void set_motor_mode(int motor, int mode);

/**********************************
Function name	:	drive_motor
Functionality	:	To set the direction/motion and speed of the robot based on the PWM value
Arguments		:	Motor type (LEFT/RIGHT), PWM Value, Minimum PWM value for motor
Return Value	:	None
Example Call	:	drive_motor(LEFT, 150, 80)
***********************************/
void drive_motor(int motor, float PWM_value, float min_value);

/**********************************
Function name	:	update_motors
Functionality	:	To update the speed and direction od motors based on the PID values computed
Arguments		:	PID value computed, Left/Right rotation offset values
Return Value	:	None
Example Call	:	update_motors(150, 10, -10)
***********************************/
void update_motors(float PID_output, float left_offset, float right_offset);

/**********************************
Function name	:	left_encoder_interrupt
Functionality	:	To handle interrupt for left encoder channel A
Arguments		:	None
Return Value	:	None
Example Call	:	Called automatically
***********************************/
void left_encoder_interrupt();

/**********************************
Function name	:	right_encoder_interrupt
Functionality	:	To handle interrupt for right encoder channel A
Arguments		:	None
Return Value	:	None
Example Call	:	Called automatically
***********************************/
void right_encoder_interrupt();

/**********************************
Function name	:	motors_init
Functionality	:	To initiate the motors and encoders
Arguments		:	None
Return Value	:	None
Example Call	:	motors_init()
***********************************/
void motors_init();

#endif