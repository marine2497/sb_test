#include"ins.h"
#include <stdint.h>
#include "time.h"
#include "ppnfo.h"
#include "gpgga.h"
#include "gprmc.h"
#include "parser.h"
//#include "parser/bestxyza.h"
#include "output.h"
#include "com.h"
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "uart3.h"


static gnss_t * g_gnss_sensor; // 用于存放解析结果。
extern __IO uint32_t  Motor_speed  ;
extern __IO uint32_t  Car_speed  ;
uint32_t Output_HZ=10;

uint32_t MACHINE_ID=1;

 
static CAR_INFO  car_info;
char f_Str_Output_Data(CAR_INFO car_info)
{
  int len,i;
  char char_data[200];   
  
  memset(char_data,0,200);   
 // len=sprintf(char_data,"$PSIC,VCM,%.3f,%x,%x,%.2f,%.2f,%x",car_info.m_Time,car_info.m_Status,car_info.m_Gear,car_info.m_Motor,car_info.m_Speed,car_info.m_Flag);
   len=sprintf(char_data,"$PSIC,VCM,%X,%.2f,%.2f,",car_info.m_Gear,car_info.m_Motor,car_info.m_Speed);
   for(i=0;i<CAR_STATUS_BITS;i++)
   {
     sprintf(char_data+len,"%X,",car_info.m_Car_status[i]);
     len+=2;
   }
   sprintf(char_data+len,"%X,",car_info.m_Recv);  
   len+=2;
   sprintf(char_data+len,"%X",car_info.m_Flag);
   len+=1;
   
  car_info.checksum=parser_get_checksum0183(char_data);  
  sprintf(char_data+len,"*%02X\r\n",car_info.checksum);               
  printf("%s",char_data); 
  uart3_transmit(char_data,len+5);//output_to A5
  return car_info.checksum;
}
int32_t ins_test_line_master(const char *line)
{
  
    
     if(gpgga_is(line))
    {
        gpgga_parser(&(g_gnss_sensor->master_gpgga),line);
        //
        //if( !(register_is_invalid_or_expried() || boot_des_is_invalid())
         //   && output_is_bit_set(output_current_type(),OUTPUT_BIT_MASTER_GPGGA))
        //   printf(line);
        //
        //output_triger(g_gnss_sensor);
        //
        return 0;
    }
    if(bestxyza_is(line))
    {
        bestxyza_parser(&(g_gnss_sensor->master_bestxyza),line);
        //
        return 0;
    }
    if(gprmc_is(line))
    {
        gprmc_parser(&(g_gnss_sensor->master_gprmc),line);
        //
        return 0;
    }
    if(ppnav_is(line))
    {
        ppnav_parser(&(g_gnss_sensor->ins_ppnav),line);
        MACHINE_ID=2;
        return 0;
    }
   
    //
    return 1;
}
void g_Get_speed(void)
{
  car_info.m_Motor=Motor_speed;
  car_info.m_Speed=Car_speed;  
}
void g_Get_Gear(void)
{
  car_info.m_Gear=0x00;
}
void g_Get_car_info(void)
{
    static uint32_t status[10]={0,0,0,0,0,0,0,0,0,0};
    static uint32_t time_num=0;
    uint32_t s_status;
    int i;
    TIME_EXECUTE_PERIODICALLY_BEGIN(10)//10ms
    {    
      status[time_num]=read_gpio();
      time_num++;
      if(time_num>9)
      {
        time_num=0;
        s_status=0;
        for(i=0;i<10;i++)
        {
          s_status |= status[i];
          status[i]=0;
        }
        car_info.m_Status=s_status;
        for(i=0;i<CAR_STATUS_BITS;i++)
        {
          if(s_status&0x04)
            car_info.m_Car_status[i]=0x01;
          else
            car_info.m_Car_status[i]=0x00;
          s_status=s_status>>1;
        }
       
        
        static uint64_t pre_time = 0;
        uint64_t now = time_nowUs();
        float interval_s = (now - pre_time) * 1e-6;
        
        car_info.m_Time=interval_s;
         
        g_Get_speed();
        g_Get_Gear();
        //car_info.m_Gear=0x00;
        //car_info.m_Motor=Motor_speed;
        //car_info.m_Speed=Car_speed;  
        car_info.m_Recv=0; 
        car_info.m_Flag=0;  
        car_info.checksum=f_Str_Output_Data(car_info);
      }
    }
    TIME_EXECUTE_PERIODICALLY_END()
    //printf("status: %x time: %f  %d %d \r\n",status,interval_s,Motor_speed ,Car_speed);
   
}