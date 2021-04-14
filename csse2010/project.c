/*
 * project.c
 *
 * Main file
 *
 * Author: Peter Sutton. Modified by <YOUR NAME HERE>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "ledmatrix.h"
#include "scrolling_char_display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "score.h"
#include "timer0.h"
#include "player.h"
#include "alien.h"
#include "game_background.h"
#include "projectile.h"
#include "game.h"
#include "sound.h"
#include "joystick.h"
#include "sound.h"
#include "ssg.h"

#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void splash_screen(void);
void new_game(void);
void play_game(void);

void handle_game_over(void);


// ASCII code for Escape character
#define ESCAPE_CHAR 27

int newgame = 0;

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware. This will turn on interrupts.
	
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	splash_screen();
	
	
		
	while(1) {
		
	
		if (newgame == 1){
		ledmatrix_clear();

		new_game();
		play_game();
		handle_game_over();
		}
		ledmatrix_clear();
			
		new_game();
		play_game();	
		handle_game_over();
	
		
	}
}

void initialise_hardware(void) {
	
	
	
	
	ledmatrix_setup();
	init_button_interrupts();
	display_lives_led();
	init_sound();
	init_timer2();
	
	
	// Setup serial port for 38400 baud communication with no echo
	// of incoming characters
	init_serial_stdio(38400,0);
	
	init_timer0();
//	init_timer2();

	ADMUX |= (1<<REFS0);
	//set prescaller to 128 and enable ADC
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADEN);
	
	// Turn on global interrupts
	sei();
}

void splash_screen(void) {
	char serial_input;
	// Reset display attributes and clear terminal screen then output a message
	set_display_attribute(TERM_RESET);
	clear_terminal();
	
	hide_cursor();	// We don't need to see the cursor when we're just doing output
	move_cursor(3,3);
	printf_P(PSTR("Space Impact"));
	
	move_cursor(3,5);
	set_display_attribute(FG_GREEN);	// Make the text green
	printf_P(PSTR("CSSE2010/7201 project by Ryan Ward 45813685"));	
	set_display_attribute(FG_WHITE);	// Return to default colour (White)
	
	// Output the scrolling message to the LED matrix
	// and wait for a push button to be pushed.
	ledmatrix_clear();
	while(1) {
		set_scrolling_display_text("SPACE IMPACT", COLOUR_ORANGE);
		// Scroll the message until it has scrolled off the 
		// display or a button is pushed. We pause for 130ms between each scroll.
		while(scroll_display()) {
			_delay_ms(130);
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				if (serial_input == 'n')
				return;
			}
			if(button_pushed() != NO_BUTTON_PUSHED) {
				
				return;
			}
		}
		set_scrolling_display_text("45813685", COLOUR_GREEN);
		// Scroll the message until it has scrolled off the
		// display or a button is pushed. We pause for 130ms between each scroll.
		while(scroll_display()) {
			_delay_ms(130);
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				if (serial_input == 'n')
				return;
			}
			if(button_pushed() != NO_BUTTON_PUSHED) {
				return;
			}
		}
	}
}

void new_game(void) {
	
	init_level();
	init_lives();
	init_speed();
	
	//InitADC();
	
	//ledmatrix_clear();
	// Initialise the game elements and display
	ledmatrix_clear();
	init_background();
	init_aliens();
	init_projectiles();
	init_player();
	newgame = 0;
	
	
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the score
	init_score();
	
	
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
	
	
	// Delay for half a second
	_delay_ms(500);
	set_sound(4);
}

void play_game(void) {
	uint32_t current_time, last_move_time, last_alien_add_time, last_alien_move_time, last_button_check, scroll_time, sound_time;
	uint32_t last_projectile_move_time, check1, check2, check3;
	int8_t button, joystick;
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	//uint8_t cycle_counter = 0;
	//DDRC = 0xFf;
	//PIND4 & 0;
	

	
	// Get the current time and remember this as the last time any of our events
	// happened.
	current_time = get_current_time();
	last_move_time = current_time;
	last_alien_add_time = current_time;
	last_alien_move_time = current_time;
	last_projectile_move_time = current_time;
	last_button_check = current_time;
	scroll_time = current_time;

	sound_time = current_time;
	
	//printf("\nScore\n");
	//printf("%d", get_score());
	
	move_cursor(8,5);
	printf_P(PSTR("Score"));
	move_cursor(3,6);
	display_score();
	move_cursor(3,7);
	printf_P(PSTR("High Score"));
	move_cursor(3,8);
	display_high_score();
	move_cursor(8, 9);
	printf_P(PSTR("Level"));
	move_cursor(3, 10);
	display_level();
	move_cursor(8, 11);
	printf_P(PSTR("Lives"));
	move_cursor(3, 12);
	display_lives_led();
	

	
	//seven_seg_display();
	
	// We play the game while the player isn't dead
	while(!is_player_dead()) {
		
		// Check for input - which could be a button push or serial input.
		// Serial input may be part of an escape sequence, e.g. ESC [ D
		// is a left cursor key press. We will be processing each character
		// independently and can't do anything until we get the third character.
		// At most one of the following three variables will be set to a value 
		// other than -1 if input is available.
		// (We don't initalise button to -1 since button_pushed() will return -1
		// if no button pushes are waiting to be returned.)
		// Button pushes take priority over serial input. If there are both then
		// we'll retrieve the serial input the next time through this loop
		serial_input = -1;
		escape_sequence_char = -1;
		button = button_pushed();
		joystick = joystick_input();
		
	
		
		
			
		
		
		
		
		if(button == -1) {
			// No push button was pushed, see if there is any serial input
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				// Check if the character is part of an escape sequence
				if(characters_into_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
					// We've hit the first character in an escape sequence (escape)
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 1 && serial_input == '[') {
					// We've hit the second character in an escape sequence
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 2) {
					// Third (and last) character in the escape sequence
					escape_sequence_char = serial_input;
					serial_input = -1;  // Don't further process this character - we
										// deal with it as part of the escape sequence
					characters_into_escape_sequence = 0;
				} else {
					// Character was not part of an escape sequence (or we received
					// an invalid second character in the sequence). We'll process 
					// the data in the serial_input variable.
					characters_into_escape_sequence = 0;
				}
			}
		}
		
		
		// Process the input. 
		if(button==3 || serial_input == ' ') {
			// Button 3 pressed or space bar
			fire_projectile_if_possible(); 
			
		} else if (escape_sequence_char=='D') {
			// Left cursor key escape sequence received
			move_player_left();
			
			
		} else if(escape_sequence_char=='C') {
			// Right cursor key
			move_player_right();
			joystick = 0;
		} else if(button == 0 ||escape_sequence_char=='B') {
			// Button 0 or down cursor key pressed - attempt to move down
			move_player_down();
			joystick = 0;
			
		} else if(button == 1 || escape_sequence_char=='A') {
			// Button 1 or up cursor key pressed - attempt to move up
			move_player_up();
			joystick = 0;
		} else if(button == 2) {
			// Button 2 pressed. Unimplemented feature - game speed toggle
			set_speed();
			
		} else if(serial_input == 'p' || serial_input == 'P') {
			// Unimplemented feature - pause/unpause the game until 'p' or 'P' is
			// pressed again
			while(serial_input != 'n' || serial_input != 'N' || serial_input != 'p' || serial_input != 'P') {
				set_sound(0);
				move_cursor(3,13);
				printf_P(PSTR("Game is Paused"));

				if(serial_input_available()) {
					
					// Serial data was available - read the data from standard input
					serial_input = fgetc(stdin);
					if (serial_input == 'n'){
						newgame = 1;
						return;
					}
					if (serial_input == 'p' || serial_input == 'P'){
						move_cursor(3,13);
						clear_to_end_of_line();
						break;
					}
				}
				
		}
		}
		else if(serial_input == 'n' || serial_input == 'N'){
			newgame = 1;
			return;
			//try return value
		} 
		if (get_counter() == 1){
			set_sound(0);
			_delay_ms(50);
			(void)button_pushed();
			clear_serial_input_buffer();
			_delay_ms(50);
			reset_counter();
			ledmatrix_clear();
			init_background();
			init_player();
			init_projectiles();
			init_aliens();
			display_level();
			
		}
		if(lives_flag() == 1){
			
			set_sound(0);
			_delay_ms(2000);
			
			
			
			
			reset_counter();
			display_lives_led();
			if (get_lives() == 0){
				
				return;
			}
			
			
			init_background();
			init_player();
			init_projectiles();
			init_aliens();
			(void)button_pushed();
			joystick = 0;
			clear_serial_input_buffer();
			
			

		}
		
		// else - invalid input or we're part way through an escape sequence -
		// do nothing
		
		current_time = get_current_time();
		if(!is_player_dead()) {
			if (get_speed() == 1){
				check1 = 300;
				check2 = 200;
				check3 = 150;
				move_cursor(3,3);
				printf_P(PSTR("Double Speed!"));

			}
			else{
				check1 = 600;
				check2 = 400;
				check3 = 300;
				move_cursor(3,3);
				clear_to_end_of_line();

			}
			
			if(current_time >= last_move_time + check1) {
				// 600ms (0.6 second) has passed since the last time we scrolled
				// the background, so scroll it now and check whether that means
				// we've finished the level. (If a crash occurs we will drop out of 
				// the main while loop so we don't need to check for that here.)
				scroll_background();
				
				last_move_time = current_time;
			}
			if(current_time > last_alien_add_time + 1000) {
				// 1 second has passed since the last time we tried to add an 
				// alien - so try now
				add_alien_to_game();
				last_alien_add_time = current_time;
			}
			if(current_time > last_alien_move_time + check2) {
				// 400ms has passed since the last time we tried to move an 
				// alien - so try now
				move_random_alien();
				last_alien_move_time = current_time;
			}
			if(current_time > last_projectile_move_time + check3) {
				// 300ms has passed since the last projecile move - try now
				advance_projectiles();
				last_projectile_move_time = current_time;
				//animate(get_alien());
			}
			if (current_time > 20){
				display_lives_led();
				

			}
			if (current_time > 15 + sound_time){
				play_sound();
				sound_time = current_time;
			}
			if (current_time > 100 + last_button_check){
			
			if (joystick == 1){
				move_player_left();
			}
			if (joystick == 2){
				move_player_right();
			}
			if (joystick == 3){
				move_player_down();
				}
			if (joystick == 4){
			move_player_up();
				}
			if (joystick == 5){
				move_player_up();
				move_player_right();
			}
			if (joystick == 6){
				move_player_up();
				move_player_left();
			}
			if (joystick == 7){
				move_player_down();
				move_player_right();
			}
			if (joystick == 8){
				move_player_down();
				move_player_left();
			}
				if (current_time > 500 + scroll_time){
				
								
				if (get_button_state() == 1){
					
					move_player_down();
					}
				else if (get_button_state() ==2){
					
					move_player_up();
				}
				else if (get_button_state() == 3){
					fire_projectile_if_possible();
				}
				scroll_time = current_time;
				}
				last_button_check = current_time;
			}
			
			
			
			
		}
	}
	// If we get here the player has died. 
}



void handle_game_over() {
	char serial_input;
	if (newgame ==1){
	return;
	}
	set_sound(0);
	move_cursor(10,14);
	// Print a message to the terminal. 
	printf_P(PSTR("GAME OVER"));
	move_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	
	while(button_pushed() == -1) {
	
	if(serial_input_available()) {
		// Serial data was available - read the data from standard input
		serial_input = fgetc(stdin);
		if (serial_input == 'n')
		return;
	}

		; // wait until a button has been pushed
	}
	
	
}

