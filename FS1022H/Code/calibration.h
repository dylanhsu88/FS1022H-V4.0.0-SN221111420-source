/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : calibration.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include "fs_type.h"

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------------*/

#define CAL_FUNC_SECTION_NUM    (10)
#define CAL_SECTION_ARRAY_MAX   (CAL_FUNC_SECTION_NUM + 1)

extern byte_bits_t calibration_flag;
#define calibration_flag_zero           calibration_flag.bits.bit0
#define calibration_flag_zero_success   calibration_flag.bits.bit1
#define calibration_flag_alarm          calibration_flag.bits.bit2
#define calibration_now                 calibration_flag.bits.bit3

/* Exported functions ---------------------------------------------------------------*/
void calibration_handle(void);
uint8_t calibration_alarm_set(uint8_t index);
uint8_t calibration_zero_set(void);
void calibration_init(void);
void calibration_data_init(void);
uint16_t calibration_calc(uint16_t adc_value);
uint16_t calibration_adc_get(uint8_t index);
uint16_t calibration_zero_get(void);
void calibration_led_close(void);

#ifdef __cplusplus
}
#endif

#endif /* __CALIBRATION_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
