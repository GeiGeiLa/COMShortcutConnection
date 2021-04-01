/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "apds930x.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define  APDS930x_SCL_PIN      11
#define  APDS930x_SDA_PIN      2

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
struct APDS930x_settings {
    uint8_t APDS930x_enable;
    uint8_t APDS930x_gain;
    uint8_t APDS930x_manual_time;
    uint8_t APDS930x_integration_time;
    uint8_t APDS930x_level_interrupt;
    uint8_t APDS930x_persistence_value;
};


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
static struct APDS930x_settings settings;





/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
 
static void apds930x_write(uint8_t* tx_data, uint8_t tx_len)
{            
   TWIx_Write(APDS930x_ADDR, tx_data, tx_len);
}    


static void apds930x_read(uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len)
{
    TWIx_Read(APDS930x_ADDR, cmd, rcv_data, rcv_len);   
}



/**
 *    @brief function to configure sensor parameters.
 * These parameters are defined in TIMING register.
 * Bit[1:0] defines Integration Time.
 * Bit[3] defines Manual Timing Control. Write Bit[1:0] as '11' and set Bit[3] to enable manual timing control.
 * Bit[4] defines Gain. Low gain = 0, High gain = 1.
 */
void APDS930x_config(void)
{
    uint8_t tx_data[2];
    
    if(settings.APDS930x_enable == 1)
    {
        tx_data[0] = (CMD | TIMING_REG);
        tx_data[1] = 0;

        switch(settings.APDS930x_gain) 
        {
            default:
            case 0:
                tx_data[1] |= APDS930x_SET_GAIN_LOW_GAIN;
                break;

            case 1:
                tx_data[1] |= APDS930x_SET_GAIN_HIGH_GAIN;
                break;
            
        }

        switch(settings.APDS930x_integration_time)
        {
            case 0:
                tx_data[1] |= APDS930x_INTG_TIME_13_7_MS;
                break;
            case 1:
                tx_data[1] |= APDS930x_INTG_TIME_101_MS;
                break;
            
            default:
            case 2:
                tx_data[1] |= APDS930x_INTG_TIME_402_MS;
                break;
            case 3:
                tx_data[1] |= APDS930x_INTG_TIME_MANUAL;
                break;           
        }

        if(settings.APDS930x_integration_time == 3)
        {
            switch(settings.APDS930x_manual_time)
            {
                case 0:
                    tx_data[1] |= APDS930x_MANUAL_TIME_DISABLE;
                    break;

                case 1:
                    tx_data[1] |= APDS930x_MANUAL_TIME_ENABLE;
                    break;
            }
        }
        apds930x_write(tx_data, sizeof(tx_data));
    }
}

/**
 * @brief function to turn the device ON
 * Bit[1:0] is Power control bit. Set Bit[1:0] to power up.
 */
void APDS930x_Power_Up(void)
{
    
    uint8_t tx_data[2];
    tx_data[0] = (CMD | CTRL_REG);
    tx_data[1] = 0x03;
    apds930x_write(tx_data, sizeof(tx_data));
    
}

/**
 * @brief function to turn the device OFF
 * Bit[1:0] is Power control bit. Reset Bit[1:0] to power down.
 */
void APDS930x_Power_Down(void)
{

    uint8_t tx_data[2];
    tx_data[0] = (CMD | CTRL_REG);
    tx_data[1] = 0x00;
    apds930x_write(tx_data, sizeof(tx_data));
}

/**
 * @brief function to initialize and power up the sensor.
 */
void APDS930x_Init(void)
{
    settings.APDS930x_enable = 1;               // enable = 1, disable = 0.
    settings.APDS930x_gain  = 0;                // low gain = 0, high gain = 1.
    settings.APDS930x_manual_time  = 0;         // start integration cycle = 1, stop integration cycle = 0.
                                                                                                // applicable only if APDS930x_integratation_time = 3.
    settings.APDS930x_integration_time = 2;     // 13.7 ms = 0, 101 ms = 1, 402 ms = 2, custom integration time = 3.
    settings.APDS930x_level_interrupt = 0;      // level interrupt enable = 1, leven interrupt disable = 0.
    settings.APDS930x_persistence_value = 0;

    if (settings.APDS930x_enable == 1) {
        APDS930x_Power_Up();
    }
    APDS930x_config();
}




/**
 * @brief function to check if device is ON or OFF.
 * Read Bit[1:0] to get current power status
 */
uint8_t APDS930x_get_power_status(void)
{
    uint8_t stat;
    uint8_t cmd = CMD | CTRL_REG;
    apds930x_read(cmd, &stat, sizeof(stat));
    return stat;
}






/**
 * @brief function to test sensor id
 */
uint8_t APDS930x_Read_ID(void)
{

    uint8_t id = 0;
    uint8_t cmd = CMD | ID_REG;
    apds930x_read(cmd, &id, sizeof(id));
    return id;
}


float APDS930x_Read_ADC_Data(void)
{
    uint16_t aadc_ch0[1] = {0};
    uint16_t aadc_ch1[1] = {0};
    uint8_t adc_data[4];
    uint8_t cmd = CMD | DATA0_LOW;
    apds930x_read(cmd, adc_data, sizeof(adc_data)); 

    *aadc_ch0 = (adc_data[1] << 8) | adc_data[0];
    *aadc_ch1 = (adc_data[3] << 8) | adc_data[2];
    return (getlux(aadc_ch0[0], aadc_ch1[0]));
}



/**
 * @brief function to compute brightness (lux) bases on ADC values
 * The equations and parameters are from APDS930x datasheet - page 4.
 */
float getlux(uint16_t adc_ch0, uint16_t adc_ch1)
{
    float result;
    float channelRatio = (adc_ch1)/(float)(adc_ch0);

    // below formula is from datasheet

    if(channelRatio >= 0 && channelRatio <= 0.50f) {
        result = (0.0304 * adc_ch0) - (0.062 * adc_ch0 * pow(channelRatio, 1.4));
    }
    else if(channelRatio > 0.50f && channelRatio <= 0.61f) {
        result = (0.0224 * adc_ch0) - (0.031 * adc_ch1);
    }
    else if(channelRatio > 0.61f && channelRatio <= 0.80f) {
        result = (0.0128 * adc_ch0) - (0.0153 * adc_ch1);
    }
    else if(channelRatio > 0.80f && channelRatio <= 1.30f) {
        result = (0.00146 * adc_ch0) - (0.00112 * adc_ch1);
    }
    else if(channelRatio > 1.30f) {
        result = 0;
    }
    else {
        result = 999.99;
    }

    return result;
}


