/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : .c
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "history.h"
#include "flash_fmc.h"
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"
#include "protocol.h"
#include "rtc.h"
#include "system.h"
#include "usart.h"

/* Private typedef ------------------------------------------------------------------*/
HistoryTime_t timer = {0};
uint8_t event_total[type_max] = {0};
const static uint8_t event_max[type_max] = {HISTORY_ALARM_MAX, 
                                            HISTORY_ALARM_RECOVERY_MAX, 
                                            HISTORY_FAULT_MAX, 
                                            HISTORY_FAULT_RECOVERY_MAX, 
                                            HISTORY_POWER_DOWN_MAX, 
                                            HISTORY_POWER_UP_MAX,
                                            HISTORY_SENSOR_MAX};

const static uint32_t event_first_addr[type_max] = {FMC_ALARM_ADDRESS,
                                                    FMC_ALARM_RECOVERY_ADDRESS,
                                                    FMC_FAULT_ADDRESS,
                                                    FMC_FAULT_RECOVERY_ADDRESS,
                                                    FMC_POWER_DOWN_ADDRESS,
                                                    FMC_POWER_UP_ADDRESS,
                                                    FMC_SENSOR_ADDRESS};

/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
 * Function Name  : history_event_add
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void history_event_add(enum_event_type eventType, uint32_t now_time)
{
    uint32_t temp_address = 0;
    rtc_parameter_struct rtc_initpara;

    switch (eventType)
    {
        case type_alarm:
            pmu_backup_write_enable();
            RTC_PROBE_IS_ALARM_TIME = RTC_TIME_CONFIRM;
            pmu_backup_write_disable();
            break;
        case type_alarm_recovery:
            pmu_backup_write_enable();
            RTC_PROBE_IS_ALARM_TIME = 0;
            pmu_backup_write_disable();
            break;
        case type_fault:
            pmu_backup_write_enable();
            RTC_PROBE_IS_FAULT_TIME = RTC_TIME_CONFIRM;
            pmu_backup_write_disable();
            break;
        case type_fault_recovery:
            pmu_backup_write_enable();
            RTC_PROBE_IS_FAULT_TIME = 0;
            pmu_backup_write_disable();
            break;
        case type_power_down:
            break;
        case type_power_up:
            break;
        case type_invalid:
            break;
        default:
            break;
    }

    eventType -= 1;

    if (event_total[eventType] >= event_max[eventType])
    {
        flash_fmc_erase_Npage(FMC_MODIFY_ADDRESS, 1);
        flash_fmc_overload(event_first_addr[eventType], FMC_MODIFY_ADDRESS, FMC_PAGE_SIZE / 4);
        flash_fmc_erase_Npage(event_first_addr[eventType], 1);
        flash_fmc_overload(FMC_MODIFY_ADDRESS + (1 * 4), event_first_addr[eventType], event_max[eventType] - 1);
        flash_fmc_erase_Npage(FMC_MODIFY_ADDRESS, 1);
        event_total[eventType]  -= 1;
    }

    temp_address = (event_total[eventType] * HISTORY_ADDRESS_SIZE) + ((eventType * FMC_PAGE_SIZE) + FMC_HISTORY_START_ADDR);
    
    if (now_time == 0)
    {
        rtc_current_time_get(&rtc_initpara);
        timer.time.year   = m_hex_force_to_dec(rtc_initpara.rtc_year);
        timer.time.month  = m_hex_force_to_dec(rtc_initpara.rtc_month);
        timer.time.day    = m_hex_force_to_dec(rtc_initpara.rtc_date);
        timer.time.hour   = m_hex_force_to_dec(rtc_initpara.rtc_hour);
        timer.time.minute = m_hex_force_to_dec(rtc_initpara.rtc_minute);
    }

    flash_fmc_program(temp_address, (uint32_t*)&timer.ulTime, 1);
    flash_fmc_program_check(temp_address, (uint32_t*)&timer.ulTime, 1);

    event_total[eventType] += 1;
}

/**************************************************************************************
 * Function Name  : history_event_total_get
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
uint16_t history_event_total_get(enum_event_type eventType)
{
    return event_total[eventType - 1];
}

/**************************************************************************************
 * Function Name  :history_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void history_init(void)
{
    uint32_t temp_alarm_time = 0;
    uint32_t temp_fault_time = 0;
    uint32_t temp_data    = 0;
    uint32_t temp_address = 0;

    uint8_t i = 0;
    uint16_t j = 0;

    usart0_init();
    protocol_init();

    memset(&event_total, 0, sizeof(event_total));

    for (i = 0; i < sizeof(event_total); i++)
    {
        temp_address = event_first_addr[i];

        for (j = 0; j < event_max[i]; j++)
        {
            flash_fmc_read(temp_address, &temp_data, 1);

            if (temp_data != 0xFFFFFFFFU)
            {
                event_total[i] += 1;
            }
            temp_address += HISTORY_ADDRESS_SIZE;
        }
    }
    
    history_event_add(type_power_up, 0);

    pmu_backup_write_enable();
    timer.ulTime            = RTC_POWER_DOWN_TIME;
    temp_alarm_time         = RTC_PROBE_IS_ALARM_TIME;
    RTC_PROBE_IS_ALARM_TIME = 0;
    temp_fault_time         = RTC_PROBE_IS_FAULT_TIME;
    RTC_PROBE_IS_FAULT_TIME = 0;
    pmu_backup_write_disable();

    if (timer.ulTime != 0)
    {
        history_event_add(type_power_down, timer.ulTime);

        if (temp_alarm_time == RTC_TIME_CONFIRM)
        {
            history_event_add(type_alarm_recovery, temp_alarm_time);
        }
        if (temp_fault_time == RTC_TIME_CONFIRM)
        {
            history_event_add(type_fault_recovery, temp_fault_time);
        }
    }
}

/**************************************************************************************
 * Function Name  :history_power_down_record
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void history_power_down_record(void)
{
    rtc_parameter_struct rtc_initpara;

    pmu_backup_write_enable();

    rtc_current_time_get(&rtc_initpara);
    timer.time.year   = m_hex_force_to_dec(rtc_initpara.rtc_year);
    timer.time.month  = m_hex_force_to_dec(rtc_initpara.rtc_month);
    timer.time.day    = m_hex_force_to_dec(rtc_initpara.rtc_date);
    timer.time.hour   = m_hex_force_to_dec(rtc_initpara.rtc_hour);
    timer.time.minute = m_hex_force_to_dec(rtc_initpara.rtc_minute);

    RTC_POWER_DOWN_TIME = timer.ulTime;

    pmu_backup_write_disable();
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
