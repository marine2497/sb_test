// parser/gprmc.h
// 2013-12-10 9:15:48
// yys@system-intergration-department

// 解析GPGA的库。

#ifndef __FILE_GPRMC_H__
#define __FILE_GPRMC_H__

#include <stdint.h>

typedef struct
{
    uint32_t utc_ms;
    uint32_t valid;
	  
	  double latitude;
    double longitude;
	
    float speed; // 单位:m/s
    float heading;
    uint32_t year;
    uint32_t month;
    uint32_t day;
} gprmc_t;

int32_t gprmc_is(const char *line);
int32_t gprmc_parser(gprmc_t *gprmc,const char *line);

#endif // __FILE_GPRMC_H__
