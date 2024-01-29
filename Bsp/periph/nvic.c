/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: nvic.c
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   �ж����ȼ��趨������
 * History:         
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/

#include "nvic.h"


/*
**************************************************************************************
** Name: void NvicConfiguration(void)
** Description: ���û����жϼ����ȼ�
** Parameters: ��
** Returns: ��   
** Author: SYSTEM-II-YYS
** Date: 2010/06/23
** Version:	1.0.0
** History: ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>	
****************************************************************************************
*/
void nvic_cfg(void)
{ 
//    SystemInit();
	/* �����ж��������ƫ�Ƶ�ַΪ0x08000000(�ϵ縴λĬ�ϵ�ַ) */ 
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
// 	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xd000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
    
}




/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/


