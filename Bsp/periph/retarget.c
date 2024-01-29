/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: retarget.c
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   �ض���һЩ�⺯��
 * History:         // ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/


#include <stdio.h>
#include "uart1.h"


/* ��printf�ض��򵽴���1 */
int fputc(int ch, FILE *f)
{
    uint8_t tmp = ch;
    uart1_transmit(&tmp,1);
    //
    return ch;
} 



/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

