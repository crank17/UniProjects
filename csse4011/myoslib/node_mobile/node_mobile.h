/*
 * @file oslib/scu_scan/scu_scan.c
 * @author Thomas Salpietro 45822490
 * @date 31/03/2022
 * @brief ble scu driver which scans for advertsiements from static nodes
 *              Also parses data and updatas variable
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SCU_SCAN_H
#define SCU_SCAN_H


#define STATIC_NODES 16
#define BLE_MAX_NAME_LEN 40

#define SCAN_STACK_SIZE 2048
#define SCAN_PRIORITY 1

#define BLE_CONNECT_STACK_SIZE 2048
#define BLE_CONNECT_PRIORITY 1

#define ULTRA_STACK 1024
#define ULTRA_PRIO 1

#define BT_STACK 1024
#define BT_PRIO 3

#define IMU_STACK_SIZE 2048
#define IMU_PRIORITY 2

#define PEDO_STACK_SIZE 1024
#define PEDO_PRIORITY 3

 //gatt characteristic user values
extern int8_t rssiValues[12];
extern uint16_t ultraSonicValues[4];


struct staticNodeData {
    char name[BLE_MAX_NAME_LEN];
    int8_t rssiData;
    uint16_t ultraSonicData;
};



void mobile_node_ble_send_data_thread_entry(void);
void mobile_node_ble_scan_thread_entry(void);

#endif