/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : key.c
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"
#include "key.h"
#include "buzzer.h"
#include "led.h"
#include "system.h"
#include "two_wire.h"

/* Private typedef ------------------------------------------------------------------*/
typedef struct
{
    uint16_t pressCount;
    uint16_t releaseCount;
    uint8_t type;
} struct_key_t;

typedef enum
{
    KeyType_NoKey = 0,
    KeyType_PressKey,
    KeyType_ReleaseKey,
    KeyType_LongKey
} enum_key_type_t;

typedef enum
{
    KeyFlag_Press = 0,
    KeyFlag_Release
} enum_key_status_t;

/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static struct_key_t key;

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
* Function Name  : key_init
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void key_init(void)
{
    rcu_periph_clock_enable(KEY_GPIO_CLK);

    gpio_mode_set(KEY_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY_GPIO_PIN);

    key.releaseCount = 0;
    key.type = KeyType_NoKey;
}

/**************************************************************************************
* Function Name  : key_scan
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static uint8_t key_scan(void)
{
    key.type = KeyType_NoKey;

    if (PORT_KEY == RESET)
    {
        if (key.pressCount < KEY_LONG_PRESS_TIME)
        {
            key.releaseCount = KEY_WAIT_FOR_RELEASE_TIME;
            key.pressCount++;
            if (key.pressCount == KEY_PRESS_TIME)
            {
                key.type = KeyType_PressKey;
            }
            if (key.pressCount == KEY_LONG_PRESS_TIME)
            {
                key.type = KeyType_LongKey;
            }
        }
    }
    else
    {
        if (key.releaseCount != 0)
        {
            key.releaseCount--;
            if (key.releaseCount == 0)
            {
                key.type = KeyType_ReleaseKey;
            }
        }
        key.pressCount = 0;
    }

    return key.type;
}
/**************************************************************************************
* Function Name  : key_handle
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void key_handle(void)
{
    static uint8_t key_long_flag = 0;

    key_scan();

    if (key.type == KeyType_PressKey)
    {
        if (system_status_get() == STATUS_FIRE)
        {
            sysFlag_reset = 1;
        }
        else
        {
            if (!sysFlag_selfcheck)
            {                
                led_test_set();
            }            
        }
    }

    if (key.type == KeyType_LongKey)
    {
        if (system_mode_get() != MODE_INDEPENDENT)
        {
            system_mode_set(MODE_INDEPENDENT);
            system_para_modify();
            led_test_close();
            led_set(yellow_led, LED_ON, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
            key_long_flag = 1;
        }
        else
        {
            key_long_flag = 0;
        }
    }

    if (key_long_flag && (key.type == KeyType_ReleaseKey))
    {
        key_long_flag = 0;
        led_set(green_led, LED_ON, LED_POLLING_ON, LED_POLLING_OFF, LED_PARA_NO);
        if (sysFlag_sensor_invalid)
        {
            led_set(yellow_led, LED_ON, LED_INVALID_ON, LED_INVALID_OFF, LED_PARA_NO); 
        }
        else if (system_status_get() == STATUS_SENSOR_TROUBLE)
        {
            led_set(yellow_led, LED_ON, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
        }
        else
        {
            led_set(yellow_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
        }
    }
}
/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
