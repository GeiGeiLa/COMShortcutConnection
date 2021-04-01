/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @defgroup nrf_adc_example main.c
 * @{
 * @ingroup nrf_adc_example
 * @brief ADC Example Application main file.
 *
 * This file contains the source code for a sample application using ADC.
 *
 * @image html example_board_setup_a.jpg "Use board setup A for this example."
 */


/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "main.h"

#if (BLE_UART_SERVICE_ENABLE==1)
#include "ble_uarts.h"
#endif

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
static POWER_UP_MODE_T power_up_mode; 


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void low_battery_process(void);
static void clock_initialization(void);



/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
/**@brief Function for placing the application in low power state while waiting for events.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}





/**
 * @brief Function for main application entry.
 */
int main(void)
{
    //Enable DCDC 
    NRF_POWER->DCDCEN = 1;  // maybe use sd_power_dcdc_mode_set();
          
    Load_Module_Info();
    power_up_mode = Get_Power_Up_Mode();  
    
    //if (power_up_mode != POWER_UP_INTO_RECOED_WITH_BLE_MODE)    
    {
        clock_initialization();         
        //LFCLK_Config();                 //Configure low frequency 32kHz clock        
    }

    Init_GPIO();    
    
    if (power_up_mode == POWER_UP_INTO_LOW_BATTERY_MODE)
    {
        low_battery_process();
    }
    
    Init_Timer();
    if (power_up_mode == POWER_UP_INTO_UART_TRANS_MODE)  
    {
        Init_UART(TX_PIN_NUMBER, RX_PIN_NUMBER);
        printf("\n%d:Hello!! UART trans mode.\n\r",SYSTEM_ON);    
        Ack_Into_UART_mode();
        W25Qxx_Init_Proc();
        LED_G_ON();
    }

    // debug for POWER_UP_INTO_RECORD_MODE
#if PRINT_MSG_VIA_UART    
    if (power_up_mode != POWER_UP_INTO_UART_TRANS_MODE)  
    {
        Init_UART(TX_PIN_NUMBER, 0xFFFFFFFF); //NRF_UART_PSEL_DISCONNECTED = 0xFFFFFFFF
        PRINT("\n%d:Hello!! Power Up mode is %d.\n\r",SYSTEM_ON, power_up_mode);    
    }
#endif 
    
    

	
#if (BLE_UART_SERVICE_ENABLE==1)
	if (power_up_mode == POWER_UP_INTO_RECOED_WITH_BLE_MODE)
        Init_BLE_UART_Service();	
#endif			
	
    while (1)
    {        
        switch(power_up_mode)
        {
            case POWER_UP_INTO_RECORD_MODE:                
                Adap_Proc();                
                break;
            case POWER_UP_INTO_UART_TRANS_MODE:
                Adap_UARTs_Proc();
                break;
            case POWER_UP_INTO_RECOED_WITH_BLE_MODE:
                Adap_Proc();
                break;
            default:break;
        }
        nrf_pwr_mgmt_run();
        //__WFE();


    }
}



static void low_battery_process(void)
{
    Init_Timer_4RTC_Only();
    TWIx_Init();
    Init_LIS2DH12();       
    while(1)
    {            
        if (IS_NOTIFY_SYSTEM_RESET())
           sd_nvic_SystemReset(); 
        nrf_pwr_mgmt_run();
    }      
}

//#if (BLE_UART_SERVICE_ENABLE==0)

/**@brief Function for initializing low frequency clock.
 */
static void clock_initialization(void)
{
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }
}

/** @} */
//#endif
