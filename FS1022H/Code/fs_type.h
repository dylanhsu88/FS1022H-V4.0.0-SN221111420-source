/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : fs_type.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once
#ifndef __FS_TYPE_H__
#define __FS_TYPE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include <stdint.h>
/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
// typedef signed char     int8_t;
// typedef signed int      int16_t;
// typedef signed long     int32_t;
// typedef unsigned char   uint8_t;
// typedef unsigned int    uint16_t;
// typedef unsigned long   uint32_t;

typedef union
{
    struct
    {
        uint8_t bit0 : 1;
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
    } bits;
    uint8_t byte;
} byte_bits_t;

typedef union
{
    struct
    {
        uint8_t uc_ll;
        uint8_t uc_l;
        uint8_t uc_m;
        uint8_t uc_h;
    } byte;

    struct
    {
        uint16_t us_l;
        uint16_t us_h;

    } half_word;
    uint32_t ul_word;
} word_byte_t;

/* Exported macro -------------------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __FS_TYPE_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
