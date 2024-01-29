#define	RCC_G
#include "rcc.h"
extern uint32_t SystemCoreClock ;

/*
**************************************************************************************
** Name: void RccConfiguration(void)
** Description: 配置串口
** Parameters: None
** Returns: None       
** Author: Gnss_yys
** Date: 2009.10.30
** Version:	
****************************************************************************************
*/
void rcc_cfg(void)
{
	//由于3.3.0	初始化时钟在启动汇编文件中调用了SystemInit函数
	SysTick_Config(SystemCoreClock/1000); //启动Systick 时基1ms
}
