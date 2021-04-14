/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include "score.h"
#include <stdio.h>
#include "terminalio.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"
#define F_CPU 8000000L
#include <util/delay.h>
#include "game.h"
#include "sound.h"





uint32_t score;
uint32_t prev_score;
uint32_t high_score = 0;
//uint8_t seven_seg[10] = { 63,6,91,79,102,109,125,7,127,111};
//volatile uint8_t digit = 0;
uint8_t value1;
//volatile uint16_t count = 0;
//volatile uint8_t seven_seg_cc = 0;



void init_score(void) {
	score = 0;
	prev_score = 0;
	
}




void add_to_score(uint16_t value) {
	score += value;
	prev_score = 1;
	set_sound(1);
	
}

void set_score_flag(void){
	if (prev_score == 0){
		prev_score = 1;
	}
	else{
		prev_score = 0;
	}
	}


uint32_t get_high_score(void) {
	return high_score;
}

uint32_t get_score(void) {
	return score;
}

uint32_t get_score_flag(void) {
	return prev_score;
}

void display_score(void) {
	move_cursor(3,6);
	
	printf("\r%12ld", get_score());
	//printf("\n");
	//fflush(stdout);

}

void display_high_score(void) {
	
	if (get_score() > get_high_score()){
		high_score = get_score();
		move_cursor(3,8);
		
		printf("\r%12ld", get_high_score());

	}
	
	
	move_cursor(3,8);
	
	printf("\r%12ld", get_high_score());

}










	
	




