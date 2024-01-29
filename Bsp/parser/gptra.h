// parser/gptra.h
// 2014-9-17 10:56:51
// yys@system-intergration-department

// 解析GPTRA的库。

#ifndef __FILE_GPTRA_H__
#define __FILE_GPTRA_H__

#include <stdint.h>

typedef struct
{
    uint32_t utc_ms;//毫秒
    float heading;
    float pitch;
    float roll;
    uint16_t valid;//解状态
    uint16_t satNo;//卫星数
    float diff_age;//差分延时
} gptra_t;

int32_t gptra_is(const char *line);
int32_t gptra_parser(gptra_t *gptra,const char *line);

#endif // __FILE_GPTRA_H__
