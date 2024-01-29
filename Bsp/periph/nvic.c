/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: nvic.c
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   中断优先级设定及配置
 * History:         
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/

#include "nvic.h"


/*
**************************************************************************************
** Name: void NvicConfiguration(void)
** Description: 配置基本中断及优先级
** Parameters: 无
** Returns: 无   
** Author: SYSTEM-II-YYS
** Date: 2010/06/23
** Version:	1.0.0
** History: 历史修改记录
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/
void nvic_cfg(void)
{ 
//    SystemInit();
	/* 设置中断向量表的偏移地址为0x08000000(上电复位默认地址) */ 
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
// 	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xd000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
    
}




/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/


