/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: aux_lib.h
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   ͨ�õ�һЩ�꼰����
 * History:         
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/

#ifndef	_AUX_LIB_H
#define	_AUX_LIB_H

#include "stm32f10x.h"


typedef void(*func_none_none_t)(void); 	/* �޷���ֵ�޲����ĺ���ָ�� */
typedef void* (*func_one_none_t)(void); /* �з���ֵ�޲����ĺ���ָ�� */
typedef void(*func_none_one_t)(void*);	/* �޷���ֵ�в����ĺ���ָ�� */
typedef void* (*funt_one_one_t)(void*);	/* �з���ֵ�в����ĺ���ָ�� */


#endif




/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

