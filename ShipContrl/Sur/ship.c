#include "ship.h"
#include "com.h"
#include "base_ship.h"
#include "water_ship.h"
#include "TargetCtrl.h"
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

uint32_t RECOGNITION_MODE_FLAG=0;

static uint32_t  CODE_MODE_STATUS=0;
SHIP_NODE ship_data_usart;
extern char BASE_MODE_CHANGE_FLAG;
extern char BASE_FLAG;

static gpgga_t    ship_data_gpgga;
static gprmc_t    ship_data_gprmc;
static gphdt_t    ship_data_gphdt;
static psic_tra_t ship_data_psic_tra;
static vspid_t    ship_data_vspid;

void ship_init()
{   
    printf("******** CODE  INIT  ******** \r\n"); 
    
    GPIO_pwm_in_init();
    GPIO_pwm_out_init();
    time_pwm_init();
    CODE_MODE_STATUS=BASED_STATION;//岸上
    Time_pwm_setCompare(1500,1500,1500,1500);   
    time_waitMs(10000);
}

void code_runing_led(void)
{
    static char flag=1;
    static unsigned short run_time=0;   
    
    run_time++;
    if(run_time>500/SHIP_CONTROL_OUTPUT_RATE) 
    {
        run_time=0;
        flag = 0x01&(~flag) ;
    }
    
    if(flag==0)    
        SHIP_LED_OFF; 
    else
        SHIP_LED_ON;
}







#define SMC_DATA_RECV        0x01   //船上
#define BSVSC_DATA_RECV      0x02   //船上
#define GPGGA_DATA_RECV      0x03   //船上
#define GPRMC_DATA_RECV      0x04   //船上
#define GPHDT_DATA_RECV      0x05   //船上
#define PSIC_TRA_DATA_RECV   0x06   //船上
#define VSPID_DATA_RECV      0x07   //船上

#define VSCBS_DATA_RECV      0x11   //岸上
#define VSAPID_DATA_RECV     0x12   //岸上

uint32_t recognition_mode(void)
{
//    $GPGGA,021535.40,2307.5868801,N,11321.9305179,E,2,21,1.5,30.315,M,-6.60,M,07,0137*7B
//    $GPHDT,196.67,T*0A
//    $GPRMC,025230.00,A,2307.44471268,N,11322.13768069,E,1.554,179.245,060916,999.9000,E,A*0D
//    $PSIC,TRA,025230.00,11.2,0.2,0.3,2,23,3,12*XX 
    static char recv_gnss_data_flag=0x00;
    static int time_sum=0; 
    
      switch (RECOGNITION_MODE_FLAG)
      {
        case GPGGA_DATA_RECV:
          recv_gnss_data_flag |=0x01;
          RECOGNITION_MODE_FLAG=0;
          break;
        case GPRMC_DATA_RECV:
          recv_gnss_data_flag |=0x02;
          RECOGNITION_MODE_FLAG=0;
          break;
        case PSIC_TRA_DATA_RECV:
          recv_gnss_data_flag |=0x04;
          RECOGNITION_MODE_FLAG=0;
          break;
        default:
          break; 
      }
      //if(recv_gnss_data_flag==0x07)
      if(recv_gnss_data_flag)
      {
          //printf("timesum %d recv_gnss_data_flag %x !\r\n",time_sum,recv_gnss_data_flag);
          time_sum++;
          recv_gnss_data_flag=0x00;
          if(time_sum>10)
          {              
              printf("timesum %d recv_gnss_data_flag %x !\r\n",time_sum,recv_gnss_data_flag);
              CODE_MODE_STATUS=VESSEL_STATION;//水上
             // time_sum=0;
              //return CODE_MODE_STATUS;
          }
      } 
      return 0;
}

int32_t ship_check_line(const char *line,int32_t length)
{
    //船上接收命令头解析
    if(smc_is(line))
        return SMC_DATA_RECV;
    if(bsvsc_is(line))
        return BSVSC_DATA_RECV;
    if(gpgga_is(line))
        return GPGGA_DATA_RECV;
    if(gprmc_is(line))
        return GPRMC_DATA_RECV;
    if(gphdt_is(line))
        return GPHDT_DATA_RECV;
    if(psic_tra_is(line))
        return PSIC_TRA_DATA_RECV;    
    if(vspid_is(line))
    {
        if(CODE_MODE_STATUS==BASED_STATION)
            uart3_transmit(line,length);//output_to A5
        return VSPID_DATA_RECV;
    }
    //岸上接收命令头解析

    
    
    if(vscbs_is(line))
        return VSCBS_DATA_RECV;
    if(vsapid_is(line))
        return VSAPID_DATA_RECV;
    return 0;
    
}
void ship_self_line( char *line,int32_t length)
{
    self_ctrl_gps(line,length);
}
/*调试参数*/
extern double gl_dblLockYaw_Kp , gl_dblLockYaw_Ki , gl_dblLockYaw_Kd ;//直线段PID 
//extern double gl_dblLockYaw_Kp = 0.03, gl_dblLockYaw_Ki = 0.0, gl_dblLockYaw_Kd =  0.06;//直线段PID 

extern double gl_dblTurning_Kp , gl_dblTurning_Ki , gl_dblTurning_Kd ; //转弯段PID

extern double gl_dblYawDis_Kp, gl_dblYawDis_Ki , gl_dblYawDis_Kd ;//偏航距修正PID
//extern double gl_dblYawDis_Kp = 10, gl_dblYawDis_Ki = 0, gl_dblYawDis_Kd = 19;//偏航距修正PID
extern double gl_dblSpeed_Kp, gl_dblSpeed_Ki , gl_dblSpeed_Kd ;//速度环PID
//extern double gl_dblSpeed_Kp = 0.3, gl_dblSpeed_Ki = 0.05, gl_dblSpeed_Kd = 10;//速度环PID

//double gl_dblRatio=0.8;//差动速度抑制比K
//double gl_dblSetSpeed=1.5;//给定直线段速度

/*预留调试参数*/
//double gl_dblLockYaw = 0.0,gl_dblLockYaw_a = 0.0,gl_dblLockYaw_T = 0.0;// 直线段给定航向 超前矫正系数
//double gl_dblSpeedDownLine = 2;//减速线
//double gl_dblTurnRadius = 5;//直角转弯半径
//double gl_dblSetTurnTime = 3 ;//给定转弯时间
extern double gl_dblPos_Kp , gl_dblPos_Ki , gl_dblPos_Kd ;//位置环PID
//extern double gl_dblPos_Kp = 0, gl_dblPos_Ki = 0, gl_dblPos_Kd = 0;//位置环PID
//double gl_dblDiffMotorRatio = 1;//电机前后动力输出比例
//double dt=0.0;//转弯时间
int32_t ship_line(const char *line,int32_t length)
{
  switch(ship_check_line(line,length))
  {
    case SMC_DATA_RECV:
      if(smc_parser(&ship_data_usart,line) == 0)
      {
          //time = (float)time_nowMs()/1000;
          printf("r %d,%d,%d,%X,%f\r\n",ship_data_usart.Motor_v_r,ship_data_usart.Motor_v_l,ship_data_usart.Motor_ctrl,ship_data_usart.status,ship_data_usart.time);
          //printf(line);
          return SMC_DATA_RECV;
      } 
    case BSVSC_DATA_RECV:
      bsvsc_parser(line);
      return BSVSC_DATA_RECV;
    case GPGGA_DATA_RECV:
      if(gpgga_parser(&ship_data_gpgga,line)==0)
      {   
        //  printf("recv gpgga\r\n");
      }
      return GPGGA_DATA_RECV;
    case GPRMC_DATA_RECV:
      if(gprmc_parser(&ship_data_gprmc,line)==0)
      {   
         // printf("recv gprmc\r\n");
      }
      return GPRMC_DATA_RECV;
    case GPHDT_DATA_RECV:
      if(gphdt_parser(&ship_data_gphdt,line)==0)
      {   
         // printf("recv gphdt\r\n");
      }
      return GPHDT_DATA_RECV;      
    case PSIC_TRA_DATA_RECV:
      if(psic_tra_parser(&ship_data_psic_tra,line)==0)
      {
         // printf("recv psic,tra\r\n");
      }      
      return PSIC_TRA_DATA_RECV;
    case VSPID_DATA_RECV:
      if(vspid_parser(&ship_data_vspid,line)==0)
      {
          gl_dblLockYaw_Kp = ship_data_vspid.headingPID.p;
          gl_dblLockYaw_Ki = ship_data_vspid.headingPID.i;
          gl_dblLockYaw_Kd = ship_data_vspid.headingPID.d;
          
          gl_dblYawDis_Kp = ship_data_vspid.trackPID.p;
          gl_dblYawDis_Ki = ship_data_vspid.trackPID.i;
          gl_dblYawDis_Kd = ship_data_vspid.trackPID.d;
          
          gl_dblSpeed_Kp = ship_data_vspid.speedPID.p;
          gl_dblSpeed_Ki = ship_data_vspid.speedPID.i;
          gl_dblSpeed_Kd = ship_data_vspid.speedPID.d;
          
          gl_dblTurning_Kp = ship_data_vspid.posPID.p;
          gl_dblTurning_Ki = ship_data_vspid.posPID.i;
          gl_dblTurning_Kd = ship_data_vspid.posPID.d; 

/*调试参数*/
//double gl_dblLockYaw_Kp = 0.03, gl_dblLockYaw_Ki = 0.0, gl_dblLockYaw_Kd =  0.06;//直线段PID 
//double gl_dblLockYaw_Kp = 0.1, gl_dblLockYaw_Ki = 0.0, gl_dblLockYaw_Kd =  0.06;//直线段PID 
//double gl_dblTurning_Kp = 0.15, gl_dblTurning_Ki = 0.0, gl_dblTurning_Kd =  0.03; //转弯段PID
//double gl_dblYawDis_Kp = 10, gl_dblYawDis_Ki = 0, gl_dblYawDis_Kd = 19;//偏航距修正PID
//double gl_dblYawDis_Kp = 10, gl_dblYawDis_Ki = 0, gl_dblYawDis_Kd = 19;//偏航距修正PID
//double gl_dblSpeed_Kp = 0.3, gl_dblSpeed_Ki = 0.05, gl_dblSpeed_Kd = 10;//速度环PID
//double gl_dblRatio=0.8;//差动速度抑制比K
//double gl_dblSetSpeed=1.5;//给定直线段速度
//double gl_dblSetSpeed=0.8;//给定直线段速度
          Parameter_Init();


          //gl_dblPos_Kp = ship_data_vspid.posPID.p;
          //gl_dblPos_Kd = ship_data_vspid.posPID.i;
          //gl_dblPos_Kd = ship_data_vspid.posPID.d; 
          if(CODE_MODE_STATUS==VESSEL_STATION)
              ship_output_vsapid();//answer ok
          printf("recv vspid\r\n");
      }      
      return VSPID_DATA_RECV;
      
  ///岸上接收解析  
    case VSCBS_DATA_RECV:
      if(vscbs_parser(line)==0)
      {   
          printf("recv vscbs\r\n");
      }
      return VSCBS_DATA_RECV;
    case VSAPID_DATA_RECV:
      if(vsapid_parser(line)==0)
      {   
          printf("recv vsapid\r\n");
      }
      return VSAPID_DATA_RECV;
      
    default:
      break;      
  }
  
    return 0;
}
void vessel_code(void)
{
    printf("********  VESSEL ******** \r\n"); 
    vessel_mode_init();
    while(1)
    {     
        com_check_event();
        vessel_go_code();
    }
}

void base_change_to_vessel_code(void)
{
    com_check_event();
    recognition_mode(); 
    if(CODE_MODE_STATUS==VESSEL_STATION) 
    {
        printf("********  CHANGE TO VESSEL ******** \r\n"); 
        vessel_code();
    }
}

void base_code(void)
{
    printf("******** BASED ******** \r\n"); 
    base_mode_init();
    while(1)
    {     
        //com_check_event();
        base_change_to_vessel_code();
        recv_telecontroller_information();//岸上控制数据输出  
        
    }      
}
uint32_t ship_code_init_mode(void)
{
    static int time_num=0;
    if(base_read_remote_controllor()==0)
        printf("Note: if this is base ,the remote controllor off ,please open it ! \r\n");
    while(1)
    {       
        com_check_event();
        recognition_mode();     
        if(CODE_MODE_STATUS==VESSEL_STATION) 
            return 0;
        TIME_EXECUTE_PERIODICALLY_BEGIN(SHIP_CONTROL_OUTPUT_RATE)//100 10Hz  
        code_runing_led();       
        if(base_read_remote_controllor()==1)
        {
            printf("read io 1 \r\n");
            time_num++;
            if(time_num>100)
            {
                CODE_MODE_STATUS=BASED_STATION;
                return 0;
            }
        }
        else
            time_num=0;
        TIME_EXECUTE_PERIODICALLY_END() 
    }
}
void ship_code_running(void)
{
   
    //ship_code_init_mode();
    
    if(CODE_MODE_STATUS==BASED_STATION)   
        base_code();  
    if(CODE_MODE_STATUS==VESSEL_STATION)
        vessel_code();
    
}


