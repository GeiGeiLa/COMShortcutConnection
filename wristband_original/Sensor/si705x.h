#ifndef SI705x_H__
#define SI705x_H__

/*----------------------------------------------------------*/
/*  Include                                                  */
/*----------------------------------------------------------*/
#include "common.h"
#include "twix.h"
#include "uart.h"

/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/
/*Pins to connect shield. */



#define SI705x_ADDR                     (0x40>>0)

#define SI705x_MEASTEMP_HOLD_CMD        0xE3
#define SI705x_MEASTEMP_NOHOLD_CMD      0xF3
#define SI705x_RESET_CMD                0xFE
#define SI705x_WRITE_REG_CMD            0xE6
#define SI705x_READ_REG_CMD             0xE7
#define SI705x_ID1_CMD                  0xFA0F
#define SI705x_ID2_CMD                  0xFCC9
#define SI705x_FIRMVERS_CMD             0x84B8


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

void Si705x_Read(uint8_t cmd);
float Get_Temperature(void);

void Test_Si705x(void);
float Si705x_Read_Temp(void);

#endif
