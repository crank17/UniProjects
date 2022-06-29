/*
 * @file oslib/scu_scan/scu_scan.c
 * @author Thomas Salpietro 45822490
 * @date 31/03/2022
 * @brief ble scu driver which scans for advertsiements from static nodes
 *              Also parses data and updatas variable
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stddef.h>

#include <stdbool.h>
#include <string.h>

#include <sys/printk.h>
#include <sys/util.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <kernel.h>
#include "node_mobile.h"

#define SCAN_PERIOD 280

const char* tempNodeNames[] = {
    "RyanNode U",
    "Toms Phone",
    "TomNode U",
    "Ryans Phone"
};

const char* nodeNames[] = {
    "4011-A",
    "4011-B",
    "4011-C",
    "4011-D",
    "4011-E",
    "4011-F",
    "4011-G",
    "4011-H",
    "4011-I",
    "4011-J",
    "4011-K",
    "4011-L",
    "S1",
    "S2",
    "S3",
    "S4"
};

//initialise data struct to hold data from the 4 nodes
struct staticNodeData nodeDataArray[STATIC_NODES];

//initialise semaphores? maybe put into a helper function ->
struct k_sem semScanFinish;
struct k_sem semNodes[STATIC_NODES];

void semaphore_init(void) {
    k_sem_init(&semScanFinish, 0, 1);

    for (int i = 0; i < STATIC_NODES; i++) {
        k_sem_init(&semNodes[i], 0, 1);
    }
    return;
}

// advertisement parse data helper function
static bool advertising_parse(struct bt_data* data, void* user_data) {

    struct staticNodeData* ad = user_data;
    if (data->type == BT_DATA_NAME_COMPLETE) {
        memcpy(ad->name, data->data, data->data_len);
        ad->name[data->data_len] = '\0';
        return true;

    }
    else if (data->type == BT_DATA_UUID128_ALL) {
        //check data length for ultrasonic
        //if (data->data_len == 2 * sizeof(uint16_t)) {
        ad->ultraSonicData = ((int16_t)(data->data[0]) << 8) | data->data[1];
        return false;
        //}
    }
    return true;
}


//scan cb -> called when advertisement found
static void scan_callback(const bt_addr_le_t* addr, int8_t rssi,
    uint8_t advType, struct net_buf_simple* ad) {

    struct staticNodeData nodeAd = { 0 };

    //parse data
    bt_data_parse(ad, advertising_parse, &nodeAd);

    //format and insert advertisement data into struct
    for (int i = 0; i < STATIC_NODES; i++) {
        //check if valid node name
        if (strcmp(nodeAd.name, nodeNames[i]) == 0) {
            nodeDataArray[i].rssiData = rssi;
            //printk("%s\n", nodeNames[i]);
            if (nodeAd.name[0] == 'S') {
                //node is ultrasonic node -> set node data
                nodeDataArray[i].ultraSonicData = nodeAd.ultraSonicData;
            }
            else {
                //node is not ultrasonic node
                nodeDataArray[i].ultraSonicData = 0;
            }
            //give semaphore to update Gatt characteristic
            k_sem_give(&semNodes[i]);
        }
    }

}

void mobile_node_ble_scan_thread_entry(void) {
    int error;
    k_msleep(2000);
    //set up scan parameters
    struct bt_le_scan_param scanParameters = {
        .type = BT_HCI_LE_SCAN_ACTIVE,
        .options = BT_LE_SCAN_OPT_NONE,
        .interval = 0x0040,
        .window = 0x0040
    };

    //maybe initialise semaphores?
    semaphore_init();

    //start scan
    int err = bt_le_scan_start(&scanParameters, scan_callback);
    if (err) {
        printk("Scanning failed\n");
        return;
    }
    printk("scan started\n");
    while (1) {

        k_sleep(K_MSEC(SCAN_PERIOD));

        k_sem_give(&semScanFinish);

    }
}

void mobile_node_ble_send_data_thread_entry(void) {
    int err;
    k_msleep(2001);
    while (1) {

        k_sem_take(&semScanFinish, K_FOREVER);

        for (int i = 0; i < STATIC_NODES; i++) {
            err = k_sem_take(&semNodes[i], K_MSEC(10));
            if (err == 0) {
                //update node data
                //printk("NAME: %s, rssi: %d, ultrasonic: %d\n", nodeNames[i], nodeDataArray[i].rssiData, nodeDataArray[0].ultraSonicData);
                if (nodeNames[i][1] == '1') {
                    ultraSonicValues[0] = nodeDataArray[i].ultraSonicData;
                }
                else if (nodeNames[i][1] == '2') {
                    ultraSonicValues[1] = nodeDataArray[i].ultraSonicData;
                }
                else if (nodeNames[i][1] == '3') {
                    ultraSonicValues[2] = nodeDataArray[i].ultraSonicData;
                }
                else if (nodeNames[i][1] == '4') {
                    ultraSonicValues[3] = nodeDataArray[i].ultraSonicData;
                }
                else {
                    rssiValues[i] = nodeDataArray[i].rssiData;
                }

            }
        }
    }

}




