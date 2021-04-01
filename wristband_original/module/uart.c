/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "uart.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define UART_TX_BUF_SIZE                    512    /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                    64      /**< UART RX buffer size. */

#define RX_BUFFLEN                          64
#define CMD_SAY_HELLO												0x00
#define CMD_UPDATE_DATETIME                 0x01
#define CMD_REPOWER_UP_INTO_RECORD_MODE     0x02
#define CMD_W25QXX_ERASR_ENTRY_CHIP         0x03
#define CMD_GET_W25QXX_HAVE_WRITEN_PAGE     0x04
#define CMD_GET_W25QXX_PAGE_DATA            0x05
#define CMD_ACK_INTO_UART_MODE              0x06

#define CMD_READ_SYSTEM_INFO                0x06

#define W25QXX_NOT_INITIAL                  0x00
#define W25QXX_ERASEING                     0x01
#define W25QXX_ERASE_DONE                   0x02
#define W25QXX_ERASE_ERROR                  0x03

#define START_BYTE                          0x55
#define END_BYTE                            0xAA

#define START_BYTE_OFFSET                   0
#define CMD_OFFSET                          1
#define LEN_OFFSET                          2
#define PAYLOAD_OFFSET                      3
#define END_BYTE_OFFSET                     rx_idx-1
#define MAX_PAYLOAD_LEN                     24
#define WOPAYLOAD_LEN                       4           // without payload length.
#define SUCCESS                             0x01
#define TIME_FORMAT_ERROR                   0x00


#define PAGE_READ_ERROR                     0xFF



    
/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
static uint8_t rx_timecnt;
static uint8_t rx_idx;
static uint8_t rx_buff[RX_BUFFLEN];
static uint8_t cmd_queue[RX_BUFFLEN];

static uint8_t rcv_cmd_flag = 0;

/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
static void data_reciver(uint8_t data);
static void packet_verify(void);
static void update_datetime_proc(uint8_t *ptr_cmd);
static void ack_update_datetime_status(uint8_t stat);
static void repower_up_into_record_mode_proc(uint8_t *ptr_cmd);
static void ack_repower_up_mode_status(uint8_t mode);
static void w25qxx_erasr_entry_chip_proc(uint8_t *ptr_cmd);
static void ack_w25qxx_erase_status(uint8_t stat);
static void get_w25qxx_have_written_page_proc(uint8_t *ptr_cmd);
static void ack_total_have_written_page(void);
static void get_w25qxx_page_data(uint8_t *ptr_cmd);
static void send_page_content(uint8_t *ptr_page_content);
static void ack_page_read_stat(uint8_t stat);
static void say_55012AA(void);

/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/

void uart_error_handle(app_uart_evt_t * p_event)
{
    uint8_t cr;
    if (p_event->evt_type == APP_UART_DATA_READY)
    {
        app_uart_get(&cr);
        data_reciver(cr);
    }
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


void BLE_Data_Reciver(uint8_t data)
{
    data_reciver(data);
}

static void data_reciver(uint8_t data)
{
    uint8_t len;
    rx_buff[rx_idx++] = data;
    if((rx_buff[0] != START_BYTE) || (rx_idx == RX_BUFFLEN))
    {
        rx_idx = 0;
        return;
    }

    if(rx_idx < 3)
        return;

    
    len = rx_buff[LEN_OFFSET];
        
    //if (rx_idx == 0x0A)    
//return;

    if(rx_idx == (len + WOPAYLOAD_LEN))
    {
        packet_verify();
        if (rcv_cmd_flag)
        rx_idx = 0;
    }
}


static void packet_verify(void)
{
    uint8_t len = rx_buff[LEN_OFFSET];
    
    if((rx_buff[START_BYTE_OFFSET] == START_BYTE) && (rx_buff[END_BYTE_OFFSET] == END_BYTE))
    {
        memcpy(&cmd_queue, &rx_buff, len + WOPAYLOAD_LEN);
        rcv_cmd_flag = 1;
    }
}

// Katsmin
// Modify command here!
void CMD_Decoder(void)
{
    switch (cmd_queue[CMD_OFFSET])
    {
			  case CMD_SAY_HELLO:
						say_55012AA();
						break;
        case CMD_UPDATE_DATETIME:
						update_datetime_proc(cmd_queue);
						break;
             
        case CMD_REPOWER_UP_INTO_RECORD_MODE:
            repower_up_into_record_mode_proc(cmd_queue);            
            break;

        case CMD_W25QXX_ERASR_ENTRY_CHIP:
            w25qxx_erasr_entry_chip_proc(cmd_queue);           
            break;
        
        case CMD_GET_W25QXX_HAVE_WRITEN_PAGE:
            get_w25qxx_have_written_page_proc(cmd_queue);  
            break;
        
        case CMD_GET_W25QXX_PAGE_DATA:
            get_w25qxx_page_data(cmd_queue);
            break;
            
        default:break;
    }
    rcv_cmd_flag = 0;
}

static void say_55012AA()
{
	  app_uart_put(START_BYTE);
    app_uart_put(0x00);
		app_uart_put(0x01);
		app_uart_put(0x02);
    app_uart_put(END_BYTE);
}
static void update_datetime_proc(uint8_t *ptr_cmd)
{
    if (ptr_cmd[LEN_OFFSET] ==0x06)
    {
        if (Update_DateTime((DATE_TIME_T *)(ptr_cmd+PAYLOAD_OFFSET)))
        {
            ack_update_datetime_status(SUCCESS);
            Set_Next_Power_Up_Mode(POWER_UP_INTO_RECORD_MODE);                
            sd_nvic_SystemReset();
        }
    }
    else
    {
        ack_update_datetime_status(TIME_FORMAT_ERROR);
    }
    
}

static void ack_update_datetime_status(uint8_t stat)
{
    uint8_t tx_buf[5]={START_BYTE, CMD_UPDATE_DATETIME, 0x01, stat, END_BYTE};
    for (uint8_t i=0; i<sizeof(tx_buf); i++)
        app_uart_put(tx_buf[i]);
    nrf_delay_ms(100);
}




static void repower_up_into_record_mode_proc(uint8_t *ptr_cmd)
{
    if (ptr_cmd[LEN_OFFSET] ==0x01)
    {
        POWER_UP_MODE_T power_up_mode = (POWER_UP_MODE_T)ptr_cmd[PAYLOAD_OFFSET];
        switch ((power_up_mode))
        {            
            case POWER_UP_INTO_RECORD_MODE:
                Set_Next_Power_Up_Mode(POWER_UP_INTO_RECORD_MODE);
                ack_repower_up_mode_status(W25QXX_ERASEING);
                nrf_delay_ms(100);
                sd_nvic_SystemReset();
            break;
            case POWER_UP_INTO_RECOED_WITH_BLE_MODE: 
                Set_Next_Power_Up_Mode(POWER_UP_INTO_RECOED_WITH_BLE_MODE); 
                ack_repower_up_mode_status(W25QXX_ERASEING);
                nrf_delay_ms(100);
                sd_nvic_SystemReset();
            break;
            default:break;
        }                   
    }
}


static void ack_repower_up_mode_status(uint8_t mode)
{
    uint8_t tx_buf[4]={START_BYTE, CMD_REPOWER_UP_INTO_RECORD_MODE, 0x00, END_BYTE};

    for (uint8_t i=0; i<sizeof(tx_buf); i++)
        app_uart_put(tx_buf[i]);
}



static void w25qxx_erasr_entry_chip_proc(uint8_t *ptr_cmd)
{

    if (ptr_cmd[LEN_OFFSET] ==0x02 && ptr_cmd[PAYLOAD_OFFSET]==0x00 && ptr_cmd[PAYLOAD_OFFSET+1]==0xFF)
    {
        Set_Module_Operate_Flag(NOTIFY_W25QXX_FLASH_ERASR_ALL_FLAG);
        if(IS_W25Qxx_INITIALIZED())
        {
            PRINT("Flash erase...\n");      
            ack_w25qxx_erase_status(W25QXX_ERASEING);
            W25Qxx_Erase_Entry_Chip();
            
            if (W25Qxx_Check_Content()==-1)
                ack_w25qxx_erase_status(W25QXX_ERASE_DONE);
            else
                ack_w25qxx_erase_status(W25QXX_ERASE_ERROR);
            
            Reset_Page_Num();
            PRINT("Done\n");
        }
        else    
        {
            ack_w25qxx_erase_status(W25QXX_NOT_INITIAL);
            PRINT("W25Qxx NOT initialized.\n");
        }
    }
}


static void ack_w25qxx_erase_status(uint8_t stat)
{
    uint8_t tx_buf[5]={START_BYTE, CMD_W25QXX_ERASR_ENTRY_CHIP, 0x01, stat, END_BYTE};

    for (uint8_t i=0; i<sizeof(tx_buf); i++)
        app_uart_put(tx_buf[i]);
}


static void get_w25qxx_have_written_page_proc(uint8_t *ptr_cmd)
{
    if (ptr_cmd[LEN_OFFSET] ==0x00)
    {        
        ack_total_have_written_page();
    }
    
}


static void ack_total_have_written_page(void)
{
    uint32_t total_have_written_page = Get_Page_Num2Write();
    uint8_t *ptr_page_num;
    ptr_page_num = (uint8_t *)(&total_have_written_page);    
    app_uart_put(START_BYTE);
    app_uart_put(CMD_GET_W25QXX_HAVE_WRITEN_PAGE);
    app_uart_put(0x04);  
    for (uint8_t i=0; i<4; i++)
        app_uart_put(*(ptr_page_num+i));
    app_uart_put(END_BYTE);
}


static void get_w25qxx_page_data(uint8_t *ptr_cmd)
{
    static uint32_t page_num;
    
    if (ptr_cmd[LEN_OFFSET] ==0x04)
    {
        if (IS_W25Qxx_INITIALIZED())
        {            
            page_num =  ((uint32_t)ptr_cmd[PAYLOAD_OFFSET +0] <<24) + 
                        ((uint32_t)ptr_cmd[PAYLOAD_OFFSET +1] <<16) + 
                        ((uint32_t)ptr_cmd[PAYLOAD_OFFSET +2] <<8) + 
                        ((uint32_t)ptr_cmd[PAYLOAD_OFFSET +3] <<0);
            if (Read_W25Qxx_Page(page_num))
            {                
                send_page_content(Get_Read_Buff_Ptr());
            }
            else
                ack_page_read_stat(PAGE_READ_ERROR);
        }
        else
            ack_page_read_stat(W25QXX_NOT_INITIAL);
    }
}


static void send_page_content(uint8_t *ptr_page_content)
{
    uint16_t checksum=0;
    app_uart_put(START_BYTE);
    app_uart_put(CMD_GET_W25QXX_PAGE_DATA);
    app_uart_put(0xFF);  
    for (uint16_t i=0; i<256; i++)
    {
        app_uart_put(ptr_page_content[i]);
        checksum+=ptr_page_content[i];
    }
    app_uart_put(0xff & checksum);  
    app_uart_put(0xff & (checksum>>8));  
    app_uart_put(END_BYTE);    
}


static void ack_page_read_stat(uint8_t stat)
{
    uint8_t tx_buf[5]={START_BYTE, CMD_GET_W25QXX_PAGE_DATA, 0x01, stat, END_BYTE};
    for (uint8_t i=0; i<sizeof(tx_buf); i++)
        app_uart_put(tx_buf[i]);    
}


void Ack_Into_UART_mode(void)
{
    uint8_t tx_buf[4]={START_BYTE, CMD_ACK_INTO_UART_MODE, 0x00, END_BYTE};

    for (uint8_t i=0; i<sizeof(tx_buf); i++)
        app_uart_put(tx_buf[i]);
}


bool Is_Rcv_CMD(void)
{
    return rcv_cmd_flag;
}


void UART_Rcv_Byte_Timer(void)
{
    if(rx_idx)
    {
        if(++rx_timecnt >= 5)
           rx_idx = 0;    // Reset RX Idx
    }
}



void Init_UART(uint32_t tx_pin, uint32_t rx_pin)
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
      {
          rx_pin,
          tx_pin,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_DISABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud115200
          //UART_BAUDRATE_BAUDRATE_Baud230400
          //UART_BAUDRATE_BAUDRATE_Baud460800
      };

    
      
    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOW,
                         err_code);

    APP_ERROR_CHECK(err_code);
    /* for UART Bug, reference: "nRF52832 UART Tx Pin Floating" on Nordic Developer Zone. */
    //nrf_gpio_cfg( TX_PIN_NUMBER, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
}

