#include "bsp.h"
#include "time.h"
#include <stdio.h>
#include "security.h"
#include "com.h"
#include "ins.h"
#include "water_sensor.h"
#include "ship.h"



extern uint32_t Output_HZ;

int main()
{
  // BSP最先初始化。
    bsp_init();
    com_init();
    printf("app started ok !\r\n");    //

    //
    // 普通模块的初始化。
    //security_init();  
    
    printf("********SOUTH "__DATE__"-"__TIME__"******** \r\n"); 
    ship_init();
    
    
    
    
    //
    // 进入循环前才配置看门狗，防止在初始化过程中重置。
   // security_setup_dog(5);
    //
    // 主循环，必须保证里面没有忙等待。
    // 所有函数都应该立即返回。
    // 如果需要定时，使用TIME_EXECUTE_PERIODICALLY_BEGIN()和TIME_EXECUTE_PERIODICALLY_END()。
    
    //engine_init();
    //sensor_acc_calibration();
    
    //sensor_test();

    ship_code_running();
}
