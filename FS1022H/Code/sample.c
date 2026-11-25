/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : sample.c
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

#include "sample.h"
#include "systick.h"
#include "usart.h"
#include "led.h"
#include "buzzer.h"
#include "system.h"
#include "history.h"
#include "flash_fmc.h"
#include "sensor.h"
#include "calibration.h"

#include "two_wire.h"


/* Private typedef ------------------------------------------------------------------*/
typedef struct
{
    uint16_t adc_value[SAMPLE_MAX]; // 采样值
    uint16_t adc_aver;
    uint8_t trouble_recovery_cnt;
    uint8_t trouble_cnt;
    uint8_t alarm_cnt;
    uint8_t lel_conc;
    uint8_t last_lel_conc;
    uint8_t lel_cnt;
    uint16_t usArr4Cnt;
    uint16_t usArr8Cnt;
    uint16_t usArrAver4[4];
    uint16_t usArrAver8[8];
} struct_sample_t;

static struct_sample_t sample;
/* Private define -------------------------------------------------------------------*/
 //#define TEST_DEBUG

/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
* Function Name  : sample_gpio_config
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void sample_gpio_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /* config the GPIO as analog mode */
    gpio_mode_set(SAMPLE_SENSOR_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, SAMPLE_SENSOR_GPIO_PIN);
    gpio_mode_set(SAMPLE_FAULT_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, SAMPLE_FAULT_GPIO_PIN);
}

/**************************************************************************************
* Function Name  : sample_adc_config
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void sample_adc_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */

    adc_channel_length_config(ADC_REGULAR_CHANNEL, 2U);

    /* ADC regular channel config */
    adc_regular_channel_config(0U, ADC_CHANNEL_8, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(1U, ADC_CHANNEL_9, ADC_SAMPLETIME_55POINT5);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC discontinuous mode */
    adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 3U);

    /* enable ADC interface */
    adc_enable();
    systick_delay_1ms(1);

    /* ADC calibration and reset calibration */
    adc_calibration_enable();

    /* ADC DMA function enable */
    adc_dma_mode_enable();
}

/**************************************************************************************
* Function Name  : sample_dma_config
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void sample_dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter = {0};
    
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* ADC DMA_channel configuration */
    dma_deinit(DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA);
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&sample.adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 2U;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;

    dma_init(DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA_CH0);
}

/**************************************************************************************
* Function Name  : sample_data_init
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void sample_data_init(void)
{
    sample.adc_aver = 0;
    sample.trouble_recovery_cnt = 0;
    sample.trouble_cnt = 0;
    sample.alarm_cnt = 0;
    sample.lel_conc = 0;
    sample.last_lel_conc = 0;
    sample.lel_cnt = 0;
    sample.usArr4Cnt = 0;
    sample.usArr8Cnt = 0;
    memset(sample.usArrAver4, 0, sizeof(sample.usArrAver4));
    memset(sample.usArrAver8, 0, sizeof(sample.usArrAver8));
}

/**************************************************************************************
* Function Name  : sample_init
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void sample_init(void)
{
    sample_gpio_config();
    sample_dma_config();
    sample_adc_config();

    sample_data_init();
}

/**************************************************************************************
* Function Name  : conc_protection
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void conc_protection(void)
{
    if (sample.lel_conc == sample.last_lel_conc) /* 相同 */
    {
        sample.last_lel_conc = sample.lel_conc;
        sample.lel_cnt = 0;
    }
    else if (sample.lel_conc > sample.last_lel_conc) /* up */
    {
        if ((sample.lel_conc - sample.last_lel_conc) > 5)
        {
            sample.lel_cnt++;
            sample.last_lel_conc = sample.last_lel_conc + 1;
            if (sample.lel_cnt > 2)
            {
                sample.lel_cnt = 0;
                sample.last_lel_conc = sample.lel_conc;
            }
        }
        else
        {
            sample.lel_cnt = 0;
            sample.last_lel_conc = sample.last_lel_conc + 1;
        }
    }
    else if (sample.lel_conc < sample.last_lel_conc) /* down */
    {
        if ((sample.last_lel_conc - sample.lel_conc) > 5)
        {
            sample.lel_cnt++;
            sample.last_lel_conc = sample.last_lel_conc - 1;
            if (sample.lel_cnt > 2)
            {
                sample.lel_cnt = 0;
                sample.last_lel_conc = sample.lel_conc;
            }
        }
        else
        {
            sample.last_lel_conc = sample.last_lel_conc - 1;
            sample.lel_cnt = 0;
        }
    }
}

/**************************************************************************************
* Function Name  : average_processing
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static uint16_t average_processing(uint16_t buffer_value, uint16_t *buffer, uint16_t *buffer_index, uint16_t buffer_length)
{
    uint16_t i;
    uint16_t buffer_sum;
    uint16_t buffer_average;

    buffer[*buffer_index] = buffer_value;
    *buffer_index = *buffer_index + 1;

    if ((*buffer_index) >= buffer_length)
    {
        *buffer_index = 0;
    }

    buffer_sum = 0;
    for (i = 0; i < buffer_length; i++)
    {
        buffer_sum += buffer[i];
    }

    buffer_average = buffer_sum / buffer_length;

    return buffer_average;
}

/**************************************************************************************
* Function Name  : fault_event
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void fault_event(void)
{
    if (system_status_get() == STATUS_NORMAL)
    {
        sample.trouble_cnt++;
        if (sample.trouble_cnt >= SAMPLE_FAULT_COUNT)
        {        
            system_status_set(STATUS_SENSOR_TROUBLE);
        }
    }
}

/**************************************************************************************
* Function Name  : sample_fault_handle
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void sample_fault_handle(uint16_t temp_fault, uint16_t temp_sensor)
{
    if (temp_fault > FAULT_AD_MAX) // 传感器 信号与电源2.5V短路
    {
        sample.trouble_recovery_cnt = SAMPLE_FAULT_RECOVERY_COUNT;

        if (temp_sensor < FAULT_SENSOR_AD_MIN)
        {
            fault_event();
        }
    }
    else if (temp_fault < FAULT_AD_MIN) // 传感器 信号与GND短路
    {
        sample.trouble_recovery_cnt = SAMPLE_FAULT_RECOVERY_COUNT;

        if (temp_sensor > FAULT_SENSOR_AD_MAX)
        {
            fault_event();
        }
    }
    else
    {
        sample.trouble_cnt = 0;
        if (sample.trouble_recovery_cnt != 0)
        {
            sample.trouble_recovery_cnt--;
            if ((sample.trouble_recovery_cnt == 0) && (system_status_get() == STATUS_SENSOR_TROUBLE))
            {                
                system_status_set(STATUS_NORMAL);
            }
        }
    }
}

/**************************************************************************************
* Function Name  : fire_alarm
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void fire_alarm(uint16_t adc_value)
{
    uint16_t temp_adc = 0;

    if (adc_value >= calibration_zero_get())
    {
        temp_adc = adc_value - calibration_zero_get();
    }

    if ((temp_adc >= calibration_adc_get(1)) 
     && ((system_status_get() == STATUS_NORMAL) || (system_status_get() == STATUS_INVALID)))
    {
        sample.alarm_cnt++;
        if (sample.alarm_cnt >= SAMPLE_ALARM_COUNT)
        {
            sample.alarm_cnt = 0;
            if (system_mode_get() == MODE_INDEPENDENT) 
            {
                system_status_set(STATUS_FIRE);
            }
            FlagFireAlarm = 1;
        }
    }
    else
    {
        sample.alarm_cnt = 0;
    }

    if (FlagFireAlarmSuccess)
    {
        FlagFireAlarmSuccess = 0;
        system_status_set(STATUS_FIRE);
    } 
}

/**************************************************************************************
* Function Name  : sample_lel_conc_get
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint8_t sample_lel_conc_get(void)
{
    return sample.last_lel_conc;
}

/**************************************************************************************
* Function Name  : sample_average_value_get
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
uint16_t sample_average_value_get(void)
{
    return sample.adc_aver;
}

/**************************************************************************************
* Function Name  : multiple_sampleing
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void multiple_sampleing(void)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint16_t temp_total[SAMPLE_MAX] = {0};
    uint16_t temp_max[SAMPLE_MAX] = {0};
    uint16_t temp_min[SAMPLE_MAX] = {0xFFFF, 0xFFFF};

    for (i = 0; i < 10; i++)
    {
        dma_flag_clear(DMA_CH0, DMA_FLAG_FTF);
        adc_software_trigger_enable(ADC_REGULAR_CHANNEL); //  执行adc转换
        while (dma_flag_get(DMA_CH0, DMA_FLAG_FTF) == 0);

        for (j = 0; j < SAMPLE_MAX; j++)
        {
            temp_total[j] += sample.adc_value[j];

            if (temp_min[j] > sample.adc_value[j])
            {
                temp_min[j] = sample.adc_value[j];
            }

            if (temp_max[j] < sample.adc_value[j])
            {
                temp_max[j] = sample.adc_value[j];
            }
            sample.adc_value[j] = 0;
        }
    }

    sample.adc_value[SAMPLE_SENSOR] = (temp_total[SAMPLE_SENSOR] - temp_min[SAMPLE_SENSOR] - temp_max[SAMPLE_SENSOR]) >> 3;
    sample.adc_value[SAMPLE_FAULT] = (temp_total[SAMPLE_FAULT] - temp_min[SAMPLE_FAULT] - temp_max[SAMPLE_FAULT]) >> 3;
}

/**************************************************************************************
* Function Name  : auto_cancel_fire
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
static void auto_cancel_fire(void)
{
    static uint8_t cancle_cnt = 0;

    if ((sample.last_lel_conc <= 3) && (system_status_get() == STATUS_FIRE))
    {
        cancle_cnt++;
        if (cancle_cnt >= 120)
        {
            cancle_cnt = 0;
            sysFlag_reset = 1;
        }
    }
    else
    {
        cancle_cnt = 0;
    }    
}

/**************************************************************************************
* Function Name  : sample_handle
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void sample_handle(void)
{
#if defined(TEST_DEBUG)
    static uint16_t debug_data[10] = {0};
    static char debug_send[200] = {0};
#endif

    multiple_sampleing();

#if defined(TEST_DEBUG)
    debug_data[0] = sample.adc_value[SAMPLE_SENSOR];
    debug_data[1] = sample.adc_value[SAMPLE_FAULT];
#endif

    sample.adc_aver = average_processing(sample.adc_value[SAMPLE_SENSOR], sample.usArrAver4, &sample.usArr4Cnt, 4); // 4个值平均值

#if defined(TEST_DEBUG)
    debug_data[2] = sample.adc_aver;
#endif

    sample.adc_aver = average_processing(sample.adc_aver, sample.usArrAver8, &sample.usArr8Cnt, 8); // 8个值平均值

#if defined(TEST_DEBUG)
    debug_data[3] = sample.adc_aver;
    debug_data[4] = calibration_zero_get();

    if (sample.adc_aver >= calibration_zero_get())
    {
        debug_data[5] = sample.adc_aver - calibration_zero_get();
    }
    else
    {
        debug_data[5] = 0;
    }
    debug_data[6] = calibration_adc_get(1);
#endif

    if (!systick_flag_power_up)
        return;

    if (system_status_get() == STATUS_START)
    {
        calibration_data_init();        
        system_status_set(STATUS_NORMAL);     
    }

    sample_fault_handle(sample.adc_value[SAMPLE_FAULT], sample.adc_value[SAMPLE_SENSOR]); // 故障值处理

    if (sample.trouble_recovery_cnt == 0)
    {
        sample.lel_conc = (uint8_t)(calibration_calc(sample.adc_aver));

#if defined(TEST_DEBUG)
        debug_data[7] = sample.lel_conc;
#endif

        conc_protection();

#if defined(TEST_DEBUG)
        debug_data[8] = sample.lel_conc;
#endif

        fire_alarm(sample.adc_aver);
    }
    else
    {
        sample.lel_conc = 0; // 检测到传感器故障，浓度为0
        sample.last_lel_conc = 0;
        sample.lel_cnt = 0;
    }

#if defined(TEST_DEBUG)
    sprintf(debug_send, "sensor:%04d, fault:%04d, aver4:%04d, aver8:%04d, zero:%04d,d_value:%04d,alarm:%04d,now_lel:%03d,last_lel:%03d %s",
            debug_data[0], debug_data[1], debug_data[2], debug_data[3], debug_data[4], debug_data[5], debug_data[6], debug_data[7], debug_data[8], "\r\n\0");
    usart0_dma_transmit((uint8_t *)debug_send, strlen(debug_send));
#endif    
    if (system_mode_get() == MODE_INDEPENDENT)
    {
        auto_cancel_fire(); // 自动取消报警
    }
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
