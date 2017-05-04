/*
* Project Name: Balance_Bot_2403
* File Name: indicators.cpp
*
* Created: 16-Mar-17 3:21:38 AM
* Author : Heethesh Vhavle
*
* Team: eYRC-BB#2403
* Theme: Balance Bot
*
* Library for LED status indicators and buzzer tones
*/

#include <Arduino.h>
#include "../Tones/anyrtttl.h"
#include "../Support/digitalWriteFast.h"
#include "../Timers/timers.h"
#include "indicators.h"

// Buzzer flags
bool BUZZER_STATE = false;
bool INITIAL_FLAG = true;
bool READY_FLAG = false;
bool BUZZER_SONG = true;
bool ERROR_STATE = false;

// LED indicator flags
bool LEFT_INDICATOR = false;
bool RIGHT_INDICATOR = false;
bool STOP_INDICATOR = true;
bool FRONT_INDICATOR = false;
bool BACK_INDICATOR = false;
bool SLOPE_INDICATOR = false;

// Timing variables
unsigned long buzz_time = 0;
unsigned long error_time = 0;
unsigned long led_bottom_time = 0;
unsigned long alpha_time = 0;
unsigned long beta_time = 0;

// RTTTL tones
const char * song_startup = "Startup:d=8,o=6,b=200:8c.6,8c.7,8g.6,8f6,4e.6,16f6,16g6,4c.7";
const char * song_ready = "Ready:d=8,o=6,b=180:16e6,16f6,16g6,8c.7";
const char * song_buzzer = "Buzzer:d=8,o=6,b=200:8g6,8e6,8c6,8f6,8c7,4g.6,16f6,16g6,4c.7";
const char * song_buzzer2 = "Buzzer2:d=16,o=7,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g";
const char * song_mission = "MissionImp:d=16,o=7,b=95:g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";

/**********************************
Function name	:	led_pin_config
Functionality	:	To configure the LED pins
Arguments		:	None
Return Value	:	None
Example Call	:	led_pin_config()
***********************************/
void led_pin_config()
{
	// LED Bottom
	pinMode(A7, OUTPUT);
	
	// RGB LED 1 (Alpha)
	pinMode(LED1_RED, OUTPUT);
	pinMode(LED1_GRN, OUTPUT);
	pinMode(LED1_BLU, OUTPUT);
	
	// RGB LED 2 (Beta)
	pinMode(LED2_RED, OUTPUT);
	pinMode(LED2_GRN, OUTPUT);
	pinMode(LED2_BLU, OUTPUT);
	
	alpha_red(LOW);
	alpha_green(LOW);
	alpha_blue(LOW);
	
	beta_red(LOW);
	beta_green(LOW);
	beta_blue(LOW);
}

// RGB LED 1
void alpha_red(bool state)		{digitalWriteFast(LED1_RED, !state);}
void alpha_green(bool state)	{digitalWriteFast(LED1_GRN, !state);}
void alpha_blue(bool state)		{digitalWriteFast(LED1_BLU, !state);}

// RGB LED 2
void beta_red(bool state)		{digitalWriteFast(LED2_RED, !state);}
void beta_green(bool state)		{digitalWriteFast(LED2_GRN, !state);}
void beta_blue(bool state)		{digitalWriteFast(LED2_BLU, !state);}

/**********************************
Function name	:	set_led_indicators
Functionality	:	Setter function for LED flags
Arguments		:	LED flags
Return Value	:	None
Example Call	:	set_led_indicators(false, true, false, false, false, false)
***********************************/
void set_led_indicators(bool stopped, bool front, bool reverse, bool left, bool right, bool slope)
{
	STOP_INDICATOR = stopped;
	FRONT_INDICATOR = front;
	BACK_INDICATOR = reverse;
	LEFT_INDICATOR = left;
	RIGHT_INDICATOR = right;
	SLOPE_INDICATOR = slope;
}

/**********************************
Function name	:	led_indicator_master
Functionality	:	Master power control for both LEDs
Arguments		:	Power state
Return Value	:	None
Example Call	:	led_indicator_master(true)
***********************************/
void led_indicator_master(bool state)
{
	alpha_red(state);  alpha_green(state);	alpha_blue(state);
	beta_red(state);   beta_green(state);	beta_blue(state);
}

/**********************************
Function name	:	blinker
Functionality	:	Non-blocking periodic LED blinker
Arguments		:	LED pin number, On time instant, Off time instant, Time period, 
					Power hardware control, Power software control
Return Value	:	None
Example Call	:	blinker
***********************************/
void blinker(uint8_t pin, int on_time, int off_time, int period, bool dir, bool power)
{
	// Software power control
	if (!power) return;
	unsigned long time_now = epoch();
	
	// Non-blocking blinker [Beacon Lights]
	if (((time_now % period)>=on_time) && ((time_now % period)<=(on_time+6))) digitalWriteFast(pin, dir);
	if (((time_now % period)>=off_time) && ((time_now % period)<=(off_time+6))) digitalWriteFast(pin, !dir);
}

/**********************************
Function name	:	led_scheduler
Functionality	:	Schedule the LED beacon lights
Arguments		:	None
Return Value	:	None
Example Call	:	led_scheduler()
***********************************/
void led_scheduler()
{
	// Bottom LED - Robot setup time
	blinker(LED_BOTTOM, 200, 260, 1000, true, true);
	
	// Initial LED status
	if (epoch() < 3000)
	{
		alpha_red(true);
		beta_red(true);
		return;
	}
	
	// Initial LED - Robot ready
	if ((epoch()>=3000) && (epoch()<3500))
	{
		alpha_red(false);	beta_red(false);
		alpha_green(true);	beta_green(true);
		return;
	}
	
	// Buzzer - White
	if (BUZZER_STATE)
	{
		led_indicator_master(true);
		return;
	}
	
	// Error Indicator - Red
	if (ERROR_STATE)
	{
		led_indicator_master(false);
		alpha_red(true);
		beta_red(true);
		return;
	}
	
	// Turn off all LEDs
	led_indicator_master(false);
	
	// Blinker Logic
	
	// Stop - Red
	blinker(LED1_RED, 0, 50, 1000, false, STOP_INDICATOR);
	blinker(LED2_RED, 0, 50, 1000, false, STOP_INDICATOR);
	blinker(LED1_RED, 100, 150, 1000, false, STOP_INDICATOR);
	blinker(LED2_RED, 100, 150, 1000, false, STOP_INDICATOR);
	
	// Forward - Green
	blinker(LED1_GRN, 0, 100, 250, false, FRONT_INDICATOR);
	blinker(LED2_GRN, 0, 100, 250, false, FRONT_INDICATOR);
	
	// Back - Cyan
	blinker(LED1_GRN, 0, 100, 250, false, BACK_INDICATOR);
	blinker(LED1_BLU, 0, 100, 250, false, BACK_INDICATOR);
	blinker(LED2_GRN, 0, 100, 250, false, BACK_INDICATOR);
	blinker(LED2_BLU, 0, 100, 250, false, BACK_INDICATOR);
	
	// Left - Yellow
	blinker(LED1_RED, 0, 100, 200, false, LEFT_INDICATOR);
	blinker(LED1_GRN, 0, 100, 200, false, LEFT_INDICATOR);
	
	// Right - Yellow
	blinker(LED2_RED, 0, 100, 200, false, RIGHT_INDICATOR);
	blinker(LED2_GRN, 0, 100, 200, false, RIGHT_INDICATOR);
	
	// Slope Mode - Blue
	blinker(LED1_BLU, 0, 150, 300, false, SLOPE_INDICATOR);
	blinker(LED2_BLU, 0, 150, 300, false, SLOPE_INDICATOR);
}

/**********************************
Function name	:	buzzer_pin_config
Functionality	:	To configure the buzzer pin
Arguments		:	None
Return Value	:	None
Example Call	:	buzzer_pin_config()
***********************************/
void buzzer_pin_config()
{
	pinMode(BUZZER_PIN, OUTPUT);
	digitalWriteFast(BUZZER_PIN, LOW);
}

void buzzer_on() {digitalWriteFast(BUZZER_PIN, HIGH);}
void buzzer_off() {digitalWriteFast(BUZZER_PIN, LOW);}

/**********************************
Function name	:	set_buzzer_state
Functionality	:	Setter function for buzzer state
Arguments		:	Buzzer power state
Return Value	:	None
Example Call	:	set_buzzer_state(true)
***********************************/
void set_buzzer_state(bool state)
{
	BUZZER_STATE = state;
	BUZZER_SONG = state;
}

// Setter and Getter functions
bool read_buzzer_state() {return BUZZER_STATE;}
void set_buzz_time() {buzz_time = epoch();}

void set_error_state(bool state) {ERROR_STATE = state;}
bool read_error_state() {return ERROR_STATE;}
void set_error_time() {error_time = epoch();}

/**********************************
Function name	:	play_music
Functionality	:	Play non-blocking RTTTL tone
Arguments		:	Song number
Return Value	:	None
Example Call	:	play_music(2)
***********************************/
void play_music(int song)
{
	if (song==1)	  anyrtttl::nonblocking::begin(BUZZER_PIN, song_startup);
	else if (song==2) anyrtttl::nonblocking::begin(BUZZER_PIN, song_ready);
	else if (song==3) anyrtttl::nonblocking::begin(BUZZER_PIN, song_buzzer);
	else if (song==4) anyrtttl::nonblocking::begin(BUZZER_PIN, song_buzzer2);
	else if (song==5) anyrtttl::nonblocking::begin(BUZZER_PIN, song_mission);
}

/**********************************
Function name	:	initial_buzz
Functionality	:	Initial start-up tone
Arguments		:	None
Return Value	:	None
Example Call	:	initial_buzz()
***********************************/
void initial_buzz()
{
	// Normal buzzer
	if (BUZZER_STATE)
	{
		if ((epoch() - buzz_time) > 2000)
		{
			buzzer_off();
			BUZZER_STATE = false;
		}
		
		else buzzer_on();
	}
	
	// Error mode
	if (ERROR_STATE)
	{
		if ((epoch() - error_time) > 200)
		{
			buzzer_off();
			ERROR_STATE = false;
		}
		
		else buzzer_on();
	}
	
	// Initial start-up tone
	if (INITIAL_FLAG)
	{
		play_music(1);
		INITIAL_FLAG = false;
		READY_FLAG = true;
	}
	
	// Initial ready tone
	if (READY_FLAG && (epoch()>3000))
	{
		play_music(2);
		READY_FLAG = false;
	}
}

/**********************************
Function name	:
Functionality	:
Arguments		:	None
Return Value	:	None
Example Call	:
***********************************/
void buzz_scheduler()
{
	initial_buzz();
	if ((epoch()<5000) || BUZZER_STATE) anyrtttl::nonblocking::play();
}