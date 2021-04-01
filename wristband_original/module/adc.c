/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "adc.h"

/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/

#define DEFAULT_VDD                     AVCC // unit: Voltage
#define VDD_MAX                         AVCC / 3.6 * 1.05 *4096
#define VDD_MIN                         AVCC / 3.6 * 0.95 *4096

#define NRF_SAADC_ACQTIME               NRF_SAADC_ACQTIME_40US

#define BAT_VOUT                        NRF_SAADC_INPUT_AIN6
#define VDD_VOLT                        NRF_SAADC_INPUT_VDD



/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/

static nrf_saadc_value_t        m_buffer_pool[2][SAMPLES_IN_BUFFER];


static int32_t                  battery_volt = 0;
static int32_t                  vdd_volt = 0;




/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void saadc_channel_init(uint8_t chx, nrf_saadc_input_t analog_input);
static void saadc_volt_channel_init(uint8_t chx, nrf_saadc_input_t analog_input);
static void cal_battery_vdd_volt(int16_t *ptr, uint16_t size);


/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/

void saadc_volt_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {        
        ret_code_t err_code;
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);        
        
        cal_battery_vdd_volt(p_event->data.done.p_buffer, p_event->data.done.size);        
        Set_Module_Operate_Flag(BAT_VDD_SAMPLING_DONE_FLAG);  
    }
}



static void cal_battery_vdd_volt(int16_t *ptr, uint16_t size)
{
    uint16_t idx; 

    battery_volt = 0;
    vdd_volt = 0;
    for(idx=0; idx<(size); idx+=2)
    {
        battery_volt += ptr[idx];
        vdd_volt += ptr[idx+1];
     
    }
    battery_volt = battery_volt >> 0;  // vdd / 16, battery / 16;
    vdd_volt = vdd_volt >> 0;  //       
}

int16_t Get_BAT(void)
{
    return (int16_t)(battery_volt);    
}



int16_t Get_VDD(void)
{
    return (int16_t)(vdd_volt);    
}


bool Is_BAT_Volt_High(void)
{
    if (battery_volt>BAT_MIN)
        return   true;
    else
        return false;    
}


bool Is_VDD_Volt_Correct(void)
{
    if ((VDD_MIN <=vdd_volt ) && (vdd_volt<=VDD_MAX)) 
        return   true;
    else
        return false;    
}







void SAADC_Calibration(void)
{

    PRINT("SAADC calibration starting...  \r\n");                                              //Print on UART
            
    NRF_SAADC->EVENTS_CALIBRATEDONE = 0;                                                        //Clear the calibration event flag
    nrf_saadc_task_trigger(NRF_SAADC_TASK_CALIBRATEOFFSET);                                     //Trigger calibration task
    while(!NRF_SAADC->EVENTS_CALIBRATEDONE);                                                    //Wait until calibration task is completed. The calibration tasks takes about 1000us with 10us acquisition time. Configuring shorter or longer acquisition time will make the calibration take shorter or longer respectively.
    while(NRF_SAADC->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos));          //Additional wait for busy flag to clear. Without this wait, calibration is actually not completed. This may take additional 100us - 300us

    nrf_drv_saadc_calibrate_offset();
    while(!NRF_SAADC->EVENTS_CALIBRATEDONE);
    while(NRF_SAADC->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos));
    PRINT("SAADC calibration complete ! \r\n");                                                //Print on UART
    
}


     
void Start_ADC_Sampling(void)
{
    nrf_drv_saadc_sample();             //Trigger the SAADC SAMPLE task
}

void Uninit_BAT_VDD_Channel(void)
{
    nrf_drv_saadc_uninit();
    nrf_drv_saadc_channel_uninit(0);          
    nrf_drv_saadc_channel_uninit(1);       
}


void Init_SAADC_Chx_4BAT_and_VDD(void)
{
    ret_code_t err_code;
    nrf_drv_saadc_config_t saadc_config;
      
    
    //Configure SAADC
    saadc_config.low_power_mode = false;                                 //Enable low power mode.
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;               //Set SAADC resolution to 12-bit. This will make the SAADC output values from 0 (when input voltage is 0V) to 2^12=2048 (when input voltage is 3.6V for channel gain setting of 1/6).
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED;            //Set oversample to 4x. This will make the SAADC output a single averaged value when the SAMPLE task is triggered 4 times.
    saadc_config.interrupt_priority = APP_IRQ_PRIORITY_LOW;      
    
   
    //Initialize SAADC
    err_code = nrf_drv_saadc_init(NULL, saadc_volt_callback);
    APP_ERROR_CHECK(err_code);

    saadc_volt_channel_init(0,BAT_VOUT);          
    saadc_volt_channel_init(1,VDD_VOLT);    
    
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);    
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
    
}




static void saadc_channel_init(uint8_t chx, nrf_saadc_input_t analog_input)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(analog_input);
    channel_config.acq_time = NRF_SAADC_ACQTIME;   
    channel_config.reference = NRF_SAADC_REFERENCE_VDD4; 
    channel_config.gain = NRF_SAADC_GAIN1_4;
    err_code = nrf_drv_saadc_channel_init(chx, &channel_config);
    APP_ERROR_CHECK(err_code);    
}


static void saadc_volt_channel_init(uint8_t chx, nrf_saadc_input_t analog_input)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(analog_input);
    channel_config.acq_time = NRF_SAADC_ACQTIME;   
    channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL; 
    channel_config.gain = NRF_SAADC_GAIN1_6;
    err_code = nrf_drv_saadc_channel_init(chx, &channel_config);
    APP_ERROR_CHECK(err_code);    
}
