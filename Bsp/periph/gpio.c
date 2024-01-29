
#include "gpio.h"
#include "stdio.h"
#include "time.h"
/*
**************************************************************************************
** Name: void GpioConfiguration(void)
** Description: MCUͨ��IO������
** Parameters: None
** Returns: None       
** Author: 
** Date: 
** Version:	
****************************************************************************************
*/
/*M1����ת�ٱ�
M2��M5���ӵ�λ������
M6-ɲ��
M7-��ת��
M8-��ת��
M9-��ɲ
M10-��ȫ��
M11-����
M12-����
M13-��ɲ
M14-���
M15-Զ���
M16-�����
M17-���
M18-��ʾ��
M19-ʾ���
M20-��ˢ
M21-�ƶ���ѹ
M22-��𿪹�
M23-������ͨ��
M24-��ʻ��λ�Ƿ�����
M25-��ۺ�
M26-�йۺ�
M27-�ҹۺ�
M28-���ε���
M29-�Ƴ�һ����ǰ
M30-�Ƴ�һ����ǰ
M31-�Ƴ�һ���Һ�
M32-�Ƴ�һ���Һ�
*/
void gpio_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  // ��ͨʱ�ӡ�
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
}

void GPIO_pwm_out_init(void)
{
       
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
 

  //GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);	
}
void GPIO_pwm_in_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);
  
  /* TIM3 channel 2 pin (PA.07) configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line6|EXTI_Line7|EXTI_Line8|EXTI_Line9 |EXTI_Line2|EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;								   
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);      
        
      
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
  
        
        
 
}

 uint32_t  PWM_time0=0;//TIM8_1---
 uint32_t  PWM_time1=0;//TIM8_2---
 uint32_t  PWM_time2=0;//TIM8_3---
 uint32_t  PWM_time3=0;//TIM8_4---

 
void EXTI9_5_IRQHandler(void)
{  
    static uint64_t t_time[4]={0,0,0,0};  
   
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{ 
          if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))
              t_time[0]=time_nowUs();
          else
              PWM_time0=(uint32_t)(time_nowUs()-t_time[0]);
          EXTI_ClearITPendingBit(EXTI_Line6); 
	}
  if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
          if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
              t_time[1]=time_nowUs();
          else
              PWM_time1=(uint32_t)(time_nowUs()-t_time[1]);
          EXTI_ClearITPendingBit(EXTI_Line7); 
	}
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{ 
         if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
              t_time[2]=time_nowUs();
          else
             PWM_time2=(uint32_t)(time_nowUs()-t_time[2]);
          EXTI_ClearITPendingBit(EXTI_Line8); 
	}
  if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
             if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9))
              t_time[3]=time_nowUs();
          else
             PWM_time3=(uint32_t)(time_nowUs()-t_time[3]);
          EXTI_ClearITPendingBit(EXTI_Line9); 
	}
}


/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

