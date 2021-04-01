#ifndef W25QXX_H__
#define W25QXX_H__

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/

#include <string.h>

#include "common.h"

// Driver
#include "nrf.h"
//#include "nrf_spi.h"
//#include "nrf_spim.h"

#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"


// Library
#include "app_error.h"
#include "app_util_platform.h"

// module 
#include "data_collect.h"
#include "raytac52.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define _W25QXX_DEBUG                   0


/* SPI Interface pins defineition*/
#define W25QXX_DIN_PIN                 16    //
#define W25QXX_CLK_PIN                 15    //
#define W25QXX_DOUT_PIN                14    //
#define W25QXX_nCS_PIN                 12    //




/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
typedef enum
{
    W25Q10=1,
    W25Q20,
    W25Q40,
    W25Q80,
    W25Q16,
    W25Q32,
    W25Q64,
    W25Q128,
    W25Q256,
    W25Q512,
    
}W25QXX_ID_t;

typedef struct
{
    W25QXX_ID_t     ID;
    uint8_t         UniqID[8];

    uint16_t        PageSize;
    uint32_t        PageCount;
    uint32_t        SectorSize;
    uint32_t        SectorCount;
    uint32_t        BlockSize;
    uint32_t        BlockCount;
    uint8_t         MaxPayloadLength;

    uint32_t        CapacityInKiloByte;
    
    uint8_t         StatusRegister1;
    uint8_t         StatusRegister2;
    uint8_t         StatusRegister3;
    
    uint8_t         Lock;
    
}w25qxx_t;

/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/
#define DUMMY                           0x5A
#define READ_STATUS_REG1                0x05
#define READ_STATUS_REG2                0x35
#define READ_STATUS_REG3                0x15
#define WRITE_ENABLE                    0x06
#define WRITE_DISABLE                   0x04
#define DEVICE_ID                       0x90
#define UNIQUE_ID                       0x4B
#define JEDEC_ID                        0x9F
#define CHIP_ERASE                      0xC7
#define SECTRO_ERASE                    0x20

#define PAGE_PROGRAM                    0x02 
#define FASE_READ                       0x0B

#define POWER_DOWN                      0xB9
#define RELEASE_POWER_DOWN              0xAB

#define BIT_WRITE_ERASE_BUSY            1<<0
/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
/**@brief Function for ADC Config.
 *
 * @details This function will get the next byte from the RX buffer. If the RX buffer is empty
 *          an error code will be returned and the app_uart module will generate an event upon
 *          reception of the first byte which is added to the RX buffer.
 *
 * @param[out] p_byte    Pointer to an address where next byte received on the UART will be copied.
 *
 * @retval NRF_SUCCESS          If a byte has been received and pushed to the pointer provided.
 * @retval NRF_ERROR_NOT_FOUND  If no byte is available in the RX buffer of the app_uart module.
 */



 /*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/

bool W25Qxx_Init(void);
void W25Qxx_UnInit(void);
void Reset_Page_Num(void);
uint32_t Get_Page_Num2Write(void);
void Set_Page_Num2Write(uint32_t num);
void W25Qxx_Power_Down(void);
void W25Qxx_Release_Power_Down(void);
void Inc_W25QXX_Cmd_Timer(void);
void W25Qxx_Read_DEVICE_ID(void);
void W25Qxx_Read_JEDEC_ID(void);
void W25Qxx_Read_Status_Reg1(void);
void W25Qxx_Read_Status_Reg2(void);
void W25Qxx_Read_Status_Reg3(void);
void W25Qxx_Erase_Entry_Chip(void);
void W25Qxx_Erase_Sector(uint32_t sector_num);
bool W25Qxx_Write_Bytes(uint8_t *pBuffer, uint32_t write_addr, uint32_t num_byte2write);
bool W25Qxx_Read_Bytes(uint8_t *pBuffer,uint32_t read_addr, uint32_t num_byte2read);
void Write_Data_2Flash(void);
void W25Qxx_Func_Test(void);
uint32_t Get_Page_Count(void);
bool Read_W25Qxx_Page(uint32_t page_num);
uint8_t *Get_Read_Buff_Ptr(void);
int32_t W25Qxx_Check_Content(void);
#endif

