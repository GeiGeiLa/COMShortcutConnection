#ifndef BLE_UARTS_H__
#define BLE_UARTS_H__

#include <stdbool.h>
#include <stdint.h>

#include "uart.h"


#if (BLE_IOS)
    #define MIN_CONN                                        20
    #define MAX_CONN                                        75
    #define LATENCY                                         0
    #define TIMEOUT                                         4000
    #define BLE_PACKET_SIZE									10
    #define BLE_PACKET_LENGTH                               2+BLE_PACKET_SIZE*4*2+2
#else
    #define MIN_CONN                                        20
    #define MAX_CONN                                        75
    #define LATENCY                                         0
    #define TIMEOUT                                         4000
    #define BLE_PACKET_SIZE									18
    #define BLE_PACKET_LENGTH                               2+BLE_PACKET_SIZE*4*2+2
#endif



// BLE_PACKET size = 1 + BLE_PACKET_SIZE*4*2 + 1 

//#pragma pack(push,1)
typedef struct{
    uint16_t    HEAD;	
    int32_t     EXG_CH1[BLE_PACKET_SIZE];
    int32_t     EXG_CH2[BLE_PACKET_SIZE];
    uint16_t    TAIL;
}BLE_PACKET;
//#pragma pack(pop)

void Init_BLE_UART_Service(void);
void BLE_UARTS_Tx_DateTime(void);
void BLE_UARTS_Tx_EXG(void);


#endif
