/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : RTC.h
* Author         : dy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------------*/
#define RTC_TIME_IS_OK              RTC_BKP0
#define RTC_POWER_DOWN_TIME         RTC_BKP1
#define RTC_PROBE_IS_ALARM_TIME     RTC_BKP2
#define RTC_PROBE_IS_FAULT_TIME     RTC_BKP3
#define RTC_TIME_CONFIRM            (0xAAAA5555)

/* Exported functions ---------------------------------------------------------------*/
void system_rtc_init(void);
void rtc_time_modify(uint8_t *time);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
