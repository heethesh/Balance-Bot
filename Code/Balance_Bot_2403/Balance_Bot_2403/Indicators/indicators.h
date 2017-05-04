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

#ifndef INDICATORS_H_
#define INDICATORS_H_

// Define Pin Numbers (Arduino Mega)
#define BUZZER_PIN	9
#define LED_BOTTOM	A7

#define LED1_RED	A14
#define LED1_GRN	A10
#define LED1_BLU	A12

#define LED2_RED	31
#define LED2_GRN	33
#define LED2_BLU	35

// Function Declarations
void led_pin_config();

void alpha_red(bool state);
void alpha_green(bool state);
void alpha_blue(bool state);

void beta_red(bool state);
void beta_green(bool state);
void beta_blue(bool state);

void set_led_indicators(bool stopped, bool front, bool reverse, bool left, bool right, bool err);
void led_indicator_master(bool state);
void blinker(uint8_t pin, int on_time, int off_time, int period, bool dir, bool power);
void led_scheduler();

void buzzer_pin_config();
void buzzer_on();
void buzzer_off();
void set_buzzer_state(bool state);
bool read_buzzer_state();
void set_buzz_time();
void set_error_state(bool state);
bool read_error_state();
void set_error_time();
void play_music(int song);
void initial_buzz();
void buzz_scheduler();

#endif /* INDICATORS_H_ */