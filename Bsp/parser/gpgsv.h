// parser/gprmc.h
// 2013-12-10 9:15:48
// yys@system-intergration-department

// 解析GPGA的库。

#ifndef __FILE_GPGSV_H__
#define __FILE_GPGSV_H__

#include <stdint.h>

typedef struct
{
    uint32_t num;//卫星编号
} gpgsv_t;

int32_t gpgsv_is(const char *line);
int32_t gpgsv_parser(gpgsv_t *gpgsv,const char *line);

#endif // __FILE_GPRMC_H__
