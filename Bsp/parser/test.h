// parser/test.h
// 2014-9-19 17:23:19
// yys@system-intergration-department

#ifndef __FILE_TEST_H__
#define __FILE_TEST_H__

#include <stdint.h>

typedef struct
{
    double latitude;
    double longitude;
    double altitude;
    uint8_t sol_n;   //卫星数
    uint8_t sol_type;//解状态
    uint8_t diff_age;//差分延时时间
    float hdop;
}test_board_t;

typedef struct
{
    uint32_t sys_time; // 上电时间
    char model[32];
    char oem_type[8]; // 两个主板的前两位并在一起
    char working_mode[32];
    //
    uint16_t utc_year;
    uint8_t  utc_month; // [1,12]
    uint8_t  utc_day;   // [1,31]
    uint32_t utc_ms_of_day;
    //
    uint32_t gps_week;
    uint32_t gps_ms;
    //
    float speed;   // 速度，m/s
    float heading; // 行驶角
    float yaw;     // 航偏角
    float pitch;   // 俯仰角
    float roll;    // 横滚角
    //
    float base_line; // 基线长
    //
    test_board_t master;
    test_board_t rover;
} test_t;

int32_t test_print(char *line, int32_t capacity, const test_t * test);

#endif // __FILE_TEST_H__
