/**
 ************************************************************************
 * @file hci.h
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Macros and Definitions for the HCI
 **********************************************************************
 * */
#ifndef HCI_H
#define HCI_H

#include <zephyr.h>
#include <device.h>

#include <stdio.h>
#include <stdlib.h>
#include "ahu_ble.h"

/* DIDS */
#define PREAMBLE 0xAA
#define REQUEST 0x01
#define RESPONSE 0x02
#define TEMPERATURE 1
#define HUMIDITY 2
#define PRESSURE 3
#define VOC 4
#define XACC 5
#define YACC 6
#define ZACC 7
#define RGBLED 8
#define BUZZER 9
#define BUTTON 10
#define DEVICE_OFF 0

#define LENGTH 0x0A
/* HCI Thread stack size and priority */
#define HCI_STACK_SIZE 512
#define HCI_PRIORITY 3

/* fifo struct */
extern struct k_fifo my_packet_fifo;

/*Function Signatures */
void construct_packet(uint16_t DID, int8_t* ahu_data, uint16_t user_data);
void thread_hci_entry(void);

#endif