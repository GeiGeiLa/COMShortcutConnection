/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "flash_access.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define POWER_UP_MODE_BACKUP_SIZE   32

#define SB                          0x55
#define EB                          0xAA

// Segment C
#define INFO_CMD                    0x10






    
/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/


#pragma pack(push,1)

typedef struct
{
    uint8_t             sb;
    uint8_t             cmd;
    uint16_t            len;    
    uint8_t             uart_buarrate_mode;
    uint8_t             acc_sampling_rate;
    POWER_UP_MODE_T     next_power_up_mode[POWER_UP_MODE_BACKUP_SIZE];
    DATE_TIME_T         dt[POWER_UP_MODE_BACKUP_SIZE];
    uint32_t            flash_page_num[POWER_UP_MODE_BACKUP_SIZE];
    uint8_t             eb;    
}MODULE_INFO_T;
//#pragma pack(pop,1)
#pragma pack(pop)


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/
#define TOTAL_PKT_LEN           sizeof(MODULE_INFO_T)
#define PAYLOAD_LEN             (TOTAL_PKT_LEN - 5)


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/

static uint32_t         pg_size;
static uint32_t         pg_num;
static uint32_t         *info_addr;
static MODULE_INFO_T    module_info;
static uint16_t         power_up_mode_idx;


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void flash_page_erase(uint32_t * page_address);
static void update_least_datetime(uint16_t index);
static void update_least_page_num2write(uint16_t index);
static void set_default_datetime(void);
static void flash_word_write(uint32_t * address, uint32_t value);
static void load_module_default_setting(void);
static void store_module_info2flash(void);
static void store_next_power_up_mode2flash(bool iserase);
static void write_seg(uint32_t* addr, uint8_t* ptr_data, uint16_t len, bool is_erase);
static uint16_t chk_power_up_mode_idx(void);

/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/




void Set_Next_Power_Up_Mode(POWER_UP_MODE_T mode)
{
    
   
    if (++power_up_mode_idx>= POWER_UP_MODE_BACKUP_SIZE )  
        power_up_mode_idx = 0;    
    
    if (power_up_mode_idx==0)
    {
        memset(module_info.next_power_up_mode, 0xff, POWER_UP_MODE_BACKUP_SIZE);
        memset(module_info.dt, 0xff, sizeof(DATE_TIME_T)*POWER_UP_MODE_BACKUP_SIZE);
        memset(module_info.flash_page_num, 0xff, POWER_UP_MODE_BACKUP_SIZE*4);   
    }
    module_info.next_power_up_mode[power_up_mode_idx] = mode;
    update_least_datetime(power_up_mode_idx);
    update_least_page_num2write(power_up_mode_idx);

    
    if (power_up_mode_idx==0)
        store_next_power_up_mode2flash(true);   
    else
        store_next_power_up_mode2flash(false); 
    
}


static void update_least_datetime(uint16_t index)
{
    DATE_TIME_T dt =Get_DateTime();
    module_info.dt[index].year = dt.year;
    module_info.dt[index].month = dt.month;
    module_info.dt[index].day = dt.day;
    module_info.dt[index].hour = dt.hour;
    module_info.dt[index].minute = dt.minute;
    module_info.dt[index].second = dt.second;
    
}


static void update_least_page_num2write(uint16_t index)
{
    module_info.flash_page_num[index] = Get_Page_Num2Write();
}    



bool Load_Module_Info(void)
{

    uint8_t * addr;                         // Initialize Flash pointer
    uint16_t payload_len;
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num  = NRF_FICR->CODESIZE - 9;       // Use last page in flash
    info_addr = (uint32_t *)(pg_size * pg_num);
    addr = (uint8_t *) (pg_size * pg_num);   
    payload_len = ((uint16_t)addr[3]<<8) + addr[2];
    
    if ((addr[0] == SB) && (addr[1] == INFO_CMD) &&
        (payload_len == PAYLOAD_LEN) && (addr[TOTAL_PKT_LEN-1] == EB))
    {
        memcpy((uint8_t *)&module_info, addr, TOTAL_PKT_LEN);  
        power_up_mode_idx = chk_power_up_mode_idx();    
        Update_DateTime(&(module_info.dt[power_up_mode_idx]));      
        Set_Page_Num2Write(module_info.flash_page_num[power_up_mode_idx]);
        return true;
    }
    else
    {
        load_module_default_setting();        
        store_module_info2flash();       
        //Update_DateTime(&(module_info.dt[power_up_mode_idx]));
        return false;
    }    
}


static uint16_t chk_power_up_mode_idx(void)
{
    uint16_t i=0;
    
    do
    {
        if (module_info.next_power_up_mode[i] == FFh)             
            break;
    }while(i++ < POWER_UP_MODE_BACKUP_SIZE-1);
            
    return (i == 0)? POWER_UP_MODE_BACKUP_SIZE: i-1; 
         
} 



static void load_module_default_setting(void)
{
    module_info.sb = SB;
    module_info.eb = EB;
    module_info.cmd = INFO_CMD;
    module_info.len = PAYLOAD_LEN;
    module_info.acc_sampling_rate = 1;          // unit: second
    module_info.uart_buarrate_mode = 1;         //  0:9600, 1:115200, 2:230400.
    memset(module_info.next_power_up_mode, 0xff, POWER_UP_MODE_BACKUP_SIZE);
    memset(module_info.dt, 0xff, sizeof(DATE_TIME_T)*POWER_UP_MODE_BACKUP_SIZE);    
    memset(module_info.flash_page_num, 0xff, POWER_UP_MODE_BACKUP_SIZE*4);    
    
    module_info.next_power_up_mode[0]= POWER_UP_INTO_RECORD_MODE;
    set_default_datetime();
    module_info.flash_page_num[0] = 0;
    power_up_mode_idx = POWER_UP_MODE_BACKUP_SIZE-1;
}



static void set_default_datetime(void)
{
    module_info.dt[0].year = 19;
    module_info.dt[0].month = 8;
    module_info.dt[0].day = 16;
    module_info.dt[0].hour = 0;
    module_info.dt[0].minute = 47;
    module_info.dt[0].second = 56;
}


POWER_UP_MODE_T Get_Power_Up_Mode(void)
{
    if (module_info.next_power_up_mode[power_up_mode_idx] == POWER_UP_MODE_NOT_DEFINE)
        return POWER_UP_INTO_RECORD_MODE;
    else           
        return module_info.next_power_up_mode[power_up_mode_idx];
}



void Flash_Access_Test(void)
{
    uint32_t * addr;

    uint8_t temp[]={1,2,3,4,5,6,7,8,9,10,11,12,13};
    uint8_t temp1[14];


    PRINT("Flashwrite example\r\n");
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num  = NRF_FICR->CODESIZE - 9;  // Use last page in flash
    
    // Start address:
    // addr = (pg_size * pg_num);
    addr = (uint32_t *)(pg_size * pg_num);

    
    write_seg(addr, temp, sizeof(temp), true);    
    addr = (uint32_t *)(pg_size * pg_num);
    memcpy(&temp1, addr, sizeof(temp1));
}




static void store_next_power_up_mode2flash(bool iserase)
{
   write_seg(info_addr, (uint8_t *)&module_info, sizeof(MODULE_INFO_T), iserase);
}


static void store_module_info2flash(void)
{
    write_seg(info_addr, (uint8_t *)&module_info, sizeof(MODULE_INFO_T), true); 
}


static void write_seg(uint32_t* addr, uint8_t* ptr_data, uint16_t len, bool is_erase)
{
    ret_code_t err_code;
    if (is_erase) flash_page_erase(addr);

    err_code = sd_flash_write(addr, (uint32_t *)&module_info, (uint32_t)sizeof(MODULE_INFO_T));
    nrf_delay_ms(100);

}


/** @brief Function for erasing a page in flash.
 *
 * @param page_address Address of the first word in the page to be erased.
 */
static void flash_page_erase(uint32_t * page_address)
{
    ret_code_t err_code;
 
    sd_flash_page_erase(pg_num);
    nrf_delay_ms(100);

		
}


/** @brief Function for filling a page in flash with a value.
 *
 * @param[in] address Address of the first word in the page to be filled.
 * @param[in] value Value to be written to flash.
 */
static void flash_word_write(uint32_t * address, uint32_t value)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    *address = value;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
