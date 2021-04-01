/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "timer.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define APP_TIMER_PRESCALER                 0                   /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE             4                   /**< Size of timer operation queues. */



/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/
#define LED_BLINK_INTERVAL          APP_TIMER_TICKS(1000)          /**< LED blink interval (ticks). */
#define CMD_TIMER_INTERVAL          APP_TIMER_TICKS(10)            /**< command timer interval (ticks). */

#if (PRINT_RAW_DATA_VIA_BLE_UART==0)
    #if (BLE_IOS)
        #define BLE_TIMER_INTERVAL          APP_TIMER_TICKS(30)            /**< command timer interval (ticks). */
    #else    
        #define BLE_TIMER_INTERVAL          APP_TIMER_TICKS(50)            /**< command timer interval (ticks). */
    #endif
#else
    #define BLE_TIMER_INTERVAL          APP_TIMER_TICKS(1000)            /**< command timer interval (ticks). */
#endif	

APP_TIMER_DEF(m_leds_timer_id);                        /**< LEDs timer. */
APP_TIMER_DEF(m_cmds_timer_id);                        /**< Command timeout timer. */
APP_TIMER_DEF(m_ble_timer_id);                        /**< BLE timeout timer. */



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
const nrf_drv_timer_t       TIMER_CTRL_SIG = NRF_DRV_TIMER_INSTANCE(1);

static uint8_t              acc_sensor_time_stamp = 0;
static uint8_t              temp_sensor_time_stamp = 0;

static uint16_t             month_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint32_t             tick_counter =0;
/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/

static void leds_timeout_1000ms_handler(void * p_context);
static void cmds_timeout_10ms_handler(void * p_context);
static void application_timers_init(void);
static void application_led_timers_start(void);
static void update_clock(void);
static void acc_sampling_time_stamp(void);
static void temp_sampling_time_stamp(void);
static bool check_datetime(DATE_TIME_T *ptr);
static uint32_t datatime_2uint32(void);
static void update_print_flag(void);
static void init_datetime(void);

#if ENABLE_TICK_TIMER    
static void tick_timer_start(void);
static void tick_timer_init(void);
#endif

/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
static void tick_timer_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            tick_counter++;
            break;
    
        default:
            //Do nothing.
            break;
    }

}


void Tic(void)
{
    tick_counter = 0;    
}


uint32_t Toc(void)
{
    return tick_counter;
}



static void ble_timeout_handler(void * p_context)
{
    if(IS_BLE_CONNECT())
    {
#if (BLE_UART_SERVICE_ENABLE)        
    #if (PRINT_RAW_DATA_VIA_BLE_UART)

            BLE_UARTS_Tx_EXG(); 
    #else	
        BLE_UARTS_Tx_DateTime();
    #endif	
#endif        
    }
}



static void cmds_timeout_10ms_handler(void * p_context)
{
    
    static uint8_t interval = 0;
    UART_Rcv_Byte_Timer();
    Inc_W25QXX_Cmd_Timer();
    Button_Click_Detect();    
    /*
    if (++interval>=2)
    {
        interval = 0;
        for(uint8_t i=0; i<1;i++)
        {
            acc_sampling_time_stamp();
        }
    }
    */
}


static void leds_timeout_1000ms_handler(void * p_context)
{
    
    UNUSED_PARAMETER(p_context);    
    update_clock();    
    update_print_flag();  
    acc_sampling_time_stamp();
    temp_sampling_time_stamp();

}



static void update_clock(void)
{
       
    if (++cdt.second >= 60)             // SECOND
    {
        cdt.second = 0;        
       // Start_ADC_Sampling();
        if (++cdt.minute >=60)          // MINUTE
        {
            cdt.minute =0;            
            if (++cdt.hour >= 24)       // HOUR
            {
                cdt.hour = 0;             
                if (++cdt.day >= month_day[cdt.month-1] )
                {
                    cdt.day = 1;    
                    if (++cdt.month >= 12 )
                    {
                        cdt.month = 1;
                        cdt.year++;                        
                    }   
                }                
            }            
        }
        //if ((cdt.minute& 0x03) == 0x00)
    
    }        
}


static void acc_sampling_time_stamp(void)
{
    if (IS_SENSOR_START_SAMPLING())
    {        
        if (acc_sensor_time_stamp == 0)
        {                       
            Update_Record_Data(datatime_2uint32());
        }
        Update_ACC_Data();
        
        acc_sensor_time_stamp = acc_sensor_time_stamp +1;        
          
        if (acc_sensor_time_stamp >= ACC_DATA_LENGTH)
        {
            Update_Next_Segment();
            acc_sensor_time_stamp = 0;            
        }
    }
}



static void temp_sampling_time_stamp(void)
{

    if (IS_SENSOR_START_SAMPLING())
    {              
        temp_sensor_time_stamp = temp_sensor_time_stamp +1;        

        if (temp_sensor_time_stamp == TEMP_SAMPLING_INTERVAL-1)
        {
            Set_Module_Operate_Flag( AMBIENT_LIGHT_SENSOR_POWER_UP_FLAG);                        
        }
        if (temp_sensor_time_stamp == TEMP_SAMPLING_INTERVAL)
        {
            Set_Module_Operate_Flag(TEMP_SAMPLING_FLAG);                       
            temp_sensor_time_stamp = 0;      
        }
    }
}


static uint32_t datatime_2uint32(void)
{
    uint32_t datatime=0;
    uint32_t tmp;

    tmp= cdt.year-19;
    datatime += (tmp<<(4+5+5+6+6));
    tmp= cdt.month;
    datatime += (tmp<<(5+5+6+6));
    tmp= cdt.day;
    datatime += (tmp<<(5+6+6));
    tmp= cdt.hour;
    datatime += (tmp<<(6+6));     
    tmp= cdt.minute;
    datatime += (tmp<<6);  
    tmp= cdt.second;
    datatime += (tmp<<0);

    return datatime;
}



void Reset_Sensor_Time_Stamp(void)
{
    acc_sensor_time_stamp = 0;
    temp_sensor_time_stamp = 0;
    Init_Sensor_Data_Buff_Param();
}


bool Update_DateTime(DATE_TIME_T *ptr)
{    
    if (check_datetime(ptr))
    {    
        memcpy(&cdt, ptr, sizeof(DATE_TIME_T));
        if ((cdt.year & 0x03) == 0x00)
            month_day[2-1] = 29;
        return true;
    }
    return false;
}



static bool check_datetime(DATE_TIME_T *ptr)
{
    if ((ptr->year+2000) < 2019)            return false;
    if (ptr->month > 12)                    return false;
    if (ptr->day > month_day[ptr->month-1]) return false;
    if (ptr->hour > 23)                     return false;
    if (ptr->minute > 59)                   return false;
    if (ptr->second > 59)                   return false;
    return true;
}


DATE_TIME_T Get_DateTime(void)
{
    return cdt;
}

static void update_print_flag(void)
{

    Set_Module_Operate_Flag(PRINT_ALARM_MSG_FLAG);

}






/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void application_timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.
    err_code = app_timer_create(&m_leds_timer_id, APP_TIMER_MODE_REPEATED, leds_timeout_1000ms_handler);    
    APP_ERROR_CHECK(err_code);


    err_code = app_timer_create(&m_cmds_timer_id, APP_TIMER_MODE_REPEATED, cmds_timeout_10ms_handler);
    APP_ERROR_CHECK(err_code);
    
    if (Get_Power_Up_Mode() == POWER_UP_INTO_RECOED_WITH_BLE_MODE)      
    {
        err_code = app_timer_create(&m_ble_timer_id, APP_TIMER_MODE_REPEATED, ble_timeout_handler);
        APP_ERROR_CHECK(err_code);    
    }

}


/**@brief Function for starting application timers.
 */
static void application_led_timers_start(void)
{
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_leds_timer_id, LED_BLINK_INTERVAL, NULL);    
    APP_ERROR_CHECK(err_code);
      
    err_code = app_timer_start(m_cmds_timer_id, CMD_TIMER_INTERVAL, NULL);    
    APP_ERROR_CHECK(err_code);   


    if (Get_Power_Up_Mode() == POWER_UP_INTO_RECOED_WITH_BLE_MODE)  
    {
        err_code = app_timer_start(m_ble_timer_id, BLE_TIMER_INTERVAL, NULL);    
        APP_ERROR_CHECK(err_code);   
    }
}




static void init_datetime(void)
{
    cdt.year = 19;
    cdt.month = 6;
    cdt.day = 6;
    cdt.hour = 8;
    cdt.minute = 47;
    cdt.second = 56;
}


#if ENABLE_TICK_TIMER    
static void tick_timer_init(void)
{

    uint32_t time_us = 100; //Time(in microseconds) between consecutive compare events.
    uint32_t time_ticks1;
    uint32_t err_code = NRF_SUCCESS;

    //Configure TIMER_CTRL_SIG for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&TIMER_CTRL_SIG, &timer_cfg, tick_timer_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks1 = nrf_drv_timer_us_to_ticks(&TIMER_CTRL_SIG, time_us);

    nrf_drv_timer_extended_compare(
         &TIMER_CTRL_SIG, NRF_TIMER_CC_CHANNEL0, time_ticks1, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

}

static void tick_timer_start(void)
{
    nrf_drv_timer_enable(&TIMER_CTRL_SIG);  
}
#endif

void Init_Timer(void)
{    
    application_timers_init();
    application_led_timers_start();    
    
#if ENABLE_TICK_TIMER    
    tick_timer_init();
    tick_timer_start();
#endif    
}


void Init_Timer_4RTC_Only(void)
{
    uint32_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.
    err_code = app_timer_create(&m_leds_timer_id, APP_TIMER_MODE_REPEATED, leds_timeout_1000ms_handler);    
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_start(m_leds_timer_id, LED_BLINK_INTERVAL, NULL);    
    APP_ERROR_CHECK(err_code);    
}

