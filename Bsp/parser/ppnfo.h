// parser/gpgga.h
// 2013-11-29 14:45:47
// yys@system-intergration-department

// 解析GPGA的库�?
#ifndef __FILE_PPNFO_H__
#define __FILE_PPNFO_H__

#include <stdint.h>

typedef struct
{
    double utc_s;//�秒
    double latitude;
    double longitude;
    double altitude;
    double roll;
    double pitch;
    double heading;
    double rod_l;
    uint8_t    magnetic_mass;
    uint8_t    rol_pitch_accuracy;
    uint8_t    head_accuracy;
    uint8_t    position_accuracy;
    double roll_gyro;
    double pitch_gyro;
    double head_gyro;
    uint8_t    cal_sch;//calibration_schedule
    int    flag;
} ppnfo_t;

int32_t ppnfo_is(const char *line);
int32_t ppnfo_parser(ppnfo_t *ppnfo,const char *line);

#endif // __FILE_PPNFO_H__
