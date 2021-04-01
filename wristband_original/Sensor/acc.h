#ifndef ACC_H__
#define ACC_H__

/*----------------------------------------------------------*/
/*  Include                                                 */
/*----------------------------------------------------------*/
//#include <string.h>
#include <math.h>
#include "common.h"
#include "twix.h"
#include "data_collect.h"

// Driver
#include "nrf_drv_gpiote.h"

// Module
#include "timer.h"





/*----------------------------------------------------------*/
/*  Define                                                  */
/*----------------------------------------------------------*/



/*Common addresses definition for accelereomter. */
#define LIS2DH12_ADDR                   (0x30>>1) // 0011000xb

#define LIS2DH12_STATUS_REG_AUX         0x07
#define LIS2DH12_OUT_TEMP_L             0x0C
#define LIS2DH12_OUT_TEMP_H             0x0D
#define LIS2DH12_WHO_AM_I               0x0F
#define LIS2DH12_TEMP_CFG_REG           0x1F
#define LIS2DH12_CTRL_REG0              0x1E
#define LIS2DH12_CTRL_REG1              0x20
#define LIS2DH12_CTRL_REG2              0x21
#define LIS2DH12_CTRL_REG3              0x22
#define LIS2DH12_CTRL_REG4              0x23
#define LIS2DH12_CTRL_REG5              0x24
#define LIS2DH12_CTRL_REG6              0x25
#define LIS2DH12_DATACAPTURE            0x25
#define LIS2DH12_STATUS_REG             0x27
#define LIS2DH12_OUT_XYZ                (0x80 |0x28)
#define LIS2DH12_OUT_X_L                0x28
#define LIS2DH12_OUT_X_H                0x29
#define LIS2DH12_OUT_Y_L                0x2A
#define LIS2DH12_OUT_Y_H                0x2B
#define LIS2DH12_OUT_Z_L                0x2C
#define LIS2DH12_OUT_Z_H                0x2D
#define LIS2DH12_FIFO_CTRL_REG          0x2E
#define LIS2DH12_FIFO_SRC_REG           0x2F
#define LIS2DH12_INT1_CFG               0x30
#define LIS2DH12_INT1_SRC               0x31
#define LIS2DH12_INT1_THS               0x32
#define LIS2DH12_INT1_DURATION          0x33
#define LIS2DH12_INT2_CFG               0x34
#define LIS2DH12_INT2_SRC               0x35
#define LIS2DH12_INT2_THS               0x36
#define LIS2DH12_INT2_DURATION          0x37
#define LIS2DH12_CLICK_CFG              0x38
#define LIS2DH12_CLICK_SRC              0x39
#define LIS2DH12_CLICK_THS              0x3A
#define LIS2DH12_TIME_LIMIT             0x3B
#define LIS2DH12_TIME_LATENCY           0x3C
#define LIS2DH12_TIME_WINDOW            0x3D
#define LIS2DH12_Act_THS                0x3E
#define LIS2DH12_Act_DUR                0x3F


 /* CTRL_REG1  */
#define ORD_LOW_PWR                     (0*0x10u)
#define ORD_1HZ                         (1*0x10u)
#define ORD_10HZ                        (2*0x10u)
#define ORD_25HZ                        (3*0x10u)
#define ORD_50HZ                        (4*0x10u)
#define ORD_100HZ                       (5*0x10u)
#define ORD_200HZ                       (6*0x10u)
#define ORD_400HZ                       (7*0x10u)
#define ORD_LOW_PWR_1620HZ              (8*0x10u)
#define ORD_1344HZ                      (9*0x10u)

#define Xen                             (0x01u)
#define Yen                             (0x02u)
#define Zen                             (0x04u)
#define LPen                            (0x08u)


 /* CTRL_REG3  */
#define I1_OVERRUN                      (0x02u)
#define I1_WTM                          (0x04u)
#define I1_DRDY2                        (0x08u)
#define I1_DRDY1                        (0x10u)
#define I1_AOI2                         (0x20u)
#define I1_AOI1                         (0x40u)
#define I1_CLICK                        (0x80u)


/* CTRL_REG_4*/
#define HR                              (0x08u)
#define FS_2G                           (0*0x10u)
#define FS_4G                           (1*0x10u)
#define FS_8G                           (2*0x10u)
#define FS_16G                          (3*0x10u)


/* CTRL_REG5 */
#define FIFO_EN                         (0x40u)


/* FIFO_CTRL_REG */
#define FIFO_MODE_CFG_BYPASS            (0*0x04u)
#define FIFO_MODE_CFG_FIFO              (1*0x04u)
#define FIFO_MODE_CFG_STREAM            (2*0x04u)
#define FIFO_MODE_CFG_STREAM2FIFO       (3*0x04u)

/*----------------------------------------------------------*/
/*  Struct                                                  */
/*----------------------------------------------------------*/
/**
 * @brief Structure for holding samples from accelerometer.
 */
typedef struct
{
    int16_t  x;
    int16_t  y;
    int16_t  z;
} ACC_DATA_T;


/*----------------------------------------------------------*/
/*  Macro                                                   */
/*----------------------------------------------------------*/



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
void Get_ACC_Data(ACC_DATA_T* ecg,  int timeout);
void Set_ACC_Start_Sampling(void);

bool Is_ACC_Not_Sampling(void);
void Init_LIS2DH12(void);
void LIS2DH12_Read_XYZ(void);
void LIS2DH12_Read_ID(void);
void Enable_LIS2DH12_INT1_FUNC(void);
bool Is_LIS2DH12_Interrupt(void);
bool Check_ACC_ID(void);

#endif
