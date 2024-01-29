// gprmc.c
// 2013-12-10 9:15:25
// yys@system-intergration-department

// 解析GPRMC

#include "gpgsv.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gpgsv_is(const char *line)
{
    return strncmp(line,"$GPGSV,",7) == 0 ;
}

int32_t gpgsv_parser(gpgsv_t *gpgsv,const char *line)
{

    return 0;
}

//#define __TEST_GPRMC_C__
#ifdef __TEST_GPRMC_C__

#include <stdio.h>

gprmc_t g_gprmc;
//const char *g_line = "$GPRMC,065141.00,A,3110.4723882,N,12123.2636328,E,0.657,140.7,050512,-0.0,W,A*00\r\n";
//const char *g_line = "$GNRMC,065029.00,A,3110.4722495,N,12123.2644026,E,0.456,330.1,050512,-0.0,W,A*12\r\n";
const char *g_line = "$GNRMC,111241.00,A,2307.5898813,N,11321.9294569,E,0.578,257.5,251213,-0.0,W,A*17";

int main(void)
{
    if(gprmc_is(g_line))
    {
        gprmc_parser(&g_gprmc,g_line);
        printf("speed=%f\n",g_gprmc.speed);
        printf("heading=%f\n",g_gprmc.heading);
        printf("valid=%d\n",g_gprmc.valid);
        printf("year=%d\n",g_gprmc.year);
        printf("month=%d\n",g_gprmc.month);
        printf("day=%d\n",g_gprmc.day);
        printf("hour=%d\n",g_gprmc.hour);
        printf("minute=%d\n",g_gprmc.minute);
        printf("second=%d\n",g_gprmc.second);
    }
    //
    return 0;
}

#endif // __TEST_GPRMC_C__
