// parser/bestxyza.h
// 2013-11-29 15:11:56
// yys@system-intergration-department

// 解析BESTXYZA

#ifndef __FILE_BESTXYZA_H__
#define __FILE_BESTXYZA_H__

#include <stdint.h>

typedef struct
{
    uint32_t gps_week;
    uint32_t gps_ms;
    float ve;
    float vn;
    float vu;
    float ae;
    float an;
    float au;
} bestxyza_t;

int32_t bestxyza_is(const char *line);
int32_t bestxyza_parser(bestxyza_t *bestxyza,const char *line);

#endif // __FILE_BESTXYZA_H__
