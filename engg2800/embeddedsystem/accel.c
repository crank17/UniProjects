/*
 * accel.c
 *
 * Utilises Peter Fleury i2c master library to provide usable data from device ADXL343
 *
 * Created: 14/10/2020 
 * Author : Komei
 */ 

#include <avr/io.h>
//#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdarg.h>

#include "i2cmaster.h"

#define DevADXL343 0x1D      // device address of ADXL343
#define ADXL343WRITE 0x3A	// device write address
#define ADXL343READ 0x3B // device read address


// Global variables
uint8_t x0, y_0, z0;

/*
	343 set up by setting registers to desirable parameters
*/
uint8_t ADXL343_setup(void) {
	// Setting up 343 for I2C
	i2c_init();
	int ret = i2c_start(ADXL343WRITE);     // set device address and write mode
	if (ret) {
		// Failed to access device
		i2c_stop();
		return 1;
		} else {
		
		i2c_write(0x2E); // Address of interrupts
		i2c_write(0); // Disable interrupts
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x1D); //THRESH_TAP
		i2c_write(0x90); // Tap threshold 9g
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x21); // DUR
		i2c_write(0x40); // Max tap duration 10ms
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x22); // Latent
		i2c_write(0x08); // 10ms 
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x23); // Window
		i2c_write(0x90); // 160ms
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x2A);
		i2c_write(0x02); // Enable y for tap
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x2D);
		i2c_write(0x08); // Enable measurements
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x31); // Change data format
		i2c_write(0);
		i2c_stop();
		
		return 0;

	}	
}

/*
	343 single and double tap setup
*/
uint8_t ADXL343_interrupt_setup(void) {
	int ret = i2c_start(ADXL343WRITE);     // set device address and write mode
	if (ret) {
		//Failed to access 343
		i2c_stop();	
		return 1;
	} else {
		// Configure interrupts before enabling (disable if already on)
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x2E); // Address of interrupts
		i2c_write(0); // Disable all interrupts
		i2c_stop();
		
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x2F); // Address of INT_MAP
		i2c_write(0b00100000); // Single tap on INT1 pin and double tap on INT2 pin
		i2c_stop();		
		
		// Last thing to do is enable interrupts
		i2c_start_wait(ADXL343WRITE);
		i2c_write(0x2E); // Address of interrupts
		i2c_write(0b01100000); // Enable single tap and double tap only
		i2c_stop();
	}
	return 0;
	
}

/*
 *	Calculates the mode the device is in depending on x and y data from the ADXL343
 *	Returns the mode and if no particular mode determined returns 0
 */
uint8_t ADXL343_calc_mode(unsigned char x, unsigned char y) {
	// Determines orientation with accel x and y values (x1 and y1)
	// Returns 1,2,3 or 4 but 0 if undetermined (unexpected x1 and/or y1 values)
	if (x == 0 && y == 255) {
		return 1;
	}
	if (x == 1 && y == 0) {
		return 2;	
	}	
	if (x == 0 && y == 1) {
		return 3;
	}
	if (x == 255 && y == 0) {
		return 4;
	}
	return 0;
}

/*
 *	Check if single or double tap has been detected
 *	Returns	0 if no tap
 *		1 if single tap
 *		2 if double tap
 */
uint8_t ADXL343_check_tap(void) {
	unsigned char tap;
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x30);
	i2c_rep_start(ADXL343READ); 
	tap = i2c_readNak();
	i2c_stop();
	
	if (tap & (1 << 6)) {
		if (tap & (1 << 5)) {
			// Double tap bit is set
			return 2;
		}
		// Single tap is set
		return 1;
	}
	return 0;
		
}

// Reads and returns register x1 of the ADXL343
unsigned char ADXL343_read_x(void) {
	unsigned char x1 = 0;
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x33);
	i2c_rep_start(ADXL343READ); 
	x1 = i2c_readNak();
	i2c_stop();
	return x1;
}

// Reads and returns register y_1 of the ADXL343
unsigned char ADXL343_read_y(void) {
	unsigned char y_1 = 0;
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x35);
	i2c_rep_start(ADXL343READ); 
	y_1 = i2c_readNak();
	i2c_stop();
	return y_1;
}

// Reads and returns register z1 of the ADXL343
unsigned char ADXL343_read_z(void) {
	unsigned char z1 = 0;
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x37);
	i2c_rep_start(ADXL343READ); 
	z1 = i2c_readNak();
	i2c_stop();
	return z1;
}

// Read x acceleration readings from ADXL343 
unsigned char ADXL343_accel_x() {
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x32);
	i2c_rep_start(ADXL343READ);
	x0 = i2c_readNak();
	i2c_stop();
	return x0;	
}

// Read y acceleration readings from ADXL343
unsigned char ADXL343_accel_y() {
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x34);
	i2c_rep_start(ADXL343READ);
	y_0 = i2c_readNak();
	i2c_stop();
	return y_0;	
}

// Read z acceleration readings from ADXL343
unsigned char ADXL343_accel_z() {
	i2c_start_wait(ADXL343WRITE);
	i2c_write(0x36);
	i2c_rep_start(ADXL343READ);
	z0 = i2c_readNak();
	i2c_stop();
	return z0;	
}