/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : driver_interface.c
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-03-18
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"

#include "buzzer.h"
#include "calibration.h"
#include "flash_fmc.h"
#include "history.h"
#include "led.h"
#include "protocol.h"
#include "rtc.h"
#include "sample.h"
#include "sensor.h"
#include "system.h"
#include "systick.h"
#include "usart.h"
#include "two_wire.h"

/* Private typedef ------------------------------------------------------------------*/
typedef struct
{
    uint8_t buff[PROTOCOL_RECEIVE_DATA_MAX];                                     // 单帧数据最大缓冲
    uint8_t recv_cnt;                                                           // 下标
    uint8_t ucBuffSpace[PROTOCOL_RECEIVE_DATA_SIZE][PROTOCOL_RECEIVE_DATA_MAX];    // 完整帧存储缓冲
    uint8_t ucBuffUse;                                                             // 已经保存数据的缓存计数
    uint8_t ucBuffUnused;                                                          // 未保存数据的缓冲计数
    uint8_t check_sum;                                                            // 累加校验和
    uint8_t time_cnt;                                                             // 帧之间的间隔计数
} struct_recv_data_t;

static struct_recv_data_t pro_recv;

static uint8_t SN_ver1[3] = {1, 0, 0};
static uint8_t SN_ver2[9] = {2, 2, 1, 1, 1, 1, 4, 2, 0};

uint8_t answerData[60] = {0};

/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
 * Function Name  : protocol_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void protocol_init(void)
{
    memset(&pro_recv, 0 , sizeof(struct_recv_data_t));
}

/**************************************************************************************
 * Function Name  : date_month_get
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
uint8_t date_month_get(void)
{
    uint8_t month = 0;

    if ((__DATE__[0] == 'J') && (__DATE__[1] == 'a') && (__DATE__[2] == 'n'))
    {
        month = 1;
    }
    else if ((__DATE__[0] == 'F') && (__DATE__[1] == 'e') && (__DATE__[2] == 'b'))
    {
        month = 2;
    }
    else if ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'r'))
    {
        month = 3;
    }
    else if ((__DATE__[0] == 'A') && (__DATE__[1] == 'p') && (__DATE__[2] == 'r'))
    {
        month = 4;
    }
    else if ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'y'))
    {
        month = 5;
    }
    else if ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'n'))
    {
        month = 6;
    }
    else if ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'l'))
    {
        month = 7;
    }
    else if ((__DATE__[0] == 'A') && (__DATE__[1] == 'u') && (__DATE__[2] == 'g'))
    {
        month = 8;
    }
    else if ((__DATE__[0] == 'S') && (__DATE__[1] == 'e') && (__DATE__[2] == 'p'))
    {
        month = 9;
    }
    else if ((__DATE__[0] == 'O') && (__DATE__[1] == 'c') && (__DATE__[2] == 't'))
    {
        month = 10;
    }
    else if ((__DATE__[0] == 'N') && (__DATE__[1] == 'o') && (__DATE__[2] == 'v'))
    {
        month = 11;
    }
    else if ((__DATE__[0] == 'D') && (__DATE__[1] == 'e') && (__DATE__[2] == 'c'))
    {
        month = 12;
    }
    return month;
}

/**************************************************************************************
 * Function Name  : date_day_get
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
uint8_t date_day_get(void)
{
    uint8_t i = 0;
    uint8_t day, a[2];

    for (i = 0; i < 2; i++)
    {
        a[i] = __DATE__[4 + i];
        if (__DATE__[4 + i] < 0x30)
        {
            a[i] = 0x30;
        }
    }

    day = ((a[0] - 0x30) * 10) + (a[1] - 0x30);

    return day;
}

/**************************************************************************************
 * Function Name  : date_year_get
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
uint16_t date_year_get(void)
{
    uint8_t i = 0;
    uint16_t year, a[4];

    for (i = 0; i < 4; i++)
    {
        a[i] = __DATE__[7 + i];
        if (__DATE__[7 + i] < 0x30)
        {
            a[i] = 0x30;
        }
    }

    year = ((a[0] - 0x30) * 1000) + ((a[1] - 0x30) * 100) + ((a[2] - 0x30) * 10) + (a[3] - 0x30);

    return year;
}

/**************************************************************************************
 * Function Name  : protocol_read_frame
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static uint8_t protocol_read_frame(uint8_t* recData)
{
    uint8_t status = 0;
    uint8_t count  = 0;

    if (pro_recv.ucBuffUse != pro_recv.ucBuffUnused)
    {
        for (count = 0; count < PROTOCOL_RECEIVE_DATA_MAX; count++)
        {
            recData[count] = pro_recv.ucBuffSpace[pro_recv.ucBuffUnused][count];
        }

        pro_recv.ucBuffUnused++;
        if (pro_recv.ucBuffUnused >= PROTOCOL_RECEIVE_DATA_SIZE)
        {
            pro_recv.ucBuffUnused = 0;
        }
        status = 1;
    }

    return status;
}

/**************************************************************************************
 * Function Name  : interface_check_sums
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static uint8_t interface_check_sums(uint8_t* frameData, uint8_t length)
{
    uint16_t sums = 0;

    while (length--)
    {
        sums += *frameData;
        frameData++;
    }

    return (uint8_t)(sums);
}

/**************************************************************************************
 * Function Name  : protocol_answer_history_total
 * Description    : 主站发送 AA 00 00 00 AA 55
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_answer_history_total(void)
{
    answerData[0] = FRAME_START_BYTE;
    answerData[1] = FRAME_C1_NO_INQUIRE_BYTE;
    answerData[2] = FRAME_C2_INQUIRE_ALL_TOTAL_BYTE;
    answerData[3] = 0x07;    // length

    answerData[4]  = history_event_total_get(type_alarm);
    answerData[5]  = history_event_total_get(type_alarm_recovery);
    answerData[6]  = history_event_total_get(type_fault);
    answerData[7]  = history_event_total_get(type_fault_recovery);
    answerData[8]  = history_event_total_get(type_power_down);
    answerData[9]  = history_event_total_get(type_power_up);
    answerData[10] = history_event_total_get(type_invalid);

    answerData[11] = interface_check_sums(answerData, 11);
    answerData[12] = FRAME_END_BYTE;

    usart0_dma_transmit(answerData, 13);    //往外发送数据
}

/**************************************************************************************
 * Function Name  : protocol_answer_event_inquire
 * Description    : 主站发送 AA 1-255 01-07 00 xx 55 ，查询第n条的eventType事件
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_answer_event_inquire(uint8_t number, enum_event_type eventType)
{
    uint8_t  count        = 0;
    uint32_t temp_address = 0;

    HistoryTime_t historyTime = {0};

    if (number > history_event_total_get(eventType))
    {
        return;
    }

    temp_address = ((number - 1) * HISTORY_ADDRESS_SIZE) + (((eventType - 1) * FMC_PAGE_SIZE) + FMC_HISTORY_START_ADDR);

    flash_fmc_read(temp_address, (uint32_t*)&historyTime.ulTime, 1);

    answerData[count++] = FRAME_START_BYTE;
    answerData[count++] = number;

    answerData[count++] = eventType;
    count++;

    answerData[count++] = number;
    answerData[count++] = 20;
    answerData[count++] = (uint8_t)(historyTime.time.year);
    answerData[count++] = historyTime.time.month;
    answerData[count++] = historyTime.time.day;
    answerData[count++] = historyTime.time.hour;
    answerData[count++] = historyTime.time.minute;
    answerData[3]       = count - 4;

    answerData[count] = interface_check_sums(answerData, count);
    count++;
    answerData[count++] = FRAME_END_BYTE;

    usart0_dma_transmit(answerData, count);    //往外发送数据
}

/**************************************************************************************
 * Function Name  : protocol_answer_time_inquire
 * Description    : 主站发送 AA 00 08 00 xx 55 ，
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_answer_time_inquire(uint8_t number, uint8_t eventType, uint8_t opt)
{
    uint8_t  count = 0;
    uint8_t  sec = 0;

    HistoryTime_t        historyTime = {0};
    rtc_parameter_struct rtc_initpara;
    rtc_current_time_get(&rtc_initpara);

    historyTime.time.year   = m_hex_force_to_dec(rtc_initpara.rtc_year);
    historyTime.time.month  = m_hex_force_to_dec(rtc_initpara.rtc_month);
    historyTime.time.day    = m_hex_force_to_dec(rtc_initpara.rtc_date);
    historyTime.time.hour   = m_hex_force_to_dec(rtc_initpara.rtc_hour);
    historyTime.time.minute = m_hex_force_to_dec(rtc_initpara.rtc_minute);
    sec                = m_hex_force_to_dec(rtc_initpara.rtc_second);

    answerData[count++] = FRAME_START_BYTE;
    answerData[count++] = number;

    answerData[count++] = eventType;
    count++;
    answerData[count++] = 20;
    answerData[count++] = (uint8_t)(historyTime.time.year);
    answerData[count++] = historyTime.time.month;
    answerData[count++] = historyTime.time.day;
    answerData[count++] = historyTime.time.hour;
    answerData[count++] = historyTime.time.minute;

    if (opt == 1)
    {
        answerData[count++] = sec;
    }

    answerData[3] = count - 4;

    answerData[count] = interface_check_sums(answerData, count);
    count++;
    answerData[count++] = FRAME_END_BYTE;

    usart0_dma_transmit(answerData, count);    //往外发送数据
}
/**************************************************************************************
 * Function Name  : protocol_answer_event_sensor_inquire
 * Description    : 主站发送 AA 00 07 00 B1 55
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_answer_event_sensor_inquire(void)
{
    uint32_t temp_address = 0;

    HistoryTime_t historyTime = {0};

    temp_address = ((1 - 1) * HISTORY_ADDRESS_SIZE) + (((type_invalid - 1) * FMC_PAGE_SIZE) + FMC_HISTORY_START_ADDR);

    flash_fmc_read(temp_address, (uint32_t*)&historyTime.ulTime, 1);    // 只存在一条历史事件

    answerData[0] = FRAME_START_BYTE;
    answerData[1] = FRAME_C1_NO_INQUIRE_BYTE;

    answerData[2] = FRAME_C2_INQUIRE_SENSOR_INVALID_BYTE;

    answerData[3] = 0x07;    // length

    if (historyTime.ulTime != 0xFFFFFFFF)
    {
        answerData[4]  = 1;    // 失效
        answerData[5]  = 20;
        answerData[6]  = (uint8_t)(historyTime.time.year);
        answerData[7]  = historyTime.time.month;
        answerData[8]  = historyTime.time.day;
        answerData[9]  = historyTime.time.hour;
        answerData[10] = historyTime.time.minute;
    }
    else
    {
        answerData[4]  = 0;
        answerData[5]  = 0;
        answerData[6]  = 0;
        answerData[7]  = 0;
        answerData[8]  = 0;
        answerData[9]  = 0;
        answerData[10] = 0;
    }

    answerData[11] = interface_check_sums(answerData, 11);
    answerData[12] = FRAME_END_BYTE;

    usart0_dma_transmit(answerData, 13);    // 往外发送数据
}

/**************************************************************************************
 * Function Name  : protocol_receive_data
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void protocol_receive_data(uint8_t recData)
{
    uint8_t count;

    pro_recv.time_cnt = 0;
    pro_recv.buff[pro_recv.recv_cnt++] = recData;

    switch (pro_recv.recv_cnt)
    {
        case 1:
            if (pro_recv.buff[0] != FRAME_START_BYTE)
            {
                pro_recv.recv_cnt = 0;
                pro_recv.check_sum  = 0;
            }
            pro_recv.check_sum = recData;
            break;
        case 2:
            pro_recv.check_sum += recData;
            break;
        case 3:
            pro_recv.check_sum += recData;
            break;
        case 4:
            /* code */
            pro_recv.check_sum += recData;
            break;
        default:
            pro_recv.check_sum += recData;

            if ((pro_recv.recv_cnt >= (pro_recv.buff[3] + 6)) &&
                (pro_recv.buff[pro_recv.recv_cnt - 1] == FRAME_END_BYTE))
            {
                pro_recv.check_sum = pro_recv.check_sum - pro_recv.buff[pro_recv.recv_cnt - 1] - pro_recv.buff[pro_recv.recv_cnt - 2];
                if (pro_recv.buff[pro_recv.recv_cnt - 2] == pro_recv.check_sum)
                {
                    if (pro_recv.buff[2] == FRAME_C2_CUSTOM_SNTP_BYTE)
                    {                        
                        rtc_time_modify(pro_recv.buff);
                    }
                    else if (pro_recv.buff[2] == FRAME_C2_CUSTOM_DEMARCATE_ZERO_BYTE)
                    {
                        calibration_zero_set();
                    }
                    else if (pro_recv.buff[2] == FRAME_C2_CUSTOM_DEMARCATE_ALARM_BYTE)
                    {
                        calibration_alarm_set(pro_recv.buff[4]);
                    }
                    else
                    {
                        for (count = 0; count < PROTOCOL_RECEIVE_DATA_MAX; count++)
                        {
                            pro_recv.ucBuffSpace[pro_recv.ucBuffUse][count] = pro_recv.buff[count];
                        }
                        pro_recv.ucBuffUse++;
                        if (pro_recv.ucBuffUse >= PROTOCOL_RECEIVE_DATA_SIZE)
                        {
                            pro_recv.ucBuffUse = 0;
                        }
                    }
                }
                pro_recv.check_sum  = 0;
                pro_recv.recv_cnt = 0;
            }
            break;
    }
}

/**************************************************************************************
 * Function Name  : protocol_sensor_speed_handle
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_sensor_speed_handle(uint8_t opt, uint8_t temp_time)
{
    uint32_t tempTime = 0;

    switch (opt)
    {
        case 0:
            sensor_time_speed_set(tempTime, 0);    // year
            break;
        case 1:
            tempTime = temp_time * 31104000;       // 86400 * 30 * 12;
            sensor_time_speed_set(tempTime, 1);    // year
            break;
        case 2:
            tempTime = temp_time * 2592000;        // 86400 * 30;
            sensor_time_speed_set(tempTime, 1);    // month
            break;
        case 3:
            tempTime = temp_time * 86400;
            sensor_time_speed_set(tempTime, 1);    // day
            break;
        case 4:
            tempTime = temp_time * 3600;
            sensor_time_speed_set(tempTime, 1);    // h
            break;
        case 5:
            tempTime = temp_time * 60;
            sensor_time_speed_set(tempTime, 1);    // m
            break;
        case 6:
            tempTime = temp_time;
            sensor_time_speed_set(tempTime, 1);    // s
            break;
        default:
            break;
    }
}

/**************************************************************************************
 * Function Name  : protocol_simulate_event_handle
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_simulate_event_handle(uint8_t temp_type)
{
    if (system_status_get() != STATUS_START)
    {
        if ((temp_type == 0x01) 
         && ((system_status_get() == STATUS_NORMAL) || (system_status_get() == STATUS_INVALID)))
        {
            if (system_mode_get() == MODE_INDEPENDENT)
            {
                system_status_set(STATUS_FIRE);
            }
            FlagFireAlarm = 1;
        }
        else if ((temp_type == 0x02) && (system_status_get() == STATUS_NORMAL))
        {
            system_status_set(STATUS_SENSOR_TROUBLE);
        }
        else if (temp_type == 0x03)
        {
            sysFlag_sensor_invalid = 0;
            system_status_set(STATUS_NORMAL);         
        }
    }
}

/**************************************************************************************
 * Function Name  : protocol_version_inquire
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_version_inquire(void)
{
    uint8_t count = 0;
    uint8_t i = 0;

    answerData[count++] = FRAME_START_BYTE;
    answerData[count++] = FRAME_C1_NO_INQUIRE_BYTE;

    answerData[count++] = FRAME_C2_CUSTOM_VERSION_BYTE;
    count++;
    
    answerData[count++] = FS1022H;   

    for (i = 0; i < 3; i++)
    {
        answerData[count++] = SN_ver1[i];
    }
    for (i = 0; i < 9; i++)
    {
        answerData[count++] = SN_ver2[i];
    }
    answerData[count++] = (uint8_t)(date_year_get() / 100);
    answerData[count++] = (uint8_t)(date_year_get() % 100);
    answerData[count++] = (uint8_t)date_month_get();
    answerData[count++] = (uint8_t)date_day_get();

    answerData[3] = count - 4;

    answerData[count] = interface_check_sums(answerData, count);
    count++;
    answerData[count++] = FRAME_END_BYTE;

    usart0_dma_transmit(answerData, count);    //往外发送数据
}

/**************************************************************************************
 * Function Name  : protocol_version_inquire
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void protocol_demarcate_data(void)
{
    uint8_t  count         = 0;
    uint16_t buff_adc[11]  = {0};
    uint8_t i = 0;

    answerData[count++] = FRAME_START_BYTE;
    answerData[count++] = FRAME_C1_NO_INQUIRE_BYTE;
    answerData[count++] = FRAME_C2_CUSTOM_DEMARCATE_DATA;
    count++;

    answerData[count++] = (uint8_t)(sample_average_value_get() >> 8);
    answerData[count++] = (uint8_t)(sample_average_value_get());
    answerData[count++] = (uint8_t)(system_lel_zero_get() >> 8);
    answerData[count++] = (uint8_t)(system_lel_zero_get());

    for (i = 0; i < 11; i++)
    {
        buff_adc[i] = calibration_adc_get(i);
    }
    for (i = 1; i < 11; i++)
    {
        answerData[count++] = (uint8_t)(buff_adc[i] >> 8);
        answerData[count++] = (uint8_t)(buff_adc[i]);
    }

    answerData[3] = count - 4;

    answerData[count] = interface_check_sums(answerData, count);
    count++;
    answerData[count++] = FRAME_END_BYTE;

    usart0_dma_transmit(answerData, count);    //往外发送数据
}

/**************************************************************************************
 * Function Name  : protocol_handle
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void protocol_handle(void)
{
    uint8_t buff[PROTOCOL_RECEIVE_DATA_MAX] = {0};

    if (protocol_read_frame(buff))
    {
        switch (buff[2])
        {
            case FRAME_C2_INQUIRE_ALL_TOTAL_BYTE:
                protocol_answer_history_total();
                break;
            case FRAME_C2_INQUIRE_ALARM_BYTE:
                protocol_answer_event_inquire(buff[1], type_alarm);
                break;
            case FRAME_C2_INQUIRE_ALARM_NORMAL_BYTE:
                protocol_answer_event_inquire(buff[1], type_alarm_recovery);
                break;
            case FRAME_C2_INQUIRE_FAULT_BYTE:
                protocol_answer_event_inquire(buff[1], type_fault);
                break;
            case FRAME_C2_INQUIRE_FAULT_NORMAL_BYTE:
                protocol_answer_event_inquire(buff[1], type_fault_recovery);
                break;
            case FRAME_C2_INQUIRE_POWER_DOWN_BYTE:
                protocol_answer_event_inquire(buff[1], type_power_down);
                break;
            case FRAME_C2_INQUIRE_POWER_UP_BYTE:
                protocol_answer_event_inquire(buff[1], type_power_up);
                break;
            case FRAME_C2_INQUIRE_SENSOR_INVALID_BYTE:
                protocol_answer_event_sensor_inquire();
                break;
            case FRAME_C2_INQUIRE_TIME_BYTE:
                protocol_answer_time_inquire(buff[1], FRAME_C2_INQUIRE_TIME_BYTE, 0);
                break;
            case FRAME_C2_CUSTOM_TIME_BYTE:
                protocol_answer_time_inquire(buff[1], FRAME_C2_CUSTOM_TIME_BYTE, 1);
                break;
            case FRAME_C2_CUSTOM_SPEED_UP_BYTE:
                protocol_sensor_speed_handle(buff[4], buff[5]);
                break;
            case FRAME_C2_CUSTOM_SIMULATE_BYTE:
                protocol_simulate_event_handle(buff[4]);
                break;
            case FRAME_C2_CUSTOM_VERSION_BYTE:
                protocol_version_inquire();
                break;
            case FRAME_C2_CUSTOM_DEMARCATE_DATA:
                protocol_demarcate_data();
                break;
            default:
                break;
        }
    }

    if (pro_recv.time_cnt < PROTOCOL_TIME_OUT)
    {
        pro_recv.time_cnt++;
        if (pro_recv.time_cnt >= PROTOCOL_TIME_OUT)
        {
            pro_recv.time_cnt   = PROTOCOL_TIME_OUT;
            pro_recv.recv_cnt = 0;
            pro_recv.check_sum  = 0;
        }
    }
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
