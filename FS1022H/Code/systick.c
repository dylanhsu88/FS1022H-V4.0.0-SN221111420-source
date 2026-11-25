/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : systick.c
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
#include "system_gd32f3x0.h"

#include "led.h"
#include "system.h"
#include "systick.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static uint32_t systick_cnt = 0;
static uint32_t systick_sec_cnt = 0;
static uint32_t systick_delay = 0;
byte_bits_t systick_flag;

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
* Function Name  : systick_init
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void systick_init(void)
{
    if (SysTick_Config(SystemCoreClock / 1000U)) /* setup systick timer for 1000Hz interrupts */
    {
        while (1)
        {
            /* capture error */
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 0x00U); /* configure the systick handler priority */

    systick_flag.byte = 0;
}

/**************************************************************************************
 * Function Name  : systick_delay_1ms
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void systick_delay_1ms(uint32_t count)
{
    systick_delay = count;

    while (systick_delay != 0)
    {
    }
}

/**************************************************************************************
* Function Name  : SysTick_Handler
* Description    : 1ms
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void SysTick_Handler(void)
{
    systick_cnt++;
    if ((systick_cnt % SYSTICK_TIME_10MS) == 0)
    {
        systick_flag_10ms = 1;
    }
    if ((systick_cnt % SYSTICK_TIME_1S) == 0)
    {
        systick_flag_1s = 1;
        systick_sec_cnt++;
        if ((!systick_flag_power_up) && ((systick_sec_cnt % SYSTICK_TIME_POWER_UP) == 0))
        {
            systick_flag_power_up = 1;
        }  
        if ((systick_sec_cnt % SYSTICK_TIME_1H) == 0)
        {
            systick_flag_1h = 1;
            systick_sec_cnt = 0;
            systick_cnt = 0;
        }
    }

    if (systick_delay != 0)
    {
        systick_delay--;
    }
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
