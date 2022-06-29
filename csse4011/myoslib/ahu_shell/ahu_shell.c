/**
 ************************************************************************
 * @file ahu_shell.c
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Source code for handling shell commands on NRF Dongle
 **********************************************************************
 * */

#include "ahu_shell.h"

 /*-------- FIFO definitions -----------*/
K_FIFO_DEFINE(my_led_fifo);
K_FIFO_DEFINE(my_ble_fifo);
K_FIFO_DEFINE(json_fifo);
K_FIFO_DEFINE(base_fifo);

/* Register LOG module */
LOG_MODULE_REGISTER(shell_module, LOG_LEVEL_DBG);



/* FIFO structs */
struct data_item_t tx_data;
struct data_item_ble tx_ble;
struct data_item_json tx_json;
struct data_item_base tx_base;

/* Set continuous sampling */
int sample_time = 5;

// /* Declare command handler prototypes */
// static int cmd_led_ctrl_on(const struct shell*, size_t, char**);
// static int cmd_led_ctrl_off(const struct shell*, size_t, char**);
// static int cmd_led_ctrl_toggle(const struct shell*, size_t, char**);
// static int get_time_formatted(const struct shell* shell, size_t argc, char** argv);
// static int get_hts_temp(const struct shell* shell, size_t argc, char** argv);
// static int get_hts_humidity(const struct shell* shell, size_t argc, char** argv);
// static int get_lps_pressure(const struct shell* shell, size_t argc, char** argv);
// static int get_ccs_voc(const struct shell* shell, size_t argc, char** argv);
// static int get_lis_acc(const struct shell* shell, size_t argc, char** argv);
// static int get_pb_status(const struct shell* shell, size_t argc, char** argv);
// static int send_led(const struct shell* shell, size_t argc, char** argv);
// static int send_buzzer(const struct shell* shell, size_t argc, char** argv);
// static int send_dc(const struct shell* shell, size_t argc, char** argv);
// static int set_sample(const struct shell* shell, size_t argc, char** argv);
// static int toggle_all(const struct shell* shell, size_t argc, char** argv);
// static int base_recv(const struct shell* shell, size_t argc, char** argv);



// /* Specify Shell Commands */
// SHELL_STATIC_SUBCMD_SET_CREATE(led_ctrl,
// 	SHELL_CMD(o, NULL, "Turn led on.", cmd_led_ctrl_on),
// 	SHELL_CMD(f, NULL, "Turn led off.", cmd_led_ctrl_off),
// 	SHELL_CMD(t, NULL, "Toggle LED.", cmd_led_ctrl_toggle),
// 	SHELL_SUBCMD_SET_END
// );

// SHELL_STATIC_SUBCMD_SET_CREATE(hts_ctrl,
// 	SHELL_CMD(t, NULL, "Read HTS Temperature", get_hts_temp),
// 	SHELL_CMD(h, NULL, "Read HTS Humidity", get_hts_humidity),
// 	SHELL_SUBCMD_SET_END
// );


// SHELL_STATIC_SUBCMD_SET_CREATE(read_ctrl,
// 	SHELL_CMD(r, &hts_ctrl, "Read", NULL),
// 	SHELL_SUBCMD_SET_END
// );

// SHELL_STATIC_SUBCMD_SET_CREATE(read_acc_ctrl,
// 	SHELL_CMD(r, NULL, "Read", get_lis_acc),
// 	SHELL_SUBCMD_SET_END
// );

// SHELL_STATIC_SUBCMD_SET_CREATE(write_led,
// 	SHELL_CMD(w, NULL, "Write", send_led),
// 	SHELL_SUBCMD_SET_END
// );

// SHELL_STATIC_SUBCMD_SET_CREATE(write_buzzer,
// 	SHELL_CMD(w, NULL, "Write", send_buzzer),
// 	SHELL_SUBCMD_SET_END
// );

// SHELL_STATIC_SUBCMD_SET_CREATE(write_sample,
// 	SHELL_CMD(w, NULL, "Write", set_sample),
// 	SHELL_SUBCMD_SET_END
// );

// SHELL_STATIC_SUBCMD_SET_CREATE(write_dc,
// 	SHELL_CMD(w, NULL, "Write", send_dc),
// 	SHELL_SUBCMD_SET_END
// );


// /* Creating root shell commands */
// SHELL_CMD_REGISTER(base, NULL, "Read mobile node data", base_recv);
// SHELL_CMD_REGISTER(all, NULL, "Set dutycycle", toggle_all);
// SHELL_CMD_REGISTER(dc, &write_dc, "Set dutycycle", NULL);
// SHELL_CMD_REGISTER(sample, &write_sample, "Set sample rate", NULL);
// SHELL_CMD_REGISTER(buzzer, &write_buzzer, "Set buzzer frequency", NULL);
// SHELL_CMD_REGISTER(rgb, &write_led, "Set RBG LED", NULL);
// SHELL_CMD_REGISTER(pb, NULL, "Read pb status", get_pb_status);
// SHELL_CMD_REGISTER(lis2dh, &read_acc_ctrl, "Read lis2dh data", NULL);
// SHELL_CMD_REGISTER(ccs811, NULL, "Read ccs811 data", get_ccs_voc);
// SHELL_CMD_REGISTER(lps22, NULL, "Read lps22 data", get_lps_pressure);
// SHELL_CMD_REGISTER(led, &led_ctrl, "Led Control (On/Off)", NULL);
// SHELL_CMD_REGISTER(hts221, &read_ctrl, "Read hts221 data", NULL);
// SHELL_CMD_REGISTER(time, NULL, "Get the current uptime.", get_time_formatted);

// /* Command handler to print the system uptime */
// static int get_time_formatted(const struct shell* shell, size_t argc, char** argv) {
// 	uint64_t uptime = k_uptime_get();
// 	if (!strcmp(argv[1], "f")) {

// 		uint64_t hours = (uptime / 1000) / 3600;
// 		uint64_t minutes = ((uptime / 1000) - (hours * 3600)) / 60;
// 		uint64_t seconds = (uptime / 1000) - (hours * 3600) - (minutes * 60);
// 		shell_print(shell, "Formatted Uptime: %02lld:%02lld:%02lld", hours, minutes, seconds);
// 		LOG_INF("Formatted time shown");

// 	}
// 	else {

// 		shell_print(shell, "Uptime: %lld seconds", uptime / 1000);
// 	}

// 	return 1;
// }


// /* Command handler to change LED colour */
// static int cmd_led_ctrl_on(const struct shell* shell, size_t argc,
// 	char** argv)
// {
// 	ARG_UNUSED(argc);
// 	ARG_UNUSED(argv);

// 	if (argv[1][0] == 'r') {

// 		tx_data.colour = RED;
// 		LOG_INF("RED LED ON");
// 	}
// 	else if (argv[1][0] == 'g') {
// 		tx_data.colour = GREEN;
// 		LOG_INF("GREEN LED ON");
// 	}
// 	else if (argv[1][0] == 'b') {
// 		tx_data.colour = BLUE;
// 		LOG_INF("BLUE LED ON");
// 	}
// 	tx_data.mode = ON;
// 	k_fifo_put(&my_led_fifo, &tx_data);
// 	return 1;
// }

// static int base_recv(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'o') {
// 		tx_base.toggle = 1;
// 		k_fifo_put(&base_fifo, &tx_base);

// 	} else {
// 		printk("Invalid Arguments");
// 	}

// }

// /* Command Handler to set buzzer frequency */
// static int send_buzzer(const struct shell* shell, size_t argc, char** argv) {
// 	tx_ble.command = 9;
// 	tx_ble.user_data = (uint8_t)atoi(argv[1]);
// 	k_fifo_put(&my_ble_fifo, &tx_ble);
// 	LOG_INF("Sending Buzzer Frequency");

// 	return 1;
// }

// /* Command handler to send the device dutycycle */
// static int send_dc(const struct shell* shell, size_t argc, char** argv) {
// 	tx_ble.command = 11;
// 	tx_ble.user_data = (uint8_t)atoi(argv[1]);
// 	k_fifo_put(&my_ble_fifo, &tx_ble);
// 	LOG_INF("Sending Duty Cycle");

// 	return 1;
// }

// /* Command handler to set the continuous sampling rate */
// static int set_sample(const struct shell* shell, size_t argc, char** argv) {
// 	sample_time = atoi(argv[1]);
// 	LOG_INF("Setting sample time");


// 	return 1;
// }

// /* Command handler to toggle continuous sampling */
// static int toggle_all(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'o') {
// 		tx_json.toggle = 1;
// 		tx_json.sample = sample_time;
// 		k_fifo_put(&json_fifo, &tx_json);
// 	}
// 	else if (argv[1][0] == 'f') {
// 		tx_json.toggle = 0;
// 		tx_json.sample = sample_time;
// 		k_fifo_put(&json_fifo, &tx_json);

// 	}
// 	else {
// 		shell_print(shell, "Invalid Arguments\n");

// 	}
// 	LOG_INF("Toggling continuous sampling");
// 	return 1;

// }

// /* Command handler to get Pushbutton status */
// static int get_pb_status(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'r') {
// 		tx_ble.command = 10;
// 		tx_ble.user_data = 0;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else {
// 		shell_print(shell, "Invalid Arguments\n");
// 	}
// 	LOG_INF("Pushbutton Status");
// 	return 1;
// }

// /* Command handler to change LED colours on connected device */
// static int send_led(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'r') {
// 		tx_ble.command = 8;
// 		tx_ble.user_data = 1;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else if (argv[1][0] == 'g') {
// 		tx_ble.command = 8;
// 		tx_ble.user_data = 2;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else if (argv[1][0]) {
// 		tx_ble.command = 8;
// 		tx_ble.user_data = 3;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else {
// 		shell_print(shell, "Invalid Arguments\n");
// 	}
// 	LOG_INF("Sending LED write command");
// 	return 1;
// }

// /* Command handler to get accelerometer data */
// static int get_lis_acc(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'x') {
// 		tx_ble.command = 5;
// 		tx_ble.user_data = 0;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else if (argv[1][0] == 'y') {
// 		tx_ble.command = 6;
// 		tx_ble.user_data = 0;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);

// 	}
// 	else if (argv[1][0] == 'z') {
// 		tx_ble.command = 7;
// 		tx_ble.user_data = 0;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else {
// 		shell_print(shell, "Invalid Arguments\n");
// 	}
// 	LOG_INF("GET Acceleration");
// 	return 1;
// }

// /* Command handler to get VOC data */
// static int get_ccs_voc(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'r') {
// 		tx_ble.command = 4;
// 		tx_ble.user_data = 0;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else {
// 		shell_print(shell, "Invalid Arguments\n");
// 	}
// 	LOG_INF("GET VOC");
// 	return 1;
// }

// /* Command handler to get Pressure data */
// static int get_lps_pressure(const struct shell* shell, size_t argc, char** argv) {
// 	if (argv[1][0] == 'r') {
// 		tx_ble.command = 3;
// 		tx_ble.user_data = 0;
// 		k_fifo_put(&my_ble_fifo, &tx_ble);
// 	}
// 	else {
// 		shell_print(shell, "Invalid Arguments\n");
// 	}
// 	LOG_INF("GET Pressure");
// 	return 1;
// }

// /* Command handler to get Temperature data */
// static int get_hts_temp(const struct shell* shell, size_t argc, char** argv) {
// 	tx_ble.command = 1;
// 	tx_ble.user_data = 0;
// 	k_fifo_put(&my_ble_fifo, &tx_ble);
// 	LOG_INF("GET Temperature");
// 	return 1;
// }

// /* Command handler to get Humidity data */
// static int get_hts_humidity(const struct shell* shell, size_t argc, char** argv) {
// 	tx_ble.command = 2;
// 	tx_ble.user_data = 0;
// 	k_fifo_put(&my_ble_fifo, &tx_ble);
// 	LOG_INF("GET Humidity");
// 	return 1;
// }

// /* Command handler to toggle LED off */
// static int cmd_led_ctrl_off(const struct shell* shell, size_t argc,
// 	char** argv)
// {
// 	ARG_UNUSED(argc);
// 	ARG_UNUSED(argv);

// 	if (argv[1][0] == 'r') {

// 		tx_data.colour = RED;
// 	}
// 	else if (argv[1][0] == 'g') {
// 		tx_data.colour = GREEN;
// 	}
// 	else if (argv[1][0] == 'b') {
// 		tx_data.colour = BLUE;
// 	}
// 	tx_data.mode = OFF;
// 	k_fifo_put(&my_led_fifo, &tx_data);
// 	LOG_INF("Turn LED Off");
// 	return 1;
// }

// /* Command handler to toggle LED */
// static int cmd_led_ctrl_toggle(const struct shell* shell, size_t argc, char** argv) {

// 	if (argv[1][0] == 'r') {

// 		tx_data.colour = RED;
// 	}
// 	else if (argv[1][0] == 'g') {
// 		tx_data.colour = GREEN;
// 	}
// 	else if (argv[1][0] == 'b') {
// 		tx_data.colour = BLUE;
// 	}
// 	tx_data.mode = TOGGLE;
// 	k_fifo_put(&my_led_fifo, &tx_data);
// 	LOG_INF("Toggle LED");

// 	return 1;

// } 

/* Shell command thread entry */
void ahu_shell_entry(void)
{

	//k_fifo_init(&my_led_fifo);

	/* Setup DTR */
	const struct device* shell_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;

	if (usb_enable(NULL))
		return;

	/* Wait on DTR - 'Data Terminal Ready'
	 * Will wait here until a terminal has been attached to the device
	 * This is not necessary, however, can be useful from reading early data
	 *
	 * Remove the following while loop if DTR is not required!!
	 */

	//while (!dtr) {
	uart_line_ctrl_get(shell_dev, UART_LINE_CTRL_DTR, &dtr);
	//	k_sleep(K_MSEC(100));
	//}

	/* DTR - OK, Continue */



}