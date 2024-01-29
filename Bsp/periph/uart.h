/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: uart.h
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/06/23
 * Description:          
 * Function List:   // ��Ҫ�����书��
   1. -------
 * History:         // ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>
   YYS        2010/06/23     1.0.0         first version   
*********************************************************************************************/

#ifndef _UART_H
#define _UART_H


/* �ļ��� */
#include "stm32f10x.h"



/* �������� */
void uart_cfg(uint32_t baud1,uint32_t baud2,uint32_t baud3);
void set_uart_baud(USART_TypeDef* USARTx, uint32_t baud);
void uart_putc(USART_TypeDef* USARTx,uint8_t c);
void uart_puts(USART_TypeDef* USARTx,const char *s);
void uart_putn(USART_TypeDef* USARTx,uint8_t *s,uint32_t len);


#endif

/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

