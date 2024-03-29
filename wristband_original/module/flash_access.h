#ifndef FLASH_ACCESS_H_
#define FLASH_ACCESS_H_

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
#include "uart.h"
#include "app_config.h"

#include "timer.h"
#include "w25qxx.h"
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


/*----------------------------------------------------------*/
/* Function Prototypes                                      */
/*----------------------------------------------------------*/
void Flash_Access_Test(void);
bool Load_Module_Info(void);
//PM25_INFO_T *Get_PM25_Info_Pointer(void);

POWER_UP_MODE_T Get_Power_Up_Mode(void);
void Set_Next_Power_Up_Mode(POWER_UP_MODE_T mode);

#endif

