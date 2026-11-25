/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : sample.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include "fs_type.h"

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
typedef enum
{
    SAMPLE_SENSOR = 0,
    SAMPLE_FAULT,
    SAMPLE_MAX
} enum_sampleValueIndex_t;

#define FAULT_AD_MAX                (2500) // 2014mv
#define FAULT_AD_MIN                (200)  // 161mv
#define FAULT_SENSOR_AD_MAX         (4000) // 3222mv
#define FAULT_SENSOR_AD_MIN         (100)  // 80mv

#define SAMPLE_ALARM_COUNT          (8)  // uint:1s
#define SAMPLE_FAULT_COUNT          (3)  // uint:1s
#define SAMPLE_FAULT_RECOVERY_COUNT (15) // uint:1s

/* Exported macro -------------------------------------------------------------------*/

// sample sensor init
#define SAMPLE_SENSOR_GPIO_CLK      (RCU_GPIOB)
#define SAMPLE_SENSOR_GPIO_PORT     (GPIOB)
#define SAMPLE_SENSOR_GPIO_PIN      (GPIO_PIN_0)

// sample fault init
#define SAMPLE_FAULT_GPIO_CLK       (RCU_GPIOB)
#define SAMPLE_FAULT_GPIO_PORT      (GPIOB)
#define SAMPLE_FAULT_GPIO_PIN       (GPIO_PIN_1)

/* Exported functions ---------------------------------------------------------------*/
void sample_init(void);
void sample_handle(void);
uint8_t sample_lel_conc_get(void);
uint16_t sample_average_value_get(void);
void sample_data_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __SAMPLE_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
