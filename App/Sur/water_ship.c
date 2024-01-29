#include "water_ship.h"
#include <stdint.h>
#include "parser.h"
#include "time.h"
#include <string.h>
#include <stdio.h>
#include "uart3.h"
#include "gpio.h"
#include <stdlib.h>
#include "TargetCtrl.h"
#include "DGPS.h"

char RECV_SHIP_FLAG=MANUAL_CONTROL;//小上船接收到的控制状态
extern SHIP_NODE ship_data_usart;
extern float gl_fMotor1 ;
extern float gl_fMotor2 ;

//"$VSCBS,A,OK*校验码"
//"$VSCBS,M,OK*校验码"
int32_t ship_output_vscbs(char status)//to A5 
{
  int len;
  char char_data[200];   
  char checksum;
  memset(char_data,0,200);   
  if(status==MANUAL_CONTROL)
      len=sprintf(char_data,"$VSCBS,M,OK");
  else
  if(status==SELF_CONTROL)   
      len=sprintf(char_data,"$VSCBS,A,OK");
  
  checksum=parser_get_checksum0183(char_data);  
  sprintf(char_data+len,"*%02X\r\n",checksum);               
  printf("%s",char_data); 
  uart3_transmit(char_data,len+5);//output_to A5
  return 0;
}

//"$VSAPID,OK*校验码"
int32_t ship_output_vsapid(void)//to A5 
{
  int len;
  char char_data[200];   
  char checksum;
  memset(char_data,0,200);   
  len=sprintf(char_data,"$VSAPID,OK");
  
  checksum=parser_get_checksum0183(char_data);  
  sprintf(char_data+len,"*%02X\r\n",checksum);               
  printf("%s",char_data); 
  uart3_transmit(char_data,len+5);//output_to A5
  return 0;
}
void self_control_init(void)
{
    Parameter_Init();
}
void self_ctrl_gps(unsigned char *pucDataBuf,unsigned int unDataLen)
{
    ParseDGPSFrm(pucDataBuf, unDataLen);
}
void self_tracking(void)
{
    static int num=0;
    num++;
    if(num%2==1)
    {
        MainCtrol();
        num=0;
    }
            
}
void ship_motor_control(int Motor_v_r,int Motor_v_l)
{
    Time_pwm_setCompare(Motor_v_r, Motor_v_l, 1500, 1500);     
}
void vessel_mode_init(void)
{
    self_control_init();
}
void vessel_go_code(void)
{
    static int Motor_v_r,Motor_v_l;
    int self_m_r,self_m_l;
    TIME_EXECUTE_PERIODICALLY_BEGIN(SHIP_CONTROL_OUTPUT_RATE)//100 10Hz
    {
        code_runing_led();
    }
    
        
    if(RECV_SHIP_FLAG==MANUAL_CONTROL)
    {
        //Motor_v_r= ship_data_usart.Motor_v_r;//船上接收数据控制电机        
        //Motor_v_l= ship_data_usart.Motor_v_l;   
        
        Motor_v_r= ship_data_usart.Motor_v_r>2000||ship_data_usart.Motor_v_r<1000?Motor_v_r:ship_data_usart.Motor_v_r;//船上接收数据控制电机        
        Motor_v_l= ship_data_usart.Motor_v_l>2000||ship_data_usart.Motor_v_l<1000?Motor_v_l:ship_data_usart.Motor_v_l;//船上接收数据控制电机        
     //   printf("Motor mannal %d %d \r\n",Motor_v_r,Motor_v_l);
    }
    else
        if(RECV_SHIP_FLAG==SELF_CONTROL)
        {
            self_tracking();
           // Motor_v_l=(int)gl_fMotor1;
           // Motor_v_r=(int)gl_fMotor2;
            self_m_r=(int)gl_fMotor1;
            self_m_l=(int)gl_fMotor2;
            
            Motor_v_r= self_m_r>2000||self_m_r<1000?Motor_v_r:self_m_r;//      
            Motor_v_l= self_m_l>2000||self_m_l<1000?Motor_v_l:self_m_l;//
            //printf("Motor Self %d %d \r\n",Motor_v_r,Motor_v_l);
            extern double dblX0, dblY0, dblX1, dblY1, dblX, dblY, dblYaw0,dblYaw0_Last;	//定义坐标XY轴
   //         printf("Motor Self %d %d %f %f %f %f \r\n",Motor_v_r,Motor_v_l, dblYaw0,dblYaw0_Last,gl_dblDiffHeading,gl_dblDiffHorSpd);
        }
    printf("Motor control %d %d %d\r\n",Motor_v_r,Motor_v_l,RECV_SHIP_FLAG);

   // printf("Motor Self %d %d %d %f %f %f %f\r\n",RECV_SHIP_FLAG,Motor_v_r,Motor_v_l, dblYaw0,dblYaw0_Last,gl_dblDiffHeading,gl_dblDiffHorSpd);

    ship_motor_control(Motor_v_r,Motor_v_l);
    TIME_EXECUTE_PERIODICALLY_END() 
}


int32_t smc_is(const char *line)
{
    return strncmp(line,"$SMC,",5) == 0;
}

int32_t smc_parser(SHIP_NODE *ship_node,const char *line)
{
    memset(ship_node,0,sizeof(*ship_node));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 7 || strcmp(section[1-1],"$SMC") != 0)
        return -1;
    //
    // 计算结构体成员的值
    ship_node->Motor_v_r           = atoi(section[2-1]);
    ship_node->Motor_v_l           = atoi(section[3-1]);
    ship_node->Motor_ctrl          = atoi(section[4-1]);
    ship_node->status              = atoi(section[5-1]);
    ship_node->time                = atof(section[6-1]);
    ship_node->checksum            = atoi(section[7-1]);
   
    return 0;
}
int32_t bsvsc_is(const char *line)
{
    return strncmp(line,"$BSVSC,",7) == 0;
}
int32_t bsvsc_parser(const char *line)//水上接收
{
  
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if((section_amount != 3 && section_amount != 4 )|| strcmp(section[1-1],"$BSVSC") != 0)
        return -1;
    //
    // 计算结构体成员的值
    if(*section[2-1]=='A')
    {
        RECV_SHIP_FLAG=SELF_CONTROL; 
        ship_output_vscbs(SELF_CONTROL);
    }
    else
    if(*section[2-1]=='M')
    {
        RECV_SHIP_FLAG=MANUAL_CONTROL;
        ship_output_vscbs(MANUAL_CONTROL);
    }
    
    printf("recv:bsvsc %d\r\n",RECV_SHIP_FLAG);
    return 0;
}

int32_t psic_tra_is(const char *line)
{
    return strncmp(line,"$PSIC,TRA,",10) == 0;
}

int32_t psic_tra_parser(psic_tra_t *psic_tra,const char *line)
{
    memset(psic_tra,0,sizeof(*psic_tra));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 11 || strcmp(section[1-1],"$PSIC") != 0|| strcmp(section[2-1],"TRA") != 0)
        return -1;
    //
    // 计算结构体成员的值
    psic_tra->utc_ms       = parser_str2utc(section[3-1]);
    psic_tra->heading      = atof(section[4-1]);
    psic_tra->pitch        = atof(section[5-1]);
    psic_tra->roll         = atof(section[6-1]);
    
    psic_tra->valid        = atoi(section[7-1]);    
    psic_tra->num          = atoi(section[8-1]);
    psic_tra->diff_age     = atoi(section[9-1]);
    psic_tra->ID           = atoi(section[10-1]);
  
    
    const int32_t DELAY_MAX = 5;
    if(psic_tra->valid == 4 && psic_tra->diff_age >= DELAY_MAX)
        psic_tra->valid = 5;
    //
    return 0;
}
int32_t vspid_is(const char *line)
{
    return strncmp(line,"$VSPID,",7) == 0;
}

int32_t vspid_parser(vspid_t *vspid,const char *line)
{
    memset(vspid,0,sizeof(*vspid));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 14 || strcmp(section[1-1],"$VSPID") != 0)
        return -1;
 
    // 计算结构体成员的值
    vspid->headingPID.p       = atof(section[2-1]);
    vspid->headingPID.i       = atof(section[3-1]);
    vspid->headingPID.d       = atof(section[4-1]);
    
    vspid->trackPID.p       = atof(section[5-1]);
    vspid->trackPID.i       = atof(section[6-1]);
    vspid->trackPID.d       = atof(section[7-1]);
    
    vspid->speedPID.p       = atof(section[8-1]);
    vspid->speedPID.i       = atof(section[9-1]);
    vspid->speedPID.d       = atof(section[10-1]);
    
    vspid->posPID.p       = atof(section[11-1]);
    vspid->posPID.i       = atof(section[12-1]);
    vspid->posPID.d       = atof(section[13-1]);
    //
    return 0;
}
