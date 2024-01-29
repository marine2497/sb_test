// parser/headinga.h
// 2013-12-2 9:23:43
// yys@system-intergration-department

// 解析HEADINGA的库。

#ifndef __FILE_HEADINGA_H__
#define __FILE_HEADINGA_H__

#include <stdint.h>

typedef struct
{
    uint16_t sol_type;  // 解状态
    uint16_t sol_n;     // 参与计算航向的卫星数
    uint32_t gps_week;
    uint32_t gps_ms;
    float base_line_length;
    float heading; // 单位：度
    float pitch;   // 单位：度
} headinga_t;

int32_t headinga_is(const char *line);
int32_t headinga_parser(headinga_t *headinga,const char *line);

#endif // __FILE_HEADINGA_H__
