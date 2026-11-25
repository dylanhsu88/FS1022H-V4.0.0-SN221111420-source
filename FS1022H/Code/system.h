/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : system.h
* Author         : dy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include "fs_type.h"

/* Exported constants ---------------------------------------------------------------*/
#ifndef RTC_CLOCK_SOURCE_LXTAL
#define RTC_CLOCK_SOURCE_LXTAL // rtc clock
#endif


/* Exported types -------------------------------------------------------------------*/
typedef enum
{
    STATUS_START = 0,
    STATUS_NORMAL,
    STATUS_SHIELD,
    STATUS_SHIELD_REMOVE,
    STATUS_SENSOR_TROUBLE,
    STATUS_FIRE,
    STATUS_INVALID
} enum_status_t;

typedef enum
{
    MODE_BUS,
    MODE_INDEPENDENT,
} en_work_mode_t;

typedef struct
{
    uint16_t id_low;
    uint16_t id_high;

    uint16_t lel_conc[11]; // ÓëCAL_SECTION_NUM + 1Ò»ÖÂ
    uint16_t lel_adc[11];

    uint8_t parent_node;
    uint8_t address;
    uint16_t lel_zero;

    uint8_t mode; // 0 bus mode ; 1 independent mode 
    uint8_t unused[3];
} struct_system_t;

/* Exported macro -------------------------------------------------------------------*/

#define SYSTEM_DEFAULT_ADDRESS_MIN  (0x01)
#define SYSTEM_DEFAULT_ADDRESS_MAX  (0xFA)
#define SYSTEM_DEFAULT_ID_H         (0x1234)
#define SYSTEM_DEFAULT_ID_L         (0x5678)
#define SYSTEM_SENSOR_LIFE_TIME     (43800) // 5year * 365 day * 24 hour = 43800 hour

extern byte_bits_t system_flag1;
#define sysFlag_sensor_invalid      system_flag1.bits.bit0
#define sysFlag_write_addr          system_flag1.bits.bit1
#define sysFlag_write_addr_success  system_flag1.bits.bit2
#define sysFlag_reset               system_flag1.bits.bit3
#define sysFlag_selfcheck           system_flag1.bits.bit4
#define sysFlag_modify              system_flag1.bits.bit5
#define sysFlag_cali_error          system_flag1.bits.bit6
#define sysFlag_restart             system_flag1.bits.bit7

#define m_hex_force_to_dec(x)       ((x / 16 * 10) + (x % 16)) // 0x16 -> 16
#define m_dec_force_to_hex(x)       ((x / 10 * 16) + (x % 10)) // 16 -> 0x16

/* Exported functions ---------------------------------------------------------------*/

void system_init(void);
void system_handle(void);
void system_status_set(uint8_t status_value);
void system_lel_zero_set(uint16_t zero_value);
void system_lel_set(uint16_t lel_conc, uint16_t lel_adc, uint8_t lel_index);
void system_address_set(uint8_t addr);
void system_lel_get(uint16_t *lel_conc, uint16_t *lel_adc, uint8_t index);
uint16_t system_id_high_get(void);
uint16_t system_id_low_get(void);
uint16_t system_lel_zero_get(void);
uint8_t system_address_get(void);
uint8_t system_status_get(void);
uint8_t system_para_modify(void);

en_work_mode_t system_mode_get(void);
void system_mode_set(en_work_mode_t param);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_H__ */

/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
