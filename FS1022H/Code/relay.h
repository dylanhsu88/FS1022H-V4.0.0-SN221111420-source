/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : relay.h
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __RELAY_H__
#define __RELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Include --------------------------------------------------------------------------*/
#include "stdint.h"

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------------*/
// relay init
#define RELAY_GPIO_CLK       (RCU_GPIOB)
#define RELAY_GPIO_PORT      (GPIOB)
#define RELAY_GPIO_PIN       (GPIO_PIN_2)
#define m_relay_open()       (GPIO_BOP(RELAY_GPIO_PORT) = RELAY_GPIO_PIN)
#define m_relay_close()      (GPIO_BC(RELAY_GPIO_PORT) = RELAY_GPIO_PIN)

/* Exported functions ---------------------------------------------------------------*/
void relay_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __RELAY_H__  */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
