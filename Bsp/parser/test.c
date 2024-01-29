// test.c
// 2014-9-19 17:24:37
// yys@system-intergration-department

#include "test.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*

  0 : #TEST
  1 : format-version
  2 : model，机�?  3 : oem-type，前后主板的前两�?  4 : working-mode
  5 : system-time-ms
  6 : utc-date
  7 : utc-time
  8 : gps-week
  9 : gps-second
 10 : speed, m/s
 11 : heading, 行驶角，即运动的方向�? 12 : yaw, 车身角，两个天线的指�? 13 : pitch
 14 : roll
 15 : base-line
 16 : master-latitude
 17 : master-longitude
 18 : master-altitude
 19 : master-sol-n
 20 : master-sol-type
 21 : master-diff-age
 22 : master-hdop
 23 : rover-latitude
 24 : rover-longitude
 25 : rover-altitude
 26 : rover-sol-n
 27 : rover-sol-type
 28 : rover-diff-age
 29 : rover-hdop
 30 : crc32
*/
int32_t test_print(char * line, int32_t capacity, const test_t * test)
{
    int32_t h  = (test->utc_ms_of_day / (60*60*1000)) % 24;
    int32_t m  = (test->utc_ms_of_day / (   60*1000)) % 60;
    int32_t s  = (test->utc_ms_of_day / (      1000)) % 60;
    int32_t ms = (test->utc_ms_of_day               ) % 1000;
    //
    int32_t len = 0;
    len += snprintf(line+len,capacity-len,"#TEST,1"
        ",%s,%s,%s"
        ";%d.%03d"
        ",%04d%02d%02d"
        ",%02d%02d%02d.%03d"
        ",%d,%.3f"
        ";%.2f,%.2f,%.2f,%.2f,%.2f"
        ",%.3f"
        ,test->model,test->oem_type,test->working_mode
        ,test->sys_time/1000,test->sys_time%1000
        ,test->utc_year,test->utc_month,test->utc_day
        ,h,m,s,ms
        ,test->gps_week,(double)(test->gps_ms*1e-3)
        ,test->speed,test->heading,test->yaw,test->pitch,test->roll
        ,test->base_line
        );
    //
    for(int i=0;i<2;i++)
    {
        const test_board_t * b = (i==0) ? &(test->master) : &(test->rover);
        //
        len += snprintf(line+len,capacity-len,
            ";%.8f,%.8f,%.3f"
            ",%d,%d,%d,%.2f"
            ,b->latitude,b->longitude,b->altitude
            ,b->sol_n,b->sol_type,b->diff_age,b->hdop);
    }
    //
    uint32_t crc = parser_get_crc32(line);
    len += snprintf(line+len,capacity-len,"*%08x\r\n",crc);
    //
    return len;
}

// #define __TEST_TEST_C__
#ifdef __TEST_TEST_C__

#include <stdio.h>
#include <string.h>
#include "parser.c"

test_t g_test;

int main(void)
{
    memset(&g_test,0,sizeof(g_test));
    int i = 1;
    strcpy(g_test.model,"SM36+");
    strcpy(g_test.oem_type,"8300");
    strcpy(g_test.working_mode,"coupling");
    g_test.sys_time=i++;
    g_test.utc_year=i++;
    g_test.utc_month=i++;
    g_test.utc_day=i++;
    g_test.utc_ms_of_day=i++;
    g_test.gps_week=i++;
    g_test.gps_ms=i++;
    g_test.speed=i++;
    g_test.heading=i++;
    g_test.yaw=i++;
    g_test.pitch=i++;
    g_test.roll=i++;
    g_test.base_line=i++;
    //
    g_test.master.latitude=i++;
    g_test.master.longitude=i++;
    g_test.master.altitude=i++;
    g_test.master.sol_n=i++;
    g_test.master.sol_type=i++;
    g_test.master.diff_age=i++;
    g_test.master.hdop=i++;
    //
    g_test.rover.latitude=i++;
    g_test.rover.longitude=i++;
    g_test.rover.altitude=i++;
    g_test.rover.sol_n=i++;
    g_test.rover.sol_type=i++;
    g_test.rover.diff_age=i++;
    g_test.rover.hdop=i++;
    //
    test_print(g_parser_print_buffer,sizeof(g_parser_print_buffer),&g_test);
    //
    printf("%s",g_parser_print_buffer);
    return 0;
}
             
#endif // __TEST_TEST_C__

