#include "node_static.h"

#ifdef CHAOS_NODE
#define NODE_NAME BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, 0x53, 0x31)
#endif

#ifdef ARTEMIS_NODE
#define NODE_NAME BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, 0x53, 0x32)
#endif

#ifdef LEVIATHAN_NODE
#define NODE_NAME BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, 0x53, 0x33)
#endif

#ifdef OLYMPUS_NODE
#define NODE_NAME BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, 0x53, 0x34)
#endif

K_FIFO_DEFINE(ultra_fifo);

void thread_bt_ad(void) {


    struct data_ultra_t* recv_distance;
    uint64_t dis;

    int err;

    err = bt_enable(NULL);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");
    const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_UUID128_ALL,
            0x78, 0x16, 0x21, 0x91, 0x78, 0x16, 0x21, 0x91,
            0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb),

    };
    err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);
    while (1) {
        recv_distance = k_fifo_get(&ultra_fifo, K_FOREVER);
        printk("%d\n", recv_distance->dis);
        dis = recv_distance->dis;
        /*const struct bt_data updated[] = {
            BT_DATA_BYTES(BT_DATA_NAME_COMPLETE,
                0x52, 0x79, 0x61, 0x6E, 0x4E, 0x6F, 0x64, 0x65, 0x20, 0x55),
            BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                dis >> 8, dis),
        };*/
        // const struct bt_data updated[] = {
        // 	BT_DATA_BYTES(BT_DATA_UUID128_ALL,
        // 		0xd8, 0x92, 0x67, 0x35, 0x78, 0x16, 0x21, 0x91,
        //                   0x26, 0x49, 0x60, 0xeb, 0x06, 0xa7, 0xca, 0xcb),
        // 	BT_DATA_BYTES(BT_DATA_UUID128_ALL,
        // 		dis >> 8, dis),
        // };


        // err = bt_le_adv_update_data(updated, ARRAY_SIZE(updated), NULL, 0);

        //err = bt_le_adv_start(BT_LE_ADV_NCONN, updated, ARRAY_SIZE(updated), NULL, 0);
        //0x52, 0x79, 0x61, 0x6E, 0x4E, 0x6F, 0x64, 0x65, 0x20, 0x55
        k_sleep(K_MSEC(50));
        const struct bt_data updated1[] = {
            BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, 0x53, 0x31),
            BT_DATA_BYTES(BT_DATA_UUID128_ALL,
                dis >> 8, dis),
        };


        err = bt_le_adv_update_data(updated1, ARRAY_SIZE(updated1), NULL, 0);

        //err = bt_le_adv_stop();

        k_sleep(K_MSEC(50));
    }





}


//Add the following to main()
void thread_ultra_entry(void)
{

    uint32_t cm;
    uint64_t total_cycles;
    uint64_t total_ns;
    timing_t start_time, end_time;
    uint8_t val;
    struct data_ultra_t send_distance;
    /* Setup DTR */
    const struct device* console_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    uint32_t dtr = 0;
    const struct device* dev = dev = device_get_binding("GPIO_1");
    k_fifo_init(&ultra_fifo);



    gpio_pin_configure(dev, TRIGPIN, GPIO_OUTPUT);
    gpio_pin_configure(dev, ECHOPIN, GPIO_INPUT | GPIO_ACTIVE_HIGH | GPIO_INT_EDGE | GPIO_INT_DEBOUNCE);


    /* Enable the USB Driver */
    if (usb_enable(NULL)) {
        return;
    }

    timing_init();

    uint64_t lock;

    while (1)
    {
        timing_start();
        lock = irq_lock();
        gpio_pin_set(dev, TRIGPIN, 1);
        k_sleep(K_USEC(5));
        gpio_pin_set(dev, TRIGPIN, 0);


        do {
            val = gpio_pin_get(dev, ECHOPIN);
        } while (val == 0);
        start_time = timing_counter_get();

        do {
            val = gpio_pin_get(dev, ECHOPIN);
        } while (val == 1);
        end_time = timing_counter_get();
        irq_unlock(lock);


        total_cycles = timing_cycles_get(&start_time, &end_time);
        total_ns = timing_cycles_to_ns(total_cycles);
        timing_stop();
        cm = total_ns / (uint64_t)58000;
        send_distance.dis = cm;
        k_fifo_put(&ultra_fifo, &send_distance);

        k_sleep(K_MSEC(50));

    }
}
