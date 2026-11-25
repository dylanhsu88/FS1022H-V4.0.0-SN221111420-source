/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : system.c
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

#include "fs_type.h"
#include "system.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "buzzer.h"
#include "sample.h"
#include "systick.h"
#include "flash_fmc.h"
#include "history.h"
#include "protocol.h"
#include "rtc.h"
#include "sensor.h"
#include "relay.h"
#include "calibration.h"
#include "two_wire.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
byte_bits_t system_flag1 = {0};

static struct_system_t system;
static uint8_t system_status = STATUS_START;

/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
* Function Name  : system_lel_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_lel_get(uint16_t *lel_conc, uint16_t *lel_adc, uint8_t index)
{
    lel_conc[index] = system.lel_conc[index];
    lel_adc[index] = system.lel_adc[index];
}

/**************************************************************************************
* Function Name  : system_lel_set
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_lel_set(uint16_t lel_conc, uint16_t lel_adc, uint8_t lel_index)
{
    system.lel_conc[lel_index] = lel_conc;
    system.lel_adc[lel_index] = lel_adc;
}

/**************************************************************************************
* Function Name  : system_lel_zero_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint16_t system_lel_zero_get(void)
{
    return system.lel_zero;
}

/**************************************************************************************
* Function Name  : system_lel_zero_set
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_lel_zero_set(uint16_t zero_value)
{
    system.lel_zero = zero_value;
}

/**************************************************************************************
* Function Name  : system_status_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint8_t system_address_get(void)
{
    if ((system.address > SYSTEM_DEFAULT_ADDRESS_MAX) || (system.address < SYSTEM_DEFAULT_ADDRESS_MIN))
    {
        return SYSTEM_DEFAULT_ADDRESS_MIN;
    }

    return system.address;
}

/**************************************************************************************
* Function Name  : system_address_set
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_address_set(uint8_t addr)
{
    system.address = addr;
}

/**************************************************************************************
* Function Name  : system_status_set
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_status_set(uint8_t status_value)
{
    if (status_value == STATUS_START)
    {
        if (!sysFlag_restart)
        {
            buzzer_mode_set(BUZZER_ENABLE, BUZZER_AUTO_TIME, BUZZER_PARA_NO);
        }
        
        system_status = status_value;
    }
    if (system_status != status_value)
    {
        if (status_value == STATUS_SHIELD_REMOVE)
        {
            system_status = STATUS_NORMAL;
            return;
        }
        if (system_status == STATUS_SHIELD) 
        {
            return;
        }
        switch (status_value)
        {
            case STATUS_NORMAL:   
                switch (system_status)
                {
                    case STATUS_SENSOR_TROUBLE:
                        history_event_add(type_fault_recovery, 0);
                        if (sysFlag_sensor_invalid)
                        {
                            led_set(yellow_led, LED_ON, LED_INVALID_ON, LED_INVALID_OFF, LED_PARA_NO);
                            system_status = STATUS_INVALID;
                        }   
                        else
                        {                        
                            led_set(yellow_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                            buzzer_mode_set(BUZZER_DISABLE, BUZZER_PARA_NO, BUZZER_PARA_NO);
                            system_status = status_value;   
                            led_test_close();  
                        }
                        break;
                    case STATUS_FIRE:
                        history_event_add(type_alarm_recovery, 0);
                        led_set(red_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                        m_relay_close(); 
                        if (sysFlag_sensor_invalid)
                        {                   
                            buzzer_mode_set(BUZZER_ENABLE, BUZZER_FAULT_ON_TIME, BUZZER_FAULT_OFF_TIME);
                            system_status = STATUS_INVALID;
                        }       
                        else
                        {
                            buzzer_mode_set(BUZZER_DISABLE, BUZZER_PARA_NO, BUZZER_PARA_NO);
                            system_status = status_value;
                        }
                        break;
                    case STATUS_START:
                        if (system_mode_get() == MODE_INDEPENDENT) 
                        {
                            led_set(green_led, LED_ON, LED_POLLING_ON, LED_POLLING_OFF, LED_PARA_NO);
                        }
                        else
                        {
                            led_set(green_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                        }                        
                        led_set(red_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                        led_set(yellow_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO); 
                        system_status = status_value;
                        break;
                    case STATUS_INVALID:
                        led_set(yellow_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                        buzzer_mode_set(BUZZER_DISABLE, BUZZER_PARA_NO, BUZZER_PARA_NO);
                        system_status = status_value;
                        break;
                }
                break;

            case STATUS_SHIELD:
                system_status = status_value;
                led_set(green_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                led_set(red_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                led_set(yellow_led, LED_OFF, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                buzzer_mode_set(BUZZER_DISABLE, BUZZER_PARA_NO, BUZZER_PARA_NO);
                m_relay_close();
                break;

            case STATUS_SENSOR_TROUBLE:
                led_test_close();
                history_event_add(type_fault, 0);   
                led_set(yellow_led, LED_ON, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                buzzer_mode_set(BUZZER_ENABLE, BUZZER_FAULT_ON_TIME, BUZZER_FAULT_OFF_TIME); 
                system_status = status_value;
                break;

            case STATUS_FIRE:
                led_test_close();
                history_event_add(type_alarm, 0);
                led_set(red_led, LED_ON, LED_PARA_NO, LED_PARA_NO, LED_PARA_NO);
                buzzer_mode_set(BUZZER_ENABLE, BUZZER_FLASH_TIME, BUZZER_FLASH_TIME);
                m_relay_open();
                
                system_status = status_value;
                break;
            case STATUS_INVALID:   
                if (!sysFlag_sensor_invalid)
                {
                    history_event_add(type_invalid, 0);      
                    sysFlag_sensor_invalid = 1;
                }
                if (system_status != STATUS_SENSOR_TROUBLE)                
                {
                    led_set(yellow_led, LED_ON, LED_INVALID_ON, LED_INVALID_OFF, LED_PARA_NO);   
                }
                if ((system_status != STATUS_FIRE) && (system_status != STATUS_SENSOR_TROUBLE))
                {
                    led_test_close();
                    buzzer_mode_set(BUZZER_ENABLE, BUZZER_FAULT_ON_TIME, BUZZER_FAULT_OFF_TIME);
                    system_status = status_value;  
                }                                                      
                break;
            default:
                break;
        }           
    }
}

/**************************************************************************************
* Function Name  : system_status_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint8_t system_status_get(void)
{
    return system_status;
}

/**************************************************************************************
* Function Name  : system_status_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint16_t system_id_high_get(void)
{
    if (system.id_high == FMC_DEFAULT_16_VALUE)
    {
        return SYSTEM_DEFAULT_ID_H;
    }   

    return system.id_high;
}

/**************************************************************************************
* Function Name  : system_status_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint16_t system_id_low_get(void)
{
    if (system.id_low == FMC_DEFAULT_16_VALUE)
    {
        return SYSTEM_DEFAULT_ID_L;
    }

    return system.id_low;
}

/**************************************************************************************
* Function Name  : system_mode_get
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
en_work_mode_t system_mode_get(void)
{
    if (system.mode == (uint8_t)FMC_DEFAULT_16_VALUE)
    {
        return MODE_BUS;
    }   

    return system.mode;
}

/**************************************************************************************
* Function Name  : system_mode_set
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_mode_set(en_work_mode_t param)
{
    system.mode = param;
}


/**************************************************************************************
* Function Name  : system_para_modify
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint8_t system_para_modify(void)
{
    struct_system_t temp_data = {0};

    flash_fmc_read(FMC_PARA_ADDRESS, (uint32_t *)&temp_data, sizeof(struct_system_t) / 4);

    memcpy((uint32_t *)&temp_data, (uint32_t *)&system, sizeof(struct_system_t));

    flash_fmc_erase_Npage(FMC_PARA_ADDRESS, 1);
    flash_fmc_program(FMC_PARA_ADDRESS, (uint32_t *)&temp_data, sizeof(struct_system_t) / 4);

    if (flash_fmc_program_check(FMC_PARA_ADDRESS, (uint32_t *)&temp_data, sizeof(struct_system_t) / 4) == 1)
    {
        flash_fmc_read(FMC_PARA_ADDRESS, (uint32_t *)&system, sizeof(struct_system_t) / 4);
        return 1;
    }
    
    return 0;
}

/**************************************************************************************
* Function Name  : system_init
* Description    :
* Input          :
* Output         :
* Return         :
**************************************************************************************/
void system_init(void)
{
    system_flag1.byte = 0;

    flash_fmc_read(FMC_PARA_ADDRESS, (uint32_t *)&system, sizeof(struct_system_t) / 4);

    systick_init();
    system_rtc_init();
    buzzer_init();
    calibration_init();
    history_init();
    key_init();
    led_init();
    relay_init();
    sample_init();
    sensor_time_init();

    twoWire_init();

    system_status_set(STATUS_START);

    if ((system.lel_zero == 0xFFFF) 
     || (system.lel_conc[1] == 0xFFFF)
     || (system.lel_adc[1] == 0xFFFF))
    {
        sysFlag_cali_error = 1;
    }
}

/**************************************************************************************
* Function Name  : system_handle
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void system_handle(void)
{
    if ((sysFlag_reset) || (sysFlag_restart))
    {
        if (sysFlag_restart)
        {
            system_status_set(STATUS_START);
            sysFlag_restart = 0;
        }
        else if (sysFlag_reset)
        {
            sysFlag_reset = 0;
            system_status_set(STATUS_NORMAL);
        }
        sample_data_init();
        calibration_led_close();
        FlagFireAlarm = 0;
        FlagFireAlarmSuccess = 0;
    }   

    if ((FlagWriteAddress) && (twoWire_is_idle() == 1))
    {
        if (system_para_modify() == 1)
        {
            FlagWriteAddress = 0;
            FlagWriteAddressSuccess = 1;
        }
    }

    if (sysFlag_modify)
    {
        sysFlag_modify = 0;
        system_para_modify();
    }
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
