/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : two_wire.h
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-05-08
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __TWO_WIRE_H__
#define __TWO_WIRE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include --------------------------------------------------------------------------*/
#include "fs_type.h"

/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/
/* Two Wire Command -----------------------------------------------------------------*/
#define ADDR_BROADCAST                  (uint8_t)(0xFB)    /* 公用的地址  */
#define CMD_DATE                        (uint8_t)(0xD1)    /* 广播月日 */
#define CMD_TIME                        (uint8_t)(0xD2)    /* 广播时分 */
#define CMD_YEAR                        (uint8_t)(0xD3)    /* 广播年 */
#define CMD_RESET                       (uint8_t)(0xD4)    /* 复位 */
#define CMD_TURN_ON_LED                 (uint8_t)(0xD5)    /* 广播点探头模块灯 */
#define CMD_TURN_OFF_LED                (uint8_t)(0xD6)    /* 广播关闭探头模块灯 */
#define CMD_CHANGE_SENSITIVITY          (uint8_t)(0xD7)    /* 广播灵敏度 */
#define CMD_WRITE_THRESHOLD             (uint8_t)(0xD8)    /* 写探头阈值 */
#define CMD_WRITE_INITIAL               (uint8_t)(0xD9)    /* 写探头初值 */
#define CMD_WRITE_ID_H                  (uint8_t)(0xDA)    /* 写高16位ID */
#define CMD_WRITE_ID_L                  (uint8_t)(0xDB)    /* 写低16位ID */
#define CMD_RESERVED                    (uint8_t)(0xDC)    /* 空操作保留指令 */
#define CMD_DIAPLAY_ADDR                (uint8_t)(0xDD)    /* LED显示地址指令 */
#define CMD_RESTART                     (uint8_t)(0xDE)    /* FS6000重启功能  */
#define CMD_ENTER_DEBUG                 (uint8_t)(0xDF)

#define CMD_PRIORITY_FIRE_ALARM         (uint8_t)(0xE1)    /* 火警优先权处理 */
#define CMD_PRIORITY_ID_HIGH            (uint8_t)(0xE2)    /* 高16位ID优先权处理 */
#define CMD_PRIORITY_ID_LOW             (uint8_t)(0xE3)    /* 低16位ID优先权处理 */
#define CMD_PRIORITY_ID_UNDATE          (uint8_t)(0xE4)    /* ID优先权重新有用命令,已经抢过ID的无法再抢ID，需要此命令重新拥有优先权 */
#define CMD_PRIORITY_CON                (uint8_t)(0xE7)    /* 浓度抢线 */

#define CMD_POLLING                     (uint8_t)(0x91)    /* 二线制轮询命令 */
#define CMD_POLLING_TROUBLE             (uint8_t)(0x92)    /* 巡检故障指令 */
#define CMD_POLLING_FIRE_ALARM          (uint8_t)(0x93)    /* 巡检取火警指令 */
#define CMD_FIRE_POLLING_AFFIRM         (uint8_t)(0xA1)    /* 火警确认 */

#define CMD_READ_THRESHOLD              (uint8_t)(0x51)    /* 读阈值指令 */
#define CMD_READ_INITIAL                (uint8_t)(0x52)    /* 读初值指令 */
#define CMD_READ_PULSEWIDTH             (uint8_t)(0x53)    /* 读脉宽指令 */
#define CMD_READ_DEV_TYPE               (uint8_t)(0x54)    /* 读设备类型 */
#define CMD_READ_ADDR                   (uint8_t)(0x55)    /* 读地址指令 */
#define CMD_READ_ID_H                   (uint8_t)(0x56)    /* 读高16位ID */
#define CMD_READ_ID_L                   (uint8_t)(0x57)    /* 读低16位ID */
#define CMD_DISPLAY_SINGLE_ID           (uint8_t)(0x58)    /* 一个设备显示id */
#define CMD_READ_FAULT_STATUS           (uint8_t)(0x61)    /* 读状态指令 */

#define CMD_WRITE_LOCAL_THRESHOLD       (uint8_t)(0x11)     /* 写阈值指令 */
#define CMD_WRITE_LOCAL_ADDR            (uint8_t)(0x12)    /* 写地址 */
#define CMD_WRITE_LOCAL_INITIAL         (uint8_t)(0x13)    /* 写初值 */
#define CMD_WRITE_ADDR_BY_ID_H          (uint8_t)(0x17)    /* 通过id写地址 */
#define CMD_WRITE_ADDR_BY_ID_L          (uint8_t)(0x18)    /* 通过id写地址 */
#define CMD_WRITE_ADDR_BY_ID_START      (uint8_t)(0x19)    
#define CMD_WRITE_ADDR_BY_ID_AFFIRM     (uint8_t)(0x1A)     /* 通过id写地址后，巡检确认指令 */
#define CMD_SHIELD                      (uint8_t)(0x21)    /* 屏蔽地址 */
#define CMD_SHIELD_REMOVE               (uint8_t)(0x22)    /* 解除屏蔽 */
#define CMD_PROBE_CONFIRM               (uint8_t)(0x23)    /* 探头火警确认 */
#define CMD_PREALARM_AFFIRM             (uint8_t)(0x24)    /* 预警确认 */
#define CMD_LAUNCH                      (uint8_t)(0x25)    /* 启动模块 */
#define CMD_MOUDLE_CONFIRM              (uint8_t)(0x26)    /* 模块动作确认 */
#define CMD_SINGLE_ADDR_RESET           (uint8_t)(0x27)    /* 单地址复位 */
#define CMD_DEBUG                       (uint8_t)(0xDB)    /* 模拟报警 */

#define CMD_DEMARCATE_ZERO              (uint8_t)(0x34)    /* 标定零值 */
#define CMD_DEMARCATE_ALARM             (uint8_t)(0x35)    /* 标定报警值 */
#define CMD_READ_REAL_VLAUE             (uint8_t)(0x31)    //读取实时采集的AD值
#define CMD_READ_ZERO_VLAUE             (uint8_t)(0x32)    //读取FLASH标定的零值
#define CMD_READ_DEMARCATE_VLAUE        (uint8_t)(0x36)    //读取FLASH标定的报警值
#define CMD_READ_SENSOR_STATUS          (uint8_t)(0x38)    /* 读取传感器得失效/寿命状态 */

/* Exported Gpio -------------------------------------------------------------------*/

#define TWO_WIRE_MODULE_PROBE_LIMIT (16)     // 脉宽1ms为0，0为探头，脉宽2.5ms为1，1为模块
#define TWO_WIRE_TYPE_MOUDLE        (1)      // 当前类型为模块
#define TWO_WIRE_TYPE_PROBE         (0)      // 当前类型为探头
#define TWO_WIRE_ABNORMAL_EXIT_TIME (60)     // 脉宽计时超过6MS退出




#define TWO_WIRE_SENSOR_INVALID     (0x1A1A)  
#define TWO_WIRE_SENSOR_FAULT       (0x1616)  

/* Exported gpio -------------------------------------------------------------------*/
#define TWO_WIRE_GPIO_R24V_CLK              (RCU_GPIOA)
#define TWO_WIRE_GPIO_R24V_PORT             (GPIOA)
#define TWO_WIRE_GPIO_R24V_PIN              (GPIO_PIN_3)
#define TWO_WIRE_GPIO_R24V_IRQ              (EXTI2_3_IRQn)
#define TWO_WIRE_GPIO_R24V_EXTI             (EXTI_3)
#define TWO_WIRE_GPIO_R24V_TRIG_MODE        (EXTI_TRIG_BOTH)
#define TWO_WIRE_GPIO_R24V_EXTI_SOURCE_PORT (EXTI_SOURCE_GPIOA)
#define TWO_WIRE_GPIO_R24V_EXTI_SOURCE_PIN  (EXTI_SOURCE_PIN3)
#define TWO_WIRE_GPIO_R24V_READ             (gpio_input_bit_get(TWO_WIRE_GPIO_R24V_PORT, TWO_WIRE_GPIO_R24V_PIN))

#define TWO_WIRE_GPIO_FEEDBACK_CLK          (RCU_GPIOA)
#define TWO_WIRE_GPIO_FEEDBACK_PORT         (GPIOA)
#define TWO_WIRE_GPIO_FEEDBACK_PIN          (GPIO_PIN_2)
#define m_feedback_close()                  (GPIO_BOP(TWO_WIRE_GPIO_FEEDBACK_PORT) = TWO_WIRE_GPIO_FEEDBACK_PIN)
#define m_feedback_open()                   (GPIO_BC(TWO_WIRE_GPIO_FEEDBACK_PORT) = TWO_WIRE_GPIO_FEEDBACK_PIN)

/* Exported Flag -------------------------------------------------------------------*/
extern volatile byte_bits_t twoWireFlag1;
extern volatile byte_bits_t twoWireFlag2;
extern volatile byte_bits_t twoWireFlag3;

#define FlagReportFeedbackDataBit      twoWireFlag1.bits.bit0 // 已经拉反馈的电平，代表着即将接收的脉宽大小，0代表2.5ms脉宽，1代表1ms脉宽
// #define FlagAlreadyRecFeedbackDataBit   twoWireFlag1.bits.bit1 // 目前已经接收的反馈脉宽
#define FlagIDPriorityUpdate            twoWireFlag1.bits.bit2 // ID优先权重新更新
#define FlagPriorityPreemption          twoWireFlag1.bits.bit3 // 优先抢占权（抢ID, 抢火警）
#define FlagIDHighMatchOK               twoWireFlag1.bits.bit4 // 高16位ID 匹配成功标志位
#define FlagIDLowMatchOK                twoWireFlag1.bits.bit5 // 低16位ID 匹配成功标志位
#define FlagWriteAddress                twoWireFlag1.bits.bit6 // 写地址标志位
#define FlagWriteAddressSuccess         twoWireFlag1.bits.bit7 // 写地址成功标志位

#define FlagFireAlarm                   twoWireFlag2.bits.bit0 // 火警抢线标志
#define FlagFeedback34Bit               twoWireFlag2.bits.bit1 // 第34位是否需要拉反馈处理
#define FlagAllowDisplayAddress         twoWireFlag2.bits.bit2 // 允许带显示盘功能的显示地址
#define FlagDemarcateZero               twoWireFlag2.bits.bit3 // 允许标定零值
#define FlagDemarcateZeroSuccess        twoWireFlag2.bits.bit4 // 允许标定零值成功
#define FlagDemarcateAlarm              twoWireFlag2.bits.bit5 // 允许标定报警值
#define FlagDemarcateAlarmSuccess       twoWireFlag2.bits.bit6 // 允许标定报警值成功
#define FlagReset                       twoWireFlag2.bits.bit7 // 复位标志

#define FlagPriorityConcentration       twoWireFlag3.bits.bit0 // 浓度抢线
#define FlagFireAlarmSuccess            twoWireFlag3.bits.bit1 
#define FlagIDHighReport                twoWireFlag3.bits.bit2 
#define FlagIDLowReport                 twoWireFlag3.bits.bit3
#define FlagIDHighSeize                 twoWireFlag3.bits.bit4


/* Exported functions ---------------------------------------------------------------*/
void twoWire_init(void);
void twoWire_falling_edge_handle(void);
void twoWire_rising_edge_handle(void);
uint8_t twoWire_is_idle(void);
void EXTI2_3_IRQHandler(void);
void TIMER15_IRQHandler(void);
void TIMER13_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __TWO_WIRE_H__ */

/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
