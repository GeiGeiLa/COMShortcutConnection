#ifndef TWIx_H__
#define TWIx_H__

/*----------------------------------------------------------*/
/*  Include                                                  */
/*----------------------------------------------------------*/
// Driver
#include "nrf_drv_twi.h"
#include "nrf_delay.h"

// Library
#include "app_util_platform.h"
#include "app_error.h"

/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
/*Pins to connect shield. */



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
void TWIx_Write(uint8_t addr, uint8_t* tx_data, uint8_t tx_len);
void TWIx_Read(uint8_t dev_addr, uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len);
void TWIx_Uninit(void);
void TWIx_Init(void);
void TWIx_Enable(void);
void TWIx_Disable(void);
#endif
