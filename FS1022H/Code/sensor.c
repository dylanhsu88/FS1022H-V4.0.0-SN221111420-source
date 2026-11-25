/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : sensor.c
 * Author         : dy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"

#include "buzzer.h"
#include "flash_fmc.h"
#include "history.h"
#include "led.h"
#include "relay.h"
#include "rtc.h"
#include "sample.h"
#include "sensor.h"
#include "system.h"
#include "systick.h"
#include "usart.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static uint16_t sensor_time_now   = 0;
static uint8_t  sensor_speed_flag = 0;
static uint32_t sensor_speed_cnt  = 0;
static uint32_t sensor_speed_time = 0;

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
 * Function Name  : sensor_time_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void sensor_time_init(void)
{
    uint16_t  temp_length = FMC_PAGE_SIZE >> 1;
    uint16_t* temp_addr   = (uint16_t*)FMC_SENSOR_TIME_ADDRESS;

    sensor_time_now = 0;

    while (temp_length--)
    {
        if (((*temp_addr) != 0xFFFF) && (sensor_time_now < (*temp_addr)))
        {
            sensor_time_now = (*temp_addr);
        }
    }

    if (sensor_time_now >= SYSTEM_SENSOR_LIFE_TIME)
    {
        sysFlag_sensor_invalid = 1;
    }

    sensor_speed_flag    = 0;
    sensor_speed_cnt     = 0;
    sensor_speed_time    = 0;
}

/**************************************************************************************
 * Function Name  : sensor_time_simulate
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void sensor_time_simulate(void)
{
    static uint8_t sec = 0;
    uint8_t status_now = 0;

    status_now = system_status_get();

    if ((sensor_speed_flag == 1) && (status_now == STATUS_NORMAL))
    {
        sensor_speed_cnt++;
        if (sensor_speed_cnt >= sensor_speed_time)
        {
            sensor_speed_flag = 0;
            sensor_speed_cnt  = 0;
            system_status_set(STATUS_INVALID);  
        }
    }

    if ((sysFlag_sensor_invalid) && (status_now == STATUS_NORMAL))
    {
        sec++;
        if (sec >= 5)
        {
            sec = 0;
            system_status_set(STATUS_INVALID); 
        }                
    }       
    else
    {
        sec = 0;
    }  
}
/**************************************************************************************
 * Function Name  : sensor_time_inc
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void sensor_time_inc(void)
{       
    if ((sensor_time_now < SYSTEM_SENSOR_LIFE_TIME) && (system_status_get() != STATUS_START))
    {
        sensor_time_now++;
        if ((sensor_time_now % 512) == 0)
        {
            flash_fmc_erase_Npage(FMC_SENSOR_TIME_ADDRESS, 1);
        }
        flash_fmc_program_halfword(((sensor_time_now % 512) * 2) + FMC_SENSOR_TIME_ADDRESS, &sensor_time_now, 1);

        if (sensor_time_now >= SYSTEM_SENSOR_LIFE_TIME)
        {
            system_status_set(STATUS_INVALID);
        }
    }
}

/**************************************************************************************
 * Function Name  : sensor_time_speed_set
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void sensor_time_speed_set(uint32_t temp_time, uint8_t enable_status)
{
    sensor_speed_cnt  = 0;
    sensor_speed_time = temp_time;
    sensor_speed_flag = enable_status;
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
