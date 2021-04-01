#ifndef DATA_COLLECT_H_
#define DATA_COLLECT_H_

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "common.h"
#include "string.h"



/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
#define ACC_DATA_LENGTH             40
#define SEGMENT_COUNT               1

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;    
}ACC_T;


typedef struct
{
    uint32_t datetime;
    int16_t temp;  
    uint16_t lux;
    ACC_T   acc[ACC_DATA_LENGTH];
}SENSOR_DATA_T;


typedef struct
{
    SENSOR_DATA_T   sensor_data[SEGMENT_COUNT];
    
}RECORD_DATA_QUEUE_T;




/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  Global Variable                                         */
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
void Set_Latest_Temp(int16_t temp);
void Set_Latest_Light_Lux(uint16_t lux);
void Set_Latest_ACC(int16_t x, int16_t y, int16_t z);
void Update_Record_Data(uint32_t datetime);
void Update_ACC_Data(void);
void Update_Next_Segment(void);
void Init_Sensor_Data_Buff_Param(void);
RECORD_DATA_QUEUE_T *Get_Record_Data_Queue_Pointer(void);
#endif
