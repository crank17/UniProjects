/*
 * sound.c
 *
 * Created: 11/06/2020 12:51:07 AM
 *  Author: Ryan
 */ 

 #include <avr/io.h>
 #include <stdint.h>
 #include "sound.h"

 #define MUTE (PIND & (1<<PIND3))


 uint16_t sound_clock = 0;
 uint8_t current_sound = 0;

 uint16_t freq = 200;	// Hz
 float dutycycle = 2;	// %
 uint16_t clockperiod;
 uint16_t pulsewidth;

 uint16_t freq_to_clock_period(uint16_t freq) {
	 return (1000000UL / freq);	
 }

 
 uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	 return (dutycycle * clockperiod) / 100;
 }



 void init_sound(void) {
	 DDRD |= (1<<4);
	 DDRD &= ~(1<<PIND3);
	 OCR1A = 0;
	 OCR1B = 0;
	 TCCR1A = (1 << COM1B1) | (0 <<COM1B0) | (1 <<WGM11) | (1 << WGM10);
	 TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
	 
	 sound_clock = 0;
	 current_sound = 0;
	
 }

 void set_sound(uint8_t val){
	current_sound = val;
	
	if(current_sound == 0) {
	 OCR1A = 0;
	 OCR1B = 0;
	 sound_clock = 0;
	 return;
	}

	if (current_sound == 1){
		dutycycle = 2;
	}

	if (current_sound == 2){
		dutycycle = 1;
	}
	if (current_sound == 3){
		dutycycle = 4;
	}
	if (current_sound == 4){
		dutycycle = 5;
	}

	sound_clock = 1;
	play_sound();
	
	
 }

 

 void play_sound(void){
	sound_clock++;

	if(current_sound == 0 || MUTE){
		return;
	}

  if(current_sound == 1) {
	 freq = 300 - 7 * sound_clock;
	 if (sound_clock >= 11) {
		 set_sound(0);
		 return;
	 }
	 }


	
	if(current_sound == 2) {
		freq = 650 - 7 * sound_clock;
		if (sound_clock >= 11) {
			set_sound(0);
			return;
		}
		}

	if (current_sound == 3){
		if (sound_clock < 10){
			freq = 300;
		}
		else if (sound_clock < 20){
			freq = 450;
		}
		else if (sound_clock < 30){
			freq = 650;
		}
		else if(sound_clock > 40){
			set_sound(0);
			return;
		}
		
	}

	if (current_sound == 4){
		if (sound_clock < 10){
			freq = 400;
		}
		else if (sound_clock < 20){
			freq = 450;
		}
		else if (sound_clock < 30){
			freq = 650;
		}
		else if (sound_clock < 40){
			freq = 700;
		}
		else if (sound_clock < 50){
			freq = 750;
		}
				else if(sound_clock > 60){
			set_sound(0);
			return;
		}
		
	}

	
 clockperiod = freq_to_clock_period(freq);
 pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
 OCR1A = clockperiod - 1;
 if(pulsewidth == 0) {
	 OCR1B = 0;
	 } else {
	 OCR1B = pulsewidth - 1;
	
 }
 }
 
 