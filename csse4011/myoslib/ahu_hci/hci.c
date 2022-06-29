/**
 ************************************************************************
 * @file hci.c
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Source code for the HCI.
 **********************************************************************
 * */
#include "hci.h"

char json_output[100];

/* Constructs packet based on DID and user data */
void construct_packet(uint16_t DID, int8_t* ahu_data, uint16_t user_data) {
    if (DID < 8 || DID == 10) {
        ahu_data[3] = DID;
        ahu_data[2] = 0x01;
    }
    else {
        ahu_data[3] = DID;

        if (DID == 8) {
            ahu_data[2] = 0x03;
            switch (user_data) {
            case 1:
                ahu_data[4] = 1;
                ahu_data[5] = 0;
                ahu_data[6] = 0;
                break;
            case 2:
                ahu_data[4] = 0;
                ahu_data[5] = 0;
                ahu_data[6] = 1;
                break;
            case 3:
                ahu_data[4] = 0;
                ahu_data[5] = 1;
                ahu_data[6] = 0;
                break;
            }

        }
        if (DID == 9) {
            ahu_data[2] = 0x01;
            ahu_data[4] = user_data;
        }
        if (DID == 11) {
            ahu_data[2] = 0x01;
            ahu_data[4] = user_data;
        }

    }

}
int dataPointer = 0;
/* Function to process and print incoming data to terminal */
void process_packet(uint8_t DID, uint8_t* data, uint8_t len, bool isJSON) {
    switch (DID) {
    case TEMPERATURE:
        if (isJSON) {
            sprintf(json_output, "\n{1: ");
            dataPointer = 5;

            for (int i = 0; i < len; i++) {
                //printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];
            }

            strcat(json_output, ",\n");
            dataPointer += len + 2;
        }
        else {

            printk("Temperature: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);
            }
            printk("°C\n");
        }
        break;
    case HUMIDITY:
        if (isJSON) {
            strcat(json_output, "2: ");
            dataPointer += 3;
            for (int i = 0; i < len; i++) {
                // printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];
            }
            strcat(json_output, ",\n");
            dataPointer += len + 2;

        }
        else {
            printk("Humidity: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);
            }
            printk("%%\n");
        }
        break;
    case PRESSURE:
        if (isJSON) {
            strcat(json_output, "3: ");
            dataPointer += 3;
            for (int i = 0; i < len; i++) {
                //printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];
            }
            strcat(json_output, ",\n");
            dataPointer += len + 2;
        }
        else {
            printk("Air Pressure: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);
            }
            printk("kPa\n");
        }
        break;
    case VOC:
        if (isJSON) {
            strcat(json_output, "4: ");
            dataPointer += 3;
            for (int i = 0; i < len; i++) {
                //printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];

            }
            strcat(json_output, ",\n");
            dataPointer += len + 2;
        }
        else {
            printk("VOC: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);
            }
            printk("ppb\n");
        }
        break;
    case XACC:
        if (isJSON) {
            strcat(json_output, "5: ");
            dataPointer += 3;
            for (int i = 0; i < len; i++) {
                //printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];
            }
            strcat(json_output, ",\n");
            dataPointer += len + 2;
        }
        else {
            printk("X Acceleration: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);
            }
            printk("m/s^2\n");
        }
        break;
    case YACC:
        if (isJSON) {
            strcat(json_output, "6: ");
            dataPointer += 3;
            for (int i = 0; i < len; i++) {
                //printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];
            }
            strcat(json_output, ",\n");
            dataPointer += len + 2;
        }
        else {
            printk("Y Acceleration: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);

            }
            printk("m/s^2\n");
        }
        break;
    case ZACC:
        if (isJSON) {
            strcat(json_output, "7: ");
            dataPointer += 3;
            for (int i = 0; i < len; i++) {
                //printk("%c", data[i]);
                json_output[i + dataPointer] = (char)data[i];
            }
            strcat(json_output, "}\n");
            printk("%s", json_output);
            memset(json_output, 0, 100);
            dataPointer = 0;
        }
        else {
            printk("Z Acceleration: ");
            for (int i = 0; i < len; i++) {
                printk("%c", data[i]);
            }
            printk("m/s^2\n");
        }
        break;
    case BUTTON:
        printk("Pushbutton: ");
        if (data[0] == '1') {
            printk("on\n");
        }
        else if (data[0] == '0') {
            printk("off\n");
        }
        break;
    case DEVICE_OFF:
        printk("Device is off\n");
        break;


    }


}


/* Process receiving data packets */
void thread_hci_entry(void) {
    struct data_packet_t* recv_packet;
    uint8_t length;
    uint8_t DID;
    uint8_t dataPointer = 4;
    while (1) {

        recv_packet = k_fifo_get(&my_packet_fifo, K_FOREVER);

        if (recv_packet != NULL) {

            length = recv_packet->packet_data[2];
            DID = recv_packet->packet_data[3];
            process_packet(DID, recv_packet->packet_data + dataPointer, length, recv_packet->jsonOutput);

        }

    }
}