#ifndef   _OEM_H
#define   _OEM_H

#define OEM_UNKNOW           0
#define OEM_K501             1  //北斗板
#define OEM_OEM615           2  //615
#define OEM_BD910            3
#define TRI_67650_10         4  //trimble小板，低精度,SM37专用
#define OEM_K505             5  //北斗板
#define OEM_OEM617           6
#define OEM_K528             7


//最终要实现 的函数,供其它部分调用
//device_mode-工作模式   oem_type-主板类型  baud--差分口波特率真  freq-数据输出频率
int32_t init_oem(uint32_t device_mode,uint8_t oem_type,uint8_t baud,uint8_t freq);


//int32_t init_oem(uint8_t device_mode,uint8_t oem_type);
#endif	
