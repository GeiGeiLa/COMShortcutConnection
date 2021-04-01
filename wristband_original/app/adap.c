/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "adap.h"



/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/




/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/

typedef enum{
    NOP_STAT                 = 0,
    PARAM_INIT_STAT,
    MODULE_SELF_TEST_STAT,
    WAITTING_CONNECT_STAT,    
    MEASURE_SIG_STAT,
    MODULE_PREPARE_RESET_STAT,
    MODULE_ERR_STAT
} ADAP_STAT_T;



typedef enum{
    SENSOR_NOP_STAT                 = 0,
    SAADC_INIT_AND_CALIBRATION_STAT,    
    CHECK_BAT_VDD_STAT,
    CHECK_W25QXX_STAT,
    INTI_TWI_DRV_STAT,
    CHECK_LIS2DH12_STAT,
    CHECK_TEMP_SENSOR_STAT,
    CHECK_APDS930X_STAT,
    SENSOR_ERR_STAT

} SELF_TEST_STAT_T;


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/

static ADAP_STAT_T                  adap_stat = NOP_STAT;
static SELF_TEST_STAT_T             self_test_stat = SENSOR_NOP_STAT;
static uint8_t                      acc_sampling_count = 0;


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void init_self_test_stat(void);
static void module_self_test_proc(void);
static void check_bat_vdd_proc(void);
static void sensor_start(void);
static void start_battery_monitor(void);

static void adc_init_proc(void);
static void w25qxx_init_proc(void);
static void acc_init_proc(void);
static void temp_sensor_init_proc(void);
static void apds930x_init_proc(void);


static void temp_sensor_sampling_proc(void);
static void apds930x_sampling_proc(void);
static void prepare_reset_proc(void);

static void print_error_message(void);

static void acc_sensor_sampling(void);
static void write_sensor_data2flash(void);
static void erasr_entry_chip(void);


/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
void Adap_UARTs_Proc(void)
{

    if (Is_Rcv_CMD())
    {
        CMD_Decoder();
    }      
}
    

void Adap_Proc(void)
{

    if (IS_NOTIFY_SYSTEM_RESET())
        adap_stat = MODULE_PREPARE_RESET_STAT;

    
    switch (adap_stat)
    {
        case NOP_STAT:    
            Reset_All_Operate_Flag();
            adap_stat = PARAM_INIT_STAT;                  
            break;
        
        case PARAM_INIT_STAT:
            init_self_test_stat();        

            adap_stat = MODULE_SELF_TEST_STAT;           
            break;

        case MODULE_SELF_TEST_STAT:
            module_self_test_proc();
            if (IS_MODULE_SELF_TEST_DONE())
            {
                if (IS_BAT_VOLT_LOW())
                {
                    Set_Next_Power_Up_Mode(POWER_UP_INTO_LOW_BATTERY_MODE);
                    adap_stat = MODULE_PREPARE_RESET_STAT;
                }   
                else if ((Chk_All_Sensor_Stat() & ALLOW_EXEC) == ALLOW_EXEC)
                {
                    sensor_start();                    
                    adap_stat = MEASURE_SIG_STAT;                          
                }
                else
                    adap_stat = MODULE_ERR_STAT;	              
            }
        break;

  
        case MEASURE_SIG_STAT:     
            if (IS_BAT_VOLT_LOW())
            {
                Set_Next_Power_Up_Mode(POWER_UP_INTO_LOW_BATTERY_MODE);
                adap_stat = MODULE_PREPARE_RESET_STAT;
            }   
            if(IS_FLASH_RAM_FULL())
            {
#if 1
                Set_Next_Power_Up_Mode(POWER_UP_INTO_LOW_BATTERY_MODE);
                adap_stat = MODULE_PREPARE_RESET_STAT;
#else           // for debug use.               
                erasr_entry_chip();
#endif                
            }   
            else if (Is_LIS2DH12_Interrupt())
            {
                acc_sensor_sampling();
            }
          
            else if(IS_SENSOR_DATA_READY_2WRITE())
            {                                                                       
                write_sensor_data2flash();
            }
            
            else if (IS_PRINT_ALARM_MSG())
            {
                DATE_TIME_T dt =Get_DateTime();
                PRINT("20%02d-%02d-%02d, %02d:%02d:%02d, BAT=%0.2fV \n", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, (double)Get_BAT()*3.6/4096/BATTERY_GAIN);
            }
            
            break;
                   
        case MODULE_PREPARE_RESET_STAT:
            prepare_reset_proc();
            sd_nvic_SystemReset();
            break;        
        
        case MODULE_ERR_STAT:

            LED_O_OFF();
            LED_G_OFF();
            print_error_message();
            //adap_stat = MEASURE_SIG_STAT;
            break;
        default:
            break;
    } 
 }




static void module_self_test_proc(void)
{
    switch (self_test_stat)
    {
        case SENSOR_NOP_STAT:
            break;

        case SAADC_INIT_AND_CALIBRATION_STAT:
            RTC_Config();    
#if IGNORE_VDD_CHECK 
            Set_Module_Operate_Flag(BAT_VOLT_HIGH_FLAG);
            Set_Module_Operate_Flag(VDD_VOLT_CORRECT_FLAG);        
            self_test_stat = CHECK_W25QXX_STAT;
#else            
            adc_init_proc();
            self_test_stat = CHECK_BAT_VDD_STAT;
#endif

            break;
        
        case CHECK_BAT_VDD_STAT:
            check_bat_vdd_proc();
            self_test_stat = CHECK_W25QXX_STAT; 
            break;
        
        case CHECK_W25QXX_STAT:
            w25qxx_init_proc();
            self_test_stat = INTI_TWI_DRV_STAT;    
            break;
        
        case INTI_TWI_DRV_STAT:
            TWIx_Init();
            self_test_stat = CHECK_APDS930X_STAT;   
            break;
        
        case CHECK_APDS930X_STAT:
            apds930x_init_proc();    
            self_test_stat = CHECK_TEMP_SENSOR_STAT;            
            break;                

        case CHECK_TEMP_SENSOR_STAT:
            temp_sensor_init_proc();            
            self_test_stat = CHECK_LIS2DH12_STAT;      
            break;
        
        case CHECK_LIS2DH12_STAT:                        
            acc_init_proc();
            Set_Module_Operate_Flag(MODULE_SELF_TEST_DONE_FLAG);          
            self_test_stat = SENSOR_NOP_STAT;   
            break;        

        case SENSOR_ERR_STAT:
            break;
        default:break;    

    }
}




static void init_self_test_stat(void)
{
    self_test_stat = SAADC_INIT_AND_CALIBRATION_STAT;
}



static void adc_init_proc(void)
{        
    //SAADC_Calibration();
    //Init_SAADC_Chx_4BAT_and_VDD();    
    //PRINT("%d:\tADC Sampling Rate= %dHz, Buffer Size= %d.\n", MESSAGE ,Get_ADC_Sampling_Rate(), SAMPLES_IN_BUFFER);
     
}


void W25Qxx_Init_Proc(void)
{
    PRINT("W25Qxx Init...");

    FLASH_PW_ON();
    nrf_delay_ms(50);
    if (W25Qxx_Init())
    {
        Set_Module_Operate_Flag(W25Qxx_INITIALIZED_FLAG);
        PRINT("Success\n");
    }
    else 
    {
        Clear_Module_Operate_Flag(W25Qxx_INITIALIZED_FLAG);
        PRINT("False\n");
    }    
}

static void w25qxx_init_proc(void)
{
    PRINT("W25Qxx Init...");

    FLASH_PW_ON();
    nrf_delay_ms(50);
    if (W25Qxx_Init())
    {
        Set_Module_Operate_Flag(W25Qxx_INITIALIZED_FLAG);
        PRINT("Success\n");
//        W25Qxx_Erase_Sector(0);
//        W25Qxx_Erase_Entry_Chip();
    }
    else 
    {
        Clear_Module_Operate_Flag(W25Qxx_INITIALIZED_FLAG);
        PRINT("False\n");
    }    
    W25Qxx_Power_Down();    
    //W25Qxx_UnInit();
}


static void acc_init_proc(void)
{
    PRINT("LIS2DH12 Init...");    
    LIS2DH12_Read_ID();    
    if (Check_ACC_ID())
    {
        Set_Module_Operate_Flag(ACC_INITIALIZED_FLAG);    
        PRINT("Success.\n");        
    }
    else
    {
        Clear_Module_Operate_Flag(ACC_INITIALIZED_FLAG); 
        PRINT("False.\n");
    }
    
}



static void temp_sensor_init_proc(void)
{
    PRINT("Si705x Init...");
    Test_Si705x();
    
    if ((Get_Temperature()!=-255))
    {
        Set_Module_Operate_Flag(SI705x_INITIALIZED_FLAG);
        PRINT("Success.\n");
        //PRINT("%d:Temperature= %.1f degreeC.\n", TEMP_SENSOR_RAWDATA ,Get_Temperature());        
    }
    else
    {
        Clear_Module_Operate_Flag(SI705x_INITIALIZED_FLAG);
        PRINT("False.\n");
    }        
}


static void temp_sensor_sampling_proc(void)
{
    static float temperature;
    PRINT("#%d,",TEMP_SENSOR_RAWDATA);
    temperature = Si705x_Read_Temp();
    PRINT("%.1f\n", temperature);
    if ((temperature != 0))
    {
        Set_Latest_Temp((int16_t)(temperature*10));
    }
}

static void apds930x_init_proc(void)
{
    PRINT("APDS930x Init...");
    APDS930x_Init();
    if (APDS930x_Read_ID() == APDS930x_ID)
    {
        Set_Module_Operate_Flag(APDS930x_INITIALIZED_FLAG);
        // Into Power down.
        APDS930x_Power_Down();
        PRINT("Success.\n");
    }
    else
    {
        Clear_Module_Operate_Flag(APDS930x_INITIALIZED_FLAG);
        PRINT("False.\n");
    }
}

static void apds930x_sampling_proc(void)
{
    float lux;
    static uint8_t stat;
    PRINT("#%d,", AMBIENT_LIGHT_SENSOR_RAWDATA);     
    lux = APDS930x_Read_ADC_Data();    
    // Into Power down.
    APDS930x_Power_Down();
    Set_Latest_Light_Lux((uint16_t)(lux*1000));
    
    PRINT("%.4f\n",lux);    
}


static void prepare_reset_proc(void)
{
    TWIx_Uninit();
    W25Qxx_UnInit();
    FLASH_PW_OFF();    
}


static void check_bat_vdd_proc(void)
{    
    Init_SAADC_Chx_4BAT_and_VDD();
    ADC_Sampling_Triggle_Enable();
    
    while(!IS_BAT_VDD_SAMPLING_DONE());
    
    // Check Initialize VDD range.
    if (Is_VDD_Volt_Correct())
        Set_Module_Operate_Flag(VDD_VOLT_CORRECT_FLAG);
    else
        Clear_Module_Operate_Flag(VDD_VOLT_CORRECT_FLAG);
    
    if (Is_BAT_Volt_High())
        Set_Module_Operate_Flag(BAT_VOLT_HIGH_FLAG);
    else
        Clear_Module_Operate_Flag(BAT_VOLT_HIGH_FLAG);       
    
    PRINT("\t%d\tBAT=%0.2fV\tDVDD = %0.2fV\n", Get_BAT(), (double)Get_BAT()*3.6/4096/BATTERY_GAIN ,(double)Get_VDD()/4096*3.6);
    ADC_Sampling_Triggle_Disable();       // Disable RTC. Power Off.
    Uninit_BAT_VDD_Channel();
}


static void sensor_start(void)
{
    Init_LIS2DH12();
    Enable_LIS2DH12_INT1_FUNC();
    Set_Module_Operate_Flag(SENSOR_START_SAMPLING_FLAG);
    Reset_Sensor_Time_Stamp();
    LIS2DH12_Read_XYZ();
}


static void start_battery_monitor(void)
{
/*   
    Init_SAADC_Chx_4BAT_and_VDD();
    Set_ADC_Sampling_Rate(LOW_ADC_SAMPLING_RATE);
    Update_ADC_Sampling_Rate();
    Start_ADC_Sampling(); 
    */
}



 
static void acc_sensor_sampling(void) 
{
    // 1. Read ACC
    //LED_G_BLINK();
    TWIx_Enable();
    LIS2DH12_Read_XYZ();                
    acc_sampling_count =acc_sampling_count + 1;
    
    if (IS_AMBIENT_LIGHT_SENSOR_POWER_UP() )
    {
        // 1. into Power up.
        APDS930x_Power_Up();    // need 500ms after Power Up.        
        Clear_Module_Operate_Flag(AMBIENT_LIGHT_SENSOR_POWER_UP_FLAG);
    }
    
    if (IS_TEMP_SAMPLING())
    {
        // 2. Lignt Sensor Samling.        
        Tic();              
        LED_G_ON();       
        apds930x_sampling_proc();
        Clear_Module_Operate_Flag(AMBIENT_LIGHT_SAMPLING_FLAG);                
        LED_G_OFF();
        PRINT("APDS930x Read Time is %.1f ms\n", (float)Toc()/10); 
        
        // 3. start temperature sampling.        
        LED_G_ON();
        Tic();
        temp_sensor_sampling_proc();
        Clear_Module_Operate_Flag(TEMP_SAMPLING_FLAG);
        LED_G_OFF();
        PRINT("Temp Read Time is %.1f ms\n", (float)Toc()/10); 
        check_bat_vdd_proc();                                    
    }
    TWIx_Disable();    
}


static void write_sensor_data2flash(void)
{
    
    W25Qxx_Release_Power_Down();        
    nrf_delay_ms(10);                    
    Tic();
    LED_G_ON();
    Write_Data_2Flash();                    
    LED_G_OFF();
    Clear_Module_Operate_Flag(SENSOR_DATA_READY_2WRITE_FLAG);                    
    W25Qxx_Power_Down();                    
    PRINT("Flash Write Time is %.1f ms\n", (float)Toc()/10);

    if(IS_FLASH_RAM_WRITE_ERROR())
        LED_O_ON();
    
}


static void erasr_entry_chip(void)
{
    static uint16_t erase_count = 0;
    PRINT("%d times Erase Flash...",++erase_count);
    W25Qxx_Release_Power_Down();
    W25Qxx_Erase_Entry_Chip();
    W25Qxx_Power_Down();
    PRINT("Done...\n");                
    Reset_Page_Num();
}





static void print_error_message(void)
{
    if(IS_PRINT_ALARM_MSG())
    {
        if(!IS_VDD_VOLT_CORRECT() )               
            PRINT("%d,%d\n",PM25_ALARM_EVENT, 1<<0);
        if(!IS_BAT_VOLT_HIGH())       
            PRINT("%d,%d\n",PM25_ALARM_EVENT, 1<<1);
        if(!IS_W25Qxx_INITIALIZED())                  
            PRINT("%d,%d\n",PM25_ALARM_EVENT, 1<<2);
        if(!IS_SI705x_INITIALIZED())                  
            PRINT("%d,%d\n",PM25_ALARM_EVENT, 1<<3);        
        if(!IS_APDS930x_INITIALIZED())                  
            PRINT("%d,%d\n",PM25_ALARM_EVENT, 1<<5);
        if(!IS_ACC_INITIALIZED())                  
            PRINT("%d,%d\n",PM25_ALARM_EVENT, 1<<6);

    }
}

