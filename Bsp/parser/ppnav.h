// parser/gprmc.h
// 2013-12-10 9:15:48
// yys@system-intergration-department

// 解析GPGA的库。

#ifndef __FILE_PPNAV_H__
#define __FILE_PPNAV_H__

#include <stdint.h>

typedef struct
{
    float heading;
    float pitch;
    float roll;
    uint32_t init_flag;//惯导模块初始化是否完成标志

} ppnav_t;

int32_t ppnav_is(const char *line);
int32_t ppnav_parser(ppnav_t *ppnav,const char *line);

#endif // __FILE_GPRMC_H__
