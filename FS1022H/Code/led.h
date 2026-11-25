/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : led.h
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Include --------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
typedef enum
{
    green_led,
    red_led,    
    yellow_led,
    led_num_max
} enum_led_type_t;

/* Exported macro -------------------------------------------------------------------*/
#define LED_ON                     (1)      
#define LED_OFF                    (0)      
#define LED_PARA_NO                (0)      // uint:10ms

#define LED_FAULT_ON               (10)     // uint:10ms
#define LED_FAULT_OFF              (90)     // uint:10ms
#define LED_INVALID_ON             (20)     // uint:10ms
#define LED_INVALID_OFF            (20)     // uint:10ms
#define LED_INIT_FLASH             (50)     // uint:10ms
#define LED_POLLING_ON             (100)    // uint:10ms
#define LED_POLLING_OFF            (200)    // uint:10ms
#define LED_FAIL_STATUS_TIME       (500)    // uint:10ms
#define LED_SELFTEST_TIME          (50)     //自检LED翻转时间  uint:10ms 
#define LED_SELFTEST_OPEN_RELAY    (18)     //自检打开继电器时间 18 * 500 = 9s
#define LED_SELFTEST_CLOSE_RELAY   (20)     //自检打开继电器时间 20 * 500 = 10s

#define LED_RED_GPIO_CLK            (RCU_GPIOB)
#define LED_RED_GPIO_PORT           (GPIOB)
#define LED_RED_GPIO_PIN            (GPIO_PIN_3)
#define LED_GREEN_GPIO_CLK          (RCU_GPIOA)
#define LED_GREEN_GPIO_PORT         (GPIOA)
#define LED_GREEN_GPIO_PIN          (GPIO_PIN_15)
#define LED_YELLOW_GPIO_CLK         (RCU_GPIOB)
#define LED_YELLOW_GPIO_PORT        (GPIOB)
#define LED_YELLOW_GPIO_PIN         (GPIO_PIN_4)
#define m_led_red_off()             GPIO_BOP(LED_RED_GPIO_PORT) = LED_RED_GPIO_PIN
#define m_led_red_on()              GPIO_BC(LED_RED_GPIO_PORT) = LED_RED_GPIO_PIN
#define m_led_green_off()           GPIO_BOP(LED_GREEN_GPIO_PORT) = LED_GREEN_GPIO_PIN
#define m_led_green_on()            GPIO_BC(LED_GREEN_GPIO_PORT) = LED_GREEN_GPIO_PIN
#define m_led_green_tg()            GPIO_TG(LED_GREEN_GPIO_PORT) = LED_GREEN_GPIO_PIN
#define m_led_yellow_off()          GPIO_BOP(LED_YELLOW_GPIO_PORT) = LED_YELLOW_GPIO_PIN
#define m_led_yellow_on()           GPIO_BC(LED_YELLOW_GPIO_PORT) = LED_YELLOW_GPIO_PIN

/* Exported functions ---------------------------------------------------------------*/
void led_init(void);
void led_handle(void);
void led_set(uint8_t led_type, uint8_t led_status, uint16_t on_time, uint16_t off_time, uint8_t cycle);
void led_test_set(void);
void led_test_close(void);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H__  */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
