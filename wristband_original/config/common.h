#ifndef COMMON_H__
#define COMMON_H__

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "nrf.h"
#include "app_config.h"



/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/


#define SI705x_INIT_SUCCESS                 (0x01u)    
#define ACC_INIT_SUCCESS                    (0x02u)    
#define W25Qxx_INIT_SUCCESS                 (0x04u) 
#define VDD_CHECK_SUCCESS                   (0x08u)    
#define BAT_CHECK_SUCCESS                   (0x10u)    
#define ALLOW_EXEC                          (0x40u)
#define SENSOR_CHKECKED                     (0x80u)

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
typedef enum
{
    POWER_UP_MODE_NOT_DEFINE = 0,
    POWER_UP_INTO_RECORD_MODE  = 1,
    POWER_UP_INTO_UART_TRANS_MODE = 2,
    POWER_UP_INTO_RECOED_WITH_BLE_MODE = 3,
    POWER_UP_INTO_LOW_BATTERY_MODE = 4,
    FFh                          = 0xff,   
 
} POWER_UP_MODE_T;


typedef enum{
    FLAG_RESET                              = 0,
    NOTIFY_SYSTEM_RESET_FLAG                    = (1<<0), 
    // Initialized
    W25Qxx_INITIALIZED_FLAG                     = (1<<1),
    SI705x_INITIALIZED_FLAG                     = (1<<2),
    APDS930x_INITIALIZED_FLAG                   = (1<<3),
    ACC_INITIALIZED_FLAG                        = (1<<4),
    VDD_VOLT_CORRECT_FLAG                       = (1<<5),
    BAT_VOLT_HIGH_FLAG                          = (1<<6),
    
    // Printf message.

    PRINT_ALARM_MSG_FLAG                        = (1<<7),
    TEMP_SAMPLING_FLAG                          = (1<<8),
    AMBIENT_LIGHT_SENSOR_POWER_UP_FLAG          = (1<<9),
    AMBIENT_LIGHT_SAMPLING_FLAG                 = (1<<10),
    
    // command timeout
    TWI0_CMD_TUMEOUT_FLAG                       = (1<<11),
    W25QXX_CMD_TIMEOUT_FLAG                     = (1<<12),
  
    
    BAT_VDD_SAMPLING_DONE_FLAG                  = (1<<13),
    MODULE_SELF_TEST_DONE_FLAG                  = (1<<14),
    SENSOR_START_SAMPLING_FLAG                  = (1<<15),
    SENSOR_DATA_READY_2WRITE_FLAG               = (1<<16),
    FLASH_RAM_FULL_FLAG                         = (1<<17),
    FLASH_RAM_WRITE_ERROR_FLAG                  = (1<<18),
   
    // Bluetooth BLE Flag
    RCV_START_RECORD_FLAG                       = (1<<19),
    NOTIFY_W25QXX_FLASH_ERASR_ALL_FLAG          = (1<<20),
    UPDATE_DATETIME_FLAG                        = (1<<21),   

    
    
    
    // Button Flag
    BTN_LONG_PUSH_FLAG                          = (1<<23),
    BTN_DOBULE_CLICK_FLAG                       = (1<<24),
    
    // BLE Flag
    BLE_CONNECT_FLAG                            = (1<<25),
    BLE_TX_DONE_FLAG                            = (1<<26),

}MODULE_STAT_FLAG_T;




/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/
#define IS_NOTIFY_SYSTEM_RESET()                Chk_Module_Operate_Flag(NOTIFY_SYSTEM_RESET_FLAG)
#define IS_W25Qxx_INITIALIZED()                 Chk_Module_Operate_Flag(W25Qxx_INITIALIZED_FLAG)
#define IS_SI705x_INITIALIZED()                 Chk_Module_Operate_Flag(SI705x_INITIALIZED_FLAG)
#define IS_APDS930x_INITIALIZED()               Chk_Module_Operate_Flag(APDS930x_INITIALIZED_FLAG)
#define IS_ACC_INITIALIZED()                    Chk_Module_Operate_Flag(ACC_INITIALIZED_FLAG)
#define IS_VDD_VOLT_CORRECT()                   Chk_Module_Operate_Flag(VDD_VOLT_CORRECT_FLAG)
#define IS_BAT_VOLT_HIGH()                      Chk_Module_Operate_Flag(BAT_VOLT_HIGH_FLAG)
#define IS_BAT_VOLT_LOW()                       (!IS_BAT_VOLT_HIGH())



#define IS_SI705x_CMD_TIMEOUT()                 Chk_Module_Operate_Flag(SI705x_CMD_TIMEOUT_FLAG)
#define IS_W25QXX_CMD_TIMEOUT()                 Chk_Module_Operate_Flag(W25QXX_CMD_TIMEOUT_FLAG)
#define IS_ACC_CMD_TIMEOUT()                    Chk_Module_Operate_Flag(ACC_CMD_TIMEOUT_FLAG)


#define IS_PRINT_ALARM_MSG()                    Get_Module_Operate_Flag(PRINT_ALARM_MSG_FLAG)
#define IS_TEMP_SAMPLING()                      Chk_Module_Operate_Flag(TEMP_SAMPLING_FLAG)
#define IS_AMBIENT_LIGHT_SAMPLING()             Chk_Module_Operate_Flag(AMBIENT_LIGHT_SAMPLING_FLAG)
#define IS_AMBIENT_LIGHT_SENSOR_POWER_UP()      Chk_Module_Operate_Flag(AMBIENT_LIGHT_SENSOR_POWER_UP_FLAG)


#define IS_BAT_VDD_SAMPLING_DONE()              Get_Module_Operate_Flag(BAT_VDD_SAMPLING_DONE_FLAG)
#define IS_MODULE_SELF_TEST_DONE()              Chk_Module_Operate_Flag(MODULE_SELF_TEST_DONE_FLAG)
#define IS_SENSOR_START_SAMPLING()              Chk_Module_Operate_Flag(SENSOR_START_SAMPLING_FLAG)
#define IS_SENSOR_DATA_READY_2WRITE()           Chk_Module_Operate_Flag(SENSOR_DATA_READY_2WRITE_FLAG)
#define IS_FLASH_RAM_FULL()                     Get_Module_Operate_Flag(FLASH_RAM_FULL_FLAG)
#define IS_FLASH_RAM_WRITE_ERROR()              Get_Module_Operate_Flag(FLASH_RAM_WRITE_ERROR_FLAG)



#define IS_ADC_BUFF_OVERFLOW()                  Get_Module_Operate_Flag(ADC_BUFF_OVERFLOW_FLAG)
#define IS_ExG_BUFF_OVERFLOW()                  Get_Module_Operate_Flag(ExG_BUFF_OVERFLOW_FLAG)

#define IS_RCV_START_RECORD_CMD()               Chk_Module_Operate_Flag(RCV_START_RECORD_FLAG)
#define IS_NOTIFY_W25QXX_FLASH_ERASR_ALL_CMD()  Chk_Module_Operate_Flag(NOTIFY_W25QXX_FLASH_ERASR_ALL_FLAG)
#define IS_PHONE_UPDATE_DATETIME()              Chk_Module_Operate_Flag(UPDATE_DATETIME_FLAG)


#define IS_DISK_INIT_ERROR()                    Chk_Module_Operate_Flag(DISK_INIT_ERROR_FLAG)
#define IS_DISK_ACCESS_ERROR()                  Chk_Module_Operate_Flag(DISK_ACCESS_ERROR_FLAG)
#define IS_NOT_ENOUGHT_SPACE()                  Chk_Module_Operate_Flag(NOT_ENOUGHT_SPACE_FLAG)

#define IS_BTN_LONG_PUSH()                      Chk_Module_Operate_Flag(BTN_LONG_PUSH_FLAG)
#define IS_BTN_DOBULE_CLICK()                   Chk_Module_Operate_Flag(BTN_DOBULE_CLICK_FLAG)

#define IS_BLE_CONNECT()                        Chk_Module_Operate_Flag(BLE_CONNECT_FLAG)
#define IS_BLE_TX_DONE()                        Chk_Module_Operate_Flag(BLE_TX_DONE_FLAG)

/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
/* System event queue. */

static uint8_t                  sensor_init_stat = 0;


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/

uint8_t Chk_All_Sensor_Stat(void);

void Set_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd);
void Clear_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd);
uint8_t Get_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd);
uint8_t Chk_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd);
void Reset_All_Operate_Flag(void);

#endif
