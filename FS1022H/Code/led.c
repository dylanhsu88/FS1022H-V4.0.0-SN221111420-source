/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : led.c
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "led.h"
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"
#include "sensor.h"
#include "system.h"
#include "systick.h"
#include "buzzer.h"
#include "relay.h"

/* Private typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t type[led_num_max];
    uint8_t status[led_num_max];
    uint8_t flag[led_num_max];
    uint16_t count[led_num_max];
    uint16_t time[led_num_max][2];
    uint8_t cycle[led_num_max];

    uint8_t first_time_cnt;
    uint8_t first_led_cnt;

    uint8_t test_cnt;
    uint8_t test_flag;
    uint8_t test_cycle;

    uint16_t poll_time;
} struct_led_t;

static struct_led_t led;

/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static const rcu_periph_enum GPIO_CLK[led_num_max] = {LED_GREEN_GPIO_CLK, LED_RED_GPIO_CLK, LED_YELLOW_GPIO_CLK};
static const uint32_t GPIO_PORT[led_num_max] = {LED_GREEN_GPIO_PORT, LED_RED_GPIO_PORT,LED_YELLOW_GPIO_PORT};
static const uint32_t GPIO_PIN[led_num_max] = {LED_GREEN_GPIO_PIN, LED_RED_GPIO_PIN, LED_YELLOW_GPIO_PIN};

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
 * Function Name  : led_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void led_init(void)
{
    uint8_t i;

    for (i = 0; i < led_num_max; i++)
    {
        rcu_periph_clock_enable(GPIO_CLK[i]);
        gpio_mode_set(GPIO_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN[i]);
        gpio_output_options_set(GPIO_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN[i]);
        GPIO_BOP(GPIO_PORT[i]) = GPIO_PIN[i];    // 1
        led.count[i] = 0;
    }

    led.first_led_cnt = yellow_led;
    led.first_time_cnt = 0;
}

/**************************************************************************************
 * Function Name  : led_set
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void led_set(uint8_t led_type, uint8_t led_status, uint16_t on_time, uint16_t off_time, uint8_t cycle)
{
    if (led_status == 1)
    {
        led.flag[led_type] = 1;
        GPIO_BC(GPIO_PORT[led_type]) = GPIO_PIN[led_type];
        led.count[led_type] = on_time;
    }
    else
    {
        led.flag[led_type] = 0;
        GPIO_BOP(GPIO_PORT[led_type]) = GPIO_PIN[led_type];    
        led.count[led_type] = off_time;
    }

    led.time[led_type][0] = off_time;
    led.time[led_type][1] = on_time;
    led.cycle[led_type] = cycle;
}

/**************************************************************************************
 * Function Name  : led_test_set
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void led_test_set(void)
{
    sysFlag_selfcheck = 1;
    m_led_red_on();
    m_led_yellow_on();
    m_led_green_on();
    led.test_cycle = 0;
    led.test_cnt++;
    buzzer_mode_set(BUZZER_ENABLE, BUZZER_FLASH_TIME, BUZZER_FLASH_TIME);
}

/**************************************************************************************
 * Function Name  : led_test_close
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void led_test_close(void)
{
    uint8_t i = 0;

    if (sysFlag_selfcheck)
    {
        m_relay_close();
        buzzer_mode_set(BUZZER_DISABLE, BUZZER_PARA_NO, BUZZER_PARA_NO);
        sysFlag_selfcheck = 0;
        led.test_cycle = 0;

        m_led_green_off();

        if (system_mode_get() == MODE_INDEPENDENT)
        {
            for (i = green_led; i < led_num_max; i++)
            {
                if (led.flag[i] == 1)
                {
                    GPIO_BC(GPIO_PORT[i]) = GPIO_PIN[i]; 
                }
                else
                {
                    GPIO_BOP(GPIO_PORT[i]) = GPIO_PIN[i]; 
                }
            }
        }
        else 
        {
            for (i = red_led; i < led_num_max; i++)
            {
                if (led.flag[i] == 1)
                {
                    GPIO_BC(GPIO_PORT[i]) = GPIO_PIN[i]; 
                }
                else
                {
                    GPIO_BOP(GPIO_PORT[i]) = GPIO_PIN[i]; 
                }
            }
        }

        if ((system_status_get() == STATUS_SENSOR_TROUBLE) 
         || (system_status_get() == STATUS_INVALID))
        {
            buzzer_mode_set(BUZZER_ENABLE, BUZZER_FAULT_ON_TIME, BUZZER_FAULT_OFF_TIME);  
        }   
    }
}

/**************************************************************************************
 * Function Name  : led_handle
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void led_handle(void)
{
    uint8_t i = 0;

    if (sysFlag_selfcheck)
    {
        led.test_cnt++;
        if (led.test_cnt >= LED_SELFTEST_TIME)
        {
            led.test_cnt = 0;
            led.test_cycle++;
            if (led.test_cycle == LED_SELFTEST_OPEN_RELAY)
            {
                m_relay_open();
            }
            if (led.test_cycle >= LED_SELFTEST_CLOSE_RELAY)
            {
                led_test_close();
                return;
            }

            GPIO_TG(GPIO_PORT[green_led]) = GPIO_PIN[green_led]; 
            GPIO_TG(GPIO_PORT[red_led]) = GPIO_PIN[red_led]; 
            GPIO_TG(GPIO_PORT[yellow_led]) = GPIO_PIN[yellow_led];
        }
    }
    else if (system_status_get() == STATUS_START)
    {
        led.first_time_cnt++;
        if (led.first_time_cnt >= LED_INIT_FLASH)
        {
            led.first_time_cnt = 0;
            m_led_red_off();
            m_led_green_off();
            m_led_yellow_off();

            if (sysFlag_cali_error)
            {
                if (led.first_led_cnt != green_led)
                {
                    led.first_led_cnt--;
                }
                else
                {
                    led.first_led_cnt = yellow_led;
                }
            }
            else
            {
                led.first_led_cnt++;
                led.first_led_cnt %= led_num_max;
            }
            GPIO_TG(GPIO_PORT[led.first_led_cnt]) = GPIO_PIN[led.first_led_cnt];
        }
    }
    else
    {
        if (system_mode_get() == MODE_INDEPENDENT) 
        {
            for (i = green_led; i < led_num_max; i++)
            {                        
                if (led.count[i] != 0)
                {
                    led.count[i]--;
                    if (led.count[i] == 0)
                    {                   
                        led.flag[i]++;
                        led.flag[i] %= 2;
                        led.count[i] = led.time[i][led.flag[i]];

                        if (led.cycle[i] != 0)
                        {
                            led.cycle[i]--;  
                            if (led.cycle[i] == 0)
                            {
                                GPIO_BOP(GPIO_PORT[i]) = GPIO_PIN[i]; // off
                                led.count[i] = 0;
                                continue;
                            }
                        }
                        GPIO_TG(GPIO_PORT[i]) = GPIO_PIN[i];
                    }
                }    
            }
        }
        else
        {
            for (i = red_led; i < led_num_max; i++)       
            {                        
                if (led.count[i] != 0)
                {
                    led.count[i]--;
                    if (led.count[i] == 0)
                    {                   
                        led.flag[i]++;
                        led.flag[i] %= 2;
                        led.count[i] = led.time[i][led.flag[i]];

                        if (led.cycle[i] != 0)
                        {
                            led.cycle[i]--;  
                            if (led.cycle[i] == 0)
                            {
                                GPIO_BOP(GPIO_PORT[i]) = GPIO_PIN[i]; // off
                                led.count[i] = 0;
                                continue;
                            }
                        }
                        GPIO_TG(GPIO_PORT[i]) = GPIO_PIN[i];
                    }
                }    
            }
        }
    }
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
