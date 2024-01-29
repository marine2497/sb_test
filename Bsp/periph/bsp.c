/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: bsp.c
 * Author:    SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/07/08
 * Description:          
 * Function List:   整个系统的驱动层
 * History: 
   <author>    <time>      <version >     <desc>
   YYS        2010/07/08     1.0.0         first version   
*********************************************************************************************/

#include "bsp.h"
#include "stm32f10x.h"
#include "uart.h"
#include "nvic.h"
#include "gpio.h"
#include "stdio.h"
#include "time.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "uart5.h"

void bsp_init(void)
{  
  //硬件版本日期：20160623
 // *    LED  --- PC2
  
 // *    MCU_RX1 --- PA10
 // *    MCU_TX1 --- PA9
  
  
 // *    MCU_RX3 --- PB11
 // *    MCU_TX3 --- PB10
  
  
  
    nvic_cfg();
   // uart_cfg(115200,115200,115200);
    uart1_init();
    //uart2_init();
    uart3_init();
    //uart4_init();
    //uart5_init();
    //
    //uart1_transmit("\r\n\r\n\r\n",6);
    //
    gpio_cfg();    
    
    time_init();
    //
    time_waitMs(100);
}


/******************* (C) COPYRIGHT 2010 SOUTH GNSS SYSTEM II *****END OF FILE****/

