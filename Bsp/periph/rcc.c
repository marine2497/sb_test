#define	RCC_G
#include "rcc.h"
extern uint32_t SystemCoreClock ;

/*
**************************************************************************************
** Name: void RccConfiguration(void)
** Description: ���ô���
** Parameters: None
** Returns: None       
** Author: Gnss_yys
** Date: 2009.10.30
** Version:	
****************************************************************************************
*/
void rcc_cfg(void)
{
	//����3.3.0	��ʼ��ʱ������������ļ��е�����SystemInit����
	SysTick_Config(SystemCoreClock/1000); //����Systick ʱ��1ms
}
