/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : rtc.c
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

#include "rtc.h"
#include "system.h"
#include "systick.h"
#include "usart.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
static __IO uint32_t prescaler_a = 0; /* RTC 0x0-0x7F */
static __IO uint32_t prescaler_s = 0; /* RTC 0x0-0x7FFF */

#define RTC_SET_OK         (0x32F0)
#define RTC_DEFAULT_YEAR   (0x20)
#define RTC_DEFAULT_MONTH  (RTC_MAY)
#define RTC_DEFAULT_DAY    (0x20)
#define RTC_DEFAULT_WEEK   (RTC_WEDSDAY)
#define RTC_DEFAULT_HOUR   (0x09)
#define RTC_DEFAULT_MINUTE (0x16)
#define RTC_DEFAULT_SECOND (0x52)
#define RTC_TIMER_FORMAT   (RTC_24HOUR)
#define RTC_TIMER_AM_PM    (RTC_AM)

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
 * Function Name  : rtc_time_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void rtc_time_init(void)
{
    rtc_parameter_struct rtc_initpara;

    /* setup RTC time value */

    rtc_initpara.rtc_factor_asyn    = prescaler_a;
    rtc_initpara.rtc_factor_syn     = prescaler_s;
    rtc_initpara.rtc_year           = RTC_DEFAULT_YEAR;
    rtc_initpara.rtc_day_of_week    = RTC_SATURDAY;
    rtc_initpara.rtc_month          = RTC_DEFAULT_MONTH;
    rtc_initpara.rtc_date           = RTC_DEFAULT_DAY;
    rtc_initpara.rtc_display_format = RTC_TIMER_FORMAT;
    rtc_initpara.rtc_am_pm          = RTC_TIMER_AM_PM;
    rtc_initpara.rtc_hour           = RTC_DEFAULT_HOUR;
    rtc_initpara.rtc_minute         = RTC_DEFAULT_MINUTE;
    rtc_initpara.rtc_second         = RTC_DEFAULT_SECOND;

    /* RTC current time configuration */
    rtc_init(&rtc_initpara);

    RTC_TIME_IS_OK = RTC_SET_OK;
}

/**************************************************************************************
 * Function Name  : rtc_pre_config
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
static void rtc_pre_config(void)
{
#if defined(RTC_CLOCK_SOURCE_IRC40K)
    rcu_osci_on(RCU_IRC40K);
    rcu_osci_stab_wait(RCU_IRC40K);
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);

    prescaler_s = 0x18F;
    prescaler_a = 0x63;
#elif defined(RTC_CLOCK_SOURCE_LXTAL)
    rcu_osci_on(RCU_LXTAL);
    rcu_osci_stab_wait(RCU_LXTAL);
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    prescaler_s = 0xFF;
    prescaler_a = 0x7F;
#else
#error RTC clock source should be defined.
#endif

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/**************************************************************************************
 * Function Name  : system_rtc_init
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void system_rtc_init(void)
{
    /* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    rtc_pre_config();
    /* check if RTC has aready been configured */
    if (RTC_SET_OK != RTC_TIME_IS_OK)
    {
        rtc_time_init();
        RTC_POWER_DOWN_TIME     = 0;
        RTC_PROBE_IS_ALARM_TIME = 0;
        RTC_PROBE_IS_FAULT_TIME = 0;
    }

    rcu_all_reset_flag_clear();
    pmu_backup_write_disable();
}

/**************************************************************************************
 * Function Name  : rtc_time_modify
 * Description    :
 * Input          :
 * Output         :
 * Return         :
 **************************************************************************************/
void rtc_time_modify(uint8_t* time)
{
    uint8_t i = 0;

    rtc_parameter_struct rtc_initpara;

    pmu_backup_write_enable();

    /* setup RTC time value */

    for (i = 0; i < 7; i++)
    {
        time[4 + i] = m_dec_force_to_hex(time[4 + i]);
    }

    rtc_initpara.rtc_factor_asyn    = prescaler_a;
    rtc_initpara.rtc_factor_syn     = prescaler_s;
    rtc_initpara.rtc_year           = time[5];    
    rtc_initpara.rtc_month          = time[6];
    rtc_initpara.rtc_date           = time[7];
    rtc_initpara.rtc_display_format = RTC_TIMER_FORMAT;
    rtc_initpara.rtc_hour           = time[8];
    rtc_initpara.rtc_minute         = time[9];
    rtc_initpara.rtc_second         = time[10];
    // rtc_initpara.rtc_day_of_week    = time[11];

    if (((rtc_initpara.rtc_hour * 3600) + (rtc_initpara.rtc_minute * 60) + rtc_initpara.rtc_second) <= (12 * 3600))
    {
        rtc_initpara.rtc_am_pm = RTC_AM;
    }
    else
    {
        rtc_initpara.rtc_am_pm = RTC_PM;
    }

    /* RTC current time configuration */
    rtc_init(&rtc_initpara);

    // rtc_show_time();

    RTC_TIME_IS_OK = RTC_SET_OK;

    pmu_backup_write_disable();
}
/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
