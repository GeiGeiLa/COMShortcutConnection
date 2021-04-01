/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "raytac52.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define IS_NO_BTN_PUSH()                nrf_gpio_pin_read(BUTTON_SW_PIN)
#define IS_BTN_PUSH()                   !nrf_gpio_pin_read(BUTTON_SW_PIN)

#define DOUBLE_CLICK_TIMEOUT            (0.5 * SECOND)
#define LONG_PUSH_THRESHOLD_TIME        (5 * SECOND)    

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
typedef enum
{
    BTN_NO_PUSH = 0,
    BTN_PUSH,
    BTN_LONG_PUSH,
    BTN_DOUBLE_CLICK,
}_BTN_STAT_T;



/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
static bool rx_interrupt_triggle = false;

/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
void uart_rx_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void uart_tx_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void uart_rx_pin_init(void);

/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
void uart_rx_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{           
    if (!rx_interrupt_triggle)
    {
        if(!(IS_SENSOR_DATA_READY_2WRITE() || IS_TEMP_SAMPLING() || IS_AMBIENT_LIGHT_SENSOR_POWER_UP()))       
        {
            if (Get_Power_Up_Mode()==POWER_UP_INTO_RECOED_WITH_BLE_MODE)  
            {
                sd_softdevice_disable();
            }        
            Set_Next_Power_Up_Mode(POWER_UP_INTO_UART_TRANS_MODE);
            Set_Module_Operate_Flag(NOTIFY_SYSTEM_RESET_FLAG);        
            LED_O_BLINK();
            rx_interrupt_triggle = true;
        }
        else{
            LED_G_BLINK();
        }
    }
}



void Button_Click_Detect(void) {   
    static uint16_t button_keep_push_timer = 0;    
    static uint8_t  click_timeout = 0;
    static uint8_t  click_cnt = 0;
    static bool enable_double_click_cnt_flag = false;
    static _BTN_STAT_T btn_stat = BTN_NO_PUSH;
    
    if (enable_double_click_cnt_flag){
        if (click_timeout ++ >  DOUBLE_CLICK_TIMEOUT) {
            click_cnt = 0;
            click_timeout = 0;
            enable_double_click_cnt_flag = false;
        }
    }
    
    switch (btn_stat)
    {
        case BTN_NO_PUSH:
            Clear_Module_Operate_Flag(BTN_LONG_PUSH_FLAG);
            Clear_Module_Operate_Flag(BTN_DOBULE_CLICK_FLAG);                   
            if (IS_BTN_PUSH()) {
                
                btn_stat = BTN_PUSH;
                button_keep_push_timer = 0;                 
            }            
            break;

        case BTN_PUSH:
            button_keep_push_timer ++;
            if (IS_NO_BTN_PUSH()) 
            {
                enable_double_click_cnt_flag = true;
                click_cnt++;
                if (click_cnt == 2) // double click.
                {
                    click_cnt = 0;          
                    enable_double_click_cnt_flag = false;
                    btn_stat = BTN_DOUBLE_CLICK;
                    click_timeout = 0;
                    PRINT("double click\r\n");  

                }
                else
                    btn_stat = BTN_NO_PUSH;
            }
            else if ( button_keep_push_timer > LONG_PUSH_THRESHOLD_TIME) 
            {                
                btn_stat = BTN_LONG_PUSH;                
                PRINT("Long push\r\n");
                Set_Next_Power_Up_Mode(POWER_UP_INTO_RECOED_WITH_BLE_MODE);
                Set_Module_Operate_Flag(NOTIFY_SYSTEM_RESET_FLAG);                
            }
            break;

        case BTN_LONG_PUSH:
            
            Set_Module_Operate_Flag(BTN_LONG_PUSH_FLAG);
        
            if (IS_NO_BTN_PUSH()) {                
                btn_stat = BTN_NO_PUSH;
            }
            break;
            
        case BTN_DOUBLE_CLICK:
            
            Set_Module_Operate_Flag(BTN_DOBULE_CLICK_FLAG);
        
            if (IS_NO_BTN_PUSH()) {                                
                btn_stat = BTN_NO_PUSH;
            }
            break;

        default: break;
    }
}


/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output, 
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void uart_rx_pin_init(void)
{
    ret_code_t err_code;

    if (!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }
    
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(RX_PIN_NUMBER, &in_config, uart_rx_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(RX_PIN_NUMBER, true); 
}


void Init_GPIO(void)
{
    nrf_gpio_cfg_output(LED_G_PIN);
    nrf_gpio_cfg_output(LED_O_PIN);
    nrf_gpio_cfg_output(LCD_nRES_PIN);
    
   
    // Button Pin.
    nrf_gpio_cfg_input(BUTTON_SW_PIN, NRF_GPIO_PIN_NOPULL);    
    
    nrf_gpio_cfg_output(FLASH_PW_ENABLE_PIN);
    nrf_gpio_cfg_output(LCD_PW_ENABLE_PIN);

   
    if(Get_Power_Up_Mode()!=POWER_UP_INTO_UART_TRANS_MODE) 
    {          
        uart_rx_pin_init();
        rx_interrupt_triggle = false;
        __NOP();
    }
    
    
    LED_G_OFF(); 
    LED_O_OFF(); 
    FLASH_PW_OFF();
    LCD_PW_OFF();
    LCD_nRES_HIGH();
   
}






