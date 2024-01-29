/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: uart.c
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


#include "uart.h"


/*
**************************************************************************************
** Name: void uart_cfg(u32 Baud1,u32 Baud2,uint32_t baud3)
** Description: �Ե�Ƭ�����д��ڽ�������
** Parameters: Baud1--����1������ Baud2--����2������
** Returns: ��
** Author: 
** Date: 
** Version:	
** History:
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/

void uart_cfg(uint32_t baud1,uint32_t baud2,uint32_t baud3)
{
 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    
	USART_InitTypeDef USART_InitStructure;
        

	//USART_Cmd(USART1, DISABLE);
	USART_Cmd(USART2, DISABLE);
  USART_Cmd(USART3, DISABLE);
  
	USART_InitStructure.USART_BaudRate = baud1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_Init(USART1, &USART_InitStructure);
    

	USART_InitStructure.USART_BaudRate = baud2;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baud3;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
    

  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //ʹ�ܴ��ڽ����ж�
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   //�����жϿ�����Ҫ���ж��������ܱ�־λ
  //USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);   
  
	
	
	//USART_Cmd(USART1, ENABLE);
  USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);
}





/*
**************************************************************************************
** Name: void UartPutc(USART_TypeDef* USARTx,char c)
** Description:  �������һ���ַ�?** Parameters: c--��Ҫ���ͳ�ȥ���ַ�
** Returns: ��
** Author: SYSTEM-II-YYS
** Date: 2010/06/23
** Version:	1.0.0
** History: ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/

void uart_putc(USART_TypeDef* USARTx,uint8_t c)
{
		USART_SendData(USARTx, c);
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET );
}


/*
**************************************************************************************
** Name: void UartPuts(USART_TypeDef* USARTx,char *s)
** Description:  �������һ���ַ�?** Parameters: 
** Returns: ��
** Author: SYSTEM-II-YYS
** Date: 2010/09/15
** Version:	1.0.0
** History: ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/
void uart_puts(USART_TypeDef* USARTx,const char *s)
{
	while(*s)
	{
		uart_putc(USARTx,*s++);
	}
}



/*
**************************************************************************************
** Name: void UartPutn(USART_TypeDef* USARTx,char *s,uint32_t len)
** Description:  �������n���ֽڵ��ַ�
** Parameters: 
** Returns: ��
** Author: SYSTEM-II-YYS
** Date: 2010/09/15
** Version:	1.0.0
** History: ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/
void uart_putn(USART_TypeDef* USARTx,uint8_t *s,uint32_t len)
{
	while(len--)
	{
		uart_putc(USARTx,*s++);
	}
}
/*********************************************************************************************************
 ** Function name:          set_uart_baud
 ** Descriptions:           �޸�ָ�����ڲ�����
 ** input parameters:       USART_TypeDef* USARTx ��Ҫ�޸ĵĴ���
                            uint32_t baud �²�����
 ** Returned value:         none
 *********************************************************************************************************/
void set_uart_baud(USART_TypeDef* USARTx, uint32_t baud)
{
    USART_InitTypeDef USART_InitStructure;
    USART_Cmd(USARTx, DISABLE);
    //
    USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART2,USART_IT_TC,DISABLE);

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USARTx, &USART_InitStructure);

    //USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    USART_Cmd(USARTx, ENABLE);
}





/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/



