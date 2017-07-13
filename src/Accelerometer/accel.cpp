/*
* Project Name: Balance_Bot_2403
* File Name: accel.cpp
*
* Created: 04-Dec-16 7:56:40 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for ADXL345 Accelerometer
*
* Functions: accel_init(), convert_accelerometer(), read_accelerometer(), calibrate_accel()
* Global Variables: None
*/

#include <math.h>
#include "../I2C/i2c_lib.h"
#include "../Support/support_lib.h"
#include "accel.h"

/**********************************
Function name	:	caliberate_accel
Functionality	:	Writes offset values to ADXL345 offset registers
Arguments		:	X0g, Y0g, Z0g offsets
Return Value	:	void
Example Call	:	calibrate_accel(0x01, 0x00, 0x03)
***********************************/
void calibrate_accel(char x_offset, char y_offset, char z_offset)
{
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_OFSX, x_offset));
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_OFSY, y_offset));
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_OFSZ, z_offset));
}

/**********************************
Function name	:	accel_init
Functionality	:	Initialize the accelerometer
Arguments		:	none
Return Value	:	void
Example Call	:	accel_init()
***********************************/
void accel_init()
{
	check_device_ID(ADXL345_ADDRESS, ADXL345_DEVID, ADXL345_KNOWN_ID);		// Verify Device ID
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x00));	// Standby Mode
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x08));	// Measurement Mode
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_DATA_FORMAT, 0x0B)); // 16g, FULL RES (13 bit)
	check_status(i2c_sendbyte(ADXL345_ADDRESS, ADXL345_BW_RATE, 0x0A));		// 100Hz Sample Rate, Normal Mode
	calibrate_accel(0x01, 0x00, 0x03);										// Calibrate offsets
}

/**********************************
Function name	:	convert_accelerometer
Functionality	:	Converts raw 2's compliment readings and normalize to 1g
Arguments		:	Raw accelerometer reading
Return Value	:	Normalized 1g value
Example Call	:	convert_accelerometer(Z_DATA)
***********************************/
float convert_accelerometer(UINT16 value)
{
	float g_value;
	
	// Convert 2's compliment to float value
	if (value>32767) g_value = (float)(value-65536);
	else g_value = (float)value;
	
	// Scale to 1g (*16/4096)
	return (g_value*0.00390625);
}

/**********************************
Function name	:	read_accelerometer
Functionality	:	Reads the acceleration along X,Y,Z axes
Arguments		:	none
Return Value	:	Computed pitch angle
Example Call	:	read_accelerometer()
***********************************/
float read_accelerometer()
{
	float pitch_angle=0, x_accel=0, z_accel=0;
	INT8 accel_raw_X[6] = {0, 0};
	INT8 accel_raw_Z[6] = {0, 0};
	
	// Read accelerometer data
	check_status(i2c_read_multi_byte(ADXL345_ADDRESS, ADXL345_DATAX0, 2, accel_raw_X));
	check_status(i2c_read_multi_byte(ADXL345_ADDRESS, ADXL345_DATAZ0, 2, accel_raw_Z));
	
	// Combine low and high bytes
	x_accel = convert_accelerometer((UINT8)accel_raw_X[0] | (UINT8)accel_raw_X[1]<<8);
	z_accel = convert_accelerometer((UINT8)accel_raw_Z[0] | (UINT8)accel_raw_Z[1]<<8);
	
	// Compute the pitch angle and convert to degrees
	pitch_angle = (atan2(-x_accel, z_accel)*180.0)/3.1416;
	
	return pitch_angle;
}


