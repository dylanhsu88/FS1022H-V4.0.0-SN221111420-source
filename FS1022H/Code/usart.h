/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : usart.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------------*/

#define USART0_BAUD_RATE    (4800U)
#define USART0_TX_GPIO_CLK  (RCU_GPIOA)
#define USART0_TX_GPIO_PORT (GPIOA)
#define USART0_TX_GPIO_AF   (GPIO_AF_1)
#define USART0_TX_GPIO_PIN  (GPIO_PIN_9)

#define USART0_RX_GPIO_CLK  (RCU_GPIOA)
#define USART0_RX_GPIO_PORT (GPIOA)
#define USART0_RX_GPIO_AF   (GPIO_AF_1)
#define USART0_RX_GPIO_PIN  (GPIO_PIN_10)

/* Exported functions ---------------------------------------------------------------*/
void usart0_init(void);
void usart0_data_transmit(uint8_t *txData, uint32_t length);
void usart0_dma_transmit(uint8_t *pData, uint32_t length);
void USART0_IRQHandler(void);

#ifdef __cplusplus
}/*  */
#endif

#endif /* __USART_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
