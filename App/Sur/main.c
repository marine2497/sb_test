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
  // BSP���ȳ�ʼ����
    bsp_init();
    com_init();
    printf("app started ok !\r\n");    //

    //
    // ��ͨģ��ĳ�ʼ����
    //security_init();  
    
    printf("********SOUTH "__DATE__"-"__TIME__"******** \r\n"); 
    ship_init();
    
    
    
    
    //
    // ����ѭ��ǰ�����ÿ��Ź�����ֹ�ڳ�ʼ�����������á�
   // security_setup_dog(5);
    //
    // ��ѭ�������뱣֤����û��æ�ȴ���
    // ���к�����Ӧ���������ء�
    // �����Ҫ��ʱ��ʹ��TIME_EXECUTE_PERIODICALLY_BEGIN()��TIME_EXECUTE_PERIODICALLY_END()��
    
    //engine_init();
    //sensor_acc_calibration();
    
    //sensor_test();

    ship_code_running();
}
