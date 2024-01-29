/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: stm32f10x_it.h
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/06/23
 * Description:          
 * Function List:   // 主要函数及其功能
   1. -------
 * History:         // 历史修改记录
   <author>    <time>      <version >     <desc>
   YYS        2010/06/23     1.0.0         first version   
*********************************************************************************************/

#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 
#include "stm32f10x.h"

#include <stdio.h>

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);

/* 用到的中断服务函数 */
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void TIM2_IRQHandler(void);
void SysTick_Handler(void);	
void USART3_IRQHandler(void);
void UART4_IRQHandler(void);



#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/


