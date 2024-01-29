/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: gpio.h
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   GPIO配置及设定的头文件
 * History:   
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/


#ifndef	_GPIO_H__
#define	_GPIO_H__

#include "stm32f10x.h"
void gpio_cfg(void);
uint32_t read_gpio(void);

void GPIO_pwm_out_init(void);
void GPIO_pwm_in_init(void);
#endif




/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

