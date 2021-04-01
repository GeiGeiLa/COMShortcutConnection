/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "si705x.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
static float       si705x_temperature=-255;


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/* Function Prototypes                                */
/*----------------------------------------------------------*/
static void si705x_write(uint8_t* data, uint8_t len);
static void si705x_read(uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len);
static float pack_decoder(uint8_t *rcv_data);
static float cal_temperature(uint16_t val);
static float read_temp(void);

/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
float Si705x_Read_Temp(void)
{    
    return(read_temp());
}



float Get_Temperature(void)
{
    return si705x_temperature;
}


void Test_Si705x(void)
{
    uint8_t data[1];
    si705x_temperature = -255;
    
    data[0] = SI705x_RESET_CMD;    
    si705x_write(data, sizeof(data));
    nrf_delay_ms(10);
    read_temp();

}

static float read_temp(void)
{
    uint8_t rcv_data[3];
    si705x_temperature = -255;    
    
    /* Read 3 bytes from the specified address. */
    si705x_read(SI705x_MEASTEMP_HOLD_CMD, rcv_data, sizeof(rcv_data));
    return (pack_decoder(rcv_data));
   
}


static float pack_decoder(uint8_t *rcv_data)
{    
    static float tmp;    
    tmp = cal_temperature((rcv_data[0]<<8) + rcv_data[1]);
    si705x_temperature = (tmp < 0) ? -1 : tmp;
    return si705x_temperature;
}



static float cal_temperature(uint16_t val)
{
    static double tmp;
    tmp = val;
    tmp *= 175.72;
    tmp /= 65536;
    tmp -= 46.85;
    return tmp;        
}



static void si705x_write(uint8_t* tx_data, uint8_t tx_len)
{            
    TWIx_Write(SI705x_ADDR, tx_data, tx_len);
}


static void si705x_read(uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len)
{
    TWIx_Read(SI705x_ADDR, cmd, rcv_data, rcv_len); 
}
