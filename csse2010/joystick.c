/*
 * joystick.c
 *
 * created: 8/06/2020 8:34:55 pm
 *  author: ryan
 */ 

 #include "joystick.h"
 #include <avr/io.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include "terminalio.h"

uint16_t x_direction;
uint16_t y_direction;
uint8_t joystick_val;
uint8_t prev_direction = 0;



uint8_t joystick_input(void){
    
	
	ADMUX &= ~1;
	
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC)) {
		; 
	}
	x_direction = ADC; 

	
	ADMUX |= 1;
	
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC)) {
		; 
	}
	y_direction = ADC;

	uint8_t direction;
		
	if(x_direction < 256) {
		direction = 3;
	} 

	else if(x_direction < 290) {
		direction = 7;
	}
	
		else if(y_direction >= 880) {
			direction = 1;
		}
		
		
		else if(y_direction >= 800) {
			direction = 6;
		}

		else if(y_direction < 50) {
			direction = 2;
			}
		else if(y_direction < 100) {
			direction = 5;
		}

		
		else if(x_direction > 767) {
		direction = 4;
		} 
		
		 else {
			direction = 0;
		}

	return direction;
	}
	





