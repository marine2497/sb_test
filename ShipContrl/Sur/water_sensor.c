#include "water_sensor.h"
#include <stdint.h>
#include "time.h"
#include "output.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include "uart3.h"
#include "uart5.h"
//read cmd
//0x01 0x03 0x00 0x01 0x00 0x05 0xD4 0x09


static WATER_SENSOR_NODE Water_data;
static char WATER_SENSOR_FLAG=0;

void water_sensor_init()
{   
    
}
void water_sensor_read_cmd(void)
{   
    
//    char CMD1[]={0x01, 0x03, 0x00, 0x01, 0x00, 0x05, 0xD4, 0x09};
//    char CMD2[]={0X01, 0X03, 0X00, 0X1A, 0X00, 0X01, 0XA5, 0XCD}; 
    
    TIME_EXECUTE_PERIODICALLY_BEGIN(500)//
    {    
      //printf("read_cmd out \r\n");
        if(WATER_SENSOR_FLAG==0)
        {
            WATER_SENSOR_FLAG=1;
//          uart5_transmit(CMD1,sizeof(CMD1));
        }
        else
            if(WATER_SENSOR_FLAG==1)
            {
              WATER_SENSOR_FLAG=0;
//              uart5_transmit(CMD2,sizeof(CMD2));
            }
    }
    TIME_EXECUTE_PERIODICALLY_END()
}
int32_t water_sensor_check_line(const char * line)
{   
    if(line[0] == 0x01)//地址对应
        return 0;//parser_check_crc32(line);
    return -1;
}
/// $PSIC,WSI,水深，水温，水压，保留，保留*校验码
///水深单位：mm;水温单位：度，水压单位：Pa
///
///
///
static int32_t water_sensor_output(void) 
{
  int len;
  char char_data[200];   
  
  memset(char_data,0,200);   
   len=sprintf(char_data,"$PSIC,WSI,%d,%.1f,%d,%X,%X",Water_data.water_depth,(float)Water_data.water_T/10,Water_data.water_P,Water_data.status,0);
 
   
  Water_data.checksum=parser_get_checksum0183(char_data);  
  sprintf(char_data+len,"*%02X\r\n",Water_data.checksum);               
  printf("%s",char_data); 
  uart3_transmit(char_data,len+5);//output_to A5
  return 0;
}
int32_t water_sensor_line(const char *line)
{
   const char *data;
   data=line;
  
   
   
   if(data[1]==0x03&&data[2]==0x0a)
   {     
     Water_data.water_depth = (data[3]<<24)|(data[4]<<16)|(data[5]<<8)|(data[6]);
     Water_data.water_T = (data[7]<<8)|(data[8]);
     Water_data.water_P = (data[9]<<24)|(data[10]<<16)|(data[11]<<8)|(data[12]);
     //printf("water  depth:%d mm  T: %f deg  Pressure: %d Pa\r\n",Water_data.water_depth,(float)Water_data.water_T/10,Water_data.water_P);    
   }
   else
   if(data[1]==0x03&&data[2]==0x02)
   {     
//     Water_data.status = (data[3]<<8)|(data[4]);
     Water_data.status = (data[4])&0x7f;     
   }
   if(WATER_SENSOR_FLAG==1)
      water_sensor_output();  
   //recved: 01 03 0A 00 00 00 8E 01 07 00 00 05 72 7D DA     
    return 1;
}




