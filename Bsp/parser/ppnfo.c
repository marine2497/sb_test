// ppnfo.c
// 2014-9-17 17:06:47
// yys@system-intergration-department

// 解析ppnfo

#include "ppnfo.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t ppnfo_is(const char *line)
{
    return strncmp(line,"$PPNFO,",7) == 0;
}

int32_t ppnfo_parser(ppnfo_t *ppnfo,const char *line)
{
    memset(ppnfo,0,sizeof(*ppnfo));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 19 || strcmp(section[1-1],"$PPNFO") != 0)
        return -1;
    //
    // 计算结构体成员的值
    ppnfo->utc_s              = atof(section[2-1]);
    ppnfo->latitude           = atof(section[3-1]);
    ppnfo->longitude          = atof(section[4-1]);
    ppnfo->altitude           = atof(section[5-1]);
    ppnfo->roll               = atof(section[6-1]);
    ppnfo->pitch              = atof(section[7-1]);
    ppnfo->heading            = atof(section[8-1]);
    ppnfo->rod_l              = atof(section[9-1]);
    ppnfo->magnetic_mass      = atoi(section[10-1]);
    ppnfo->rol_pitch_accuracy = atoi(section[11-1]);
    ppnfo->head_accuracy      = atoi(section[12-1]);
    ppnfo->position_accuracy  = atoi(section[13-1]);
    ppnfo->roll_gyro          = atof(section[14-1]);
    ppnfo->pitch_gyro         = atof(section[15-1]);
    ppnfo->head_gyro          = atof(section[16-1]);
    ppnfo->cal_sch            = atoi(section[17-1]);//calibration_schedule
    ppnfo->flag               = atoi(section[18-1]);
    
   
    return 0;
}

//#define __TEST_GPGGA_C__
#ifdef __TEST_GPGGA_C__

#include <stdio.h>
#include "parser.c"

ppnfo_t g_ppnfo;

const char * g_lines[] = 
{
"$PPNFO,8.900,0.000000000,0.000000000,0.000,30.93,-42.87,66.60,0.000,0,169,0,1,-1.74,25.06,-24.38,0,1*5f",
"$PPNFO,9.000,0.000000000,0.000000000,0.000,23.88,-86.94,73.39,0.000,0,233,0,1,-5.53,46.12,-31.17,0,1*59",
"$PPNFO,9.100,0.000000000,0.000000000,0.000,3.83,-87.74,77.27,0.000,0,289,0,1,-7.44,25.23,-35.05,0,1*60",
"$PPNFO,9.200,0.000000000,0.000000000,0.000,-38.62,-57.96,70.89,0.000,0,399,0,1,-10.72,-10.78,-28.67,0,1*65",
"$PPNFO,9.300,0.000000000,0.000000000,0.000,-34.43,-36.48,58.20,0.000,0,178,0,1,7.62,-21.77,-15.98,0,1*72",
"$PPNFO,9.400,0.000000000,0.000000000,0.000,-28.34,-33.74,56.06,0.000,0,122,0,1,12.65,-12.39,-13.84,0,1*45",
"$PPNFO,9.500,0.000000000,0.000000000,0.000,-16.57,-14.79,51.44,0.000,0,134,0,1,18.94,-19.15,-9.22,0,1*74",
"$PPNFO,9.600,0.000000000,0.000000000,0.000,-5.70,-11.27,67.70,0.000,0,124,0,1,22.39,-12.78,-25.48,0,1*71",
"$PPNFO,9.700,0.000000000,0.000000000,0.000,19.62,-13.36,65.53,0.000,0,156,0,1,34.18,-3.19,-23.31,0,1*5c",
"$PPNFO,9.800,0.000000000,0.000000000,0.000,25.33,-30.92,59.82,0.000,0,180,0,1,22.62,19.23,-17.60,0,1*49",
};

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && ppnfo_is(g_lines[i]))
        {
            int32_t rs = ppnfo_parser(&g_ppnfo,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf("  valid       = %d\n",g_ppnfo.valid       );
            printf("  utc_ms      = %d\n",g_ppnfo.utc_ms      );
            printf("  nosv        = %d\n",g_ppnfo.nosv        );
            printf("  latitude    = %f\n",g_ppnfo.latitude    );
            printf("  longitude   = %f\n",g_ppnfo.longitude   );
            printf("  altitude    = %f\n",g_ppnfo.altitude    );
            printf("  hdop        = %f\n",g_ppnfo.hdop        );
            printf("  diff_age    = %d\n",g_ppnfo.diff_age    );
            printf("}\n");
        }
        else
        {
            printf("[%d] is not a GPGGA line.\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}
             
#endif // __TEST_GPGGA_C__

