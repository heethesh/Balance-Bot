/*
* Project Name: Balance_Bot_2403
* File Name: gyro.h
*
* Created: 04-Dec-16 7:56:40 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for L3G4200D Gyroscope
*/

#ifndef GYRO_H_
#define GYRO_H_

// Register Map
#define L3G4200D_ADDRESS		0x69 << 1
#define L3G4200D_WHO_AM_I		0x0F
#define L3G4200D_KNOWN_ID		0xD3
#define L3G4200D_CTRL_REG1		0x20
#define L3G4200D_CTRL_REG4		0x23
#define L3G4200D_OUT_X_L		0x28
#define L3G4200D_OUT_Y_L		0x2A


// Function Declarations

/**********************************
Function name	:	gyro_init
Functionality	:	Initialize the gyroscope
Arguments		:	none
Return Value	:	void
Example Call	:	gyro_init()
***********************************/
void gyro_init();

/**********************************
Function name	:	convert_gyro
Functionality	:	Converts raw 2's compliment gyroscope readings to DPS, removes offset error
Arguments		:	Raw gyroscope reading, offset error value
Return Value	:	Angular velocity in DPS
Example Call	:	convert_gyro(X_DATA, X_OFFSET)
***********************************/
float convert_gyro(UINT16 value, float offset);

/**********************************
Function name	:	read_gyro
Functionality	:	To read Y-axis angular velocity from Gyroscope
Arguments		:	none
Return Value	:	Raw angular velocity
Example Call	:	read_gyro()
***********************************/
float read_gyro();

/**********************************
Function name	:	get_gyro_angle
Functionality	:	Computes the angular position by integrating angular velocity
Arguments		:	Current program time, current combined pitch angle
Return Value	:	Gyroscope pitch angle
Example Call	:	get_gyro_angle(epoch(), tilt_angle)
***********************************/
float get_gyro_angle(unsigned long current_time, float pitch_angle);

/**********************************
Function name	:	calibrate_gyro
Functionality	:	Reads values of Gyroscope when static, computes average offset
Arguments		:	None
Return Value	:	void
Example Call	:	calibrate_gyro()
***********************************/
//void calibrate_gyro();

#endif