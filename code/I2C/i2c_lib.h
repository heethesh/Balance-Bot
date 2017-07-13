/*
* Project Name: Balance_Bot_2403
* File Name: i2c_lib.h
*
* Created: 04-Dec-16 7:56:40 PM
* Author : e-Yantra ERTS
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* I2C Communication Protocol Library
*/

#ifndef I2C_LIB_H
#define I2C_LIB_H

/********typedef for data types***********/
typedef unsigned char 		UINT8;
typedef signed char			INT8;
typedef unsigned short int 	UINT16;
typedef signed short int 	INT16;
typedef enum status_check_cond{START_ERR=-8, SLAVEW_ERR, SLAVER_ERR, WRITE_ERR, READ_ERR, REPSTART_ERR, ACK_ERR, NACK_ERR, OK }STAT;

/**********i2c related terms*************/
#define done 		 	(1<<7)
#define eack 		 	(1<<6)
#define start		 	(1<<5)
#define stop 		 	(1<<4)
#define i2cen			(1<<2)
#define i2write			 0x00
#define i2read			 0x01

/**********function declaration**********/
void i2c_init();
void i2c_start();
void clear_twint();
void wait();
void i2c_stop();
UINT8 i2c_getstatus();
void i2c_write(UINT8 data);
void i2c_get(INT8 *data);
STAT i2c_sendbyte(UINT8 dev_add, UINT8 int_add,UINT8 data);
STAT i2c_getbyte(UINT8 dev_add,UINT8 int_add,INT8 *data);
STAT i2c_read_multi_byte(UINT8 dev_add,UINT8 int_add,UINT16 n, INT8 *data);

#endif