
#ifndef __FILE_WATER_SENSOR_H__
#define __FILE_WATER_SENSOR_H__

#include <stdint.h>
#include "gnss.h"
typedef struct {
  unsigned int water_depth;//水位 water depth
  short water_T;//温度temperature
  unsigned int water_P;//压力pressure
  unsigned short status;
  //大气压atmospheric pressure
  //年yyyy  year
  //月日mmdd  moth
  //时分hhMM  
  //秒ss
  //剩余电量
  //电池电压
  //工作状态
  //产品序列号
  //硬件版本叼
  //软件版本号
  //生产日期
  //通信地址  
  //波特率
  //校验位
  //超时时间
  //水位单位
  //压力单位
  //温度单位
  //水位零点
  //水位满程
  //介质密度
  //调整日期
  //电池控制
  //电池供电总长
  //验证码。
  char checksum;
}WATER_SENSOR_NODE;
void water_sensor_init(void);
void water_sensor_read_cmd(void);
int32_t water_sensor_line(const char *line);
int32_t water_sensor_check_line(const char * line);

#endif // __FILE_GNSS_MODE_DIVIDED_H__

