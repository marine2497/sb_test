// gphpd.h
// 2013-11-29 16:37:11
// yys@system-intergration-department

// 解析GPHPD

#ifndef __FILE_GPHPD_H__
#define __FILE_GPHPD_H__

#include <stdint.h>

typedef struct
{
    uint32_t valid;
    uint32_t gpsweek;
    double gpstime;
    float heading;//pianhang
    float pitch;//fuyang
    float roll;//henggun
    double latitude;
    double longitude;
    double altitude;
    double x;
    double y;
    double z;
    float ve;
    float vn;
    float vu;
    float ae;
    float an;
    float au;
    float baseline;
    uint8_t nsv1;//前天线可用星数。
    uint8_t nsv2;//后天线可用星数。
} gphpd_t;

int32_t gphpd_is(const char *line);
int32_t gphpd_parser(gphpd_t *gphpd,const char *line);
int32_t gphpd_print(char *line,const gphpd_t *gphpd);

#endif // __FILE_GPHPD_H__
