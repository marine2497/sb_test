// gphpd.h
// 2013-11-29 16:37:11
// ys@system-intergration-department

// 解析GPHPD

#ifndef __FILE_GPHDT_H__
#define __FILE_GPHDT_H__

#include <stdint.h>

typedef struct
{
    float heading;//航向�?
    char valid;
} gphdt_t;

int32_t gphdt_is(const char *line);
int32_t gphdt_parser(gphdt_t *gphdt,const char *line);
int32_t gphdt_print(char *line,const gphdt_t *gphdt);

#endif // __FILE_GPHPD_H__
