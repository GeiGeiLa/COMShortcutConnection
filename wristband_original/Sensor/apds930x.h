#ifndef APDS930x_H__
#define APDS930x_H__
/**
 * APDS930x.h
 *
 * nRF52 TWI interface for APDS930x ambient light sensor.
 *
 */
/*----------------------------------------------------------*/
/*  Include                                                  */
/*----------------------------------------------------------*/
#include <math.h>

// Driver
#include "twix.h"

// Library
#include "app_util_platform.h"
#include "app_error.h"

#include "common.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
/*Pins to connect shield. */
#define APDS930x_ADDR           (0x39>>0)

#define CMD                     0x80
#define APDS930x_ID             0x73
#define CTRL_REG                0x0
#define TIMING_REG              0x1
#define THRES_LOW_LOW           0x2    // Interrupt Threshold Register - ADC channel 0 lower byte of the low threshold
#define THRES_LOW_HIGH          0x3    // Interrupt Threshold Register - ADC channel 0 upper byte of the low threshold
#define THRES_HIGH_LOW          0x4    // Interrupt Threshold Register - ADC channel 0 lower byte of the high threshold
#define THRES_HIGH_HIGH         0x5    // Interrupt Threshold Register - ADC channel 0 upper byte of the high threshold
#define INT_CTRL_REG            0x6    // Interrupt Control Register
#define ID_REG                  0xA    // ID Register for identifiation
#define DATA0_LOW               0xC    // ADC Channel Data Register - ADC channel 0 lower byte
#define DATA0_HIGH              0xD    // ADC Channel Data Register - ADC channel 0 upper byte
#define DATA1_LOW               0xE    // ADC Channel Data Register - ADC channel 1 lower byte
#define DATA1_HIGH              0xF    // ADC Channel Data Register - ADC channel 1 upper byte



/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/

/*******************************************************************************
* Register      : CTRL_REG
* Address       : 0h
* Bit Group Name: POWER
*******************************************************************************/
typedef enum {
    APDS930x_PWR_DISABLE            = 0x00,
    APDS930x_PWR_ENABLE             = 0x03,
} APDS930x_PWR_t;

/*******************************************************************************
* Register      : TIMING_REG
* Address       : 1h
* Bit Group Name: GAIN
*******************************************************************************/
typedef enum {
    APDS930x_SET_GAIN_LOW_GAIN      = 0x00,
    APDS930x_SET_GAIN_HIGH_GAIN     = 0x10,
} APDS930x_SET_GAIN_t;

/*******************************************************************************
* Register      : TIMING_REG
* Address       : 1h
* Bit Group Name: MANUAL TIME
*******************************************************************************/
typedef enum {
    APDS930x_MANUAL_TIME_DISABLE    = 0x00,
    APDS930x_MANUAL_TIME_ENABLE     = 0x08,
} APDS930x_MANUAL_TIME_t;

/*******************************************************************************
* Register      : CTRL_REG
* Address       : 1h
* Bit Group Name: INTEGRATION TIME
*******************************************************************************/
typedef enum {
    APDS930x_INTG_TIME_13_7_MS      = 0x00,
    APDS930x_INTG_TIME_101_MS       = 0x01,
    APDS930x_INTG_TIME_402_MS       = 0x02,
    APDS930x_INTG_TIME_MANUAL         = 0x03,
} APDS930x_INTG_TIME_t;

/*******************************************************************************
* Register      : INT_CTRL_REG
* Address       : 6h
* Bit Group Name: INTERRUPT CONTROL SELECT
*******************************************************************************/
typedef enum {
    APDS930x_INTR_CTRL_LEVEL_INTR_DISABLED  = 0x00,
    APDS930x_INTR_CTRL_LEVEL_INTR_ENABLED   = 0x10,
} APDS930x_INTR_CTRL_LEVEL_INTR_t;

/*******************************************************************************
* Register      : INT_CTRL_REG
* Address       : 6h
* Bit Group Name: INTERRUPT PERSISTENCE SELECT
*******************************************************************************/
typedef enum {
    APDS930x_INTR_PRSIST_ADC_VALUE      = 0x00,
    APDS930x_INTR_PRSIST_THRESHOLD      = 0x01,
    APDS930x_INTR_PRSIST_INTG_TIME_2    = 0x02,
    APDS930x_INTR_PRSIST_INTG_TIME_3    = 0x03,
    APDS930x_INTR_PRSIST_INTG_TIME_4    = 0x04,
    APDS930x_INTR_PRSIST_INTG_TIME_5    = 0x05,
    APDS930x_INTR_PRSIST_INTG_TIME_6    = 0x06,
    APDS930x_INTR_PRSIST_INTG_TIME_7    = 0x07,
    APDS930x_INTR_PRSIST_INTG_TIME_8    = 0x08,
    APDS930x_INTR_PRSIST_INTG_TIME_9    = 0x09,
    APDS930x_INTR_PRSIST_INTG_TIME_10    = 0x0A,
    APDS930x_INTR_PRSIST_INTG_TIME_11    = 0x0B,
    APDS930x_INTR_PRSIST_INTG_TIME_12    = 0x0C,
    APDS930x_INTR_PRSIST_INTG_TIME_13    = 0x0D,
    APDS930x_INTR_PRSIST_INTG_TIME_14    = 0x0E,
    APDS930x_INTR_PRSIST_INTG_TIME_15    = 0x0F,
} APDS930x_INTR_PERSIST_INTG_TIME_t;



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/

/**
 * @brief function to test sensor id
 */
uint8_t APDS930x_Read_ID(void);

/**
 * @brief function to turn the device ON
 *
 * Bit[1:0] is Power control bit. Set Bit[1:0] to power up.
 */
void APDS930x_Power_Up(void);

/**
 * @brief function to turn the device OFF
 *
 * Bit[1:0] is Power control bit. Reset Bit[1:0] to power down.
 */
void APDS930x_Power_Down(void);

/**
 * @brief function to initialize and power up the sensor.
 */
void APDS930x_Init(void);

/**
 * @brief function to check if device is ON or OFF.
 *
 * Read Bit[1:0] to get current power status
 */
uint8_t APDS930x_get_power_status(void);

/**
 * @brief function to read ADC value
 *
 * Data is present in two channels, namely, DATA0 and DATA1, where each includes two 8 bit registers.
 * So each channel provides a 16-bit ADC data.
 */
void APDS930x_read_adc_data(uint16_t *adc_ch0, uint16_t *adc_ch1);

float APDS930x_Read_ADC_Data(void);


/**
 * @brief function to compute brightness (lux) bases on ADC values
 *
 * The equations and parameters are from APDS930x datasheet - page 4.
 */
float getlux(uint16_t adc_ch0, uint16_t adc_ch1);


#endif
