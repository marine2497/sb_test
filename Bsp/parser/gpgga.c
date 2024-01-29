// gpgga.c
// 2014-9-17 17:06:47
// yys@system-intergration-department

// 解析GPGGA

#include "gpgga.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gpgga_is(const char *line)
{
    return strncmp(line,"$GPGGA,",7) == 0;
}

int32_t gpgga_parser(gpgga_t *gpgga,const char *line)
{
    memset(gpgga,0,sizeof(*gpgga));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 16 || strcmp(section[1-1],"$GPGGA") != 0)
        return -1;
    //
    // 计算结构体成员的值
    gpgga->utc_ms       = parser_str2utc(section[2-1]);
    if(strlen(section[3-1]) < 6)
        gpgga->latitude = 0;
    else
        gpgga->latitude     = (section[3-1][0]-'0')*10  +(section[3-1][1] - '0') + (atof(section[3-1]+2) * (1.0/60.0));
    if(strlen(section[5-1]) < 7)
        gpgga->longitude = 0;
    else
        gpgga->longitude    = (section[5-1][0]-'0')*100 +(section[5-1][1] - '0')*10 + (section[5-1][2] - '0') + (atof(section[5-1]+3) * (1.0/60.0));
    gpgga->valid        = atoi(section[7-1]);
    gpgga->nosv         = atoi(section[8-1]);
    gpgga->hdop         = atof(section[9-1]);
    gpgga->altitude     = atof(section[10-1]);
    gpgga->diff_age     = atoi(section[14-1]);
    //
    // 判断东西南北半球
    if(section[4-1][0] == 'S')
        gpgga->latitude = - gpgga->latitude;
    if(section[6-1][0] == 'W')
        gpgga->longitude = - gpgga->longitude;
    //
    // 差分延迟大于一定值，就强制拉低解状态
    const int32_t DELAY_MAX = 5;
    if(gpgga->valid == 4 && gpgga->diff_age >= DELAY_MAX)
        gpgga->valid = 5;
    //
    return 0;
}

//#define __TEST_GPGGA_C__
#ifdef __TEST_GPGGA_C__

#include <stdio.h>
#include "parser.c"

gpgga_t g_gpgga;

const char * g_lines[] = 
{
    "$GPGGA,073409.60,2307.4361146,N,11322.1866961,E,4,15,3.8,-3.2339,M,0.000,M,00,0003*4E\r\n",
    "$GPGGA,073409.80,2307.4376482,N,11322.1867576,E,4,15,3.8,-3.2182,M,0.000,M,00,0003*42\r\n",
    "$GPGGA,073410.00,2307.4391799,N,11322.1868215,E,4,15,2.0,-3.2070,M,0.000,M,01,0003*4B\r\n",
    "$GPGGA,073410.20,2307.4407108,N,11322.1868870,E,4,15,2.0,-3.2016,M,0.000,M,01,0003*46\r\n",
    "$GPGGA,092419.10,2307.5870545,N,11321.9297949,E,5,15,2.3,35.5333,M,-6.651,M,03,0005*7D\r\n",
    "$GPGGA,,,,,,0,,,,,,,,*66\r\n",
};

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && gpgga_is(g_lines[i]))
        {
            int32_t rs = gpgga_parser(&g_gpgga,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf("  valid       = %d\n",g_gpgga.valid       );
            printf("  utc_ms      = %d\n",g_gpgga.utc_ms      );
            printf("  nosv        = %d\n",g_gpgga.nosv        );
            printf("  latitude    = %f\n",g_gpgga.latitude    );
            printf("  longitude   = %f\n",g_gpgga.longitude   );
            printf("  altitude    = %f\n",g_gpgga.altitude    );
            printf("  hdop        = %f\n",g_gpgga.hdop        );
            printf("  diff_age    = %d\n",g_gpgga.diff_age    );
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

