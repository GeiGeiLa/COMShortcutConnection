#ifndef TIMER_H_
#define TIMER_H_

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "nrf.h"

#include "common.h"
#include "raytac52.h"

// module
#include "data_collect.h"

// Sensor
#include "w25qxx.h"


// driver
#include "uart.h"
#include "app_timer.h"
#include "nrf_drv_timer.h"

#if (BLE_UART_SERVICE_ENABLE==1)
#include "ble_uarts.h"
#endif

/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
typedef struct
{
    uint8_t     year;
    uint8_t     month;    
    uint8_t     day;
    uint8_t     hour;
    uint8_t     minute;    
    uint8_t     second;    
}DATE_TIME_T;


static DATE_TIME_T          cdt;    // current date time;


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
void Start_SAADC_Ctrl_Timer(void);
void Stop_SAADC_Ctrl_Timer(void);
void Clear_Change_File_Flag(void);
DATE_TIME_T Get_DateTime(void);
void Reset_Sensor_Time_Stamp(void);
bool Update_DateTime(DATE_TIME_T *ptr);
void Tic(void);
uint32_t Toc(void);

void Init_Timer(void);
void Init_Timer_4RTC_Only(void);

#endif

