/*  
 * For ACC Module nRF52-ver1 Board, 2019-05-23
 */
#ifndef RAYTAC52_H__
#define RAYTAC52_H__

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "softdevice_handler.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_config.h"
#include "common.h"
#include "flash_access.h"

/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/




#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

    
    // LEDs definitions for EEG


#define LED_START                   28
#define LED_G_PIN                   28
#define LED_O_PIN                   29
#define LED_STOP                    29

#define LED_G_ON()                  nrf_gpio_pin_clear(LED_G_PIN)  
#define LED_G_OFF()                 nrf_gpio_pin_set(LED_G_PIN)      
#define LED_G_BLINK()               nrf_gpio_pin_toggle(LED_G_PIN)  
#define LED_O_ON()                  nrf_gpio_pin_clear(LED_O_PIN)  
#define LED_O_OFF()                 nrf_gpio_pin_set(LED_O_PIN)  
#define LED_O_BLINK()               nrf_gpio_pin_toggle(LED_O_PIN)   
    


/* UART */
#define RX_PIN_NUMBER               21
#define TX_PIN_NUMBER               20 
#define CTS_PIN_NUMBER              NULL
#define RTS_PIN_NUMBER              NULL
#define HWFC                        false

/* Button */
#define BUTTON_SW_PIN               25

/* Power Control */
#define FLASH_PW_ENABLE_PIN         9
#define LCD_PW_ENABLE_PIN           8
#define LCD_nRES_PIN                4
#define FLASH_PW_ON()               nrf_gpio_pin_set(FLASH_PW_ENABLE_PIN)  
#define FLASH_PW_OFF()              nrf_gpio_pin_clear(FLASH_PW_ENABLE_PIN)         
#define LCD_PW_ON()                 nrf_gpio_pin_set(LCD_PW_ENABLE_PIN)  
#define LCD_PW_OFF()                nrf_gpio_pin_clear(LCD_PW_ENABLE_PIN)    
#define LCD_nRES_HIGH()             nrf_gpio_pin_set(LCD_nRES_PIN)  
#define LCD_nRES_LOW()              nrf_gpio_pin_clear(LCD_nRES_PIN)    

  

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

#ifdef __cplusplus
}
#endif



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

void Button_Click_Detect(void);
void Power_Switch_Test(void);
void Init_GPIO(void);


#endif // RAYTAC52.H
