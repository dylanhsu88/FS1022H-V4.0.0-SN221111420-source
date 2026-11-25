/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : history.h
 * Author         : ldy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __HISTORY_H__
#define __HISTORY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Include --------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants ---------------------------------------------------------------*/

typedef enum
{
    type_alarm          = 1,
    type_alarm_recovery = 2,
    type_fault          = 3,
    type_fault_recovery = 4,
    type_power_down     = 5,
    type_power_up       = 6,
    type_invalid        = 7,
    type_max            = 7
} enum_event_type;

typedef struct
{
    uint16_t usYear;
    uint8_t  ucDay;
    uint8_t  ucMonth;
    uint8_t  ucMinute;
    uint8_t  ucHour;
} struct_HistoryEventTime;

typedef union
{
    struct
    {
        uint32_t year   : 12;   // max value 4095
        uint32_t month  : 4;    // max value 15
        uint32_t day    : 5;    // max value 31
        uint32_t hour   : 5;    // max value 31
        uint32_t minute : 6;    // max value 63
    }time;

    uint32_t ulTime;
} HistoryTime_t;

/* Exported types -------------------------------------------------------------------*/
#define HISTORY_ALARM_MAX           (250)
#define HISTORY_ALARM_RECOVERY_MAX  (250)
#define HISTORY_FAULT_MAX           (200)
#define HISTORY_FAULT_RECOVERY_MAX  (200)
#define HISTORY_POWER_DOWN_MAX      (100)
#define HISTORY_POWER_UP_MAX        (100)
#define HISTORY_SENSOR_MAX          (1)    // gb15322.2-2019 有且只有一条

#define HISTORY_ALARM_DEC           (1)
#define HISTORY_ALARM_RECOVERY_DEC  (1)
#define HISTORY_FAULT_DEC           (1)
#define HISTORY_FAULT_RECOVERY_DEC  (1)
#define HISTORY_POWER_DOWN_DEC      (1)
#define HISTORY_POWER_UP_DEC        (1)
#define HISTORY_SENSOR_DEC          (1)

#define HISTORY_ADDRESS_SIZE        ((uint8_t)0x4U)


/* Exported macro -------------------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------------*/
void history_event_add(enum_event_type eventType, uint32_t now_time);
uint16_t history_event_total_get(enum_event_type eventType);
void history_power_down_envet_add(void);
void history_power_down_record(void);
void history_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __HISTORY_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
