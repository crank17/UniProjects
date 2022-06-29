#ifndef NODE_STATIC
#define NODE_STATIC
#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>   
#include <sys_clock.h>
#include <timing/timing.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <zephyr/types.h>
#include <irq.h>
#include <stddef.h>



#define TRIGPIN 1
#define ECHOPIN 2
#define ULTRA_STACK 1024
#define ULTRA_PRIO 1
#define BT_STAT_STACK 1024
#define BT_STAT_PRIO 2






struct data_ultra_t {
	void* fifo_reserved;
	uint32_t dis;
};

void thread_ultra_entry(void);

void thread_bt_ad(void);



#endif