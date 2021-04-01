/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "w25qxx.h"




/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define PAGE_PROGRAM_TIME_MIN               1       // ms
#define SECTOR_ERASE_TIME_MIN               100     // ms
#define CHIP_ERASE_TIME_MIN                 10000   // ms

#define FLASH_CMD_TIMEOUT_THR               100


#define _FLASH_POOL_LEN                     256
#define _FLASH_POOL_LEN_BLE                 1024


/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
/* SPI instance. */
static const nrf_drv_spi_t w25qxx_spi = NRF_DRV_SPI_INSTANCE(2);  


/* Flag used to indicate that SPI instance completed the transfer. */
static volatile bool spi_xfer_done = true;  

/* ECG command Timer*/
static uint16_t                         _flash_cmd_timeout_thr = 0;
static uint16_t                         _flash_cmd_timer_cnt = 0;
static uint8_t                          _enable_flash_cmd_timer_flag = 0;


static uint8_t                          tx_buf[256]={0};
static uint8_t                          rx_buf[256]={0};
static w25qxx_t                         w25qxx;

static uint32_t write_page_num = 0; // Each page is 256 byte, each sector(4 k byte) are 16 pages.

static uint8_t ttx_buf[300];  
static uint8_t trx_buf[300]; 






/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void w25qxx_spi_init (void);
static bool read_w25qxx_spec(void);

static void wq25xx_write_enable(void);
static void wq25xx_write_disable(void);





static uint16_t w25qxx_read_device_id(void);
static uint32_t w25qxx_read_jedec_id(void);
static void w25qxx_read_unique_id(void);

static void w25qxx_wait4write_end(uint16_t waiting_ms);
static uint8_t w25qxx_read_regx(uint8_t cmd);
static uint8_t w25qxx_cmd_exe(uint8_t cmd);
static bool w25qxx_erase_sector(uint32_t sector_num);

static bool w25qxx_write_bytes(uint8_t *ptr_data, uint32_t addr, uint8_t num_byte2write);
static bool w25qxx_read_bytes(uint8_t *pBuffer, uint32_t addr, uint8_t num_byte2read);

//static void _disable_exg_cmd_timer(void);
static void _enable_flash_cmd_timer(uint16_t t);

/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
 /**
 * @brief SPI user event handler.
 * @param event
 */
void w25qxx_spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{   
    switch(p_event->type)
    {
       case NRF_DRV_SPI_EVENT_DONE:
            spi_xfer_done = true;           
            
        break;
        default: break;
    }
}
 


/*-----------------------------------------------------------------------------
 * Function : void SPORTER_Init(void)
 * Purpose : Initial procedure for SPORTER_Task
 * Parameters :
 * Input : none
 * Output : none
 * Returns : none
 * Note :
 * MODIFICTION HISTORY:
 * 2016/09/30: Initial version --- 
 *-----------------------------------------------------------------------------*/










/**@brief Function for count when transit command to ECG.
 *
 * @details This function is called from the Timer handler.
 *
 */
void Inc_W25QXX_Cmd_Timer(void)
{
    if ( _enable_flash_cmd_timer_flag == 1 )
    {
        if (_flash_cmd_timer_cnt++ > _flash_cmd_timeout_thr)
        {
            Set_Module_Operate_Flag(W25QXX_CMD_TIMEOUT_FLAG);
            _enable_flash_cmd_timer_flag = 0;
        }
    }
    
}

/* Not Used.
static void _disable_exg_cmd_timer(void)
{
    _enable_flash_cmd_timer_flag = 0;
    Clear_Module_Operate_Flag(ADS129x_CMD_TIMEOUT_FLAG);
}
*/


static void _enable_flash_cmd_timer(uint16_t t)
{
    _enable_flash_cmd_timer_flag = 1;
    _flash_cmd_timer_cnt = 0;
    _flash_cmd_timeout_thr = t;
    Clear_Module_Operate_Flag(W25QXX_CMD_TIMEOUT_FLAG);
}


static void w25qxx_write_disable(void)
{
    tx_buf[0] = WRITE_DISABLE;
    spi_xfer_done = false;    
    _enable_flash_cmd_timer(10);
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 1, rx_buf, 0);
    while (!spi_xfer_done)    
    {
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }      
}

static void wq25xx_write_enable(void)
{
    tx_buf[0] = WRITE_ENABLE;
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 1, rx_buf, 0);
    while (!spi_xfer_done)    
    {
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }      
}


void W25Qxx_Read_Status_Reg1(void)
{
    w25qxx.StatusRegister1 = w25qxx_read_regx(READ_STATUS_REG1);
}

void W25Qxx_Read_Status_Reg2(void)
{
    w25qxx.StatusRegister2 = w25qxx_read_regx(READ_STATUS_REG2);
}

void W25Qxx_Read_Status_Reg3(void)
{
    w25qxx.StatusRegister3 = w25qxx_read_regx(READ_STATUS_REG3);
}

static uint8_t w25qxx_read_regx(uint8_t cmd)
{
    tx_buf[0] = cmd;
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 1, rx_buf, 2);    // the 2 is cmd length + return byte length.
    while (!spi_xfer_done)    
    {
        __WFE();
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }
    return rx_buf[1];
}



static uint8_t w25qxx_cmd_exe(uint8_t cmd)
{
    tx_buf[0] = cmd;
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 1, rx_buf, 0);
    while (!spi_xfer_done)    
    {
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }
    return rx_buf[1];
}



static bool w25qxx_erase_sector(uint32_t sector_num)
{    
    uint8_t len = 0;
    uint32_t sector_addr = sector_num * w25qxx.SectorSize;
    
    memset(tx_buf, 0xFF, sizeof(tx_buf));
    
    tx_buf[len++] = SECTRO_ERASE;
    if (w25qxx.ID>=W25Q256)
        tx_buf[len++] = ((sector_addr & 0xFF000000) >> 24);
    tx_buf[len++] = ((sector_addr & 0xFF0000) >> 16);
    tx_buf[len++] = ((sector_addr & 0xFF00) >> 8);
    tx_buf[len++] = ((sector_addr & 0xFF) >> 0);
    
    // check length, total length must be small than 256 byte.
    if ((sector_addr > w25qxx.SectorCount))
        return false;
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, len, tx_buf, 0);  // the 5 is cmd length, the 8 is return 8 bytes.
    while (!spi_xfer_done)    
    {
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }      
    return true;
}




static uint16_t w25qxx_read_device_id(void)
{    
    tx_buf[0] = DEVICE_ID;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 4, rx_buf, 6);
    while (!spi_xfer_done)    
    {
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    } 
    return ( (rx_buf[4]<<8) | rx_buf[5]);
      
}


static void w25qxx_read_unique_id(void)
{    
    tx_buf[0] = UNIQUE_ID;
    tx_buf[1] = 0x00;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x00;
    tx_buf[4] = 0x00;
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 5, rx_buf, 5+8);
    while (!spi_xfer_done)    
    {
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    } 
    for(uint8_t i =0; i<8; i++)
        w25qxx.UniqID[i] = rx_buf[5+i];
      
}


static uint32_t w25qxx_read_jedec_id(void)
{

    tx_buf[0] = JEDEC_ID;//JEDEC_ID
    spi_xfer_done = false; 
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, 1, rx_buf, 4);
    while (!spi_xfer_done)    
    {
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }    
    return ( (rx_buf[1]<<16) | (rx_buf[2]<<8) | rx_buf[3]);  
}




static bool read_w25qxx_spec(void)
{    
    uint32_t    id;
    id = w25qxx_read_jedec_id();
    switch(id&0x0000FFFF)
    {
        case 0x401A:    //     w25q512
            w25qxx.ID=W25Q512;
            w25qxx.BlockCount=1024;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q512\r\n");
            #endif
        break;
        case 0x4019:    //     w25q256
            w25qxx.ID=W25Q256;
            w25qxx.BlockCount=512;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q256\r\n");
            #endif
        break;
        case 0x4018:    //     w25q128
            w25qxx.ID=W25Q128;
            w25qxx.BlockCount=256;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q128\r\n");
            #endif
        break;
        case 0x4017:    //    w25q64
            w25qxx.ID=W25Q64;
            w25qxx.BlockCount=128;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q64\r\n");
            #endif
        break;
        case 0x4016:    //    w25q32
            w25qxx.ID=W25Q32;
            w25qxx.BlockCount=64;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q32\r\n");
            #endif
        break;
        case 0x4015:    //    w25q16
            w25qxx.ID=W25Q16;
            w25qxx.BlockCount=32;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q16\r\n");
            #endif
        break;
        case 0x4014:    //    w25q80
            w25qxx.ID=W25Q80;
            w25qxx.BlockCount=16;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q80\r\n");
            #endif
        break;
        case 0x4013:    //    w25q40
            w25qxx.ID=W25Q40;
            w25qxx.BlockCount=8;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q40\r\n");
            #endif
        break;
        case 0x4012:    //    w25q20
            w25qxx.ID=W25Q20;
            w25qxx.BlockCount=4;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q20\r\n");
            #endif
        break;
        case 0x4011:    //    w25q10
            w25qxx.ID=W25Q10;
            w25qxx.BlockCount=2;
            #if (_W25QXX_DEBUG==1)
            PRINT("w25qxx Chip: w25q10\r\n");
            #endif
        break;
        default:
                #if (_W25QXX_DEBUG==1)
                PRINT("w25qxx Unknown ID\r\n");
                #endif
            w25qxx.Lock=0;    
            return false;                
    }      
    w25qxx.MaxPayloadLength = ((w25qxx.ID>=W25Q256))? 249: 250;
    w25qxx.PageSize=256;
    w25qxx.SectorSize=0x1000;
    w25qxx.SectorCount=w25qxx.BlockCount*16;
    w25qxx.PageCount=(w25qxx.SectorCount*w25qxx.SectorSize)/w25qxx.PageSize;
    w25qxx.BlockSize=w25qxx.SectorSize*16;
    w25qxx.CapacityInKiloByte=(w25qxx.SectorCount*w25qxx.SectorSize)/1024;   
    w25qxx_read_unique_id();
    W25Qxx_Read_Status_Reg1();
    W25Qxx_Read_Status_Reg2();
    W25Qxx_Read_Status_Reg3();
    return true;
}


static void w25qxx_wait4write_end(uint16_t waiting_ms)
{  
    do
    {
        nrf_delay_ms(waiting_ms);
        w25qxx.StatusRegister1 = w25qxx_read_regx(READ_STATUS_REG1);          
    }
    while ((w25qxx.StatusRegister1 & BIT_WRITE_ERASE_BUSY) == BIT_WRITE_ERASE_BUSY); 
}



void W25Qxx_Erase_Sector(uint32_t sector_num)
{

    #if (_W25QXX_DEBUG==1)
    uint32_t    StartTime=HAL_GetTick();    
    PRINT("w25qxx EraseChip Begin...\r\n");
    #endif
    wq25xx_write_enable();

    w25qxx_erase_sector(sector_num);

    w25qxx_wait4write_end(SECTOR_ERASE_TIME_MIN);
    #if (_W25QXX_DEBUG==1)
    PRINT("w25qxx EraseBlock done after %d ms!\r\n",HAL_GetTick()-StartTime);
    #endif    
    
}

void W25Qxx_Power_Down(void)
{
    w25qxx_cmd_exe(POWER_DOWN);
}

void W25Qxx_Release_Power_Down(void)
{
    w25qxx_cmd_exe(RELEASE_POWER_DOWN);
}

void W25Qxx_Erase_Entry_Chip(void)
{
#if 1
    #if (_W25QXX_DEBUG==1)
    uint32_t    StartTime=HAL_GetTick();    
    PRINT("w25qxx EraseChip Begin...\r\n");
    #endif
    wq25xx_write_enable();

    w25qxx_cmd_exe(CHIP_ERASE);

    w25qxx_wait4write_end(CHIP_ERASE_TIME_MIN);
    #if (_W25QXX_DEBUG==1)
    PRINT("w25qxx EraseBlock done after %d ms!\r\n",HAL_GetTick()-StartTime);
    #endif
#else   
    uint32_t sector_num=0;
    uint32_t page_num;
    static uint8_t xff_buff[256], rx_buf[256];
    memset(xff_buff, 0xff, sizeof(xff_buff));    
    REREASE: 
    do        
    {
        W25Qxx_Erase_Sector(sector_num);

        for (uint8_t page=0; page<16; page++)
        {
            page_num = sector_num * 16 + page;

            memset(trx_buf, 0x00, sizeof(rx_buf));
            W25Qxx_Read_Bytes(rx_buf, page_num*256, 256);
            if (memcmp(rx_buf,xff_buff,256 )!=0)
            {
                //FLASH_PW_OFF();                
                //FLASH_PW_ON();
                //nrf_delay_ms(5);
                printf("sector erase fail: %d\n\r", sector_num);
                goto REREASE;
            }
            
        }
    }
    while (sector_num++<w25qxx.SectorCount);
#endif  
}



static bool w25qxx_write_bytes(uint8_t *ptr_data, uint32_t addr, uint8_t num_byte2write)
{    
    uint8_t len = 0;
    memset(tx_buf, 0xFF, sizeof(tx_buf));
    tx_buf[len++] = PAGE_PROGRAM;
    if (w25qxx.ID>=W25Q256)
        tx_buf[len++] = ((addr & 0xFF000000) >> 24);
    tx_buf[len++] = ((addr & 0xFF0000) >> 16);
    tx_buf[len++] = ((addr & 0xFF00) >> 8);
    tx_buf[len++] = ((addr & 0xFF) >> 0);
    
    // check length, total length must be small than 256 byte.
    if ((num_byte2write > w25qxx.MaxPayloadLength) || (num_byte2write==0))
        return false;
    
    memcpy(&tx_buf[len], ptr_data, num_byte2write);
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, len + num_byte2write, NULL, 0);  // the 5 is cmd length, the 8 is return 8 bytes.
    while (!spi_xfer_done)    
    {
        __WFE();
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }      
    return true;
}


/*
bool W25Qxx_Write_Bytes(uint8_t *pBuffer, uint32_t write_addr, uint32_t num_byte2write)
{
    bool stat;
    #if (_W25QXX_DEBUG==1)
    uint32_t    StartTime=HAL_GetTick();
    PRINT("w25qxx WriteByte 0x%02X at address %d begin...",pBuffer,write_addr);
    #endif
    w25qxx_wait4write_end(5);
    wq25xx_write_enable();
    
    stat = w25qxx_write_bytes(pBuffer, write_addr, num_byte2write);

    w25qxx_wait4write_end(5);
    #if (_W25QXX_DEBUG==1)
    PRINT("w25qxx WriteByte done after %d ms\r\n",HAL_GetTick()-StartTime);
    #endif
    return stat;
}
*/



bool W25Qxx_Write_Bytes(uint8_t *pBuffer, uint32_t write_addr, uint32_t num_byte2write)
{
    bool stat;
    #if (_W25QXX_DEBUG==1)
    uint32_t    StartTime=HAL_GetTick();
    PRINT("w25qxx WriteByte 0x%02X at address %d begin...",pBuffer,write_addr);
    #endif
    w25qxx_wait4write_end(PAGE_PROGRAM_TIME_MIN);    
    
    if  (num_byte2write <= w25qxx.MaxPayloadLength) 
    {
        wq25xx_write_enable();
        stat = w25qxx_write_bytes(pBuffer, write_addr, num_byte2write);
        w25qxx_wait4write_end(PAGE_PROGRAM_TIME_MIN);
    }
    else
    {
        uint16_t total_page = num_byte2write / w25qxx.MaxPayloadLength;
        uint8_t rem_byte = num_byte2write % w25qxx.MaxPayloadLength;
        uint8_t bytes2write;
        total_page = (rem_byte>0) ? total_page + 1: total_page;
        for (uint8_t i=0; i<total_page; i++)
        {
            wq25xx_write_enable();
            bytes2write = (i < (total_page-1)) ? w25qxx.MaxPayloadLength : rem_byte;
                
            stat = w25qxx_write_bytes(pBuffer + w25qxx.MaxPayloadLength*i, write_addr+w25qxx.MaxPayloadLength*i, bytes2write);
            w25qxx_wait4write_end(PAGE_PROGRAM_TIME_MIN);
        }
    }
    
    #if (_W25QXX_DEBUG==1)
    PRINT("w25qxx WriteByte done after %d ms\r\n",HAL_GetTick()-StartTime);
    #endif
    return stat;
}


static bool w25qxx_read_bytes(uint8_t *pBuffer, uint32_t addr, uint8_t num_byte2read)
{    
    uint8_t len = 0;
    memset(tx_buf, 0xFF, sizeof(tx_buf));
    memset(rx_buf, 0xFF, sizeof(rx_buf));
    if (w25qxx.ID>=W25Q256)
        tx_buf[len++] = ((addr & 0xFF000000) >> 24);
    tx_buf[len++] = FASE_READ;
    tx_buf[len++] = ((addr & 0xFF0000) >> 16);
    tx_buf[len++] = ((addr & 0xFF00) >> 8);
    tx_buf[len++] = ((addr & 0xFF) >> 0);
    tx_buf[len++] = DUMMY;
    
    // check length, total length must be small than 256 byte.
    if ((num_byte2read > w25qxx.MaxPayloadLength) || (num_byte2read==0))
        return false; 
    spi_xfer_done = false;
    _enable_flash_cmd_timer(10);    
    nrf_drv_spi_transfer(&w25qxx_spi, tx_buf, len, rx_buf, len + num_byte2read);  // the 5 is cmd length, the 1 is return 1 bytes.
    while (!spi_xfer_done)    
    {
        __WFE();
        nrf_delay_ms(1);
        if (IS_W25QXX_CMD_TIMEOUT())
            break;
    }    
    memcpy(pBuffer, &rx_buf[len], num_byte2read); 
    return true;
}


/*
bool W25Qxx_Read_Bytes(uint8_t *pBuffer, uint32_t read_addr, uint32_t num_byte2read)
{
    bool stat;
    #if (_W25QXX_DEBUG==1)
    uint32_t    StartTime=HAL_GetTick();
    PRINT("w25qxx ReadByte at address %d begin...\r\n",Bytes_Address);
    #endif
    
    stat = w25qxx_read_bytes(pBuffer, read_addr, num_byte2read);
    
    #if (_W25QXX_DEBUG==1)
    PRINT("w25qxx ReadByte 0x%02X done after %d ms\r\n",*pBuffer,HAL_GetTick()-StartTime);
    #endif

    return  stat;
}
*/


bool W25Qxx_Read_Bytes(uint8_t *pBuffer, uint32_t read_addr, uint32_t num_byte2read)
{
    bool stat;
    #if (_W25QXX_DEBUG==1)
    uint32_t    StartTime=HAL_GetTick();
    PRINT("w25qxx ReadByte at address %d begin...\r\n",Bytes_Address);
    #endif
    
    if  (num_byte2read <= w25qxx.MaxPayloadLength) 
    {
        stat = w25qxx_read_bytes(pBuffer, read_addr, num_byte2read);
    }
    else
    {
        uint16_t total_page = num_byte2read / w25qxx.MaxPayloadLength;
        uint8_t rem_byte = num_byte2read % w25qxx.MaxPayloadLength;
        uint8_t bytes2read;
        total_page = (rem_byte>0) ? total_page + 1: total_page;
        for (uint8_t i=0; i<total_page; i++)
        {
            bytes2read = (i < (total_page-1)) ? w25qxx.MaxPayloadLength : rem_byte;
                
            stat = w25qxx_read_bytes(pBuffer + w25qxx.MaxPayloadLength*i, read_addr+w25qxx.MaxPayloadLength*i, bytes2read);

        }
    }
    
    
    #if (_W25QXX_DEBUG==1)
    PRINT("w25qxx ReadByte 0x%02X done after %d ms\r\n",*pBuffer,HAL_GetTick()-StartTime);
    #endif

    return  stat;
}


static void generator_test_patten(void)
{
    uint8_t bias = 0x00;
    for(uint32_t i=0; i<sizeof(trx_buf); i++) 
        ttx_buf[i]=i+bias;
    memset(trx_buf, 0x00, sizeof(trx_buf));
}


void W25Qxx_Func_Test(void)
{
    generator_test_patten();
    static uint32_t bias = sizeof(tx_buf) *2;
    //W25Qxx_Erase_Entry_Chip();    
    W25Qxx_Erase_Sector(0);
    W25Qxx_Write_Bytes(ttx_buf, 0x000000+bias, 256);
    W25Qxx_Write_Bytes(ttx_buf+256, 0x000000+bias+bias, 300-256);
    
    W25Qxx_Read_Bytes(trx_buf,0x000000+bias, 256);
    W25Qxx_Read_Bytes(trx_buf+256,0x000000+bias+bias, 300-256);

}


void Write_Data_2Flash(void)
{
  
    static RECORD_DATA_QUEUE_T *ptr_data_queue;        
    uint32_t write_addr = write_page_num * 256;
    
    if (write_page_num < w25qxx.PageCount)
    {
        ptr_data_queue = Get_Record_Data_Queue_Pointer();
        W25Qxx_Write_Bytes((uint8_t*)(ptr_data_queue), write_addr, 256);
        memset(trx_buf, 0x00, sizeof(trx_buf));
        W25Qxx_Read_Bytes(trx_buf, write_addr, 256);
        if (memcmp(trx_buf,(uint8_t*)(ptr_data_queue),256 )==0)
        {
            PRINT("==> Page %5d writed\n", write_page_num);
            write_page_num = write_page_num + 1;
        }
        else
        {
            Set_Module_Operate_Flag(FLASH_RAM_WRITE_ERROR_FLAG);            
            PRINT("==> Page %5d write error.\n", write_page_num);        
        }
        
    }
    else if (write_page_num >= w25qxx.PageCount)
    {
        PRINT("==> Flash is Full");
        write_page_num = w25qxx.PageCount;
        Set_Module_Operate_Flag(FLASH_RAM_FULL_FLAG);
    }        
} 



int32_t W25Qxx_Check_Content(void)
{       
    static uint8_t xff_buff[256];
    memset(xff_buff, 0xff, sizeof(xff_buff));
    for(uint32_t page_num = 0; page_num< w25qxx.PageCount; page_num++)
    {
        memset(trx_buf, 0x00, sizeof(trx_buf));
        W25Qxx_Read_Bytes(trx_buf, page_num*256, 256);
        if (memcmp(trx_buf,xff_buff,256 )!=0)
        {
            Set_Module_Operate_Flag(FLASH_RAM_WRITE_ERROR_FLAG);
            return page_num;
        }
    }   
    return -1;
}
    



bool Read_W25Qxx_Page(uint32_t page_num)
{
    if (page_num >= w25qxx.PageCount)
        return false;
    uint32_t page_addr = page_num*256;
    if (W25Qxx_Read_Bytes(trx_buf, page_addr, 256))
    {

        return true;
    }
    else
        return false;
}


uint8_t *Get_Read_Buff_Ptr(void)
{
   return trx_buf;
}


void Reset_Page_Num(void)
{
    write_page_num = 0;
}

void Set_Page_Num2Write(uint32_t num)
{
    write_page_num = num;
}

uint32_t Get_Page_Num2Write(void)
{
    return write_page_num;
}




 /**
 * @brief SPI initialization.
 */
static void w25qxx_spi_init (void)
{
    nrf_drv_spi_config_t w25qxx_spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    w25qxx_spi_config.sck_pin = W25QXX_CLK_PIN;
    w25qxx_spi_config.mosi_pin = W25QXX_DIN_PIN;
    w25qxx_spi_config.miso_pin = W25QXX_DOUT_PIN;
    w25qxx_spi_config.ss_pin = W25QXX_nCS_PIN;
    w25qxx_spi_config.frequency = NRF_DRV_SPI_FREQ_1M;  
    w25qxx_spi_config.mode = NRF_DRV_SPI_MODE_0;   
    w25qxx_spi_config.bit_order= NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    APP_ERROR_CHECK(nrf_drv_spi_init(&w25qxx_spi, &w25qxx_spi_config, w25qxx_spi_event_handler,NULL));

//    nrf_gpio_cfg(w25qxx_spi_config.miso_pin, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);    
//    nrf_gpio_cfg(w25qxx_spi_config.sck_pin, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);    
//    nrf_gpio_cfg(w25qxx_spi_config.mosi_pin, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);    
//    nrf_gpio_cfg(w25qxx_spi_config.ss_pin, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_CONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_H0H1, NRF_GPIO_PIN_NOSENSE);    
    
}



void W25Qxx_UnInit(void)
{
    nrf_drv_spi_uninit(&w25qxx_spi);
}


bool W25Qxx_Init(void)
{
    w25qxx_spi_init();
    
    return read_w25qxx_spec();
}


