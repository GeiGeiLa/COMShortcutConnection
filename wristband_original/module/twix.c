/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "twix.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define  TWIx_SCL_PIN      11
#define  TWIx_SDA_PIN      2

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
/* TWI instance. */
static const nrf_drv_twi_t m_twix = NRF_DRV_TWI_INSTANCE(0);

/* Indicates if reading operation from accelerometer has ended. */
static volatile bool m_read_reg_done = true;

/* Indicates if setting mode operation has ended. */
static volatile bool m_write_reg_done = false;





/*----------------------------------------------------------*/
/* Function Prototypes                                */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
 /**
 * @brief TWI events handler.
 */
void twix_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{         
    switch(p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_TX)
            {
                m_write_reg_done  = true;                
            }
            else    // TWI RX
            {                
               m_read_reg_done = true;
            }
            break;
        default:
            break;        
    }   
}
 



void TWIx_Write(uint8_t addr, uint8_t* tx_data, uint8_t tx_len)
{            
    ret_code_t err_code;    
    // 1. Make sure Tx& Rx command is done.
    do
    {
        __WFE();
    }while((m_write_reg_done == false) || (m_read_reg_done == false)); 

    
    m_write_reg_done = false;
    err_code = nrf_drv_twi_tx(&m_twix, addr, tx_data, tx_len, false);  
    APP_ERROR_CHECK(err_code);

    do {
        __WFE();
    }
    while(m_write_reg_done == false);
}


void TWIx_Read(uint8_t dev_addr, uint8_t cmd, uint8_t* rcv_data, uint8_t rcv_len)
{
    ret_code_t err_code; 
    
    // Make sure Tx& Rx command is done. 
    do
    {
        __WFE();
    }while((m_write_reg_done == false) || (m_read_reg_done == false)); 
    
    // STEP1. Send read register address command.
    m_write_reg_done = false;
    err_code = nrf_drv_twi_tx(&m_twix, dev_addr, &cmd, 1, true); 
    APP_ERROR_CHECK(err_code);
    do {
        __WFE();
    }
    while(m_write_reg_done == false);    
    
     
    // STEP2. Send read length of read register command.    
    m_read_reg_done = false;
    err_code = nrf_drv_twi_rx(&m_twix, dev_addr, rcv_data, rcv_len);
    APP_ERROR_CHECK(err_code);
    do {
        __WFE();
    }
    while(m_read_reg_done == false);   
}




 /**
 * @brief TWI(I2C) initialization.
 */
static void twix_init(void)
{
    ret_code_t err_code;
    
    const nrf_drv_twi_config_t twix_config = {
       .scl                = TWIx_SCL_PIN,
       .sda                = TWIx_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_LOW
    };
    
    err_code = nrf_drv_twi_init(&m_twix, &twix_config, twix_handler, NULL);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_twi_enable(&m_twix);
}


void TWIx_Enable(void)
{
     nrf_drv_twi_enable(&m_twix);
}


void TWIx_Disable(void)
{
     nrf_drv_twi_disable(&m_twix);
}



void TWIx_Uninit(void)
{    
    nrf_drv_twi_uninit(&m_twix);    
}

void TWIx_Init(void)
{
    twix_init();
    m_read_reg_done = true;
    m_write_reg_done = true;
}





