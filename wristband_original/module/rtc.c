
/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "rtc.h"



/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define RTC_FREQUENCY 256            //Determines the RTC frequency and prescaler
#define RTC_CC_VALUE 2              //Determines the RTC interrupt frequency and thereby the SAADC sampling frequency 

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/

const  nrf_drv_rtc_t           rtc = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC2. */

/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/





/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
/**@brief Function for placing the application in low power state while waiting for events.
 */
 
 static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    uint32_t err_code;
	
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {                                     
		Start_ADC_Sampling();                                           //Trigger the SAADC SAMPLE task      			
        err_code = nrf_drv_rtc_cc_set(&rtc,0,RTC_CC_VALUE,true);        //Set RTC compare value. This needs to be done every time as the nrf_drv_rtc clears the compare register on every compare match
        APP_ERROR_CHECK(err_code);
        nrf_drv_rtc_counter_clear(&rtc);                                //Clear the RTC counter to start count from zero
    }
}

void LFCLK_Config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();                        //Initialize the clock source specified in the nrf_drv_config.h file, i.e. the CLOCK_CONFIG_LF_SRC constant
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

void RTC_Config(void)
{

    uint32_t err_code;

    //Initialize RTC instance
    nrf_drv_rtc_config_t rtc_config;
    rtc_config.prescaler = RTC_FREQ_TO_PRESCALER(RTC_FREQUENCY);
    err_code = nrf_drv_rtc_init(&rtc, &rtc_config, rtc_handler);        //Initialize the RTC with callback function rtc_handler. The rtc_handler must be implemented in this applicaiton. Passing NULL here for RTC configuration means that configuration will be taken from the sdk_config.h file.
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_rtc_cc_set(&rtc,0,RTC_CC_VALUE,true);            //Set RTC compare value to trigger interrupt. Configure the interrupt frequency by adjust RTC_CC_VALUE and RTC_FREQUENCY constant in top of main.c
    APP_ERROR_CHECK(err_code);

    //Power on RTC instance
    //nrf_drv_rtc_enable(&rtc);                                           //Enable RTC
}


void ADC_Sampling_Triggle_Disable(void)
{
    nrf_drv_rtc_disable(&rtc); 
}


void ADC_Sampling_Triggle_Enable(void)
{
    nrf_drv_rtc_enable(&rtc); 
}
