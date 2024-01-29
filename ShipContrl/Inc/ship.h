#ifndef __FILE_SHIP_H__
#define __FILE_SHIP_H__

#include <stdint.h>
#include "gnss.h"

#define SHIP_LED_OFF GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define SHIP_LED_ON  GPIO_SetBits(GPIOC,GPIO_Pin_2)

#define BASE_READ_REMOTE_CONTROLLOR  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)

#define BASED_STATION  0X01 //∞∂…œ
#define VESSEL_STATION  0X02 //ÀÆ…œ¥¨

#define MANUAL_CONTROL 0X00
#define SELF_CONTROL   0X01

#define BASE_SEND_CONTROL_ON  0x00
#define BASE_SEND_CONTROL_OFF 0x01

#define SHIP_CONTROL_OUTPUT_RATE_10Hz 100 //10Hz
#define SHIP_CONTROL_OUTPUT_RATE_50Hz 20  //50Hz

#define SHIP_CONTROL_OUTPUT_RATE SHIP_CONTROL_OUTPUT_RATE_10Hz 


typedef struct {
  int Motor_v_r;
  int Motor_v_l;
  int Motor_ctrl;
  char status;
  float time;
  char checksum;
}SHIP_NODE;



void ship_init();
void code_runing_led(void);

void ship_self_line( char *line,int32_t length);
int32_t ship_line(const char *line,int32_t length);
int32_t ship_check_line(const char * line,int32_t length);
uint32_t recognition_mode(void);
void base_change_to_vessel_code(void);
void ship_code_running(void);
#endif // __FILE_GNSS_MODE_DIVIDED_H__