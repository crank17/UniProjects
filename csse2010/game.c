/*
 * CFile1.c
 *
 * Created: 27/05/2020 8:12:11 PM
 *  Author: Ryan
 */ 
 #include "game.h"
 #include <avr/io.h>
 #include "score.h"
 #include "ledmatrix.h"
 #include "scrolling_char_display.h"
 #include "pixel_colour.h"
 #include "game_background.h"
 #include "player.h"
 #include "alien.h"
 #include "projectile.h"
 #define F_CPU 8000000L
 #include <util/delay.h>
 #include <stdio.h>
 #include "terminalio.h"
 #include <stdio.h>

 uint32_t level;
 uint8_t count;
 uint32_t lives;
 uint8_t flag;
 uint8_t game_speed;

 void set_speed(void){
 if (game_speed == 0){
	game_speed = 1;
	

	}
 else{
	game_speed = 0;
	
	}
	
 }
 void init_speed(){
	game_speed = 0;
 }

 uint8_t get_speed(void){
	return game_speed;
 }

 void init_level(void){
	level = 1;
	
 }

 void reset_counter(void){
	count = 0;
	flag = 0;
 }

 uint8_t get_counter(void){
	return count;
	
 }

 void update_level(void){
	level += 1;
	count = 1;
 }

 uint32_t get_level(void){
	return level;
 }
 void display_level(void){
	move_cursor(3, 10);
	
	printf("\r%12ld", get_level());
 }

uint8_t get_button_state(void){
	uint8_t button_state = PINB & 0x0F;
	
	uint8_t val = 0;
	if ((button_state & (1<<0)) && 1){
	
		val = 1;
		
		//button_state = 0;
	}
	else if ((button_state & (1<<1)) && 1){
	
		val = 2;
	}
	else if ((button_state & (1<<3)) && 1){
	
		val = 3;
	}
	else{
	return 0;
	}
	
	return val;
	
	
	
}



 

 void check_level(void){
		if(get_score() % 100 == 0){
		ledmatrix_clear();
			
		
		update_level();
		
		ledmatrix_clear();

		while (1){
			set_scrolling_display_text("LEVEL UP", COLOUR_RED);
			while(scroll_display()) {
				_delay_ms(130);
			}
			
			return;
		}
		_delay_ms(500);
		
		}
	
 }

 void init_lives(void){
	lives = 4;
 }

 void lose_life(void){
	lives = lives - 1;
	
	flag = 1;
 }
 uint32_t get_lives(void){
	return lives;
 }

 uint8_t lives_flag(void){
	return flag;
 }

 void display_lives_led(void){
 // compress into print function
	//DDRC = 0xF0;
	//if (lives == 4){
	//PORTC = 0b11110000;
	//move_cursor(3, 12);
	//
	//printf("\r%12d", get_lives());
	//}
	//else if (lives == 3){
	//PORTC = 0b01110000;
	//move_cursor(3, 12);
	//
	//printf("\r%12d", get_lives());
	//}
	//else if (lives == 2){
	//PORTC = 0b01100000;
	//move_cursor(3, 12);
	//
	//printf("\r%12d", get_lives());
	//}
	//else if (lives == 1){
	//PORTC = 0b00100000;
	//move_cursor(3, 12);
	//
	//printf("\r%12d", get_lives());
	//}
	//else{
	//PORTC = 0b00000000;
	//move_cursor(3, 12);
	//
	//printf("\r%12d", get_lives());
	//}

	DDRA = 0xF0;
	if (lives == 4){
		PORTA |= 0b11110000;
		move_cursor(3, 12);
		
		printf("\r%12ld", get_lives());
	}
	else if (lives == 3){
		PORTA |= 0b01110000;
		move_cursor(3, 12);
		
		printf("\r%12ld", get_lives());
	}
	else if (lives == 2){
		PORTA |= 0b01100000;
		move_cursor(3, 12);
		
		printf("\r%12ld", get_lives());
	}
	else if (lives == 1){
		PORTA |= 0b01000000;
		move_cursor(3, 12);
		
		printf("\r%12ld", get_lives());
	}
	else{
		PORTA = 0b00000000;
		move_cursor(3, 12);
		
		printf("\r%12d", 0);
	}


	
 }

 

