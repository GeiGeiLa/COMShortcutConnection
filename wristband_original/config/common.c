/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "common.h"


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

static MODULE_STAT_FLAG_T                  module_operate_stat;


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
uint8_t Chk_All_Sensor_Stat(void)
{
    if (IS_W25Qxx_INITIALIZED())
        sensor_init_stat += W25Qxx_INIT_SUCCESS ; 
    if (IS_SI705x_INITIALIZED())
        sensor_init_stat += SI705x_INIT_SUCCESS ;       
    if (IS_ACC_INITIALIZED())
        sensor_init_stat += ACC_INIT_SUCCESS;    
    if (IS_VDD_VOLT_CORRECT())
        sensor_init_stat += VDD_CHECK_SUCCESS ;    
    if (IS_BAT_VOLT_HIGH())
        sensor_init_stat += BAT_CHECK_SUCCESS;    
    if (sensor_init_stat == (W25Qxx_INIT_SUCCESS + 
                             SI705x_INIT_SUCCESS + 
                             ACC_INIT_SUCCESS + 
                             VDD_CHECK_SUCCESS + 
                             BAT_CHECK_SUCCESS))
        sensor_init_stat += ALLOW_EXEC; 
    sensor_init_stat += SENSOR_CHKECKED;
    return sensor_init_stat;
}



void Set_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd)
{
    module_operate_stat |= cmd;
}


uint8_t Chk_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd)
{
    return((module_operate_stat & cmd) == cmd);
}



uint8_t Get_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd)
{
    if ((module_operate_stat & cmd) == cmd)
    {
        Clear_Module_Operate_Flag(cmd);
        return 1;
    }
    else return 0;

}


void Clear_Module_Operate_Flag(MODULE_STAT_FLAG_T cmd)
{
    module_operate_stat &= ~cmd;
}


void Reset_All_Operate_Flag(void)
{
    module_operate_stat = FLAG_RESET;
}


