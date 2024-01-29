// parser/gpgga.h
// 2013-11-29 14:45:47
// yys@system-intergration-department

// 解析GPGA的库�?
#ifndef __FILE_GPGGA_H__
#define __FILE_GPGGA_H__

#include <stdint.h>

typedef struct
{
    uint32_t utc_ms;//毫秒
    uint8_t nosv;//卫星�?    
    uint8_t valid;//解状�?    
    uint8_t diff_age;//差分延时时间 yys2014/06/24添加
    double latitude;
    double longitude;
    double altitude;
    float hdop;
} gpgga_t;

int32_t gpgga_is(const char *line);
int32_t gpgga_parser(gpgga_t *gpgga,const char *line);

#endif // __FILE_GPGGA_H__
