/*
 * score.h
 * 
 * Author: Peter Sutton
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <stdint.h>

void init_score(void);
void add_to_score(uint16_t value);
uint32_t get_score(void);
void display_score(void);
void display_high_score(void);
void display_seg(void);
void update_seg(void);
void set_score_flag(void);
uint32_t get_score_flag(void);

#endif /* SCORE_H_ */