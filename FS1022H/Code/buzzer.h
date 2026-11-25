/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : buzzer.h
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Include --------------------------------------------------------------------------*/
#include "stdint.h"

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------------*/
#define BUZZER_CYCLE_PRE        (71)
#define BUZZER_CYCLE            (279)
#define BUZZER_DUTY_CYCLE       (140)
#define BUZZER_DUTY_LOW         (280)
// pwm cycle = SystemCoreClock / (prescaler + 1) / (period + 1) = 72M /(71 + 1) * (279 + 1)  280US 

#define BUZZER_TIME_TYPE        (TIMER0)
#define BUZZER_TIME_CLK         (RCU_TIMER0)

#define BUZZER_AUTO_TIME        (1000 * 1000 / BUZZER_DUTY_LOW)    // uint 281us 
#define BUZZER_FLASH_TIME       (300 * 1000 / BUZZER_DUTY_LOW)     // uint 281us
#define BUZZER_FAULT_ON_TIME    (100 * 1000 / BUZZER_DUTY_LOW)     // uint 281us
#define BUZZER_FAULT_OFF_TIME   (900 * 1000 / BUZZER_DUTY_LOW)     // uint 281us
#define BUZZER_PARA_NO          (0)
#define BUZZER_ENABLE           (1)
#define BUZZER_DISABLE          (0)

#define BUZZER_GPIO_CLK         (RCU_GPIOA)
#define BUZZER_GPIO_PORT        (GPIOA)
#define BUZZER_GPIO_PIN         (GPIO_PIN_8)
#define BUZZER_GPIO_AF          (GPIO_AF_2)
#define BUZZER_OUTPUT_CHx       (TIMER_CH_0)

/* Exported functions ---------------------------------------------------------------*/
void buzzer_init(void);
void buzzer_mode_set(uint8_t status, uint16_t on_time, uint16_t off_time);
void TIMER0_BRK_UP_TRG_COM_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /*  */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
