/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : relay.c
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "relay.h"
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
 * Function Name  : relay_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void relay_init(void)
{
    m_relay_close();

    /* enable the clock */
    rcu_periph_clock_enable(RELAY_GPIO_CLK);
    /* configure led GPIO port */
    gpio_mode_set(RELAY_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RELAY_GPIO_PIN);
    gpio_output_options_set(RELAY_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RELAY_GPIO_PIN);

    m_relay_close();
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
