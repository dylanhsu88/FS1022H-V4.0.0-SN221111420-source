/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : key.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __KEY_H_
#define __KEY_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------------*/
#define KEY_GPIO_CLK (RCU_GPIOB)
#define KEY_GPIO_PORT (GPIOB)
#define KEY_GPIO_PIN (GPIO_PIN_7)

#define PORT_KEY gpio_input_bit_get(KEY_GPIO_PORT, KEY_GPIO_PIN)

#define KEY_LONG_PRESS_TIME         (500) // uint:10ms
#define KEY_WAIT_FOR_RELEASE_TIME   (3)   // uint:10ms
#define KEY_PRESS_TIME              (2)   // uint:10ms

/* Exported functions ---------------------------------------------------------------*/
void key_handle(void);
void key_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H__ */

/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
