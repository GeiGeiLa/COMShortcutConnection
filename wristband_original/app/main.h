#ifndef MAIN_H
#define MAIN_H

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nordic_common.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "flash_access.h"

#include "adap.h"
#include "w25qxx.h"

#include "timer.h"
#include "uart.h"

#include "ssd1306.h"



#include "nrf_pwr_mgmt.h"
#include "nrf_drv_power.h"







/* Sensor */





/*
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
*/

/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/

#define APP_TIMER_PRESCALER      0                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE  2                           /**< Size of timer operation queues. */


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

#endif
