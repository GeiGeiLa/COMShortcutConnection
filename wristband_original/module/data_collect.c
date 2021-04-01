/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "data_collect.h"


/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/
static int16_t              latest_temp;  
static uint16_t             latest_lux;
static ACC_T                latest_acc;
static uint8_t              segment_index = 0;
static uint8_t              queue_index = 0;
static uint8_t              acc_index = 0;
static RECORD_DATA_QUEUE_T   record_data_queue[2];
static RECORD_DATA_QUEUE_T   *ptr_record_data_2flash;

/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/




/*----------------------------------------------------------*/
/* Inner Function Prototypes                                */
/*----------------------------------------------------------*/
void Set_Latest_Temp(int16_t temp)    
{
    latest_temp = temp;
    record_data_queue[queue_index].sensor_data[segment_index].temp = latest_temp;
}

void Set_Latest_Light_Lux(uint16_t lux)    
{
    latest_lux = lux;
    record_data_queue[queue_index].sensor_data[segment_index].lux = latest_lux;
}

void Set_Latest_ACC(int16_t x, int16_t y, int16_t z)    
{
    latest_acc.x = x;
    latest_acc.y = y;
    latest_acc.z = z;
}

void Update_Next_Segment(void)
{
    if (++segment_index >= SEGMENT_COUNT)
    {
        segment_index = 0;    
        ptr_record_data_2flash = &(record_data_queue[queue_index]);
        Set_Module_Operate_Flag(SENSOR_DATA_READY_2WRITE_FLAG);
        if (++queue_index >= 2)
        {
            queue_index = 0;
        }        
    }
}


void Update_ACC_Data(void)
{
    record_data_queue[queue_index].sensor_data[segment_index].acc[acc_index] = latest_acc;
    if (++acc_index >= ACC_DATA_LENGTH)
        acc_index = 0;       
}


void Update_Record_Data(uint32_t datetime)
{
    record_data_queue[queue_index].sensor_data[segment_index].datetime = datetime;
    record_data_queue[queue_index].sensor_data[segment_index].temp = latest_temp;
    record_data_queue[queue_index].sensor_data[segment_index].lux = latest_lux;
    acc_index = 0;
}


void Set_Temperature(void)
{
    record_data_queue[queue_index].sensor_data[segment_index].temp = latest_temp;
}

void Set_Light_Lux(void)
{
    record_data_queue[queue_index].sensor_data[segment_index].lux = latest_lux;
}


void Init_Sensor_Data_Buff_Param(void)
{
    memset(&record_data_queue, 0x00, sizeof(record_data_queue));
    queue_index = 0;
    segment_index = 0;
    acc_index = 0;
}

RECORD_DATA_QUEUE_T *Get_Record_Data_Queue_Pointer(void)
{
    return ptr_record_data_2flash;
}

