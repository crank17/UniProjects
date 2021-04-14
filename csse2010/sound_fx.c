//
//
//#include <avr/io.h>
//#define F_CPU 8000000UL	// 8MHz
//#include <util/delay.h>
//#include <stdint.h>
//#include "alien.h"
//
//uint16_t score_counter = 0;
//uint16_t freq = 200;	// Hz
//float dutycycle = 2;	// %
//uint16_t clockperiod;
//uint16_t pulsewidth;
//
//
//uint16_t freq_to_clock_period(uint16_t freq) {
	//return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
	//// and ensures we do 32 bit arithmetic, not 16
//}
//
//uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	//return (dutycycle * clockperiod) / 100;
//}
//
//void init_sound(void){
	//DDRD = (1<<4);
	//uint16_t clockperiod = freq_to_clock_period(freq);
	//uint16_t pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
//
//
//
//
	//// use inerrupt ot turn on?
//
//
	//// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	//OCR1A = clockperiod - 1;
//
	//// Set the count compare value based on the pulse width. The value will be 1 less
	//// than the pulse width - unless the pulse width is 0.
//
//
//
	//// Set up timer/counter 1 for Fast PWM, counting from 0 to the value in OCR1A
	//// before reseting to 0. Count at 1MHz (CLK/8).
	//// Configure output OC1B to be clear on compare match and set on timer/counter
	//// overflow (non-inverting mode).
	//TCCR1A = (1 << COM1B1) | (0 <<COM1B0) | (1 <<WGM11) | (1 << WGM10);
	//TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
//
	//
//}
//
//
//
//void alien_hit_sound(void){
//
	//
	//
	//if (score_counter != 3){
		//if (score_counter == 1){
			//freq = 1000;
		//}
		//else if (score_counter == 0){
			//freq = 500;
		//}
		//else if (score_counter = 2){
			//freq = 1000;
		//}
		//else {
			//freq = 0;
		//}
		//clockperiod = freq_to_clock_period(freq);
		//pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
		//
		//// Update the PWM registers
		//if(pulsewidth > 0) {
			////// The compare value is one less than the number of clock cycles in the pulse width
			//OCR1B = pulsewidth - 1;
			//} else {
			//OCR1B = 0;
		//}
		//// Note that a compare value of 0 results in special behaviour - see page 130 of the
		//// datasheet (2018 version)
		//
		//// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
		//freq = 0;
		//OCR1A = clockperiod - 1;
		//score_counter += score_counter;
		//
		//
	//}
	//
//}
//
//ISR(TIMER1_COMPA_vect){
	  //
	//
//}
