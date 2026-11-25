/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : two_wire.c
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-05-08
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"
#include "two_wire.h"
#include "fs_type.h"
#include "led.h"
#include "system.h"
#include "sample.h"
#include "sensor.h"
#include "rtc.h"
#include "calibration.h"

/* Private typedef ------------------------------------------------------------------*/

typedef enum
{
    BIT_START = 0,
    BIT_DISTINGUISH = 1,
    BIT_ADDRESS_START = 2,
    BIT_ADDRESS_END = 9,
    BIT_COMMAND_START = 10,
    BIT_COMMAND_END = 17,
    BIT_FEEDBACK1_START = 18,
    BIT_FEEDBACK1_END = 25,
    BIT_FEEDBACK2_START = 26,
    BIT_FEEDBACK2_END = 33,
    BIT_SPECIAL = 34,
    BIT_STOP = 35
} enum_bus_bit_t; // 二线制总线1-35位描述说明

typedef enum
{
    INDEX_TYPE = 0,
    INDEX_ADDR,
    INDEX_COMMAND,
    INDEX_FEEDBACK1,
    INDEX_FEEDBACK2,
    INDEX_MAX
} enum_recv_index_t;

typedef struct
{
    uint8_t bit;                    // 位数 1 - 35位
    uint8_t time_cnt;               // 脉宽计数 2.5ms为0 模块，1ms为1 探头
    uint16_t need_feedback_data;    // 需要反馈的数据
    uint16_t feedback_bit_now;      // 正在反馈的第N位
} stc_bus_recv_t;

static uint8_t  gucCmdCnt = 0;
static uint8_t recv_data[INDEX_MAX] = {0};
static uint8_t time_data[20] = {0};
static uint8_t polling_led_enable = 1;
static stc_bus_recv_t bus_recv;
volatile byte_bits_t twoWireFlag1;
volatile byte_bits_t twoWireFlag2;
volatile byte_bits_t twoWireFlag3;

/* Private define -------------------------------------------------------------------*/
#define FEEDBACK_INVALID_DATA   (0xFFFF) // 当前数据不执行拉反馈
#define FEEDBACK_VAILD_BIT      (0)      // 反馈有效
#define SEGMENT_ADDRESS         (0xF0)   // 二总线分段地址
#define CONFIRM_DATA            (0xAA55) // 反馈确认

/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
* Function Name  : twoWire_feedback_gpio
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void twoWire_feedback_gpio(void)
{
    m_feedback_close();

    rcu_periph_clock_enable(TWO_WIRE_GPIO_FEEDBACK_CLK);

    gpio_mode_set(TWO_WIRE_GPIO_FEEDBACK_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TWO_WIRE_GPIO_FEEDBACK_PIN);
    gpio_output_options_set(TWO_WIRE_GPIO_FEEDBACK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TWO_WIRE_GPIO_FEEDBACK_PIN);

    m_feedback_close();
}

/**************************************************************************************
* Function Name  : twoWire_r24v_gpio
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void twoWire_r24v_gpio(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);

    rcu_periph_clock_enable(TWO_WIRE_GPIO_R24V_CLK);

    gpio_mode_set(TWO_WIRE_GPIO_R24V_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, TWO_WIRE_GPIO_R24V_PIN);

    rcu_periph_clock_enable(RCU_CFGCMP);
    nvic_irq_enable(TWO_WIRE_GPIO_R24V_IRQ, 0U, 0U);

    syscfg_exti_line_config(TWO_WIRE_GPIO_R24V_EXTI_SOURCE_PORT, TWO_WIRE_GPIO_R24V_EXTI_SOURCE_PIN);

    exti_init(TWO_WIRE_GPIO_R24V_EXTI, EXTI_INTERRUPT, TWO_WIRE_GPIO_R24V_TRIG_MODE);
    exti_interrupt_flag_clear(TWO_WIRE_GPIO_R24V_EXTI);
}

/**************************************************************************************
* Function Name  : timer_base_init
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void timer_base_init(uint32_t timer_periph, uint16_t channel, uint16_t prescaler, uint32_t period, uint32_t pluse)
{
    /* ----------------------------------------------------------------------------
    timer update rate = (prescaler + 1) * (period + 1) / SystemCoreClock
    ---------------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    switch (timer_periph)
    {
        case TIMER13:
            rcu_periph_clock_enable(RCU_TIMER13);
            break;
        case TIMER15:
            rcu_periph_clock_enable(RCU_TIMER15);
            break;
        default:
            return;
    }

    /* deinit a TIMER */
    timer_deinit(timer_periph);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER configuration */
    timer_initpara.prescaler = prescaler;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = period;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_channel_output_config(timer_periph, channel, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(timer_periph, channel, pluse);
    /* CH0 configuration in OC timing mode */
    timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_TIMING);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(timer_periph, channel, TIMER_OC_SHADOW_DISABLE);

    timer_interrupt_enable(timer_periph, TIMER_INT_CH0);

    /* enable a TIMER */
    // timer_enable(timer_periph);

    switch (timer_periph)
    {
        case TIMER13:
            nvic_irq_enable(TIMER13_IRQn, 0U, 0U);
            break;
        case TIMER15:
            nvic_irq_enable(TIMER15_IRQn, 0U, 0U);
            break;
        default:
            return;
    }
}

/**************************************************************************************
* Function Name  : twoWire_init
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void twoWire_init(void)
{
    twoWire_r24v_gpio();
    twoWire_feedback_gpio();

    timer_base_init(TIMER13, TIMER_CH_0, 71, 99, 50);   // pluse width count 100us
    timer_base_init(TIMER15, TIMER_CH_0, 71, 349, 175); // feenback count 350us

    memset(&recv_data, 0, sizeof(recv_data));
    memset(&bus_recv, 0, sizeof(bus_recv));

    twoWireFlag1.byte = 0;
    twoWireFlag2.byte = 0;
    twoWireFlag3.byte = 0;
}

/**************************************************************************************
* Function Name  : feedback_is_report
* Description    : 执行总线反馈
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void feedback_is_report(void)
{
    if (bus_recv.need_feedback_data == FEEDBACK_INVALID_DATA)
        return;

    if ((bus_recv.need_feedback_data & bus_recv.feedback_bit_now) == FEEDBACK_VAILD_BIT)
    {
        FlagReportFeedbackDataBit = 0;
        m_feedback_open();
        timer_enable(TIMER15);
    }
    else
    {
        FlagReportFeedbackDataBit = 1;
    }

    bus_recv.feedback_bit_now >>= 1;
}

/**************************************************************************************
* Function Name  : recv_bit_to_byte
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void recv_bit_to_byte(enum_recv_index_t index)
{
    uint8_t recv_feedback_data_bit = 0;

    recv_data[index] = recv_data[index] << 1;
    if (bus_recv.time_cnt < TWO_WIRE_MODULE_PROBE_LIMIT)
    {
        recv_data[index] |= 0x01;
        recv_feedback_data_bit = 1;
    }

    if ((index == INDEX_FEEDBACK1) || (index == INDEX_FEEDBACK2))
    {
        if (recv_feedback_data_bit != FlagReportFeedbackDataBit) // 反馈的数据与接收到的反馈数据不一样时，代表由其他前端在抢线，当前不再进行抢线
        {
            if (FlagPriorityConcentration)
            {
                FlagPriorityConcentration = 0;
                goto PRIORITY_END;        
            }
            if (FlagPriorityPreemption)
            {
                FlagPriorityPreemption = 0;
                if(recv_data[INDEX_COMMAND] == CMD_PRIORITY_ID_HIGH)
                {
                    FlagIDHighSeize = 0;
                }
                goto PRIORITY_END;
            }

            return;
            
        PRIORITY_END:
            bus_recv.need_feedback_data = FEEDBACK_INVALID_DATA;
            FlagIDHighReport = 0;
            FlagIDLowReport = 0;
        }
    }
}

/**************************************************************************************
* Function Name  : cammand_handle
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void cammand_handle(void)
{
    static uint8_t shield_count = 0;

    uint16_t temp_lel_conc[CAL_SECTION_ARRAY_MAX] = {0};
    uint16_t temp_lel_adc[CAL_SECTION_ARRAY_MAX] = {0};
    uint8_t local_addr = system_address_get();
    uint8_t rec_addr = recv_data[INDEX_ADDR];
    uint8_t local_status = system_status_get();
    uint8_t SendTemp = 0;
    uint8_t polling_led_status = 0;

    bus_recv.need_feedback_data = FEEDBACK_INVALID_DATA;
    bus_recv.feedback_bit_now = 0x8000;


    if (recv_data[INDEX_TYPE] != TWO_WIRE_TYPE_PROBE)
    {
        return;
    }
    if (recv_data[INDEX_COMMAND] != CMD_SHIELD)
    {
        shield_count = 0;
    }

    if (recv_data[INDEX_COMMAND] != CMD_DEBUG)
    {
        gucCmdCnt = 0;
    }

    switch (recv_data[INDEX_COMMAND])
    {
    case CMD_POLLING:
        if (((rec_addr & SEGMENT_ADDRESS) == (local_addr & SEGMENT_ADDRESS)) 
        && (local_status != STATUS_SHIELD))
        {            
            bus_recv.need_feedback_data = ~(0x8000 >> (uint8_t)(local_addr - rec_addr));

            if ((!sysFlag_selfcheck) && (local_status != STATUS_START) && (polling_led_enable))
            {
                polling_led_status = 1;
            }
        }
        break;

    case CMD_POLLING_FIRE_ALARM:
        if ((local_status == STATUS_FIRE)
         && ((rec_addr & SEGMENT_ADDRESS) == (local_addr & SEGMENT_ADDRESS)))
        {
            bus_recv.need_feedback_data = ~(0x8000 >> (uint8_t)(local_addr - rec_addr));
        }
        break;

    case CMD_PROBE_CONFIRM:
        if ((local_addr == rec_addr) && FlagFireAlarm)
        {
            if (!sysFlag_selfcheck && polling_led_enable) 
            {
                polling_led_status = 1;
            }
            FlagFireAlarm = 0;
            FlagFireAlarmSuccess = 1;
            bus_recv.need_feedback_data = CONFIRM_DATA;
        }
        break;

    case CMD_POLLING_TROUBLE:
        if (((local_status == STATUS_SENSOR_TROUBLE) || (local_status == STATUS_INVALID))
         && ((rec_addr & SEGMENT_ADDRESS) == (local_addr & SEGMENT_ADDRESS)))
        {
            bus_recv.need_feedback_data = ~(0x8000 >> (uint8_t)(local_addr - rec_addr));
        }
        break;

    case CMD_READ_FAULT_STATUS:
        if ((rec_addr & SEGMENT_ADDRESS) == (local_addr & SEGMENT_ADDRESS))
        {
            if (local_status == STATUS_SENSOR_TROUBLE)
            {
                bus_recv.need_feedback_data = TWO_WIRE_SENSOR_FAULT;
            }
            else if (local_status == STATUS_INVALID)
            {
                bus_recv.need_feedback_data = TWO_WIRE_SENSOR_INVALID;
            }
        }
        break;

    case CMD_READ_SENSOR_STATUS:
        if (rec_addr == local_addr)
        {
            if (sysFlag_sensor_invalid)
            {
                bus_recv.need_feedback_data = ((uint16_t)local_addr << 8) | (uint16_t)local_addr;
            }
            else
            {
                bus_recv.need_feedback_data = CONFIRM_DATA;
            }
        }
        break;

    case CMD_RESET:
    case CMD_SINGLE_ADDR_RESET:
        if ((rec_addr == local_addr) || (rec_addr == ADDR_BROADCAST))
        {
            gucCmdCnt = 0;

            if (local_status != STATUS_START)
            {
                sysFlag_reset = 1;
            }
        }
        break;

    case CMD_RESTART:
        if (rec_addr == ADDR_BROADCAST)
        {
            if (local_status != STATUS_START)
            {
                sysFlag_restart = 1;
            }
        }
        break;

    case CMD_TURN_ON_LED:
        if (rec_addr == ADDR_BROADCAST)
        {
            polling_led_enable = 1;
        }
        break;
    case CMD_TURN_OFF_LED:
        if (rec_addr == ADDR_BROADCAST)
        {
            polling_led_enable = 0;
        }
        break;

    case CMD_PRIORITY_FIRE_ALARM:
        if (system_mode_get() == MODE_INDEPENDENT)
        {
            if (FlagFireAlarm)
            {
                FlagPriorityPreemption = 1;
                bus_recv.need_feedback_data = ((uint16_t)local_addr << 8) | (uint16_t)local_addr;
            }
            else
            {
                sysFlag_modify = 1;
                system_mode_set(MODE_BUS);
            }
        }
        else 
        {
            if ((local_status != STATUS_SHIELD) && (local_status != STATUS_FIRE) && FlagFireAlarm)
            {
                FlagPriorityPreemption = 1;
                bus_recv.need_feedback_data = ((uint16_t)local_addr << 8) | (uint16_t)local_addr;
            }
        }

        break;

    case CMD_READ_ADDR:
        if (FlagAllowDisplayAddress)
        {
            FlagAllowDisplayAddress = 0;
            if (!sysFlag_selfcheck && polling_led_enable)
            {
                polling_led_status = 1;
            }
            bus_recv.need_feedback_data = ((uint16_t)local_addr << 8) | (uint16_t)local_addr;
        }
        break;

    case CMD_DISPLAY_SINGLE_ID:
        if (FlagAllowDisplayAddress)
        {
            FlagAllowDisplayAddress = 0;
            bus_recv.need_feedback_data = ((uint16_t)local_addr << 8) | (uint16_t)local_addr;
        }
        break;

    case CMD_PRIORITY_ID_UNDATE:
        FlagIDPriorityUpdate = 0;
        break;

    case CMD_PRIORITY_ID_HIGH:
        if (!FlagIDPriorityUpdate)
        {
            FlagIDHighReport = 1;
            FlagPriorityPreemption = 1;
            FlagIDHighSeize = 1;
            bus_recv.need_feedback_data = system_id_high_get();
        }
        break;

    case CMD_PRIORITY_ID_LOW:
        if (!FlagIDPriorityUpdate && FlagIDHighSeize)
        {
            FlagIDLowReport = 1;
            FlagPriorityPreemption = 1;
            bus_recv.need_feedback_data = system_id_low_get();
        }
        break;

    case CMD_PRIORITY_CON:
        if ((rec_addr == ADDR_BROADCAST) && (local_status != STATUS_SHIELD))
        {
            if (!sysFlag_selfcheck && polling_led_enable)
            {
                polling_led_status = 1;
            }
            FlagPriorityConcentration = 1;
            SendTemp = ~sample_lel_conc_get();
            bus_recv.need_feedback_data = ((uint16_t)SendTemp << 8) + local_addr;
        }
        break;

    case CMD_WRITE_ADDR_BY_ID_AFFIRM:
        if (rec_addr == local_addr)
        {
            if (!sysFlag_selfcheck && polling_led_enable)
            {
                polling_led_status = 1;
            }

            if (FlagIDHighMatchOK && FlagIDLowMatchOK)            
            {
                FlagWriteAddressSuccess = 1;
            }

            bus_recv.need_feedback_data = CONFIRM_DATA;
            FlagIDHighMatchOK = 0;
            FlagIDLowMatchOK = 0;
        }
        break;

    case CMD_SHIELD:
        if (rec_addr == local_addr)
        {
            shield_count++;
            if (shield_count >= 2)
            {
                shield_count = 0;
                if (local_status != STATUS_SHIELD)
                {
                    system_status_set(STATUS_SHIELD);
                }
            }
        }
        break;

    case CMD_SHIELD_REMOVE:
        if (rec_addr == local_addr)
        {
            system_status_set(STATUS_SHIELD_REMOVE);
        }
        break;

    case CMD_DEMARCATE_ZERO:
        if (((local_addr & SEGMENT_ADDRESS) == (rec_addr & SEGMENT_ADDRESS))
         && (local_status != STATUS_SHIELD))
        {
            if (calibration_zero_set() == 1)
            {
                bus_recv.need_feedback_data = ~(0x8000 >> (uint8_t)(local_addr - rec_addr));
            }
        }
        break;

    case CMD_DEMARCATE_ALARM:
        if (((local_addr & SEGMENT_ADDRESS) == (rec_addr & SEGMENT_ADDRESS))
         && (local_status != STATUS_SHIELD))
        {
            if (calibration_alarm_set(1) == 1)
            {
                bus_recv.need_feedback_data = ~(0x8000 >> (uint8_t)(local_addr - rec_addr));
            }
        }
        break;

    case CMD_READ_REAL_VLAUE:
        if ((rec_addr == local_addr) && (local_status != STATUS_SHIELD))
        {
            bus_recv.need_feedback_data = sample_average_value_get();
        }
        break;

    case CMD_READ_ZERO_VLAUE:
        if ((rec_addr == local_addr) && (local_status != STATUS_SHIELD))
        {
            bus_recv.need_feedback_data = system_lel_zero_get();
        }
        break;

    case CMD_READ_DEMARCATE_VLAUE:
        system_lel_get(temp_lel_conc, temp_lel_adc, 1);

        if ((rec_addr == local_addr) && (local_status != STATUS_SHIELD))
        {
            bus_recv.need_feedback_data = temp_lel_adc[1];
        }
        break;

    case CMD_READ_PULSEWIDTH:
        if ((rec_addr == local_addr) && (local_status != STATUS_SHIELD))
        {
            if (!sysFlag_selfcheck && polling_led_enable)
            {
                polling_led_status = 1;
            }
            SendTemp = sample_lel_conc_get();
            bus_recv.need_feedback_data = ((uint16_t)SendTemp << 8) + SendTemp;
        }
        break;

    case CMD_READ_ID_H:
        if (rec_addr == local_addr)
        {
            bus_recv.need_feedback_data = system_id_high_get();
        }
        break;

    case CMD_READ_ID_L:
        if (rec_addr == local_addr)
        {
            bus_recv.need_feedback_data = system_id_low_get();
        }
        break;

    case CMD_DEBUG:     //模拟报警
        gucCmdCnt++;
        if (gucCmdCnt >= 3)
        {
            gucCmdCnt = 0;
            FlagFireAlarm = 1;
        }
        break;

    default:
        break;
    }

    if (polling_led_status)
    {
        m_led_green_on();
    }
}

/**************************************************************************************
* Function Name  : special_bit_handle
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void special_bit_handle(void)
{
    uint8_t i = 0;
    static uint8_t time_flag[3] = {0};

    if (recv_data[INDEX_COMMAND] == CMD_WRITE_ADDR_BY_ID_H)
    {
        if (system_id_high_get() == (((uint16_t)recv_data[INDEX_FEEDBACK1] << 8) | (uint16_t)recv_data[INDEX_FEEDBACK2]))
        {
            FlagIDHighMatchOK = 1;
        }
        else
        {
            FlagIDHighMatchOK = 0;
        }
    }
    else if (recv_data[INDEX_COMMAND] == CMD_WRITE_ADDR_BY_ID_L)
    {
        if (system_id_low_get() == (((uint16_t)recv_data[INDEX_FEEDBACK1] << 8) | (uint16_t)recv_data[INDEX_FEEDBACK2]))
        {
            FlagIDLowMatchOK = 1;
        }
        else
        {
            FlagIDLowMatchOK = 0;
        }
    }
    else if (recv_data[INDEX_COMMAND] == CMD_WRITE_ADDR_BY_ID_START)
    {
        if ((!FlagWriteAddressSuccess) 
            && (FlagIDHighMatchOK) 
            && (FlagIDLowMatchOK) 
            && (recv_data[INDEX_FEEDBACK1] != 0xFF) 
            && (recv_data[INDEX_FEEDBACK2] != 0xFF) 
            && (recv_data[INDEX_FEEDBACK1] == recv_data[INDEX_FEEDBACK2]))
        {
            // 开始写地址
            FlagWriteAddress = 1;
            system_address_set(recv_data[INDEX_FEEDBACK1]);
        }
    }
    else if (recv_data[INDEX_COMMAND] == CMD_YEAR)
    {
        if ((recv_data[INDEX_FEEDBACK1] == recv_data[INDEX_FEEDBACK2])
         && (recv_data[INDEX_FEEDBACK1] != 0xFF)
         && (recv_data[INDEX_FEEDBACK2] != 0xFF))
        {
            for (i = 0; i < sizeof(time_data); i++)
            {
                time_data[i] = 0;
            }
            time_flag[0] = 1;
            time_data[5] = recv_data[INDEX_FEEDBACK1];
        }
    }
    else if (recv_data[INDEX_COMMAND] == CMD_DATE)
    {
        if ((recv_data[INDEX_FEEDBACK1] <= 12) && (recv_data[INDEX_FEEDBACK2] <= 31))
        {
            time_flag[1] = 1;
            time_data[6] = recv_data[INDEX_FEEDBACK1];
            time_data[7] = recv_data[INDEX_FEEDBACK2];
        }
    }
    else if (recv_data[INDEX_COMMAND] == CMD_TIME)
    {
        if ((recv_data[INDEX_FEEDBACK1] <= 24) && (recv_data[INDEX_FEEDBACK2] <= 60))
        {
            time_flag[2] = 1;
            time_data[8] = recv_data[INDEX_FEEDBACK1];
            time_data[9] = recv_data[INDEX_FEEDBACK2];
            if ((time_flag[0] == 1)
             && (time_flag[1] == 1)
             && (time_flag[2] == 1))
            {
                time_flag[0] = 0;
                time_flag[1] = 0;
                time_flag[2] = 0;
                rtc_time_modify(time_data);
            }
            for (i = 0; i < sizeof(time_data); i++)
            {
                time_data[i] = 0;
            }
        }
    }
}

/**************************************************************************************
* Function Name  : twoWire_rising_edge_handle
* Description    : 总线上升沿处理，主要是开始计算脉宽
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void twoWire_rising_edge_handle(void)
{
    m_feedback_close();

    if ((bus_recv.bit == BIT_DISTINGUISH)
        || ((bus_recv.bit >= BIT_ADDRESS_START) && (bus_recv.bit <= BIT_ADDRESS_END))
        || ((bus_recv.bit >= BIT_COMMAND_START) && (bus_recv.bit <= (BIT_COMMAND_END - 1)))
        || ((bus_recv.bit >= BIT_FEEDBACK1_START) && (bus_recv.bit <= BIT_FEEDBACK2_END))
        || (bus_recv.bit == BIT_SPECIAL))
    {
        bus_recv.time_cnt = 0;
        timer_enable(TIMER13);
    }
    else if (bus_recv.bit == BIT_COMMAND_END)
    {
        recv_bit_to_byte(INDEX_COMMAND);
        cammand_handle();
    }
    else if (bus_recv.bit == BIT_STOP) // 第35位的上升沿时，已经退出总线收码
    {
        bus_recv.bit = BIT_START;

        if (FlagFeedback34Bit 
         && FlagIDHighReport
         && FlagIDLowReport
         && (!FlagIDPriorityUpdate)
         && (recv_data[INDEX_COMMAND] == CMD_PRIORITY_ID_LOW))
        {
            FlagIDHighReport = 0;
            FlagIDLowReport = 0;
            FlagIDHighSeize = 0;
            FlagIDPriorityUpdate = 1;
            FlagAllowDisplayAddress = 1;
        }

        FlagFeedback34Bit = 0;

        if ((!sysFlag_selfcheck) && (system_status_get() != STATUS_START))
        {
            m_led_green_off();
        }
    }
}

/**************************************************************************************
* Function Name  : twoWire_falling_edge_handle
* Description    : 总线下降沿处理，处理数据，位数增加，停止脉宽计算
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void twoWire_falling_edge_handle(void)
{
    timer_disable(TIMER13);

    if (bus_recv.bit == BIT_START)
    {
        memset(&recv_data, 0, sizeof(recv_data));
        FlagFeedback34Bit = 0;
        FlagPriorityPreemption = 0;
        FlagPriorityConcentration = 0;
        bus_recv.bit++;
    }
    else if (bus_recv.bit == BIT_DISTINGUISH)
    {
        if (bus_recv.time_cnt > TWO_WIRE_MODULE_PROBE_LIMIT)
        {
            recv_data[INDEX_TYPE] = TWO_WIRE_TYPE_PROBE;
        }
        else
        {
            recv_data[INDEX_TYPE] = TWO_WIRE_TYPE_MOUDLE;
        }
        bus_recv.bit++;
    }
    else if ((bus_recv.bit >= BIT_ADDRESS_START) && (bus_recv.bit <= BIT_ADDRESS_END))
    {
        recv_bit_to_byte(INDEX_ADDR);
        bus_recv.bit++;
    }
    else if ((bus_recv.bit >= BIT_COMMAND_START) && (bus_recv.bit <= (BIT_COMMAND_END - 1)))
    {
        recv_bit_to_byte(INDEX_COMMAND);
        bus_recv.bit++;
        if (bus_recv.bit == BIT_COMMAND_END)
        {
            bus_recv.time_cnt = 0;
            timer_enable(TIMER13);
        }
    }
    else if (bus_recv.bit == BIT_COMMAND_END)
    {
        feedback_is_report(); 
        bus_recv.bit++;
    }
    else if ((bus_recv.bit >= BIT_FEEDBACK1_START) && (bus_recv.bit <= BIT_FEEDBACK1_END))
    {
        recv_bit_to_byte(INDEX_FEEDBACK1);
        feedback_is_report();
        bus_recv.bit++;
    }
    else if ((bus_recv.bit >= BIT_FEEDBACK2_START) && (bus_recv.bit <= BIT_FEEDBACK2_END))
    {
        recv_bit_to_byte(INDEX_FEEDBACK2);

        if (bus_recv.bit == BIT_FEEDBACK2_END)
        {
            if ((!FlagIDPriorityUpdate)
             && (recv_data[INDEX_TYPE] == TWO_WIRE_TYPE_PROBE)
             && ((recv_data[INDEX_COMMAND] == CMD_PRIORITY_ID_HIGH)
             || (recv_data[INDEX_COMMAND] == CMD_PRIORITY_ID_LOW)))
            {
                FlagFeedback34Bit = 1;
                if (FlagFeedback34Bit)
                {
                    m_feedback_open();
                    timer_enable(TIMER15);
                }
            }
        }
        else
        {
            feedback_is_report(); 
        }

        bus_recv.bit++;
    }
    else if (bus_recv.bit == BIT_SPECIAL)
    {
        bus_recv.bit++;
        special_bit_handle();
    }
    // 无第35位的下降沿
}

/**************************************************************************************
* Function Name  : twoWire_is_idle
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint8_t twoWire_is_idle(void)
{
    if (bus_recv.bit == BIT_START)
    {
        return 1;
    }

    return 0;
}

/**************************************************************************************
* Function Name  : EXTI2_3_IRQHandler
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void EXTI2_3_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(TWO_WIRE_GPIO_R24V_EXTI))
    {
        if (TWO_WIRE_GPIO_R24V_READ == 1)
        {            
            twoWire_rising_edge_handle();
        }
        else
        {
            twoWire_falling_edge_handle();
        }
        exti_interrupt_flag_clear(TWO_WIRE_GPIO_R24V_EXTI);
    }
}

/**************************************************************************************
* Function Name  : TIMER15_IRQHandler
* Description    : 350us
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void TIMER15_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER15, TIMER_INT_CH0))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER15, TIMER_INT_CH0);
        m_feedback_close();
        timer_disable(TIMER15);
    }
}

/**************************************************************************************
* Function Name  : TIMER13_IRQHandler
* Description    : 100us
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void TIMER13_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER13, TIMER_INT_CH0))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER13, TIMER_INT_CH0);

        bus_recv.time_cnt++;
        if (bus_recv.time_cnt > TWO_WIRE_ABNORMAL_EXIT_TIME)
        {
            timer_disable(TIMER13);
            bus_recv.bit = 0;
            memset(&recv_data, 0, sizeof(recv_data));
                 
            if ((!sysFlag_selfcheck) && (system_status_get() != STATUS_START))
            {
                m_led_green_off();
            }
        } 
    }
}


/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
