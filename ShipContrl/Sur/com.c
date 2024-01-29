
// 通信模块的主c文件。

#include "com.h"
#include "time.h"
#include <stdio.h>
#include <string.h>
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "uart5.h"
#include "getline.h"
#include "parser.h"
#include "mode-test.h"
#include "water_sensor.h"
#include "prompt.h"
#include "ins.h"
#include "ship.h"
#include "string-split.h"
gnss_t g_gnss; // 用于存放解析结果。

static getline_t g_getline_1; 
//static getline_t g_getline_2; 
static getline_t g_getline_3; 
//static getline_t g_getline_4; 
//static getline_t g_getline_5;
static char g_line_1[512];
//static char g_line_2[512];
static char g_line_3[512];
//static char g_line_4[512];
//static char g_line_5[512];
static void com_on_getline_1(char *line,int32_t length,void *param);
//static void com_on_getline_2(char *line,int32_t length,void *param);
static void com_on_getline_3(char *line,int32_t length,void *param);
//static void com_on_getline_4(char *line,int32_t length,void *param);
//static void com_on_getline_5(char *line,int32_t length,void *param);

extern char RECV_SHIP_FLAG;//小上船接收到的控制状态
extern uint32_t RECOGNITION_MODE_FLAG;

void com_init(void)
{
    memset(&g_gnss,0,sizeof(g_gnss)); 
    //memset(&save_node_data,0,sizeof(save_node_data));
//    mode_test_init(&g_gnss);
//    water_sensor_init(&g_gnss);
    prompt_init();
    getline_init(&g_getline_1,g_line_1,sizeof(g_line_1),com_on_getline_1,0,'\n');
    getline_init(&g_getline_3,g_line_3,sizeof(g_line_3),com_on_getline_3,0,'\n');  
}

void com_check_event(void)
{
     char buffer[32]; // 加static不占用栈。
    if(uart1_receiveValidBufferSize() > 0)
    {
       
        int32_t len = uart1_readReceiveBuffer(buffer,sizeof(buffer));        
        getline_input(&g_getline_1,buffer,len);
       
    } 
    //
 
    if(uart3_receiveValidBufferSize() > 0)
    {
        int32_t len = uart3_readReceiveBuffer(buffer,sizeof(buffer));        
        getline_input(&g_getline_3,buffer,len);
    }   
    
    /*
     static uint64_t pre_time = 0;
     uint64_t now ;
     now = time_nowUs();
     pre_time = time_nowUs(); 
     float interval_s = (now - pre_time) * 1e-3;//ms
    */
}
void com_on_getline_1(char *line,int32_t length,void *param)
{
    int32_t is_line_handled = 0;
    //uart1_transmit(line,length);
    //if(RECV_SHIP_FLAG==SELF_CONTROL)    
        ship_self_line(line,length);
   
    if(parser_check_line(line,length) == 0)
    {
//        prompt_handle_line(line);
        RECOGNITION_MODE_FLAG=ship_line(line,length);
        is_line_handled = 1;
      
    }    
   // 根据转发状态和行是否被解析，判断是否应该打印主板发来的行。   
    if(is_line_handled)
    {      
      //save_node_data.uart_1.utc_s   = g_gnss.ins_ppnfo.utc_s;
      //save_node_data.uart_1.roll    = g_gnss.ins_ppnfo.roll;
     // save_node_data.uart_1.pitch   = g_gnss.ins_ppnfo.pitch;
      //save_node_data.uart_1.heading = g_gnss.ins_ppnfo.heading;
     // printf(" node 1:%f,%f,%f,%f \r\n",save_node_data.uart_1.utc_s,save_node_data.uart_1.roll,\
        save_node_data.uart_1.pitch,save_node_data.uart_1.heading);     
    }
    
}

void com_on_getline_3(char *line,int32_t length,void *param)
{
    int32_t is_line_handled = 0;
    
    //uart3_transmit(line,length);
    //if(RECV_SHIP_FLAG==SELF_CONTROL)
        ship_self_line(line,length);

    
    if(parser_check_line(line,length) == 0)
    {
      RECOGNITION_MODE_FLAG=ship_line(line,length);
      is_line_handled = 1;       
    }
    if(is_line_handled)
    {    
    }
}
