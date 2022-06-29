/**
 ************************************************************************
 * @file ahu_shell.h
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Macros and Definitions for handling shell commands on NRF Dongle
 **********************************************************************
 * */
#ifndef AHU_SHELL
#define AHU_SHELL
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>
#include <shell/shell.h>
#include <logging/log.h>
#include <stdlib.h>
#include "hci.h"

/* Sleep Macro */
#define SLEEP_TIME_MS 500


/* Shell stack size and priority */
#define SHELL_STACK_SIZE 1024
#define SHELL_PRIORITY -1

/* ENUMS */
enum {OFF, ON, TOGGLE};
enum {RED, GREEN, BLUE};

/* --------- FIFO DATA STRUCTS ------------- */
struct data_item_t {
	void *fifo_reserved;
	uint8_t colour;
	uint8_t mode;
};

struct data_item_ble {
	void *fifo_reserved;
	uint8_t command;
	uint16_t user_data;
};

struct data_item_json {
	void *fifo_reserved;
	uint8_t toggle;
	int sample;
};

struct data_item_base {
	void* fifo_reserved;
	uint8_t toggle;
};

/* Function Definitions */
void ahu_shell_entry(void);

#endif
