#ifndef ACCEL_H
#define ACCEL_H

#define DevADXL343 0x1D      // device address of ADXL343
#define ADXL343WRITE 0x3A
#define ADXL343READ 0x3B

uint8_t ADXL343_setup(void);
uint8_t ADXL343_interrupt_setup(void);
uint8_t ADXL343_calc_mode(unsigned char x, unsigned char y);
uint8_t ADXL343_check_tap(void);
unsigned char ADXL343_read_x(void);
unsigned char ADXL343_read_y(void);
unsigned char ADXL343_read_z(void);
unsigned char ADXL343_accel_x(void);
unsigned char ADXL343_accel_y(void);
unsigned char ADXL343_accel_z(void);

#endif