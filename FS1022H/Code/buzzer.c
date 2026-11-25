/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : buzzer.c
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    : 
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "buzzer.h"
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"

/* Private typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t  ucType;
    uint16_t usOnTime;
    uint16_t usOffTime;
    uint16_t usTimeCount;
    uint16_t usTime;
    uint8_t cycle;
} struct_buzzer_t;

/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static struct_buzzer_t buzzer;

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
 * Function Name  : buzzer_gpio
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void buzzer_gpio(void)
{
    rcu_periph_clock_enable(BUZZER_GPIO_CLK);

    gpio_mode_set(BUZZER_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, BUZZER_GPIO_PIN);
    gpio_output_options_set(BUZZER_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BUZZER_GPIO_PIN);
    gpio_af_set(BUZZER_GPIO_PORT, BUZZER_GPIO_AF, BUZZER_GPIO_PIN);
}

/**************************************************************************************
 * Function Name  : buzzer_timer_pwm_mode
 * Description    : TIMER2CLK = SystemCoreClock / 72 = 1MHz count = 1us;
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void buzzer_timer_pwm_mode(void)
{
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct    timer_initpara;

    rcu_periph_clock_enable(BUZZER_TIME_CLK);

    timer_deinit(BUZZER_TIME_TYPE);

    timer_struct_para_init(&timer_initpara);

    timer_initpara.prescaler         = BUZZER_CYCLE_PRE;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = BUZZER_CYCLE;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(BUZZER_TIME_TYPE, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocinitpara);

    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, &timer_ocinitpara);

    /* CH0 configuration in PWM mode0*/

    timer_channel_output_pulse_value_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, BUZZER_DUTY_LOW);
    timer_channel_output_mode_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, TIMER_OC_MODE_PWM1);
    timer_channel_output_state_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, TIMER_CCXN_DISABLE);

    // timer_primary_output_config(BUZZER_TIME_TYPE, ENABLE);

    timer_auto_reload_shadow_enable(BUZZER_TIME_TYPE);

    timer_interrupt_enable(BUZZER_TIME_TYPE, TIMER_INT_UP);

    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(TIMER0_BRK_UP_TRG_COM_IRQn, 0U, 0U);

    // timer_enable(BUZZER_TIME_TYPE);
}
/**************************************************************************************
 * Function Name  : buzzer_handle
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void buzzer_init(void)
{
    buzzer_gpio();
    buzzer_timer_pwm_mode();

    buzzer.usOnTime    = 0;
    buzzer.usOffTime   = 0;
    buzzer.usTimeCount = 0;

    buzzer.ucType = BUZZER_DISABLE;
}

/**************************************************************************************
 * Function Name  : buzzer_mode_set
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void buzzer_mode_set(uint8_t status, uint16_t on_time, uint16_t off_time)
{
    buzzer.usTimeCount = 0;
    buzzer.usOnTime = on_time;
    buzzer.usOffTime = off_time;
    buzzer.usTime = buzzer.usOnTime;
    buzzer.ucType = status;

    if (status == BUZZER_DISABLE)
    {
        buzzer.usTime = 0;
        timer_disable(BUZZER_TIME_TYPE);
        timer_channel_output_pulse_value_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, BUZZER_DUTY_LOW);
        timer_primary_output_config(BUZZER_TIME_TYPE, DISABLE);
    }
    else
    {
        timer_channel_output_pulse_value_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, BUZZER_DUTY_CYCLE);
        timer_primary_output_config(BUZZER_TIME_TYPE, ENABLE);
        timer_enable(BUZZER_TIME_TYPE);
    }
}

/**************************************************************************************
 * Function Name  : TIMER0_BRK_UP_TRG_COM_IRQHandler
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void TIMER0_BRK_UP_TRG_COM_IRQHandler(void)
{
    timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);

    buzzer.usTimeCount++;
    if (buzzer.usTimeCount >= buzzer.usTime)
    {
        buzzer.usTimeCount = 0;

        if (buzzer.ucType == 1)
        {
            buzzer.ucType = 0;
            buzzer.usTime = buzzer.usOffTime;
            timer_channel_output_pulse_value_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, BUZZER_DUTY_LOW);
            if (buzzer.usOffTime == 0)
            {
                buzzer.usTime = 0;
                timer_disable(BUZZER_TIME_TYPE);
            }
        }
        else
        {
            buzzer.ucType = 1;
            buzzer.usTime = buzzer.usOnTime;
            timer_channel_output_pulse_value_config(BUZZER_TIME_TYPE, BUZZER_OUTPUT_CHx, BUZZER_DUTY_CYCLE);
        }
    }
}


/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
