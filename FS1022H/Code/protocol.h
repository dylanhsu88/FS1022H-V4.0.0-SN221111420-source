/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : protocol.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-05-08
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
typedef enum
{
    FS3023H = 0x01,
    FS3023HL = 0x02,
    FS3023HNB = 0x03,
    FS1022H = 0x04
} enum_dev_type_t;

// frame format is  (start frame(0xAA) + control frame 1 + control frame 2 + length frame + check frame + end frame(0x55))

#define FRAME_START_BYTE                        (0xAA) /* 帧开始 */
#define FRAME_C1_NO_INQUIRE_BYTE                (0x00) /* 非数据记录查询 当为1-255时查询对应的记录 */
#define FRAME_C2_INQUIRE_ALL_TOTAL_BYTE         (0x00) /* 查询各类记录总数 */
#define FRAME_C2_INQUIRE_ALARM_BYTE             (0x01) /* 查询第n条探测器报警记录 */
#define FRAME_C2_INQUIRE_ALARM_NORMAL_BYTE      (0x02) /* 查询第n条探测器报警恢复记录 */
#define FRAME_C2_INQUIRE_FAULT_BYTE             (0x03) /* 查询第n条探测器故障记录 */
#define FRAME_C2_INQUIRE_FAULT_NORMAL_BYTE      (0x04) /* 查询第n条探测器故障恢复记录 */
#define FRAME_C2_INQUIRE_POWER_DOWN_BYTE        (0x05) /* 查询第n条探测器掉电记录 */
#define FRAME_C2_INQUIRE_POWER_UP_BYTE          (0x06) /* 查询第n条探测器上电记录 */
#define FRAME_C2_INQUIRE_SENSOR_INVALID_BYTE    (0x07) /* 查询探测器气体传感器失效记录 */
#define FRAME_C2_INQUIRE_TIME_BYTE              (0x08) /* 查询探测器内部计时器当前时间 */
#define FRAME_END_BYTE                          (0x55) /* 帧结束 */

#define FRAME_C2_CUSTOM_SNTP_BYTE               (0x09) /* 自定义字节，时间同步 */
#define FRAME_C2_CUSTOM_SPEED_UP_BYTE           (0x10) /* 传感器寿命加速计时 */
#define FRAME_C2_CUSTOM_SIMULATE_BYTE           (0x11) /* 模拟火警，火警恢复，故障，故障恢复 */
#define FRAME_C2_CUSTOM_VERSION_BYTE            (0x12) /* 自定义字节，版本 */
#define FRAME_C2_CUSTOM_TIME_BYTE               (0x18) /* 查询探测器内部计时器当前时间+秒 */
#define FRAME_C2_CUSTOM_DEMARCATE_ZERO_BYTE     (0x34) /* 自定义字节，调零 */
#define FRAME_C2_CUSTOM_DEMARCATE_ALARM_BYTE    (0x35) /* 自定义字节，标定 */
#define FRAME_C2_CUSTOM_DEMARCATE_DATA          (0x36) /* 数据查询 */

#define PROTOCOL_TIME_OUT                       (20) // uint:1ms
#define PROTOCOL_RECEIVE_DATA_MAX               (20)
#define PROTOCOL_RECEIVE_DATA_SIZE              (5)

/* Exported macro -------------------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------------*/
void protocol_init(void);
void protocol_handle(void);
void protocol_receive_data(uint8_t recData);

uint8_t date_month_get(void);
uint8_t date_day_get(void);
uint16_t date_year_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __PROTOCOL_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
