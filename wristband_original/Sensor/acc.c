/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "acc.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
/*Pins to connect shield. */
#define LIS2DH12_INT1_PIN                   3

#define _ACC_RX_POOL_LEN                    16

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
static bool acc_not_sampling = true;

/* Recoed accelerometer X, Y, Z*/
static ACC_DATA_T                       acc;   

/*LIS2DH12 ID*/
static uint8_t lis2dh12_id = 0x00;
static bool lis2dh12_interrupt_flag = false;


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void lis2dh12_write(uint8_t* tx_data, uint8_t tx_len);
static void lis2dh12_read(uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len);




/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
 
static void lis2dh12_write(uint8_t* tx_data, uint8_t tx_len)
{            
    TWIx_Write(LIS2DH12_ADDR, tx_data, tx_len);
}


static void lis2dh12_read(uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len)
{
    TWIx_Read(LIS2DH12_ADDR, cmd, rcv_data, rcv_len); 
}
 
 
void lis2dh12_int1_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{       
    
    lis2dh12_interrupt_flag = true; 
    /*
    Tic();
    acc_not_sampling = false;
    PRINT("#%d, ",ACC_RAWDATA);
    LIS2DH12_Read_XYZ();
    PRINT("%d,%d,%d\r\n", acc.x, acc.y, acc.z);
    acc_not_sampling = true; 
    PRINT("ACC Read Time is %.1f ms\n", (float)Toc()/10);
    */
}
 
bool Is_ACC_Not_Sampling(void)
{
    return acc_not_sampling;
}
    

bool Is_LIS2DH12_Interrupt(void)
{
    if (lis2dh12_interrupt_flag)
    {
        lis2dh12_interrupt_flag =false;            
        return true;
    }
    
    return false;
}

void Set_ACC_Start_Sampling(void)
{
    Init_LIS2DH12();
    LIS2DH12_Read_XYZ();
}





/**
 * @brief Function for setting active mode on MMA7660 accelerometer.
 */

bool Check_ACC_ID(void)
{
    return (lis2dh12_id == 0x33);
}



void Init_LIS2DH12(void)
{
    uint8_t data[2];      

    acc_not_sampling = true;
    
    data[0] = LIS2DH12_CTRL_REG0;
    data[1] = 0x90;
    lis2dh12_write(data, sizeof(data));
    
    data[0] = LIS2DH12_CTRL_REG1;
    data[1] = ORD_1HZ + Xen + Yen + Zen ; //ORD_1HZ + LPen + Xen + Yen + Zen
    lis2dh12_write(data, sizeof(data));

    data[0] = LIS2DH12_CTRL_REG3;
    data[1] = I1_DRDY1 ; 
    lis2dh12_write(data, sizeof(data));
    
    data[0] = LIS2DH12_CTRL_REG4;
    data[1] = FS_8G + HR;               // data[1] = FS_8G, for 10 bit. data[1] =FS_8G + HR, for 12 bit 
    lis2dh12_write(data, sizeof(data));

    data[0] = LIS2DH12_CTRL_REG5;
    data[1] = FIFO_EN;// FIFO_EN;
    lis2dh12_write(data, sizeof(data));   
	
	data[0] = LIS2DH12_FIFO_CTRL_REG;
    data[1] = FIFO_MODE_CFG_STREAM;	
    lis2dh12_write(data, sizeof(data));
    
}


void LIS2DH12_Read_XYZ(void)
{
    static uint8_t second=0;
    uint8_t rcv_data[6];
    uint32_t tick_count;
   
    
    PRINT("#%d,", ACC_RAWDATA); 
    Tic();
    lis2dh12_read(LIS2DH12_OUT_XYZ, rcv_data, sizeof(rcv_data));
    acc.x = ((int8_t)rcv_data[1] <<4) + (rcv_data[0]>>4);  // ((int8_t)rcv_data[1] <<2) + (rcv_data[0]>>6), for 10 bit
    acc.y = ((int8_t)rcv_data[3] <<4) + (rcv_data[2]>>4);
    acc.z = ((int8_t)rcv_data[5] <<4) + (rcv_data[4]>>4);
#if 0    
    static uint32_t sum;    
    sum=(0x3FF00000&((uint32_t)acc.x<<20)) + 
        (0x000FFC00&((uint32_t)acc.y<<10)) + 
        (0x000003FF&((uint32_t)acc.z));
#endif    
    Set_Latest_ACC(acc.x, acc.y, acc.z);
    tick_count = Toc();
    PRINT("%2d,%d,%d,%d\r\n", second++, acc.x, acc.y, acc.z);
    PRINT("ACC Read Time is %.1f ms\n", (float)tick_count/10);
    if (second >=ACC_DATA_LENGTH*2)
        second = 0;  
}


void LIS2DH12_Read_ID(void)
{
    uint8_t rcv_data[1];
    lis2dh12_read(LIS2DH12_WHO_AM_I, rcv_data, sizeof(rcv_data));
    lis2dh12_id = rcv_data[0];
}



/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output, 
 * and configures GPIOTE to give an interrupt on pin change.
 */
void Enable_LIS2DH12_INT1_FUNC(void)
{
    ret_code_t err_code;

    if (!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }
    
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
    in_config.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(LIS2DH12_INT1_PIN, &in_config, lis2dh12_int1_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(LIS2DH12_INT1_PIN, true);
}



