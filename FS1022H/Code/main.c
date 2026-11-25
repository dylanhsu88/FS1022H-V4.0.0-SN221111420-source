/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : main.c
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

#include "led.h"
#include "key.h"
#include "rtc.h"
#include "system.h"
#include "usart.h"
#include "buzzer.h"
#include "flash_fmc.h"
#include "sample.h"
#include "history.h"
#include "protocol.h"
#include "systick.h"
#include "fs_type.h"
#include "sensor.h"
#include "relay.h"
#include "calibration.h"

#include "two_wire.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
* Function Name  : main
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
int main(void)
{
    system_init();
    
    while (1)
    {
        system_handle();
        calibration_handle();

        if (systick_flag_10ms)
        {
            systick_flag_10ms = 0;
            key_handle();
            led_handle();
            protocol_handle();
        }

        if (systick_flag_1s)
        {
            systick_flag_1s = 0;
            sample_handle();
            sensor_time_simulate();
            history_power_down_record();
        }

        if (systick_flag_1h)
        {
            systick_flag_1h = 0;
            sensor_time_inc();
        }
    }
}

/**************************************************************************************
* Function Name  : fputc
* Description    : retarget the C library printf function to the USART
* Input          :
* Output         :
* Return         :
**************************************************************************************/
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TC));
    return ch;
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
