
#ifndef __FILE_WATER_SENSOR_H__
#define __FILE_WATER_SENSOR_H__

#include <stdint.h>
#include "gnss.h"
typedef struct {
  unsigned int water_depth;//ˮλ water depth
  short water_T;//�¶�temperature
  unsigned int water_P;//ѹ��pressure
  unsigned short status;
  //����ѹatmospheric pressure
  //��yyyy  year
  //����mmdd  moth
  //ʱ��hhMM  
  //��ss
  //ʣ�����
  //��ص�ѹ
  //����״̬
  //��Ʒ���к�
  //Ӳ���汾��
  //����汾��
  //��������
  //ͨ�ŵ�ַ  
  //������
  //У��λ
  //��ʱʱ��
  //ˮλ��λ
  //ѹ����λ
  //�¶ȵ�λ
  //ˮλ���
  //ˮλ����
  //�����ܶ�
  //��������
  //��ؿ���
  //��ع����ܳ�
  //��֤�롣
  char checksum;
}WATER_SENSOR_NODE;
void water_sensor_init(void);
void water_sensor_read_cmd(void);
int32_t water_sensor_line(const char *line);
int32_t water_sensor_check_line(const char * line);

#endif // __FILE_GNSS_MODE_DIVIDED_H__

