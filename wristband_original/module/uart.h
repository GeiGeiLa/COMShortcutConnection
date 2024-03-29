#ifndef UART_H_
#define UART_H_

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "app_uart.h"
#include "adc.h"
#include "raytac52.h"
#include "common.h"

#include "timer.h"
/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
void BLE_Data_Reciver(uint8_t data);
void UART_Rcv_Byte_Timer(void);
void Ack_Into_UART_mode(void);
bool Is_Rcv_CMD(void);
void CMD_Decoder(void);
void Init_UART(uint32_t tx_pin, uint32_t rx_pin);

#endif

