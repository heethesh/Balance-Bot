/*
* PROJECT DETAILS:
* Project Name: Balance_Bot_2403
* File Name: Balance_Bot_2403.cpp
* Version: 4.2.0
* Release Date: 05-May-17 6:14:00 AM
*
* Created: 10-Jan-17 6:40:00 AM
* Author : Heethesh Vhavle
* Email: heethesh@gmail.com
*
* GitHub: https://github.com/heethesh/Balance-Bot
* YouTube video demo link: https://youtu.be/-k-lZ_CcU4U
*
* Team ID: eYRC-BB#2403
* Theme: Balance Bot
*
* First prize winner at National Level e-Yantra Robotics Competition 2016-17
* conducted by IIT Bombay, India and sponsored by MHRD, Government of India
* 
* PURPOSE:
* Program to control a balance bot using Cascaded PID control architecture
* Four PID controllers are implemented to control the position, velocity
* rotation and the tilt angle of the robot.
*
* USAGE:
* Refer the wiki for further details on the working of the robot.
*
* LICENSE:
* MIT License
*
* Copyright (c) 2017 Heethesh Vhavle
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sub license, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

// Libraries
#include <Arduino.h>
#include "Timers/timers.h"
#include "I2C/i2c_lib.h"
#include "Accelerometer/accel.h"
#include "Gyroscope/gyro.h"
#include "Motors/motors.h"
#include "Controller/controller.h"
#include "Indicators/indicators.h"
#include "Balance_Bot_2403.h"

/**********************************
Function name	:	ISR(TIMER3_OVF_vect)
Functionality	:	ISR for measuring the tilt angle at 100Hz
Arguments		:	Timer 3 overflow vector
Return Value	:	None
Example Call	:	Called automatically
***********************************/
ISR(TIMER3_OVF_vect)
{
	TIMSK3 = 0x00;	
	read_tilt_angle();	
	TCNT3 = 0xFF70;
	TIMSK3 = 0x01;
}

/**********************************
Function name	:	ISR(TIMER1_OVF_vect)
Functionality	:	ISR for measuring the RPM of DC Motors at 50Hz
Arguments		:	Timer 1 overflow vector
Return Value	:	None
Example Call	:	Called automatically
***********************************/
ISR(TIMER1_OVF_vect)
{
	TCNT1 = 0xFB80;
	
	// Make a local copy of the global encoder count
	volatile float left_current_count = left_encoder_count;
	volatile float right_current_count = right_encoder_count;
	
	//		 (Change in encoder count) * (60 sec/1 min)
	// RPM = __________________________________________
	//		 (Change in time --> 20ms) * (PPR --> 840)
	left_RPM = (float)(((left_current_count - left_prev_count) * 60)/(0.02*420));
	right_RPM = (float)(((right_current_count - right_prev_count) * 60)/(0.02*420));
	
	// Store current encoder count for next iteration
	left_prev_count = left_current_count;
	right_prev_count = right_current_count;
}

/**********************************
Function name	:	encoder_count
Functionality	:	Returns the current encoder count
Arguments		:	None
Return Value	:	Encoder count sum
Example Call	:	encoder_count()
***********************************/
float encoder_count()
{
	return (left_encoder_count + right_encoder_count);
}

/**********************************
Function name	:	complimentary_filter
Functionality	:	First order complimentary filter for sensor fusion
Arguments		:	Sensor data to fuse, alpha value
Return Value	:	Fused value
Example Call	:	complimentary_filter(gyro_angle, accel_angle, 0.98)
***********************************/
float complimentary_filter(float angle1, float angle2, float alpha)
{
	return (alpha*angle1 + (1-alpha)*angle2);
}

/**********************************
Function name	:	read_tilt_angle
Functionality	:	Compute tilt angle from Gyroscope, Accelerometer and Complimentary Filter
Arguments		:	None
Return Value	:	None
Example Call	:	read_tilt_angle()
***********************************/
void read_tilt_angle()
{
	// Compute pitch angle from Gyroscope
	gyro_angle = get_gyro_angle(epoch(), angle.position);
	
	// Compute pitch angle from Accelerometer
	accel_angle = read_accelerometer();
	
	// Fuse the pitch angles using a Complimentary Filter
	angle.position = complimentary_filter(gyro_angle, accel_angle, COMP_FILTER_ALPHA);
}

/**********************************
Function name	:	handle_buttons
Functionality	:	To handle the button inputs of the Joystick Controller 
					and update the status/motion of the robot accordingly
Arguments		:	None
Return Value	:	None
Example Call	:	handle_buttons()
***********************************/
void handle_buttons()
{
	// Button 4 - Move Forward
	if ((joystick.button_4 == HIGH) && ((epoch() - joystick.b4_time) >= 1))
	{
		joystick.b4_time = epoch();
		
		// Update set-point
		encoder.set_point += 0.1*FULL_SPEED; // Increment the Position Set Point at a Defined Speed
		velocity.set_point = FORWARD_SPEED;
		move_offset = -0.05;
		
		// Update flags
		set_led_indicators(false, true, false, false, false, false);
		
		// Set STOP_FLAG to False to avoid Static Balance/Position Holding
		STOP_FLAG = false;
		SLOPE_FLAG = false;
	}
	
	// Button 1 - Move Back
	else if ((joystick.button_1 == HIGH) && ((epoch() - joystick.b1_time) >= 1))
	{
		joystick.b1_time = epoch();
		
		// Update set-point
		encoder.set_point -= 0.1*FULL_SPEED; // Increment the Position Set Point at a Defined Speed
		velocity.set_point = -REVERSE_SPEED;
		move_offset = 0.05;
		
		// Update flags
		set_led_indicators(false, false, true, false, false, false);
		
		// Set STOP_FLAG to False to avoid Static Balance/Position Holding
		STOP_FLAG = false;
		SLOPE_FLAG = false;
	}
	
	// Button 2 - Slope Mode
	else if ((joystick.button_2 == HIGH) && ((epoch() - joystick.b2_time) >= 1))
	{
		joystick.b2_time = epoch();
		
		// Gradually increase slope offset angle
		slope_offset -= 0.1;
		slope_offset = constrain(slope_offset, -SLOPE_ANGLE, 0);
		velocity.set_point = SLOPE_SPEED;
		
		// Update flags
		set_led_indicators(false, false, false, false, false, true);
		
		// Set STOP_FLAG to False to avoid Static Balance/Position Holding
		STOP_FLAG = false;
		SLOPE_FLAG = true;
	}
	
	// Button 0 - Hold Position
	else if (!STOP_FLAG)
	{
		// Reset count to current position
		left_encoder_count = right_encoder_count = 0;
		encoder.set_point = encoder_count();
		
		velocity.set_point = 0;
		slope_offset = 0;
		move_offset = 0;
		
		// Update flags
		set_led_indicators(true, false, false, false, false, false);
		STOP_FLAG = true;
		SLOPE_FLAG = false;
	}

	// Button 3 - Sound Buzzer
	if ((joystick.button_3 == HIGH) && ((epoch() - joystick.b3_time) >= 50))
	{
		joystick.b3_time = epoch();
		if (!read_buzzer_state())
		{
			set_buzzer_state(true);
			set_buzz_time();
		}
	}
}

/**********************************
Function name	:	steer_robot
Functionality	:	To set the direction, speed and the status of the robot
Arguments		:	None
Return Value	:	None
Example Call	:	steer_robot()
***********************************/
void steer_robot()
{
	// Update the rotation variables
	switch(joystick.x_position)
	{
		// Zone -2/-1 --> Rotate Right
		case -2:
		case -1:
			// Set the turning speed
			rotation_left = TURN_SPEED;
			rotation_right = -TURN_SPEED;			
			set_led_indicators(false, false, false, false, true, false);
			ROTATION_FLAG = true;
		break;
		
		// Neutral Position
		case 0:
			rotation_left = 0;
			rotation_right = 0;			
			set_led_indicators(true, false, false, false, false, false);
			ROTATION_FLAG = false;
		break;
		
		// Zone 1/2 --> Rotate Left
		case 1:
		case 2:
			// Set the turning speed
			rotation_left = -TURN_SPEED;
			rotation_right = TURN_SPEED;			
			set_led_indicators(false, false, false, true, false, false);
			ROTATION_FLAG = true;
		break;
	}
	
	// Handle the Controller's Push Button Inputs
	handle_buttons();
}

/**********************************
Function name	:	compute_rotation_PID
Functionality	:	To prevent the rotational drift of the robot during static balance using a 
					simple Proportional controller to minimize difference in encoder counts
Arguments		:	None
Return Value	:	None
Example Call	:	compute_rotation_PID()
***********************************/
void compute_rotation_PID()
{
	// Avoid rotational correction when joystick controller is being used to turn the robot
	if (ROTATION_FLAG)
	{
		left_encoder_count = right_encoder_count;	// Equalize the offset	
		return;
	}
	
	// Correct rotation drift by computing the difference between the encoder positions
	rotation_left = -1 * LEFT_GAIN * (left_encoder_count - right_encoder_count) * 0.05;
	rotation_right = RIGHT_GAIN * (left_encoder_count - right_encoder_count) * 0.05;
}

/**********************************
Function name	:	compute_velocity_PID
Functionality	:	To compute the angle set point using a PID Controller.
					Process variable is velocity in RPM of the motors.
					Velocity is measured to control the angle set point in order to
					prevent the robot from accelerating too much and falling down and 
					to control the target velocity of the robot while in motion.
					This is the dominant controller when robot is moving.
Arguments		:	None
Return Value	:	None
Example Call	:	compute_velocity_PID()
***********************************/
void compute_velocity_PID()
{
	float velocity_KP = 0;
	
	// Get current linear velocity measured using encoders
	velocity.position = (left_RPM + right_RPM)/2.0; // Average of both motor RPMs
	
	// Compute error
	velocity.error = velocity.set_point - velocity.position;
	
	// Normal motion
	if (!STOP_FLAG)
	{
		velocity_KP = velocity.con_KP;
		if (abs(velocity.error) < 60) max_angle_vel = 2.5;
		if (abs(velocity.error) >= 60) max_angle_vel = 4;
	}
	
	// Slope
	else if (SLOPE_FLAG)
	{
		velocity_KP = velocity.agr_KP;
		max_angle_vel = 6;
	}
	
	// Static balance
	else
	{
		velocity_KP = velocity.con_KP;
		max_angle_vel = 2;
	}
	
	// Compute derivative
	velocity.derivative = velocity.position - velocity.last_position;

	// Compute integral and prevent wind-up	
	velocity.integral += velocity.con_KI*0.001*velocity.error;
	velocity.integral = constrain(velocity.integral, -max_angle_vel, max_angle_vel);

	// Compute velocity PID output
	velocity.output = (velocity_KP*0.001*velocity.error) + velocity.integral - (velocity.con_KD*0.001*velocity.derivative);
	
	// Constrain velocity PID output to PWM range
	velocity.output = velocity.direction * constrain(velocity.output, -max_angle_vel, max_angle_vel);
	
	// Store current velocity for next iteration
	velocity.last_position = velocity.position;
}

/**********************************
Function name	:	compute_encoder_PID
Functionality	:	To compute the angle set point using a Proportional controller.
					Process variables is position (encoder count).
					This is the dominant controller when robot is still and is 
					used to hold the position of the robot and prevent drifting.
Arguments		:	None
Return Value	:	None
Example Call	:	compute_encoder_PID()
***********************************/
void compute_encoder_PID()
{
	float encoder_KP = 0;
	
	// Get current encoder position
	encoder.position = encoder_count();
	
	// Compute position error
	encoder.error = encoder.set_point - encoder.position;
	
	// Normal motion
	if (!STOP_FLAG)
	{
		encoder_KP = encoder.con_KP;
		max_angle_enc = 2;
	}
	
	// Slope
	else if (SLOPE_FLAG)
	{
		encoder_KP = encoder.con_KP;
		max_angle_enc = 4;
	}
	
	// Static balance
	else
	{
		encoder_KP = encoder.agr_KP;
		max_angle_enc = 2;
	}

	// Compute encoder PID output
	encoder.output = (encoder_KP*encoder.error*0.0001);
	
	// Constrain the encoder PID output
	encoder.output = encoder.direction * constrain(encoder.output, -max_angle_enc, max_angle_enc);
	
	//if (abs(encoder.error) < 50) encoder.output = 0;	// For smoothing output
}

/**********************************
Function name	:	compute_angle_PID
Functionality	:	To compute the PWM value to set the motor speed using a PID Controller
					Process variable is tilt angle
Arguments		:	None
Return Value	:	None
Example Call	:	compute_angle_PID()
***********************************/
void compute_angle_PID()
{
	float angle_KP=0, angle_KI=0, angle_KD=0;
	float angle_position;
	
	// Obtain the current tilt angle and make a copy to avoid
	// abrupt changes during current PID computation loop
	angle_position = angle.position;
	
	// Add all the offsets to the angle set-point
	angle.set_point = TILT_ANGLE_OFFSET + move_offset + slope_offset + encoder.output + velocity.output;
	
	// Compute tilt angle error
	angle.error = angle.set_point - angle_position;
	
	// Turn motors off if robot falls beyond recoverable angle and await human rescue
	if (abs(angle.error) >= 75)
	{
		angle.output = 0;
		return;
	}

	// Conservative PID gains for |errors| < 3 degress
	if (abs(angle.error) < 3.0)
	{
		angle_KP = angle.con_KP;
		angle_KI = angle.con_KI;
		angle_KD = angle.con_KD;
	}
	
	// Aggressive PID gains for |errors| >= 3 degress
	else
	{
		angle_KP = angle.agr_KP;
		angle_KI = angle.agr_KI;
		angle_KD = angle.agr_KD;
	}
	
	// Compute derivative term
	angle.derivative = angle_position - angle.last_position;
	
	// Compute integral sum
	angle.integral += angle_KI*angle.error;
	
	// Constrain integral term to prevent wind-up
	angle.integral = constrain(angle.integral, -255, 255);
	
	// Compute angle PID output
	angle.output = (angle_KP*angle.error) + (angle.integral) - (angle_KD*angle.derivative);
	
	// Constrain angle PID output to PWM range
	angle.output = constrain(angle.output, -255, 255);
	
	//if (abs(angle.error) < 0.2) angle.output = 0;		// For smoothing output
	
	// Store variable for next iteration
	angle.last_position = angle_position;
}

/**********************************
Function name	:	compute_PID
Functionality	:	Compute all the PID values
Arguments		:	None
Return Value	:	None
Example Call	:	compute_PID()
***********************************/
void compute_PID()
{
	compute_encoder_PID();
	compute_velocity_PID();
	compute_angle_PID();
	compute_rotation_PID();
}

/**********************************
Function name	:	task_scheduler
Functionality	:	To schedule various tasks
Arguments		:	None
Return Value	:	None
Example Call	:	task_scheduler()
***********************************/
void task_scheduler()
{
	read_joystick();	// Read the raw data from Joystick Controller
	led_scheduler();	// Run the LED scheduler for status/beacon indicator
	buzz_scheduler();	// Run the buzzer scheduler and play non-blocking RTTTL tones
	
	// PID task scheduling every 20ms
	if ((epoch() - last_task_time_PID) >= 20)
	{
		last_task_time_PID = epoch();
		
		steer_robot();	// Update the set-points for the various PID loop
		compute_PID();	// Compute PID values
		
		// Update the motor speed and direction
		update_motors(angle.output, rotation_left, rotation_right);
	}
}

/**********************************
Function name	:	init_devices
Functionality	:	To initiate all devices
Arguments		:	None
Return Value	:	void
Example Call	:	init_devices()
***********************************/
void init_devices()
{
	cli();					// Clear global interrupts
	
	Serial.begin(9600);		// Setup serial communication
	
	timer1_init();			// Timer 1 for RPM measurement
	timer3_init();			// Timer 3 for reading GY80 sensor
	timer4_init();			// Timer 4 for calculating program time in ms
	
	i2c_init();				// Initialize I2C
	accel_init();			// Initialize ADXL345
	gyro_init();			// Initialize L3G4200D
	motors_init();			// Initialize motors and encoders
	
	buzzer_pin_config();	// Initialize buzzer
	led_pin_config();		// Initialize LEDs
	
	sei();					// Enable global interrupts
}

/**********************************
Function name	:	setup
Functionality	:	To setup all parameters once in main()
Arguments		:	None
Return Value	:	None
Example Call	:	setup() - Called internally by main()
***********************************/
void setup()
{
	init_devices();			// Initiate all devices
	start_timer4();			// Timer for epoch()
	start_timer3();			// Timer for reading GY80 sensor
	start_timer1();			// Timer for calculating RPM of motors
	
	// Set PID controller directions
	angle.direction = 1;
	velocity.direction = -1;
	encoder.direction = -1;
}

/**********************************
Function name	:	loop
Functionality	:	To loop the program in main()
Arguments		:	None
Return Value	:	None
Example Call	:	loop() - Called internally by main()
***********************************/
void loop()
{
	task_scheduler();	// Schedule tasks
}