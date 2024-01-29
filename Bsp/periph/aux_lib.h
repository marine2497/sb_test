/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: aux_lib.h
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   通用的一些宏及函数
 * History:         
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/

#ifndef	_AUX_LIB_H
#define	_AUX_LIB_H

#include "stm32f10x.h"


typedef void(*func_none_none_t)(void); 	/* 无返回值无参数的函数指针 */
typedef void* (*func_one_none_t)(void); /* 有返回值无参数的函数指针 */
typedef void(*func_none_one_t)(void*);	/* 无返回值有参数的函数指针 */
typedef void* (*funt_one_one_t)(void*);	/* 有返回值有参数的函数指针 */


#endif




/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

