/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : flash_fmc.c
* Author         : ldy
* Version        : V 4.0.0
* Date           : 2020-09-16
* Description    :
* Vision History : NO.   Date    Revised by Description
***************************************************************************************
**************************************************************************************/
/* Include --------------------------------------------------------------------------*/
#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"
#include "flash_fmc.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/
/**************************************************************************************
* Function Name  : flash_fmc_program
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_program(uint32_t tempAddress, uint32_t *tempData, uint32_t tempLength)
{
    uint32_t temp_address = tempAddress;

    fmc_unlock();

    while (tempLength--)
    {
        fmc_word_program(temp_address, *tempData);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
        tempData++;
        temp_address += 4U;
    }

    fmc_lock();
}

/**************************************************************************************
* Function Name  : flash_fmc_program_check
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
uint8_t flash_fmc_program_check(uint32_t dest_addr, uint32_t *sour_addr, uint32_t tempLength)
{
    uint32_t *ptrAddr = (uint32_t *)dest_addr;

    while (tempLength--)
    {
        if ((*ptrAddr++) != (*sour_addr++))
        {
            return 0;
        }
    }

    return 1;
}
/**************************************************************************************
* Function Name  : flash_fmc_program_halfword
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_program_halfword(uint32_t source_addr, uint16_t *dest_addr, uint32_t length)
{
    uint32_t ptrAddr = source_addr;

    fmc_unlock();

    while (length--)
    {
        /* program flash */
        fmc_halfword_program(ptrAddr, *dest_addr);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
        dest_addr++;
        ptrAddr += 2U;
    }

    fmc_lock();
}

/**************************************************************************************
* Function Name  : flash_fmc_erase_Npage
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_erase_Npage(uint32_t tempAddress, uint32_t tempCount)
{
    uint16_t i = 0;

    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* erase the flash pages */
    for (i = 0; i < tempCount; i++)
    {
        fmc_page_erase(tempAddress + (FMC_PAGE_SIZE * i));
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    fmc_lock();
}

/**************************************************************************************
* Function Name  : flash_fmc_overload
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_overload(uint32_t tempAddress, uint32_t destAddress, uint32_t tempLength)
{
    uint32_t *ptrAddr = (uint32_t *)tempAddress;
    uint32_t temp_destAddress = destAddress;

    fmc_unlock();
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    while (tempLength--)
    {
        fmc_word_program(temp_destAddress, *ptrAddr);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

        ptrAddr++;
        temp_destAddress = temp_destAddress + 4U;
    }

    fmc_lock();
}

/**************************************************************************************
* Function Name  : flash_fmc_erase_Npage_check
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_erase_Npage_check(uint32_t tempAddress, uint32_t tempCount)
{
    uint32_t i = 0;
    uint32_t *ptrAddr = (uint32_t *)tempAddress;

    for (i = 0; i < (tempCount * 256); i++)
    {
        if (0xFFFFFFFFU == (*ptrAddr))
        {
            ptrAddr++;
        }
    }
}

/**************************************************************************************
* Function Name  : flash_fmc_read
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_read(uint32_t tempAddress, uint32_t *tempData, uint32_t tempLength)
{
    uint32_t *ptrAddr = (uint32_t *)tempAddress;

    tempLength = tempLength;

    while (tempLength--)
    {
        *tempData++ = *ptrAddr++;
    }
}

/**************************************************************************************
* Function Name  : flash_fmc_read_halfword
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void flash_fmc_read_halfword(uint32_t source_addr, uint16_t *dest_addr, uint32_t length)
{
    uint16_t *ptrAddr = (uint16_t *)source_addr;

    while (length--)
    {
        *dest_addr++ = *ptrAddr++;
    }
}

/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
