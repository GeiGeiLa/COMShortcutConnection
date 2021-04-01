#ifndef APP_CONFIG_H__
#define APP_CONFIG_H__

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "nrf.h"



/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define nRF52_BOARD_VER                     1       // Select target board. 1:Ver1. 2:Ver2

#if (nRF52_BOARD_VER == 1)
    #define ADAP_FW_VERSION                 "0.7.3B"
    #define ADAP_HW_VERSION                 "1.0.0"
#else
    #define ADAP_FW_VERSION                 "UnDef"
    #define ADAP_HW_VERSION                 "UnDef"
#endif



// Debug printf.
#define PRINT_MSG_VIA_UART	                0
#define PRINT_RAW_DATA_VIA_BLE_UART         0

#define ENABLE_TICK_TIMER                   false

#define BLE_IOS                             0



#if (PRINT_MSG_VIA_UART)
    #define PRINT_LIGHT_RAWDATA             true
    #define PRINT_ACC_RAWDATA               true
    #define PRINT_TEMP_RAWDATA              true
#else
    #define PRINT_LIGHT_RAWDATA             false
    #define PRINT_ACC_RAWDATA               false
    #define PRINT_TEMP_RAWDATA              false
#endif


// Hardware Check.
#define IGNORE_VDD_CHECK                    false



// Message defineition.
#define SYSTEM_ON                           255

#define ACC_RAWDATA                         2
#define TEMP_SENSOR_RAWDATA                 3   
#define AMBIENT_LIGHT_SENSOR_RAWDATA        4
#define MESSAGE                             5
#define PM25_ALARM_EVENT                    15
#define ERROR                               254

// Time unit
#define SECOND                              100 //unit: 10ms, Base on Timer

#define TEMP_SAMPLING_INTERVAL              20  // must be bit than 2. 

//Analog signal definition
#define AVCC                                (double)3.0     // unit:Voltage
#define ADC_RESULATION                      12      // unit: bit
#define BATTERY_LOW_VOLT_THRESHOLD          (double)3.55 // unit: Voltage
#define BATTERY_GAIN                        ((double)1000/(560+1000))
#define BAT_MIN                             (uint16_t)(BATTERY_LOW_VOLT_THRESHOLD/3.6 * 4096 * BATTERY_GAIN)





// SAADC sampling rate
#define ULTRA_LOW_ADC_SAMPLING_RATE         1    // unit:Hz, for ADC raw data
#define LOW_ADC_SAMPLING_RATE               2    // unit:Hz, for ADC raw data, 25000Hz for UART is 1MHz, 10000Hz for UART is 460800bps
#define HIGH_ADC_SAMPLING_RATE              32   // unit:Hz, for normal mode


#if PRINT_MSG_VIA_UART
    #define PRINT(...) printf(__VA_ARGS__)
#else
    #define PRINT(...) __NOP()
#endif

#endif
