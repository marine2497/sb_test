/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: stm32f10x_it.c
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

#include "stm32f10x_it.h"
//#include "detect.h"
#include "time.h"
#include <string.h>
/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	printf("\r\nNMI_Handler\r\n");

}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  	printf("\r\nHardFault_Handler\r\n");
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  	printf("\r\nMemManage_Handler\r\n");

  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
    printf("\r\nBusFault_Handler\r\n");

  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
    printf("\r\nUsageFault\r\n");
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
// void SysTick_Handler(void)
// {
//     g_tim_sys.cal_time++;
//     g_tim_sys.output_time++;
// }


/*
**************************************************************************************
** Name: void TIM2_IRQHandler(void)
** Description: Tim2中断程序
** Parameters: 无
** Returns: 无   
** Author: SYSTEM-II-YYS
** Date: 2010/06/23
** Version:	1.0.0
** History: 历史修改记录
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/

void TIM2_IRQHandler(void)
{
   
	/* 基准时间设置为10ms */
	//if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	//{
	   
	//	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	//}
}








/*
**************************************************************************************
** Name: void SPI1_IRQHandler(void)
** Description: SPI1中断程序
** Parameters: 无
** Returns: 无   
** Author: SYSTEM-II-YYS
** Date: 2010/06/23
** Version:	1.0.0
** History: 历史修改记录
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/

void SPI1_IRQHandler(void)
{

}

#define HISTORY_LENGTH 16
#define OUTPUT_INTERVAL_US 1000*1000 // 频率输出间隔，微秒

typedef struct
{
    volatile int32_t interval_us[HISTORY_LENGTH];
    volatile int32_t next_index;
    volatile uint64_t last_trgger_time_us;
} freq_t;
freq_t g_freq;
void freq_init(freq_t * freq)
{
    memset(freq,0,sizeof(*freq));
}


__IO uint32_t  Motor_speed = 0 ;
__IO uint32_t  Car_speed = 0 ;
void EXTI15_10_IRQHandler(void)
{  
  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{ 
          Motor_speed++;
          EXTI_ClearITPendingBit(EXTI_Line12); 
	}
  if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
          Car_speed++;
          EXTI_ClearITPendingBit(EXTI_Line14); 
	}

}





/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/
