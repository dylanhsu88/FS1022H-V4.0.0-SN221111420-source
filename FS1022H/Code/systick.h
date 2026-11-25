/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : systick.h
* Author         : dy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include "fs_type.h"

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/

/* Exported macro -------------------------------------------------------------------*/
#define SYSTICK_TIME_10MS     (10)         // uint:1ms
#define SYSTICK_TIME_1S       (1000)       // uint:1ms
#define SYSTICK_TIME_POWER_UP (120)        // uint:1s
#define SYSTICK_TIME_1H       (3600)       // uint:1s

extern byte_bits_t systick_flag;
#define systick_flag_10ms           systick_flag.bits.bit0
#define systick_flag_1s             systick_flag.bits.bit1
#define systick_flag_1h             systick_flag.bits.bit2
#define systick_flag_power_up       systick_flag.bits.bit3

/* Exported functions ---------------------------------------------------------------*/
void systick_init(void);
void systick_time_handle(void);
void systick_delay_1ms(uint32_t count);

void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTICK_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
