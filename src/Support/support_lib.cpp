/*
* Project Name: Balance_Bot_2403
* File Name: support_lib.cpp
*
* Created: 04-Dec-16 7:56:40 PM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Support Library for GY-80 for verifying Device ID and I2C status
*
* Functions: check_status(), read_device_ID(), check_device_ID()
* Global Variables: txf
*/

#include <Arduino.h>
#include "../I2C/i2c_lib.h"
#include "../Indicators/indicators.h"

/**********************************
Function name	:	check_status
Functionality	:	Check the status of I2C communication
Arguments		:	Status code
Return Value	:	void
Example Call	:	check_status(i2c_getbyte(device_address, device_ID_address, &device_ID))
***********************************/
void check_status(STAT status)
{
	if(status != OK)
	{
		if (!read_error_state())
		{
			set_error_state(true);
			set_error_time();
		}
	}
}

/**********************************
Function name	:	read_device_ID
Functionality	:	Function to read device ID
Arguments		:	Device address, ID register address
Return Value	:	Device ID value
Example Call	:	read_device_ID(ADXL345_ADDRESS, ADXL345_DEVID)
***********************************/
INT8 read_device_ID(UINT8 device_address, UINT8 device_ID_address)
{
	INT8 device_ID;
	check_status(i2c_getbyte(device_address, device_ID_address, &device_ID));
	return device_ID;
}

/**********************************
Function name	:	check_device_ID
Functionality	:	Function to verify the device ID
Arguments		:	Device address, ID register address, default device ID
Return Value	:	void
Example Call	:	check_device_ID(ADXL345_ADDRESS, ADXL345_DEVID, ADXL345_KNOWN_ID)
***********************************/
void check_device_ID(UINT8 device_address, UINT8 device_ID_address, INT8 known_ID)
{
	INT8 check_ID;
	check_ID = read_device_ID(device_address, device_ID_address);
	
	if(known_ID != check_ID)
	{
		if (!read_error_state())
		{
			set_error_state(true);
			set_error_time();
		}
	}
}