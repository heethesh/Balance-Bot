/*
* Project Name: Balance_Bot_2403
* File Name: accel.h
*
* Created: 04-Dec-16 7:56:40 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for ADXL345 Accelerometer
*/

#ifndef ACCEL_H_
#define ACCEL_H_

// Register Map
#define ADXL345_ADDRESS			0x53 << 1
#define	ADXL345_DEVID			0x00
#define ADXL345_KNOWN_ID		0xE5
#define ADXL345_OFSX			0x1E
#define ADXL345_OFSY			0x1F
#define ADXL345_OFSZ			0x20
#define ADXL345_BW_RATE			0x2C
#define ADXL345_POWER_CTL		0x2D
#define ADXL345_DATA_FORMAT		0x31
#define ADXL345_DATAX0			0x32
#define ADXL345_DATAZ0			0x36


// Function Declarations

/**********************************
Function name	:	accel_init
Functionality	:	Initialize the accelerometer
Arguments		:	none
Return Value	:	void
Example Call	:	accel_init()
***********************************/
void accel_init();

/**********************************
Function name	:	caliberate_accel
Functionality	:	Writes offset values to ADXL345 offset registers
Arguments		:	X0g, Y0g, Z0g offsets
Return Value	:	void
Example Call	:	calibrate_accel(0x01, 0x00, 0x03)
***********************************/
void calibrate_accel(char x_offset, char y_offset, char z_offset);

/**********************************
Function name	:	convert_accelerometer
Functionality	:	Converts raw 2's compliment readings and normalize to 1g
Arguments		:	Raw accelerometer reading
Return Value	:	Normalized 1g value
Example Call	:	convert_accelerometer(Z_DATA)
***********************************/
float convert_accelerometer(UINT16 value);

/**********************************
Function name	:	read_accelerometer
Functionality	:	Reads the acceleration along X,Y,Z axes
Arguments		:	none
Return Value	:	Computed pitch angle
Example Call	:	read_accelerometer()
***********************************/
float read_accelerometer();

#endif