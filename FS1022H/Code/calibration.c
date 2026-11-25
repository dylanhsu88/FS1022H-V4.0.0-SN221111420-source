/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : calibration.c
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
#include "system.h"
#include "calibration.h"
#include "flash_fmc.h"
#include "systick.h"
#include "fs_type.h"
#include "sample.h"
#include "led.h"
#include "two_wire.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
#define CALC_MULTIPLE 10
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static uint16_t default_adc[CAL_SECTION_ARRAY_MAX] =
    {
        0,
        299,  // 10
        590,  // 20
        866,  // 30
        1128, // 40
        1392, // 50
        1657, // 60
        1910, // 70
        2121, // 80
        2482, // 90
        2732  // 100
};

static uint16_t default_conc[CAL_SECTION_ARRAY_MAX] =
    {
        0,
        10, // 10
        20, // 20
        30, // 30
        40, // 40
        50, // 50
        60, // 60
        70, // 70
        80, // 80
        90, // 90
        100 // 100
};

static uint8_t alarm_index = 0;
static uint16_t flag_alarm_success[CAL_SECTION_ARRAY_MAX] = {0};
static uint16_t lel_zero_value = 0;
static uint16_t function_k_value[CAL_FUNC_SECTION_NUM] = {0};

byte_bits_t calibration_flag = {0};

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
* Function Name  : calibration_calc
* Description    : 放大10倍计算，保留小数点一位
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint16_t calibration_calc(uint16_t adc_value)
{
    uint16_t y_adc = 0;
    uint16_t x_cons = 0;
    uint16_t temp_adc = 0;
    uint8_t i = 0;

    temp_adc = adc_value;

    if (temp_adc >= lel_zero_value)
    {
        y_adc = temp_adc - lel_zero_value;
    }

    // (▲y - y1) / (▲x - x1) = (y2 - y1) / (x2 - x1) = K
    // (▲x - x1)  = (▲y - y1) / K
    //  ▲x  = (▲y - y1) / K + x1
    for (i = 0; i < CAL_FUNC_SECTION_NUM; i++)
    {
        if ((y_adc >= default_adc[i]) && (y_adc < default_adc[i + 1]))
        {
            x_cons = (((y_adc - default_adc[i]) * CALC_MULTIPLE * CALC_MULTIPLE) / function_k_value[i]) + (default_conc[i] * CALC_MULTIPLE);
        }
    }

    x_cons = (x_cons + 5) / CALC_MULTIPLE;

    if (y_adc >= (default_adc[CAL_FUNC_SECTION_NUM]))
    {
        x_cons = 100;
    }

    return x_cons;
}

/**************************************************************************************
* Function Name  : calibration_func_init
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void calibration_func_init(void)
{
    uint8_t i = 0;

    for (i = 0; i < CAL_FUNC_SECTION_NUM; i++)
    {
        function_k_value[i] = (((default_adc[i + 1] - default_adc[i]) * CALC_MULTIPLE) / (default_conc[i + 1] - default_conc[i])); // k = (y2-y1) / (x2-x1)
        // 放大10倍，保留k值小数点一位
    }
}

/**************************************************************************************
* Function Name  : calibration_init
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void calibration_init(void)
{
    calibration_flag.byte = 0;
}

/**************************************************************************************
* Function Name  : calibration_zero_set
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint8_t calibration_zero_set(void)
{
    uint8_t status = 0;

    if (system_status_get() != STATUS_START)
    {
        calibration_now = 1;

        if (calibration_flag_zero_success == 0)
        {        
            if (calibration_flag_zero == 0)
            {
                calibration_flag_zero = 1;
                system_lel_zero_set(sample_average_value_get());
            }        
        }
        else
        {
            status = 1;
        }
    }

    return status;
}

/**************************************************************************************
* Function Name  : calibration_alarm_set
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint8_t calibration_alarm_set(uint8_t index)
{
    uint16_t alarm_value = 0;
    uint16_t average_value = 0;
    uint8_t status = 0;

    if ((system_status_get() != STATUS_START) && calibration_now)
    {
        average_value = sample_average_value_get();

        if (average_value >= lel_zero_value)
        {
            alarm_value = average_value - lel_zero_value;
        }
        else
        {
            alarm_value = lel_zero_value;
        }

        if (flag_alarm_success[index] == 0)
        {
            if (calibration_flag_alarm == 0)
            {
                calibration_flag_alarm = 1;
                alarm_index = index;
                system_lel_set(default_conc[index], alarm_value, index);
            }
        }
        else
        {
            status = 1;
        }
    }

    return status;
}

/**************************************************************************************
* Function Name  : calibration_adc_get
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint16_t calibration_adc_get(uint8_t index)
{
    return default_adc[index];
}

/**************************************************************************************
* Function Name  : calibration_zero_get
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint16_t calibration_zero_get(void)
{
    return lel_zero_value;
}

/**************************************************************************************
* Function Name  : calibration_handle
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void calibration_handle(void)
{
    if (twoWire_is_idle() == 0)
    {
        return;
    }

    if (calibration_flag_zero)
    {
        if (system_para_modify() == 1)
        {
            led_set(yellow_led,LED_ON, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
            calibration_flag_zero = 0;
            calibration_flag_zero_success = 1;
            calibration_data_init();
        }
    }
    if (calibration_flag_alarm)
    {
        if (system_para_modify() == 1)
        {
            led_set(yellow_led, LED_ON, 50, 50, 120);
            calibration_flag_alarm = 0;
            flag_alarm_success[alarm_index] = 1;
            calibration_data_init();
        }
    }
}

/**************************************************************************************
* Function Name  : calibration_led_close
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void calibration_led_close(void)
{
    if ((calibration_flag_zero_success)
     || (flag_alarm_success[1] == 1))
    {
        led_set(yellow_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
    }
}

/**************************************************************************************
* Function Name  : calibration_data_init
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void calibration_data_init(void)
{
    uint16_t temp_lel_conc[CAL_SECTION_ARRAY_MAX] = {0};
    uint16_t temp_lel_adc[CAL_SECTION_ARRAY_MAX] = {0};
    uint8_t j = 0;

    if ((system_lel_zero_get() == FMC_DEFAULT_16_VALUE))
    {
        lel_zero_value = sample_average_value_get();
    }
    else
    {
        lel_zero_value = system_lel_zero_get();
    }

    for (j = 0; j < CAL_SECTION_ARRAY_MAX; j++)
    {
        system_lel_get(temp_lel_conc, temp_lel_adc, j);

        if ((temp_lel_conc[j] != FMC_DEFAULT_16_VALUE) && (temp_lel_adc[j] != FMC_DEFAULT_16_VALUE))
        {
            default_conc[j] = temp_lel_conc[j];
            default_adc[j] = temp_lel_adc[j];
        }
    }

    calibration_func_init();
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
