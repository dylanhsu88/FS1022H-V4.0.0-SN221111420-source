/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : flash_fmc.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __FLASH_FMC_H__
#define __FLASH_FMC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/

#define FMC_PAGE_SIZE               ((uint16_t)0x400U)      // erase size 1k
#define FMC_PARA_ADDRESS            ((uint32_t)0x800FC00U)  // 64K

#define FMC_SENSOR_ADDRESS          ((uint32_t)0x800F800U)  // (1k)
#define FMC_POWER_UP_ADDRESS        ((uint32_t)0x800F400U)  // (1k)
#define FMC_POWER_DOWN_ADDRESS      ((uint32_t)0x800F000U)  // (1k)
#define FMC_FAULT_RECOVERY_ADDRESS  ((uint32_t)0x800EC00U)  // (1k)
#define FMC_FAULT_ADDRESS           ((uint32_t)0x800E800U)  // (1k)
#define FMC_ALARM_RECOVERY_ADDRESS  ((uint32_t)0x800E400U)  // (1K)
#define FMC_ALARM_ADDRESS           ((uint32_t)0x800E000U)  // (1K)

#define FMC_HISTORY_END_ADDR        ((uint32_t)0x800FC00U)  // (1K)
#define FMC_HISTORY_START_ADDR      ((uint32_t)0x800E000U)  // (1K)  history event size 7k

#define FMC_SENSOR_TIME_ADDRESS     ((uint32_t)0x800DC00U)  // (1K)
#define FMC_MODIFY_ADDRESS          ((uint32_t)0x800D800U)  // (1k)

#define FMC_DEFAULT_16_VALUE        (0xFFFF)
#define FMC_DEFAULT_32_VALUE        (0xFFFFFFFF)

/* Exported macro -------------------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------------*/
void flash_fmc_program(uint32_t tempAddress, uint32_t *tempData, uint32_t tempLength);
uint8_t flash_fmc_program_check(uint32_t dest_addr, uint32_t *sour_addr, uint32_t tempLength);
void flash_fmc_erase_Npage(uint32_t tempAddress, uint32_t tempCount);
void flash_fmc_erase_Npage_check(uint32_t tempAddress, uint32_t tempCount);
void flash_fmc_read(uint32_t tempAddress, uint32_t *tempData, uint32_t tempLength);
void flash_fmc_overload(uint32_t tempAddress, uint32_t destAddress, uint32_t tempLength);
void flash_fmc_read_halfword(uint32_t source_addr, uint16_t *dest_addr, uint32_t length);
void flash_fmc_program_halfword(uint32_t source_addr, uint16_t *dest_addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_FMC_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
