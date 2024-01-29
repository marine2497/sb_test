#include "base_ship.h"
#include "ship.h"
#include <stdint.h>
#include "parser.h"
#include "time.h"
#include <string.h>
#include <stdio.h>
#include "uart3.h"
#include "gpio.h"
#include <stdlib.h>


extern uint32_t  PWM_time0;//TIM8_1---CH3--油门
extern uint32_t  PWM_time1;//TIM8_2---CH4--左右
extern uint32_t  PWM_time2;//TIM8_3---CH5--开关
extern uint32_t  PWM_time3;//TIM8_4---CH6--左右微调
static char BASE_SEND_CONTROL_FLAG=BASE_SEND_CONTROL_ON;
uint32_t change_max_min(uint32_t in)
{
    uint32_t t;
    if(in<1100) 
         t=1100;
    if(in>1900)
         t=1900;
    
    return (t-1100)*10/8+1000;
}
uint32_t Motor_change_max_min(uint32_t in)
{
    if(in<1000) 
         return 1000;
    if(in>2000)
         return 2000;
    if(in>1450&&in<1550)
        return 1500;
    return in;
}

static SHIP_NODE ship_data;
char BASE_FLAG=MANUAL_CONTROL;//岸上控制船状态
//static char SEND_SHIP_FLAG=MANUAL_CONTROL;
char BASE_MODE_CHANGE_FLAG=0;//为0正常，为1，当前状态没有确认


///"$SMC,Motor_v_r,Motor_v_L,Motor_ctrl,0,0x2a*校验码"
static int32_t ship_output(void)//to A5 
{
  int len;
  char char_data[200];   
  
  memset(char_data,0,200);   
  len=sprintf(char_data,"$SMC,%d,%d,%d,%X,%.2f",ship_data.Motor_v_r,ship_data.Motor_v_l,ship_data.Motor_ctrl,ship_data.status,ship_data.time);
  
  ship_data.checksum=parser_get_checksum0183(char_data);  
  sprintf(char_data+len,"*%02X\r\n",ship_data.checksum);               
 // printf("%s",char_data); 
  uart3_transmit(char_data,len+5);//output_to A5
  return 0;
}
//"$BSVSC,A*校验码"
static int32_t ship_output_bsvsc(char status)//to A5 
{
  int len;
  char char_data[200];   
  char checksum;
  memset(char_data,0,200);   
  if(status==MANUAL_CONTROL)
      len=sprintf(char_data,"$BSVSC,M");
  else
  if(status==SELF_CONTROL)   
      len=sprintf(char_data,"$BSVSC,A");
  
  checksum=parser_get_checksum0183(char_data);  
  sprintf(char_data+len,"*%02X\r\n",checksum);               
  printf("%s",char_data); 
  uart3_transmit(char_data,len+5);//output_to A5
  return 0;
}

void mode_change_check(uint16_t val3)
{
    static int Output_num=0;
    if(BASE_FLAG==MANUAL_CONTROL)//手动切换到自动
    {    
        if(val3<1500)//输出出控制 
        {
            Output_num++;
            if(Output_num>2)
            {
                BASE_FLAG=SELF_CONTROL;
                ship_output_bsvsc(BASE_FLAG);
                BASE_MODE_CHANGE_FLAG=1;
                Output_num=0;                
            }
        }
        else
        {
            Output_num=0;
        }
    }
    if(BASE_FLAG==SELF_CONTROL)//自动切换到手动
    {
        if(val3>1500)//输出出控制 
        {
            Output_num++;
            if(Output_num>2)
            {
                BASE_FLAG=MANUAL_CONTROL;
                ship_output_bsvsc(BASE_FLAG);
                BASE_MODE_CHANGE_FLAG=1;     
                Output_num=0;
            }
        }
        else
        {
            Output_num=0;
        }
    }
}
uint8_t base_read_remote_controllor(void)
{
    return BASE_READ_REMOTE_CONTROLLOR;
}
void base_mode_init(void)//强制为手动才能开始启动
{
    static uint16_t val1=1500;//
    static uint16_t val3=1500;//
    uint16_t Output_num=0;
    val3=PWM_time2>3000?val3:PWM_time2;
    val3=change_max_min(val3);
    if(val3<1500)
    {
        printf("Note: this is base self control ,please change to mannual mode for running \r\n");
        printf("      if base manual control ,please confirm the rod in the middle. \r\n");
    }
    else
        printf("Note: this is base manual control ,please confirm the rod in the middle. \r\n");
    while(1)
    {
        
      base_change_to_vessel_code();
      TIME_EXECUTE_PERIODICALLY_BEGIN(200)//100 10Hz
      {   
          val1=PWM_time0>2000||PWM_time0<1000?val1:PWM_time0;
         // val3=PWM_time2>2100||PWM_time2<900?val3:PWM_time2;          
          val3=PWM_time2;
          val3=change_max_min(val3);  
          val1=change_max_min(val1);
          printf("mid oil : %d %d\r\n",val1,val3);
          if(val3>1500&&val1<1600&&val1>1400)//输出出控制 
          {
              Output_num++;
              if(Output_num>20)
              {
                  printf("Note: this is base manual control ,code running. \r\n");
                  time_waitMs(1000);
                  if(base_read_remote_controllor()==1)
                  return ;
              }
          }
          else
          {
              Output_num=0;
          }
          
      }    
      TIME_EXECUTE_PERIODICALLY_END() 
    }
    
}
int16_t turn_corner_change(int16_t in)
{
  return (in*in*in/150000);
}
void recv_telecontroller_information(void)
{   
    
//TIM8_1---CH3--油门
//TIM8_2---CH4--左右
//TIM8_3---CH5--开关
//TIM8_4---CH6--左右微调
    static uint16_t val1=1500;//
    static uint16_t val2=1500;//
    static uint16_t val3=1500;//
    static uint16_t val4=1500;//
    static char remote_flag=0;
     TIME_EXECUTE_PERIODICALLY_BEGIN(SHIP_CONTROL_OUTPUT_RATE)//100 10Hz
     {
         code_runing_led();
         //printf("pwm time is : %d %d %d %d \r\n",PWM_time0,PWM_time1,PWM_time2,PWM_time3);   
         val1=PWM_time0>2000||PWM_time0<1000?val1:PWM_time0;
         val2=PWM_time1>2000||PWM_time1<1000?val2:PWM_time1;
         val3=PWM_time2>2100||PWM_time2<900?val3:PWM_time2;
         //val3=PWM_time2;
         val4=PWM_time3>2000||PWM_time3<1000?val4:PWM_time3;
         
         val1=change_max_min(val1);
         val2=change_max_min(val2);
         val3=change_max_min(val3);
         val4=change_max_min(val4);
         printf("change time is : %d %d %d %d  change %d %d %d %d \r\n",PWM_time0,PWM_time1,PWM_time2,PWM_time3,val1,val2,val3,val4);   
         
         

         ship_data.Motor_v_r=Motor_change_max_min(val1+(val4-1500)/3+turn_corner_change(val2-1500));
         ship_data.Motor_v_l=Motor_change_max_min(val1-(val4-1500)/3-turn_corner_change(val2-1500));

         ship_data.time = (float)time_nowMs()/1000;
         ship_data.status=BASE_FLAG;
//         Time_pwm_setCompare(ship_data.Motor_v_r, ship_data.Motor_v_l, 1500, 1500); 
         if(base_read_remote_controllor()==0)
         {
             remote_flag++;
             if(remote_flag>2)remote_flag=2;
             if(remote_flag==1)
                 printf("Note: remote controllor off,the ship may be not controled ,please open ! \r\n");
             return ;
         }
         else remote_flag=0;
         mode_change_check(val3);
         
         if(BASE_MODE_CHANGE_FLAG==1)
         {
             ship_output_bsvsc(BASE_FLAG);  
             BASE_SEND_CONTROL_FLAG=BASE_SEND_CONTROL_ON;
         }
         if(BASE_FLAG==MANUAL_CONTROL)//手动控制
         {
             if(BASE_SEND_CONTROL_FLAG==BASE_SEND_CONTROL_ON)
                 ship_output();                          
         }
         //else if(SHIP_FLAG==SELF_CONTROL)//自主循迹
         //{
             
        // }
         //printf("motor control: %d,%d\r\n",ship_data.Motor_v_r,ship_data.Motor_v_l);
     }      
     TIME_EXECUTE_PERIODICALLY_END()
}



int32_t vscbs_is(const char *line)
{
    return strncmp(line,"$VSCBS,",7) == 0;
}
int32_t vscbs_parser(const char *line)//岸上接收
{
  
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 4 || strcmp(section[1-1],"$VSCBS") != 0)
        return -1;
    //
    // 计算结构体成员的值
    if(strcmp(section[3-1],"OK") == 0)
    {
        if(*section[2-1]=='A')
        {
            if(BASE_FLAG==SELF_CONTROL)
                BASE_MODE_CHANGE_FLAG=0;            
            else
            if(BASE_FLAG==MANUAL_CONTROL)
                BASE_SEND_CONTROL_FLAG=BASE_SEND_CONTROL_OFF;
        }
        else
            if(*section[2-1]=='M')
            {
                if(BASE_FLAG==MANUAL_CONTROL)
                {
                    BASE_MODE_CHANGE_FLAG=0;
                    BASE_SEND_CONTROL_FLAG=BASE_SEND_CONTROL_ON;
                }
                else
                if(BASE_FLAG==SELF_CONTROL)
                    BASE_FLAG=MANUAL_CONTROL;
            }
    }
    else
        return -1;
    printf("recv:vscbs %d\r\n",BASE_FLAG);
    return 0;
   
}


int32_t vsapid_is(const char *line)
{
    return strncmp(line,"$VSAPID,",8) == 0;
}
int32_t vsapid_parser(const char *line)//岸上接收
{
  
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 3 || strcmp(section[1-1],"$VSAPID") != 0)
        return -1;
    //
    // 计算结构体成员的值
    if(strcmp(section[2-1],"OK") == 0)
    {
        printf("recv:vsapid  set pid ok\r\n");
    }
    else
        return -1;
    return 0;
   
}
