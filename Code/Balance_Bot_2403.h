/*
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
* Main Header File
*/

#ifndef BALANCE_BOT_2403_H_
#define BALANCE_BOT_2403_H_

// Definitions
#define F_CPU 14745600L

// Angle Values
#define TILT_ANGLE_OFFSET -0.33		// Angle offset to correct CG of robot
#define COMP_FILTER_ALPHA 0.98
#define SLOPE_ANGLE 2.8

// Speed values
#define FULL_SPEED 50				// Position increment rate
#define FORWARD_SPEED 155
#define REVERSE_SPEED 140
#define SLOPE_SPEED 155
#define TURN_SPEED 60

// Minimum PWM Values for Motors
#define LEFT_PWM_MIN 45
#define RIGHT_PWM_MIN 55

// Rotation Proportional Gains
#define LEFT_GAIN 10
#define RIGHT_GAIN 5

// External Variables
extern JoystickController joystick;
extern volatile float left_encoder_count;
extern volatile float right_encoder_count;

// Global Variables
float slope_offset=0, move_offset=0, max_angle_vel=4, max_angle_enc=2;
volatile float accel_angle=0, gyro_angle=0;
volatile float rotation_left=0, rotation_right=0;
volatile float left_RPM=0, right_RPM=0, left_prev_count=0, right_prev_count=0;
unsigned long last_task_time_PID=0;

// Flags
bool STOP_FLAG = true;
bool ROTATION_FLAG = false;
bool SLOPE_FLAG = false;

// PID Structure Definition
typedef struct PID
{
	volatile float con_KP;			// Conservative proportional gain
	volatile float con_KI;			// Conservative integral gain
	volatile float con_KD;			// Conservative derivative gain
	
	volatile float agr_KP;			// Aggressive proportional gain
	volatile float agr_KI;			// Aggressive integral gain
	volatile float agr_KD; 			// Aggressive derivative gain
	
	volatile float set_point;		// Set point value
	volatile float error;			// Error value
	volatile float position;		// Current position
	volatile float last_position;	// Previous position
	
	volatile float integral;		// Integral sum
	volatile float derivative;		// Derivative term
	volatile float output; 			// PID output	
	volatile int direction;			// Controller direction
};

// Structure Initializations
PID angle    = {14, 3.2, 27, 20, 4, 32};
PID velocity = {15, 0, 4, 5, 0, 0, 0};
PID encoder  = {1.5, 0, 0, 8.2, 0, 0, 0};


// Function Definitions

/**********************************
Function name	:	encoder_count
Functionality	:	Returns the current encoder count
Arguments		:	None
Return Value	:	Encoder count sum
Example Call	:	encoder_count()
***********************************/
float encoder_count();

/**********************************
Function name	:	complimentary_filter
Functionality	:	First order complimentary filter for sensor fusion
Arguments		:	Sensor data to fuse, alpha value
Return Value	:	Fused value
Example Call	:	complimentary_filter(gyro_angle, accel_angle, 0.98)
***********************************/
float complimentary_filter(float angle1, float angle2, float alpha);

/**********************************
Function name	:	read_tilt_angle
Functionality	:	Compute tilt angle from Gyroscope, Accelerometer and Complimentary Filter
Arguments		:	None
Return Value	:	None
Example Call	:	read_tilt_angle()
***********************************/
void read_tilt_angle();

/**********************************
Function name	:	handle_buttons
Functionality	:	To handle the button inputs of the Joystick Controller 
					and update the status/motion of the robot accordingly
Arguments		:	None
Return Value	:	None
Example Call	:	handle_buttons()
***********************************/
void handle_buttons();

/**********************************
Function name	:	steer_robot
Functionality	:	To set the direction, speed and the status of the robot
Arguments		:	None
Return Value	:	None
Example Call	:	steer_robot()
***********************************/
void steer_robot();

/**********************************
Function name	:	compute_rotation_PID
Functionality	:	To prevent the rotational drift of the robot during static balance using a 
					simple Proportional controller to minimize difference in encoder counts
Arguments		:	None
Return Value	:	None
Example Call	:	compute_rotation_PID()
***********************************/
void compute_rotation_PID();

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
void compute_velocity_PID();

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
void compute_encoder_PID();

/**********************************
Function name	:	compute_angle_PID
Functionality	:	To compute the PWM value to set the motor speed using a PID Controller
					Process variable is tilt angle
Arguments		:	None
Return Value	:	None
Example Call	:	compute_angle_PID()
***********************************/
void compute_angle_PID();

/**********************************
Function name	:	compute_PID
Functionality	:	Compute all the PID values
Arguments		:	None
Return Value	:	None
Example Call	:	compute_PID()
***********************************/
void compute_PID();

/**********************************
Function name	:	task_scheduler
Functionality	:	To schedule various tasks
Arguments		:	None
Return Value	:	None
Example Call	:	task_scheduler()
***********************************/
void task_scheduler();

/**********************************
Function name	:	init_devices
Functionality	:	To initiate all devices
Arguments		:	None
Return Value	:	void
Example Call	:	init_devices()
***********************************/
void init_devices();

#endif