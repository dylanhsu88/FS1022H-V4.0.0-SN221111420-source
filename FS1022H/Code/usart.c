/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*********************************
* File Name      : usart.c
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
#include "usart.h"
#include "system.h"
#include "protocol.h"

/* Private typedef ------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------*/

/**************************************************************************************
* Function Name  : usart0_data_transmit
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void usart0_data_transmit(uint8_t *txData, uint32_t length)
{
    while (length--)
    {
        USART_TDATA(USART0) = *txData;
        while (RESET == usart_flag_get(USART0, USART_FLAG_TC));
        txData++;
    }
}

/**************************************************************************************
* Function Name  : usart0_dma_transmit
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void usart0_dma_transmit(uint8_t *pData, uint32_t length)
{
    dma_parameter_struct dma_init_struct;

    rcu_periph_clock_enable(RCU_DMA);

    dma_deinit(DMA_CH1);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)pData;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = length;
    dma_init_struct.periph_addr = (uint32_t)&USART_TDATA(USART0);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA_CH1);
    dma_memory_to_memory_disable(DMA_CH1);

    /* enable DMA channel1 */
    dma_channel_enable(DMA_CH1);

    /* USART DMA enable for transmission */
    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
    /* enable DMA channel1 transfer complete interrupt */
    //dma_interrupt_enable(DMA_CH1, DMA_INT_FTF);
}

/**************************************************************************************
* Function Name  : usart0_gpio_config
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void usart0_gpio_config(void)
{
    rcu_periph_clock_enable(USART0_TX_GPIO_CLK);
    gpio_af_set(USART0_TX_GPIO_PORT, USART0_TX_GPIO_AF, USART0_TX_GPIO_PIN);
    gpio_mode_set(USART0_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_TX_GPIO_PIN);
    gpio_output_options_set(USART0_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, USART0_TX_GPIO_PIN);

    rcu_periph_clock_enable(USART0_RX_GPIO_CLK);
    gpio_af_set(USART0_RX_GPIO_PORT, USART0_RX_GPIO_AF, USART0_RX_GPIO_PIN);
    gpio_mode_set(USART0_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_RX_GPIO_PIN);
    gpio_output_options_set(USART0_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, USART0_RX_GPIO_PIN);
}

/**************************************************************************************
* Function Name  : usart0_config
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void usart0_config(void)
{
    /* enable usart clock */
    rcu_periph_clock_enable(RCU_USART0);
    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_9BIT); 
    usart_stop_bit_set(USART0, USART_STB_1BIT);  
    usart_parity_config(USART0, USART_PM_EVEN);   
    usart_baudrate_set(USART0, USART0_BAUD_RATE); 

    /* configure tx/rx  */
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    /* usart enable */
    usart_enable(USART0);
}

/**************************************************************************************
* Function Name  : usart0_nvic_config
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
static void usart0_nvic_config(void)
{
    //nvic_irq_enable(DMA_Channel1_2_IRQn, 0, 0);

    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0U, 0U);

    // /* enable USART TBE interrupt */
    // usart_interrupt_enable(USART0, USART_INT_TBE);
    /* enable USART RBNE interrupt */
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

/**************************************************************************************
* Function Name  : usart0_init
* Description    : 
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void usart0_init(void)
{
    usart0_gpio_config();
    usart0_config();
    usart0_nvic_config();
}

/**************************************************************************************
* Function Name  : USART0_IRQHandler
* Description    : interface
* Input          : 
* Output         :
* Return         :
**************************************************************************************/
void USART0_IRQHandler(void)
{
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);

        protocol_receive_data((uint8_t)usart_data_receive(USART0));
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE))
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_TBE);
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE))
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RT)) //
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RT);
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE_ORERR)) //
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE_ORERR);
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_ERR_NERR)) //
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_ERR_NERR);
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_ERR_FERR)) //
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_ERR_FERR);
    }

    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_ERR_ORERR)) //
    {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_ERR_ORERR);
    }
}


/***************** (C) COPYRIGHT 2020;Forsafe Co.,Ltd.*****END OF FILE ***************/
