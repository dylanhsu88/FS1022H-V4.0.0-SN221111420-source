/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
 * File Name      : sensor.h
 * Author         : dy
 * Version        : V 4.0.0
 * Date           : 2020-09-16
 * Description    :
 * Vision History : NO.   Date    Revised by Description
 ***************************************************************************************
 **************************************************************************************/
/* Define to prevent recursive inclusion --------------------------------------------*/
#ifndef __SENSOR_H__
#define __SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Include --------------------------------------------------------------------------*/
/* Exported constants ---------------------------------------------------------------*/
/* Exported types -------------------------------------------------------------------*/

/* Exported macro -------------------------------------------------------------------*/

/* Exported functions ---------------------------------------------------------------*/
void sensor_time_init(void);
void sensor_time_inc(void);
void sensor_time_simulate(void);
void sensor_time_speed_set(uint32_t temp_time, uint8_t enable_status);


#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H__ */
/* *************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
