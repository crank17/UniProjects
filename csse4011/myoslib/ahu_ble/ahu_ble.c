/**
 ************************************************************************
 * @file ahu_ble.c
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Source code for the AHU ble drivers
 **********************************************************************
 * */

#include "ahu_ble.h"

 /* Get Pushbutton GPIO from devicetree */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

/* Register LOG Module */
LOG_MODULE_REGISTER(ble_module, LOG_LEVEL_DBG);

/* Function Signature to start BLE Scan */
static void start_scan(void);

/* FIFO definition */
K_FIFO_DEFINE(my_packet_fifo);

/* Connection Struct */
static struct bt_conn* default_conn;
/* Fifo data structs */
struct data_packet_t pkt_data;
struct data_item_ble json_data;

/* Search and discover bools */
bool dev_found = false;
bool discovered = false;
/* Toggle JSON continuous sampling */
bool toggle_all_c = false;
bool writeJSON = true;

/* GATT handles */
static uint16_t chrc_handle;
static uint16_t long_chrc_handle;

char* nodeNames[] = { "Ryan Node", "Tom Phone", "Tom Node", "Ryan Phone" };

char* iBeacons[] = { "4011-A", "4011-B", "4011-C", "4011-D", "4011-E", "4011-F", "4011-G", "4011-H", "4011-I", "4011-J", "4011-K", "4011-L", "TomNode", "RyanNode" };

/* UUID for device */
uint16_t dongle_uuid[] = { 0xd0, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
						  0x26, 0x49, 0x60, 0xeb, 0x12, 0x34, 0x56, 0x78 };

/* UUID for device */

static struct bt_uuid_128 rssiUUID = BT_UUID_INIT_128(
	0xd2, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
	0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb);

static struct bt_uuid_128 ultraSonicUUID = BT_UUID_INIT_128(
	0xd3, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
	0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb);

static struct bt_uuid_128 timeStampUUID = BT_UUID_INIT_128(
	0xd7, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
	0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb);

static struct bt_uuid_128 testuuid = BT_UUID_INIT_128(
	0xd0, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
	0x26, 0x49, 0x60, 0xeb, 0x12, 0x34, 0x56, 0x78);

/* GATT receive uuid */
static struct bt_uuid_128 node_rx_uuid = BT_UUID_INIT_128(
	0xd2, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
	0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb);

/* Gatt send uuid */
static struct bt_uuid_128 node_tx_uuid = BT_UUID_INIT_128(
	0xd1, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
	0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb);

/* HCI packets */
uint8_t ahu_data[] = { PREAMBLE, REQUEST, LENGTH, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t rx_data[] = { PREAMBLE, RESPONSE, LENGTH, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int8_t rssiData[14] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint16_t ultrasonicData[4] = { 0x00, 0x00, 0x00, 0x01 };
int64_t mobileTimestamp;

/* Button GPIO handler */
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
	{ 0 });

/* Button callback struct */
static struct gpio_callback button_cb_data;

/* Button Callback function */
void button_pressed(const struct device* dev, struct gpio_callback* cb,
	uint32_t pins)
{
	writeJSON = !writeJSON;
}

/* Function to check connecting device UUID */
static bool parse_device(struct bt_data* data, void* user_data)
{
	bt_addr_le_t* addr = user_data;
	int i;
	int matchedCount = 0;

	if (data->type == BT_DATA_UUID128_ALL)
	{
		uint16_t temp = 0;
		printk("Found Device\n");
		for (i = 0; i < data->data_len; i++)
		{
			temp = data->data[i];
			if (temp == dongle_uuid[i])
			{
				matchedCount++;
			}
		}

		if (matchedCount == UUID_BUFFER_SIZE)
		{
			// MOBILE UUID MATCHED
			LOG_INF("Device Found");
			int err = bt_le_scan_stop();
			k_msleep(10);
			if (err)
			{
				printk("Stop LE scan failed (err %d)\n", err);
				return true;
			}
			struct bt_le_conn_param* param = BT_LE_CONN_PARAM(BT_GAP_INIT_CONN_INT_MIN, BT_GAP_INIT_CONN_INT_MAX, 0, 800);
			err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN_AUTO,
				param, &default_conn);
			LOG_INF("Device Connected");
			if (err)
			{
				printk("Create conn failed (err %d)\n", err);
				start_scan();
			}

			return false;
		}
	}
	return true;
}

/* Device found callback */
static void device_found(const bt_addr_le_t* addr, int8_t rssi, uint8_t type,
	struct net_buf_simple* ad)
{

	if (default_conn)
	{
		return;
	}

	/* We're only interested in connectable events */

	bt_data_parse(ad, parse_device, (void*)addr);

}

/* Device disconnected callback */
static void disconnected(struct bt_conn* conn, uint8_t reason) {
	char addr[BT_ADDR_LE_STR_LEN];

	if (conn != default_conn)
	{
		return;
	}

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_WRN("Device disconnected, %d", reason);

	bt_conn_unref(default_conn);
	default_conn = NULL;
	dev_found = false;
	discovered = false;

	start_scan();
}

/* Gatt read callback */
static ssize_t read_test_chrc(struct bt_conn* conn,
	const struct bt_gatt_attr* attr,
	void* buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
		(void*)rx_data, sizeof(rx_data));
}

/* Gatt write callback - puts buffer into fifo to be processed */
static ssize_t write_test_chrc(struct bt_conn* conn,
	const struct bt_gatt_attr* attr,
	const void* buf, uint16_t len,
	uint16_t offset, uint8_t flags)
{
	(void)memcpy(rx_data + offset, buf, len);
	memcpy(pkt_data.packet_data, rx_data, len);
	pkt_data.jsonOutput = toggle_all_c;
	k_fifo_put(&my_packet_fifo, &pkt_data);

	return len;
}

/* Discover GATT characteristics */
static uint8_t discover_func(struct bt_conn* conn,
	const struct bt_gatt_attr* attr,
	struct bt_gatt_discover_params* params)
{
	int err;

	if (attr == NULL)
	{
		if (chrc_handle == 0 || long_chrc_handle == 0)
		{
			LOG_INF("Found Characteristics");
		}

		(void)memset(params, 0, sizeof(*params));

		return BT_GATT_ITER_STOP;
	}

	if (params->type == BT_GATT_DISCOVER_PRIMARY &&
		bt_uuid_cmp(params->uuid, &testuuid.uuid) == 0)
	{
		params->uuid = NULL;
		params->start_handle = attr->handle + 1;
		params->type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, params);
		if (err != 0)
		{
			LOG_WRN("Discover failed");
		}

		return BT_GATT_ITER_STOP;
	}
	else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC)
	{
		struct bt_gatt_chrc* chrc = (struct bt_gatt_chrc*)attr->user_data;

		if (bt_uuid_cmp(chrc->uuid, &node_tx_uuid.uuid) == 0)
		{
			LOG_INF("Found characteristics");
			chrc_handle = chrc->value_handle;
		}
		else if (bt_uuid_cmp(chrc->uuid, &ultraSonicUUID.uuid) == 0)
		{
			long_chrc_handle = chrc->value_handle;
		}
	}

	return BT_GATT_ITER_CONTINUE;
}

/* Discover GATT characteristics */
static void gatt_discover(void)
{
	static struct bt_gatt_discover_params discover_params;
	int err;

	LOG_INF("Discovering services and characteristics");

	discover_params.uuid = &testuuid.uuid;
	discover_params.func = discover_func;
	discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	discover_params.type = BT_GATT_DISCOVER_PRIMARY;

	err = bt_gatt_discover(default_conn, &discover_params);

	LOG_INF("Discover complete");
	discovered = true;
}

/* GATT write callback */
static void gatt_write_cb(struct bt_conn* conn, uint8_t err,
	struct bt_gatt_write_params* params)
{
	(void)memset(params, 0, sizeof(*params));
}

/* Write to GATT characteristic */
static void gatt_write()
{
	static struct bt_gatt_write_params write_params;
	int err;
	write_params.data = ahu_data;
	write_params.length = sizeof(ahu_data);
	write_params.func = gatt_write_cb;
	write_params.handle = chrc_handle;
	err = bt_gatt_write(default_conn, &write_params);
}

/* Register GATT service */
BT_GATT_SERVICE_DEFINE(test_svc,
	BT_GATT_PRIMARY_SERVICE(&testuuid.uuid),
	BT_GATT_CHARACTERISTIC(&node_tx_uuid.uuid, BT_GATT_CHRC_WRITE | BT_GATT_CHRC_READ, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, read_test_chrc, write_test_chrc, NULL),

	);


/* Device connected callback */
static void connected(struct bt_conn* conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err)
	{
		LOG_INF("Failed to connect");

		bt_conn_unref(default_conn);
		default_conn = NULL;

		start_scan();
		return;
	}

	if (conn != default_conn)
	{
		return;
	}
	default_conn = conn;

	gatt_discover();

	dev_found = true;
}

/* BT disconnected and connected callback functions */
BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

/* GATT read funtion */
uint8_t read_rx_data(struct bt_conn* conn, uint8_t err,
	struct bt_gatt_read_params* params,
	const void* data, uint16_t length)
{
	memcpy(&rx_data, data, sizeof(rx_data));
	return 0;
}

/* Scan for devices */
static void start_scan(void)
{
	int err;


	/* This demo doesn't require active scan */
	err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
	if (err)
	{
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}
	LOG_INF("Scanning successfully started\n");
}

uint8_t read_rssi_data(struct bt_conn* conn, uint8_t err,
	struct bt_gatt_read_params* params,
	const void* data, uint16_t length)
{
	memcpy(&rssiData, data, sizeof(rssiData));

	//printk("RSSI: N1:%d, N2:%d, N3:%d, N4:%d\n", rx_rssi[0], rx_rssi[1], rx_rssi[2], rx_rssi[3]);
	return 0;
}

uint8_t read_ultrasonic_data(struct bt_conn* conn, uint8_t err,
	struct bt_gatt_read_params* params,
	const void* data, uint16_t length)
{
	memcpy(&ultrasonicData, data, sizeof(ultrasonicData));
	//printk("RSSI: N1:%d, N2:%d, N3:%d, N4:%d\n", rx_rssi[0], rx_rssi[1], rx_rssi[2], rx_rssi[3]);
	//memset(params, 0, sizeof(params));
	return 0;
}

uint8_t read_time_data(struct bt_conn* conn, uint8_t err,
	struct bt_gatt_read_params* params,
	const void* data, uint16_t length)
{
	memcpy(&mobileTimestamp, data, sizeof(int64_t));
	//printk("RSSI: N1:%d, N2:%d, N3:%d, N4:%d\n", rx_rssi[0], rx_rssi[1], rx_rssi[2], rx_rssi[3]);
	//memset(params, 0, sizeof(params));
	return 0;
}

/* Thread for searching and discovery of BLE devices */
void thread_ble_entry(void)
{
	int err;
	err = bt_enable(NULL);
	if (err)
	{
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
	LOG_INF("Bluetooth initialized");
	start_scan();
}

static uint8_t gatt_read_cb(struct bt_conn* conn, uint8_t err,
	struct bt_gatt_read_params* params,
	const void* data, uint16_t length)
{
	if (err != BT_ATT_ERR_SUCCESS) {
		LOG_INF("Read failed: 0x%02X\n", err);
	}

	uint16_t offset = params->single.offset;
	printk("ULTRA: %d OFFSET: %d\n", length, offset);
	uint16_t* datap = (uint16_t*)data;

	memcpy(&ultrasonicData[offset], &datap, (length / 2) * sizeof(uint16_t));

	printk("\n");


	return 0;
}

static void gatt_read() {
	static struct bt_gatt_read_params read_params;

	read_params.func = gatt_read_cb;
	read_params.handle_count = 1;
	read_params.single.handle = long_chrc_handle;
	read_params.single.offset = 0;

	int err = bt_gatt_read(default_conn, &read_params);

}

void thread_base_read(void) {
	struct data_item_base* rx_base;
	uint8_t mode = 0;
	static struct bt_gatt_read_params read_params_rssi = {
		.func = read_rssi_data,
		.handle_count = 0,
		.by_uuid.uuid = &rssiUUID.uuid,
		.by_uuid.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE,
		.by_uuid.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE,
	};

	static struct bt_gatt_read_params read_param_ultra = {
		.func = read_ultrasonic_data,
		.handle_count = 0,
		.by_uuid.uuid = &ultraSonicUUID.uuid,
		.by_uuid.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE,
		.by_uuid.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE,
	};

	static struct bt_gatt_read_params read_timestamp = {
		.func = read_time_data,
		.handle_count = 0,
		.by_uuid.uuid = &timeStampUUID.uuid,
		.by_uuid.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE,
		.by_uuid.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE,
	};


	int i = 0;
	int err;
	int64_t baseUptime;
	while (1) {
		//rx_base = k_fifo_get(&base_fifo, K_NO_WAIT);
		//if (rx_base != NULL){
		//	mode = rx_base->toggle;

		if (dev_found) {
			bt_gatt_read(default_conn, &read_params_rssi);
			//gatt_read();
			k_sleep(K_MSEC(10));
			err = bt_gatt_read(default_conn, &read_param_ultra);
			k_sleep(K_MSEC(10));
			err = bt_gatt_read(default_conn, &read_timestamp);
			baseUptime = k_uptime_get();
			//printk("ERROR: %d\n", err);
			//printk("{");
			//for (int i = 0; i < 14; i++) {
			//	printk("%s: [Ultra: %d, RSSI: %d],", iBeacons[i], ultrasonicData[i], rssiData[i]);
			//}
			//printk("}\n");
			printk("{4011-A: [RSSI: %d]," \
				"4011-B: [RSSI: %d]," \
				"4011-C: [RSSI: %d]," \
				"4011-D: [RSSI: %d]," \
				"4011-E: [RSSI: %d]," \
				"4011-F: [RSSI: %d]," \
				"4011-G: [RSSI: %d]," \
				"4011-H: [RSSI: %d]," \
				"4011-I: [RSSI: %d]," \
				"4011-J: [RSSI: %d]," \
				"4011-K: [RSSI: %d]," \
				"4011-L: [RSSI: %d]," \
				"Uptime: [Base: %lld, Mobile: %lld]," \
				"ULTRASONIC: [%d, %d, %d, %d]}", \
				rssiData[0], \
				rssiData[1], \
				rssiData[2], \
				rssiData[3], \
				rssiData[4], \
				rssiData[5], \
				rssiData[6], \
				rssiData[7], \
				rssiData[8], \
				rssiData[9], \
				rssiData[10], \
				rssiData[11], \
				baseUptime, mobileTimestamp, \
				ultrasonicData[0], ultrasonicData[1], ultrasonicData[2], ultrasonicData[3]);
			printk("\n");


		}
		k_sleep(K_MSEC(10));
	}

}


/* Thread to write to the connected device over GATT */
void thread_read_data(void)
{

	struct data_item_ble* rx_shell;


	while (1)
	{
		rx_shell = k_fifo_get(&my_ble_fifo, K_FOREVER);
		uint8_t command = rx_shell->command;
		uint16_t user_data = rx_shell->user_data;
		if ((command > 0 && command < 8) || command == 10)
		{
			construct_packet(command, ahu_data, 0);

			gatt_write();
		}
		else if ((command >= 8 && command < 10) || command == 11)
		{
			construct_packet(command, ahu_data, user_data);
			gatt_write();
		}
		k_sleep(K_SECONDS(1));
	}
}

/* Thread to handle continuous receiving of data */
void thread_continuous_read(void)
{
	struct data_item_json* rx_json;
	int sample_time = 5;
	int ret;
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);

	ret = gpio_pin_interrupt_configure_dt(&button,
		GPIO_INT_EDGE_TO_ACTIVE);

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	while (1)
	{
		rx_json = k_fifo_get(&json_fifo, K_MSEC(5));
		if (rx_json != NULL)
		{
			sample_time = rx_json->sample;
		}
		if (rx_json->toggle == 1)
		{
			toggle_all_c = true;
		}
		else if (rx_json->toggle == 0)
		{
			toggle_all_c = false;
		}
		if (toggle_all_c && writeJSON)
		{
			for (int i = 1; i < 8; i++)
			{
				json_data.command = i;
				json_data.user_data = 0;
				k_fifo_put(&my_ble_fifo, &json_data);
				k_sleep(K_SECONDS(1));
			}
		}
		k_sleep(K_SECONDS(sample_time));
	}
}