// parser/gphpr.h
// 2013-12-3 14:14:56
// yys@system-intergration-department

// 解析GPHPR的库。

#ifndef __FILE_GPHPR_H__
#define __FILE_GPHPR_H__

#include <stdint.h>

typedef struct
{
    uint32_t utc_ms;
    float heading;
    float pitch;
} gphpr_t;

int32_t gphpr_is(const char *line);
int32_t gphpr_parser(gphpr_t *gphpr,const char *line);

#endif // __FILE_GPHPR_H__
