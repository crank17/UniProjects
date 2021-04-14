/*
 * ssg.c
 *
 * Created: 10/06/2020 11:10:39 PM
 *  Author: Ryan
 */ 

 #include <avr/io.h>
 #include <avr/interrupt.h>

 #include "ssg.h"
 #include "score.h"
 #include "game.h"
 #include "ssg.h"
 #define F_CPU 8000000L
 #include <util/delay.h>
 #include "terminalio.h"


 

 uint8_t seven_seg[10] = {63,6,91,79,102,109,125,7,127,111};

 

 volatile uint8_t digit = 0;


 void init_timer2(void) {
	
	DDRC = 0xFF;
	DDRD |= (1<<5);
	OCR2A = 255; 
	TCCR2A = (1<<WGM21); 
	TCCR2B = (1<<CS22)|(1<<CS21); 
	
	TIMSK2 |= (1<<OCIE2A);
	
	TIFR2 &= (1<<OCF2A);
}

void display_ssd(void){
	
		//display_lives_led();
		digit = 1 ^ digit;
	

		//DDRA = (1<<DDRA3);
		DDRD |= (1<<5);
		
		
		if (digit == 0){
			
			PORTC = seven_seg[(get_score()) % 10];

			if (get_speed() == 1){
				PORTC = PORTC | 0x80;
			}
			
			
		}
		else{
			
			PORTC = seven_seg[(get_score() / 10) % 10];
			
		}
		PORTD  ^= (-digit ^ PORTD) & (1UL << 5);
		
		

			
		
	}
	


ISR(TIMER2_COMPA_vect) {
	
	display_ssd();

	
	
	
	
}
