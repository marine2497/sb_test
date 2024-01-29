/********************************************************************************************
 * Copyright (C), 1989-2010, South Gnss  Co., Ltd.
 * FileName: cpld.c
 * Author:	SYSTEM-II-YYS      
 * Version: 1.0.0         
 * Date: 2010/06/23
 * Description:          
 * Function List:   // ��Ҫ�����书��
   1. -------
 * History:         // ��ʷ�޸ļ�¼
   <author>    <time>      <version >     <desc>
   YYS        2010/11/2     1.0.0         first version   
*********************************************************************************************/
#include "stm32f4xx.h"
#include <stdio.h> 
#include "oem_o.h"
#include "uart.h"
#include "cpld.h"
#include "time.h"
#include "param/param.h"
#include "gnss.h"

#define	CMD_ON		(uint8_t)1
#define CMD_OFF		(uint8_t)0

#define	CMD_TYPE_NULL  (uint8_t)1 //无需改变
#define	CMD_TYPE_BAUD  (uint8_t)2 //改变baud
#define	CMD_TYPE_FREQ  (uint8_t)3 //改变freq

typedef struct
{
	const char* cmd;//命令内容
	uint8_t cmd_enable;
//	uint8_t baud_freq;//差分波特率及输出频率
	uint8_t cmd_type;//表示是更改差分波特率命令还是更改频率命令
} board_cmd_t;

#define delayms(ms) time_waitMs(ms)
#define delay_ms(ms) delayms(ms)
//#if 0
//北斗主板，CPU计算航向
//master初始化命令
board_cmd_t master_compass_cmd[] =
{
	{"unlogall\r\n",                          CMD_ON,CMD_TYPE_NULL},
	{"set cpufreq 624\r\n",                   CMD_ON,CMD_TYPE_NULL},
	{"set pvtfreq 10\r\n",                    CMD_ON,CMD_TYPE_NULL},
	{"set rtkfreq 10\r\n",                    CMD_ON,CMD_TYPE_NULL},
	{"fix none\r\n",                          CMD_ON,CMD_TYPE_NULL},
	//{"rtktimeout 6\r\n",                    CMD_ON,CMD_TYPE_NULL},
	{"com com2 115200 n 8 1 n off\r\n",       CMD_ON,CMD_TYPE_BAUD},//差分口波特率
	{"log com1 bestxyza ontime 0.1\r\n",      CMD_ON,CMD_TYPE_FREQ},//10Hz
	{"log com1 gprmc ontime 0.1\r\n",         CMD_ON,CMD_TYPE_FREQ},//10Hz
	{"log com1 gpgga ontime 0.1\r\n",         CMD_ON,CMD_TYPE_FREQ},//10HZ
    {"log com1 refstationa ontime 10\r\n",  CMD_ON,CMD_TYPE_NULL},// 获取基准站坐标的命令，固定0.1Hz即可。
	{"interfacemode com2 auto auto on\r\n",   CMD_ON,CMD_TYPE_NULL},// compass主板必须设置成auto auto on
	{"saveconfig\r\n",                        CMD_ON,CMD_TYPE_NULL},
};
//rover初始化命令
board_cmd_t rover_compass_cmd[] =
{
	{"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
	{"set cpufreq 624\r\n",CMD_ON,CMD_TYPE_NULL},
	{"set pvtfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
	{"set rtkfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
	{"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
	//{"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
	{"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_BAUD},//差分口波特率
	{"log com1 gpgga ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},//10HZ
	{"interfacemode com2 auto auto on\r\n",CMD_ON,CMD_TYPE_NULL},// compass主板必须设置成auto auto on
	{"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};





//北斗主板，主板自身计算航向
//master初始化命令
board_cmd_t master_compass_cx_cmd[] =
{
    {"set cpufreq 624\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set pvtfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set rtkfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
    {"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
    //{"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"com com3 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_BAUD},//差分口波特率
    {"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_NULL},
    {"interfacemode com2 auto auto on\r\n",CMD_ON,CMD_TYPE_NULL},
    {"interfacemode com3 auto auto on\r\n",CMD_ON,CMD_TYPE_NULL},
    {"log com2 rtcmcompassb ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},//发给rover的测向差分
    {"log com1 gprmc ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 bestxyza ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 gpgga ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 refstationa ontime 10\r\n",CMD_ON,CMD_TYPE_NULL}, // 获取基准站坐标的命令，固定0.1Hz即可。
    {"log com1 gptra ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},//计算出航向角给master
    {"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};
//rover初始化命令
board_cmd_t rover_compass_cx_cmd[]=
{
    {"set cpufreq 624\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set pvtfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set rtkfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
    {"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
    //{"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_NULL},
    {"interfacemode com2 auto auto on\r\n",CMD_ON,CMD_TYPE_NULL},
    {"log com2 rtcmcompass3b ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"rtkrefmode 1\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set diffmatchmode synch\r\n",CMD_ON,CMD_TYPE_NULL},
    {"log com1 gpgga ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};









//novatel主板，CPU计算航向
//master初始化命令
board_cmd_t master_novatel_cmd[] =
{
    {"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
    {"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n",CMD_ON,CMD_TYPE_NULL},
    {"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_BAUD},
    {"log com1 bestxyza ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 gprmc ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 gpggalong ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 refstationa ontime 10\r\n",CMD_ON,CMD_TYPE_NULL}, // 获取基准站坐标的命令，固定0.1Hz即可。
    {"interfacemode com2 auto novatel off\r\n",CMD_ON,CMD_TYPE_NULL},
    {"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};
//rover初始化命令
board_cmd_t rover_novatel_cmd[] =
{
	{"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
	{"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
	{"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n",CMD_ON,CMD_TYPE_NULL},
	{"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_FREQ},
	{"log com1 gpggalong ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
	{"interfacemode com2 auto novatel off\r\n",CMD_ON,CMD_TYPE_NULL},
	{"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};


//novatel主板，主板计算航向
//master初始化命令
board_cmd_t master_novatel_cx_cmd[] =
{
	{"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
	{"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
	{"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n",CMD_ON,CMD_TYPE_NULL},
	{"interfacemode usb1 none none\r\n",CMD_ON,CMD_TYPE_NULL},
	{"interfacemode usb2 none none\r\n",CMD_ON,CMD_TYPE_NULL},
	{"interfacemode usb3 none none\r\n",CMD_ON,CMD_TYPE_NULL},
	{"markcontrol mark1 disable\r\n",CMD_ON,CMD_TYPE_NULL},
	{"interfacemode com3 novatel novatel off\r\n",CMD_ON,CMD_TYPE_NULL},
	{"com com3 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_BAUD}, //差分口波特率
	{"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_NULL},//测向差分口波特率，固定住
	{"interfacemode com3 auto none off\r\n",CMD_ON,CMD_TYPE_NULL},
	{"movingbasestation enable\r\n",CMD_ON,CMD_TYPE_NULL},
	{"interfacemode com2 none rtca off\r\n",CMD_ON,CMD_TYPE_NULL},
	//{"log com2 rtcaobs2 ontime 0.2\r\n"},
	//{"log com2 rtcaref  ontime 5\r\n"},
	{"log com2 rtcaobs3 ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
	{"log com2 rtcarefext  ontime 5\r\n",CMD_ON,CMD_TYPE_NULL},//扩展差分，固定为5s一次就够
	{"log com1 gprmc ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
	{"log com1 bestxyza ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
	{"log com1 gpgsv ontime 1\r\n",CMD_OFF,CMD_TYPE_NULL},
	{"log com1 gpggalong ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 refstationa ontime 10\r\n",CMD_ON,CMD_TYPE_NULL}, // 获取基准站坐标的命令，固定0.1Hz即可。
	{"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};
board_cmd_t rover_novatel_cx_cmd[] =
{
    {"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
    {"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n",CMD_ON,CMD_TYPE_NULL},
    {"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_NULL},//
    {"interfacemode com2 rtca none off\r\n",CMD_ON,CMD_TYPE_NULL},
    {"log com1 heading3a ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},//
    {"log com1 gpggalong ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};

//单主板，接master,北斗
board_cmd_t single_compass_cmd[] =
{
    {"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set cpufreq 624\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set pvtfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
    {"set rtkfreq 10\r\n",CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
    {"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_BAUD},//差分口波特率
    {"log com1 bestxyza ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},//10Hz
	{"log com1 gprmc ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},//10Hz
    {"interfacemode com2 auto auto on\r\n",CMD_ON,CMD_TYPE_NULL},//
    {"log com1 gpgga ontime 0.1\r\n",CMD_ON,CMD_TYPE_FREQ},//10HZ
    {"log com1 refstationa ontime 10\r\n",CMD_ON,CMD_TYPE_NULL}, // 获取基准站坐标的命令，固定0.1Hz即可。
    {"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};


//单主板，接master,novatel主板
board_cmd_t single_novatel_cmd[] =
{
    {"unlogall\r\n",CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n",CMD_ON,CMD_TYPE_NULL},
    {"rtktimeout 6\r\n",CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n",CMD_ON,CMD_TYPE_NULL},
    {"com com2 115200 n 8 1 n off\r\n",CMD_ON,CMD_TYPE_BAUD},//差分口波特率
    {"log com1 bestxyza ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 gprmc ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 gpggalong ontime 0.2\r\n",CMD_ON,CMD_TYPE_FREQ},
    {"log com1 refstationa ontime 10\r\n",CMD_ON,CMD_TYPE_NULL}, // 获取基准站坐标的命令，固定0.1Hz即可。
    {"interfacemode com2 auto novatel off\r\n",CMD_ON,CMD_TYPE_NULL},
    {"saveconfig\r\n",CMD_ON,CMD_TYPE_NULL},
};

//单主板双天线，接master,北斗
board_cmd_t single_2antenna_compass_cmd[] =
{
    {"unlogall\r\n"                         ,CMD_ON,CMD_TYPE_NULL},
    {"set cpufreq 624\r\n"                  ,CMD_ON,CMD_TYPE_NULL},
    {"set pvtfreq 10\r\n"                   ,CMD_ON,CMD_TYPE_NULL},
    {"set rtkfreq 10\r\n"                   ,CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n"                         ,CMD_ON,CMD_TYPE_NULL},
    {"com com3 115200 n 8 1 n off\r\n"      ,CMD_ON,CMD_TYPE_BAUD},//差分口波特率
    {"interfacemode com3 auto auto on\r\n"  ,CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n"              ,CMD_ON,CMD_TYPE_NULL},
    {"log com2 gpgga ontime 0.1\r\n"        ,CMD_ON,CMD_TYPE_FREQ},
    {"log com2 gptra ontime 0.1\r\n"        ,CMD_ON,CMD_TYPE_FREQ},
    {"log com2 gprmc ontime 0.1\r\n"        ,CMD_ON,CMD_TYPE_FREQ},
    {"log com2 bestxyza ontime 0.1\r\n"     ,CMD_ON,CMD_TYPE_FREQ},
    {"log com2 refstationa ontime 10\r\n"   ,CMD_ON,CMD_TYPE_NULL},
    {"saveconfig\r\n"                       ,CMD_ON,CMD_TYPE_NULL},
};


//单主板双天线，接master,novatel主板
board_cmd_t single_2antenna_novatel_cmd[] =
{
    {"unlogall\r\n"                         ,CMD_ON,CMD_TYPE_NULL},
    {"fix none\r\n"                         ,CMD_ON,CMD_TYPE_NULL},
    {"com com2 115200 n 8 1 n off\r\n"      ,CMD_ON,CMD_TYPE_BAUD},//差分口波特率
    {"interfacemode com2 auto none on\r\n"  ,CMD_ON,CMD_TYPE_NULL},
    {"dualantennaalign enable 5 1\r\n"      ,CMD_ON,CMD_TYPE_NULL},
    {"undulation user 0.0\r\n"              ,CMD_ON,CMD_TYPE_NULL},
    {"log com1 gpggartk ontime 0.1\r\n"     ,CMD_ON,CMD_TYPE_FREQ},
    {"log com1 heading3a ontime 0.1\r\n"    ,CMD_ON,CMD_TYPE_FREQ},
    {"log com1 gprmc ontime 0.1\r\n"        ,CMD_ON,CMD_TYPE_FREQ},
    {"log com1 bestxyza ontime 0.1\r\n"     ,CMD_ON,CMD_TYPE_FREQ},
    {"log com1 refstationa ontime 10\r\n"   ,CMD_ON,CMD_TYPE_NULL},
    {"saveconfig\r\n"                       ,CMD_ON,CMD_TYPE_NULL},
};

//计算各种情况的命令数目
#define K501_MASTER_CMD_NUM       (sizeof(master_compass_cmd)   /sizeof(master_compass_cmd[0]))     
#define K501_ROVER_CMD_NUM        (sizeof(rover_compass_cmd)    /sizeof(rover_compass_cmd[0]))      

#define K501_MASTER_CX_CMD_NUM    (sizeof(master_compass_cx_cmd)/sizeof(master_compass_cx_cmd[0]))  
#define K501_ROVER_CX_CMD_NUM     (sizeof(rover_compass_cx_cmd) /sizeof(rover_compass_cx_cmd[0]))   

#define OEM615_MASTER_CMD_NUM     (sizeof(master_novatel_cmd)   /sizeof(master_novatel_cmd[0]))     
#define OEM615_ROVER_CMD_NUM      (sizeof(rover_novatel_cmd)    /sizeof(rover_novatel_cmd[0]))      

#define OEM615_MASTER_CX_CMD_NUM  (sizeof(master_novatel_cx_cmd)/sizeof(master_novatel_cx_cmd[0]))  
#define OEM615_ROVER_CX_CMD_NUM   (sizeof(rover_novatel_cx_cmd) /sizeof(rover_novatel_cx_cmd[0]))   

#define K501_SINGLE_CMD_NUM       (sizeof(single_compass_cmd)/sizeof(single_compass_cmd[0]))  
#define OEM615_SINGLE_CMD_NUM     (sizeof(single_novatel_cmd)/sizeof(single_novatel_cmd[0]))   

#define K528_SINGLE_2ANTENNA_CMD_NUM       (sizeof(single_2antenna_compass_cmd)/sizeof(single_2antenna_compass_cmd[0]))  
#define OEM617_SINGLE_2ANTENNA_CMD_NUM     (sizeof(single_2antenna_novatel_cmd)/sizeof(single_2antenna_novatel_cmd[0]))  

#define MIN(a, b)  ((a < b) ? a : b)
#define MAX(a, b)  ((a > b) ? a : b)
char *bdr[] = {"","4800","9600","19200","38400","57600","115200"};
char *freq[] ={"","0.1\r\n","0.2\r\n","1\r\n","5\r\n","10\r\n"};

//in_string 进入的主板命令 out_string 拼装后的命令 type-命令类型(改baud还是freq) in_parrameter-修改的参数
void replace(const char *in_string ,char *out_string,uint8_t type,uint8_t in_parameter)
{
	uint8_t null_count = 0;
	uint8_t replace_num = 0;
	uint8_t lock = 0;
	char *temp;
	if(type == CMD_TYPE_BAUD) replace_num = 2;
	if(type == CMD_TYPE_FREQ) replace_num = 4;
	//strcpy(out_string,in_string);
	while(*in_string!='\n')
	{

		if((*in_string) == ' ')
		{
			null_count ++;

			*(out_string++) = *(in_string++);
		}
		else
		{
			if(null_count!=replace_num) lock = 0;
			if(lock == 0)
			{
				if(null_count!=replace_num)
				{
					*(out_string++) = *(in_string);
				}
				else
				{
					if(type == CMD_TYPE_BAUD)
					{
						temp = bdr[in_parameter];
						while(*temp!='\0')
						{
							*(out_string++) = *temp;
							temp ++;
						}
				  }
					else
					{
						temp = freq[in_parameter];
						while(*temp!='\n')
						{
							*(out_string++) = *(temp++);
						}
					}
					lock = 1;
				}
		  }
			in_string++;
		}

		if((replace_num == 4)&&(lock ==1))
		{
			*(out_string++) = '\n';
	    *(out_string++) = '\0';
	    return ;
		}
	}
  if(replace_num == 2)
	{
		*(out_string++) = '\n';
		*(out_string++) = '\0';
	}
}
// /*********************************************************************************************************
//  ** Function name:           sm36_cpu_cal_heading_no_ins
//  ** Descriptions:            sm36 双天线 不带惯导 cpu计算航向初始化主板
//  ** input parameters:        oem_type: 主板类型  baud-外部差分波特率 freq-数据输出频率
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm36_cpu_cal_heading_no_ins(uint8_t oem_type,uint8_t baud,uint8_t freq)
{
	uint8_t i=0;
	uint8_t all_cmd = 0;
	char sbuf[128];
	cpld_set(SM36_CPU_CAL_HEADING_NO_INS);
	delay_ms(200);//让 cpld状态稳定
	switch(oem_type)
	{
		case OEM_K501:
		case OEM_K505:
			  all_cmd = MAX(K501_MASTER_CMD_NUM,K501_ROVER_CMD_NUM);
				for(i = 0; i < all_cmd;i++)
				{
					if(i<K501_MASTER_CMD_NUM)
					{
						if(master_compass_cmd[i].cmd_enable == CMD_ON)
						{
							if(master_compass_cmd[i].cmd_type == CMD_TYPE_FREQ)//
							{
								replace(master_compass_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART2,sbuf);
							}
							else if(master_compass_cmd[i].cmd_type == CMD_TYPE_BAUD)//
							{
								replace(master_compass_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART2,sbuf);
							}
							else
							{
								uart_puts(USART2,master_compass_cmd[i].cmd);
							}
						}
				  }
				if(i<K501_ROVER_CMD_NUM)
					{
						if(rover_compass_cmd[i].cmd_enable == CMD_ON)
						{
							if(rover_compass_cmd[i].cmd_type == CMD_TYPE_FREQ)//
							{
								replace(rover_compass_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART3,sbuf);
							}
							else if(rover_compass_cmd[i].cmd_type == CMD_TYPE_BAUD)//党蜊疏杻薹
							{
								replace(rover_compass_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART3,sbuf);
							}
							else
							{
								uart_puts(USART3,rover_compass_cmd[i].cmd);
							}
						}
					}
					delay_ms(200);
				}

		break;
		case OEM_OEM615:
			  all_cmd = MAX(OEM615_MASTER_CMD_NUM,OEM615_ROVER_CMD_NUM);
				for(i = 0; i < all_cmd;i++)
				{
					if(i<OEM615_MASTER_CMD_NUM)
					{
						if(master_novatel_cmd[i].cmd_enable == CMD_ON)
						{
							if(master_novatel_cmd[i].cmd_type == CMD_TYPE_FREQ)
							{
								replace(master_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART2,sbuf);
							}
							else if(master_novatel_cmd[i].cmd_type == CMD_TYPE_BAUD)
							{
								replace(master_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART2,sbuf);
							}
							else
							{
								uart_puts(USART2,master_novatel_cmd[i].cmd);
							}
						}
				  }
					if(i<OEM615_ROVER_CMD_NUM)
					{
						if(rover_novatel_cmd[i].cmd_enable == CMD_ON)
						{
							if(rover_novatel_cmd[i].cmd_type == CMD_TYPE_FREQ)
							{
								replace(rover_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART3,sbuf);
							}
							else if(rover_novatel_cmd[i].cmd_type == CMD_TYPE_BAUD)
							{
								replace(rover_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART3,sbuf);
							}
							else
							{
								uart_puts(USART3,rover_novatel_cmd[i].cmd);
							}
						}
					}
					delay_ms(200);
				}
		break;
		default:
            printf("oem: not support this mode with this OEM type. 5\r\n");
		break;
	}
}
// /*********************************************************************************************************
//  ** Function name:           sm36_cpu_cal_heading_ins
//  ** Descriptions:            sm36 双天线 带惯导 cpu计算航向初始化主板
//  ** input parameters:        oem_type: 主板类型  baud-外部差分波特率 freq-数据输出频率
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm36_cpu_cal_heading_ins(uint8_t oem_type,uint8_t baud,uint8_t freq)
{
	sm36_cpu_cal_heading_no_ins(oem_type,baud,freq);
	cpld_set(SM36_CPU_CAL_HEADING_INS); //设置完主板再切换CPLD
}


// /*********************************************************************************************************
//  ** Function name:           sm36_oem_cal_heading_no_ins
//  ** Descriptions:            sm36 双天线 不带惯导 主板计算航向初始化主板
//  ** input parameters:        oem_type: 主板类型  baud-外部差分波特率 freq-数据输出频率
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm36_oem_cal_heading_no_ins(uint8_t oem_type,uint8_t baud,uint8_t freq)
{
	uint8_t i;
	uint8_t all_cmd = 0;
	char sbuf[128];
	cpld_set(SM36_OEM_CAL_HEADING_NO_INS);
	delay_ms(200);//让 cpld状态稳定
	switch(oem_type)
	{
		case OEM_K501:
		case OEM_K505:
			  all_cmd = MAX(K501_MASTER_CX_CMD_NUM,K501_ROVER_CX_CMD_NUM);
				for(i = 0; i < all_cmd;i++)
				{
					if(i<K501_MASTER_CX_CMD_NUM)
					{
						if(master_compass_cx_cmd[i].cmd_enable == CMD_ON)
						{
							if(master_compass_cx_cmd[i].cmd_type == CMD_TYPE_FREQ)//
							{
								replace(master_compass_cx_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART2,sbuf);
							}
							else if(master_compass_cx_cmd[i].cmd_type == CMD_TYPE_BAUD)//
							{
								replace(master_compass_cx_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART2,sbuf);
							}
							else
							{
								uart_puts(USART2,master_compass_cx_cmd[i].cmd);
							}
						}
				  }
					if(i<K501_ROVER_CX_CMD_NUM)
					{
						if(rover_compass_cx_cmd[i].cmd_enable == CMD_ON)
						{
							if(rover_compass_cx_cmd[i].cmd_type == CMD_TYPE_FREQ)//
							{
								replace(rover_compass_cx_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART3,sbuf);
							}
							else if(rover_compass_cx_cmd[i].cmd_type == CMD_TYPE_BAUD)//
							{
								replace(rover_compass_cx_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART3,sbuf);
							}
							else
							{
								uart_puts(USART3,rover_compass_cx_cmd[i].cmd);
							}
						}
					}
					delay_ms(200);
				}

		break;
		case OEM_OEM615:
			  all_cmd = MAX(OEM615_MASTER_CX_CMD_NUM,OEM615_ROVER_CX_CMD_NUM);
				for(i = 0; i < all_cmd;i++)
				{
					if(i<OEM615_MASTER_CX_CMD_NUM)
					{
						if(master_novatel_cx_cmd[i].cmd_enable == CMD_ON)
						{
							if(master_novatel_cx_cmd[i].cmd_type == CMD_TYPE_FREQ)//
							{
								replace(master_novatel_cx_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART2,sbuf);
							}
							else if(master_novatel_cx_cmd[i].cmd_type == CMD_TYPE_BAUD)//
							{
								replace(master_novatel_cx_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART2,sbuf);
							}
							else
							{
								uart_puts(USART2,master_novatel_cx_cmd[i].cmd);
							}
						}
				  }
					if(i<OEM615_ROVER_CX_CMD_NUM)
					{
						if(rover_novatel_cx_cmd[i].cmd_enable == CMD_ON)
						{
							if(rover_novatel_cx_cmd[i].cmd_type == CMD_TYPE_FREQ)//
							{
								replace(rover_novatel_cx_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
								uart_puts(USART3,sbuf);
							}
							else if(rover_novatel_cx_cmd[i].cmd_type == CMD_TYPE_BAUD)//
							{
								replace(rover_novatel_cx_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
								uart_puts(USART3,sbuf);
							}
							else
							{
								uart_puts(USART3,rover_novatel_cx_cmd[i].cmd);
							}
						}
					}
					delay_ms(200);
				}
		break;
		default:
            printf("oem: not support this mode with this OEM type. 4\r\n");
		break;
	}
}
// /*********************************************************************************************************
//  ** Function name:           sm36_oem_cal_heading_ins
//  ** Descriptions:            sm36 双天线 带惯导 主板计算航向初始化主板
//  ** input parameters:        oem_type: 主板类型  baud-外部差分波特率 freq-数据输出频率
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm36_oem_cal_heading_ins(uint8_t oem_type,uint8_t baud,uint8_t freq)
{
	sm36_oem_cal_heading_no_ins(oem_type,baud,freq);
	cpld_set(SM36_OEM_CAL_HEADING_INS); 
}

// /*********************************************************************************************************
//  ** Function name:           sm39_move_no_ins
//  ** Descriptions:            sm39 单天线 不带惯导 初始化主板
//  ** input parameters:        oem_type: 主板类型  baud-外部差分波特率 freq-数据输出频率
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm39_move_no_ins(uint8_t oem_type,uint8_t baud,uint8_t freq)
{
	uint8_t i;
	char sbuf[128];
	cpld_set(SM39_NO_INS);
	delay_ms(200);//让 cpld状态稳定
	switch(oem_type)
	{
		case OEM_K501:
		case OEM_K505:
			for(i = 0; i < K501_SINGLE_CMD_NUM;i++)
			{
				if(single_compass_cmd[i].cmd_enable == CMD_ON)
				{
					if(single_compass_cmd[i].cmd_type == CMD_TYPE_FREQ)//
					{
						replace(single_compass_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
						uart_puts(USART2,sbuf);
					}
					else if(single_compass_cmd[i].cmd_type == CMD_TYPE_BAUD)//
					{
						replace(single_compass_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
						uart_puts(USART2,sbuf);
					}
					else
					{
						uart_puts(USART2,single_compass_cmd[i].cmd);
					}
					delay_ms(200);
				}

			}
		break;
		case OEM_OEM615:
			for(i = 0; i < OEM615_SINGLE_CMD_NUM;i++)
			{
				if(single_novatel_cmd[i].cmd_enable == CMD_ON)
				{
					if(single_novatel_cmd[i].cmd_type == CMD_TYPE_FREQ)//
					{
						replace(single_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
						uart_puts(USART2,sbuf);
					}
					else if(single_novatel_cmd[i].cmd_type == CMD_TYPE_BAUD)//
					{
						replace(single_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
						uart_puts(USART2,sbuf);
					}
					else
					{
						uart_puts(USART2,single_novatel_cmd[i].cmd);
					}
					delay_ms(200);
				}
			}
		break;
		default:
            printf("oem: not support this mode with this OEM type. 3\r\n");
		break;
	}
}
// /*********************************************************************************************************
//  ** Function name:           sm39_move_ins
//  ** Descriptions:            sm39 单天线 带惯导 初始化主板
//  ** input parameters:        oem_type: 主板类型  baud-外部差分波特率 freq-数据输出频率
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm39_move_ins(uint8_t oem_type,uint8_t diff_baud,uint8_t freq)
{
	sm39_move_no_ins(oem_type,diff_baud,freq);
	cpld_set(SM39_INS);      //
}
// /*********************************************************************************************************
//  ** Function name:           sm39_base
//  ** Descriptions:            
//  ** input parameters:        oem_type: 主板类型
// 								latitude:纬度
// 							    longitude:经度
// 								height:高程
//  ** Returned value:          none
//  *********************************************************************************************************/
void sm39_base(uint8_t oem_type, double latitude, double longitude,double height,uint8_t diff_baud)
{
	char s_latitude[16],s_longitude[16],s_height[16];

	cpld_set(SM39_NO_INS);
	delay_ms(200);//让 cpld状态稳定
	sprintf((char*)s_latitude, "%.9f", latitude);
    sprintf((char*)s_longitude, "%.9f", longitude);
    sprintf((char*)s_height, "%.4f\r\n", height);

    char sbuf[32];

	switch(oem_type)
	{
		case OEM_K501:
		case OEM_K505:
			uart_puts(USART2,"unlogall\r\n");
			delayms(20);
			uart_puts(USART2,"fix none\r\n");
			delayms(20);
			uart_puts(USART2,"interfacemode com2 auto auto on\r\n");
			delayms(50);
			replace("com com2 115200 n 8 1 n off\r\n" ,sbuf,CMD_TYPE_BAUD,diff_baud);
			uart_puts(USART2,sbuf);
			delayms(50);
			uart_puts(USART2,"log com2 rtcm1004b ontime 1\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1104b ontime 1\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1006b ontime 5\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1008b ontime 5\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1033b ontime 10\r\n");
			delayms(20);
			uart_puts(USART2,"fix position ");
			uart_puts(USART2,s_latitude);
			uart_puts(USART2," ");
			uart_puts(USART2,s_longitude);
			uart_puts(USART2," ");
			uart_puts(USART2,s_height);
			delayms(20);
			uart_puts(USART2,"log com1 gpgga ontime 1\r\n");
			delayms(20);
			uart_puts(USART2,"saveconfig\r\n");
			delayms(20);
		break;
		case OEM_OEM615:
			uart_puts(USART2,"unlogall\r\n");
			delayms(20);
			uart_puts(USART2,"fix none\r\n");
			delayms(20);
			uart_puts(USART2,"undulation user 0.0\r\n");
			delayms(20);
			uart_puts(USART2,"interfacemode com2 none rtcmv3 off\r\n");
			delayms(50);
			replace("com com2 115200 n 8 1 n off\r\n" ,sbuf,CMD_TYPE_BAUD,diff_baud);
			uart_puts(USART2,sbuf);
			delayms(50);
			uart_puts(USART2,"log com2 rtcm1004b ontime 1\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1012 ontime 1\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1006b ontime 5\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1008b ontime 5\r\n");
			delayms(20);
			uart_puts(USART2,"log com2 rtcm1033b ontime 10\r\n");
			delayms(20);
			uart_puts(USART2,"fix position ");
			uart_puts(USART2,s_latitude);
			uart_puts(USART2," ");
			uart_puts(USART2,s_longitude);
			uart_puts(USART2," ");
			uart_puts(USART2,s_height);
			delayms(20);
			uart_puts(USART2,"log com1 gpggartk ontime 1\r\n");
			delayms(20);
			uart_puts(USART2,"saveconfig\r\n");
			delayms(20);
		break;
		default:
            printf("oem: not support this mode with this OEM type. 2\r\n");
		break;
	}
}

void sm39_2antenna(uint8_t oem_type,uint8_t baud,uint8_t freq)
{
	uint8_t i;
	char sbuf[128];
	cpld_set(SM39_NO_INS);
	delay_ms(200);//让 cpld状态稳定
	switch(oem_type)
	{
		case OEM_K528:
			for(i = 0; i < K528_SINGLE_2ANTENNA_CMD_NUM;i++)
			{
				if(single_2antenna_compass_cmd[i].cmd_enable == CMD_ON)
				{
					if(single_2antenna_compass_cmd[i].cmd_type == CMD_TYPE_FREQ)//
					{
						replace(single_2antenna_compass_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
						uart_puts(USART2,sbuf);
					}
					else if(single_2antenna_compass_cmd[i].cmd_type == CMD_TYPE_BAUD)//
					{
						replace(single_2antenna_compass_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
						uart_puts(USART2,sbuf);
					}
					else
					{
						uart_puts(USART2,single_2antenna_compass_cmd[i].cmd);
					}
					delay_ms(200);
				}
			}
		break;
		case OEM_OEM617:
			for(i = 0; i < OEM617_SINGLE_2ANTENNA_CMD_NUM;i++)
			{
				if(single_2antenna_novatel_cmd[i].cmd_enable == CMD_ON)
				{
					if(single_2antenna_novatel_cmd[i].cmd_type == CMD_TYPE_FREQ)//
					{
						replace(single_2antenna_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_FREQ,freq);
						uart_puts(USART2,sbuf);
					}
					else if(single_2antenna_novatel_cmd[i].cmd_type == CMD_TYPE_BAUD)//
					{
						replace(single_2antenna_novatel_cmd[i].cmd ,sbuf,CMD_TYPE_BAUD,baud);
						uart_puts(USART2,sbuf);
					}
					else
					{
						uart_puts(USART2,single_2antenna_novatel_cmd[i].cmd);
					}
					delay_ms(200);
				}
			}
		break;
		default:
            printf("oem: not support this mode with this OEM type. 1\r\n");
		break;
	}
}


/*********************************************************************************************************
 ** Function name:           set_board_baud
 ** Descriptions:            设置主板COM1口波特率
 ** input parameters:        oem_type: ��������
 ** Returned value:          none
 *********************************************************************************************************/
static void set_board_baud(uint8_t oem_type)
{
	switch(oem_type)
	{
    case OEM_K501:
    case OEM_K505:
    case OEM_K528:
    case OEM_OEM615:
    case OEM_OEM617:
        uart_puts(USART3,"com 115200\r\n");
        uart_puts(USART2,"com 115200\r\n");
        break;
    default:
			break;
	}
	delayms(20);
}
/*********************************************************************************************************
 ** Function name:           match_baud
 ** Descriptions:            强制设置每块主板COM1口波特率为115200
 ** input parameters:        oem_type: 
 ** Returned value:          none
 *********************************************************************************************************/
void match_baud(uint8_t oem_type)
{
	uint32_t baud[5] = {9600, 19200, 38400, 57600, 115200};
	uint8_t i;
	cpld_set(SM36_CPU_CAL_HEADING_NO_INS); //先将CPLD切换至这种情况下，以方便进行主板设置
	delay_ms(200);//让 cpld状态稳定
	for(i = 0;i<5;i++)
	{
		set_uart_baud(USART3,baud[i]);
		set_uart_baud(USART2,baud[i]);
		set_board_baud(oem_type);
	}
	printf("Match The OEM Board Baud Rate OK\r\n");
	
}
/*********************************************************************************************************
 ** Function name:           init_oem
 ** Descriptions:           初始化主板
 ** input parameters:        device_mode-工作模式 oem_type 主板类型 baud-差分波特率 freq:数据输出频率
														 
 ** Returned value:          none
 *********************************************************************************************************/
int32_t init_oem(uint32_t device_mode,uint8_t oem_type,uint8_t baud,uint8_t freq)
{
    //printf("device_mode=0x%02X oem_type=0x%02X\r\n",device_mode,oem_type);
    //return -1;
    param_t * param = param_get();
	match_baud(oem_type);
	switch (device_mode)
	{
		case MODE_WORK_DIVIDED:
		  sm36_cpu_cal_heading_no_ins(oem_type,baud,freq);
		break;

		//case SM36_CPU_CAL_HEADING_INS:
		//	sm36_cpu_cal_heading_ins(oem_type,baud,freq);
		//break;

		case MODE_WORK_COUPLING:
			sm36_oem_cal_heading_no_ins(oem_type,baud,freq);
		break;

		case MODE_WORK_COUPLING_INS:
			sm36_oem_cal_heading_ins(oem_type,baud,freq);
		break;

		case MODE_WORK_SINGLE:
            sm39_move_no_ins(oem_type,baud,freq);                
        break;

		case MODE_WORK_SINGLE_INS:
			sm39_move_ins(oem_type,baud,freq);                  
		break;

        case MODE_WORK_SINGLE_BASE:
            sm39_base(oem_type,param->gnss.base_latitude,param->gnss.base_longitude,param->gnss.base_altitude,baud);
        break;

        case MODE_WORK_SINGLE_2ANTENNA:
            sm39_2antenna(oem_type,baud,freq);
        break;
        
        case MODE_WORK_SINGLE_2ANTENNA_INS:
            sm39_2antenna(oem_type,baud,freq);
            cpld_set(SM39_INS);
        break;
        
		default:
            return -1;
		//break;
	}
    //printf("OEM boards were initialized successfully.\r\n");
	//printf("OEM INIT IS OK\r\n");
    return 0;
}
//#endif

