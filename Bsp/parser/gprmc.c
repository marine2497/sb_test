// gprmc.c
// 2014-9-18 09:01:50
// yys@system-intergration-department

// 解析GPRMC

#include "gprmc.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gprmc_is(const char *line)
{
    return strncmp(line,"$GPRMC,",7) == 0 || strncmp(line,"$GNRMC,",7) == 0;
}

//$GPRMC,065141.00,A,3110.4723882,N,12123.2636328,E,0.657,140.7,050512,-0.0,W,A*00
//$GNRMC,065029.00,A,3110.4722495,N,12123.2644026,E,0.456,330.1,050512,-0.0,W,A*12
//$GPRMC,010448.00,A,2307.5867125,N,11321.9300875,E,000.020,026.6,180914,-0.0,W,A*0A
//日期格式：dd/mm/yy 180914=2014年9月18号
//时间格式: hhmmss.ss 065141.00=6点51分41秒
int32_t gprmc_parser(gprmc_t *gprmc,const char *line)
{
    memset(gprmc,0,sizeof(*gprmc));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 14 || (strcmp(section[1-1],"$GPRMC") != 0 && strcmp(section[1-1],"$GNRMC") != 0))
        return -1;
    //
    const char * date = section[10-1];
    if(strlen(date) != 6)
        date = "000000";
    //
    gprmc->utc_ms       = parser_str2utc(section[2-1]);
    gprmc->valid        = section[3-1][0] == 'A' ? 1 : 0;
		
		if(strlen(section[4-1]) < 6)
        gprmc->latitude = 0;
    else
        gprmc->latitude     = (section[4-1][0]-'0')*10  +(section[4-1][1] - '0') + (atof(section[4-1]+2) * (1.0/60.0));
    if(strlen(section[6-1]) < 7)
        gprmc->longitude = 0;
    else
        gprmc->longitude    = (section[6-1][0]-'0')*100 +(section[6-1][1] - '0')*10 + (section[6-1][2] - '0') + (atof(section[6-1]+3) * (1.0/60.0));
    
		
		   // 判断东西南北半球
    if(section[5-1][0] == 'S')
        gprmc->latitude = - gprmc->latitude;
    if(section[7-1][0] == 'W')
        gprmc->longitude = - gprmc->longitude;
    //
    
		
    gprmc->speed        = atof(section[8-1]) * (1.852f/3.6f);
    gprmc->heading      = atof(section[9-1]);
    gprmc->day          = (date[0]-'0') * 10 + (date[1]-'0');
    gprmc->month        = (date[2]-'0') * 10 + (date[3]-'0');
    gprmc->year         = (date[4]-'0') * 10 + (date[5]-'0') + 2000;
    //
    return 0;
}

// #define __TEST_GPRMC_C__
#ifdef __TEST_GPRMC_C__

#include <stdio.h>

gprmc_t g_gprmc;
const char * g_lines[] = 
{
    "$GPRMC,065141.00,A,3110.4723882,N,12123.2636328,E,0.657,140.7,050512,-0.0,W,A*00\r\n",
    "$GNRMC,065029.00,A,3110.4722495,N,12123.2644026,E,0.456,330.1,050512,-0.0,W,A*12\r\n",
    "$GNRMC,111241.22,A,2307.5898813,N,11321.9294569,E,0.578,257.5,251213,-0.0,W,A*17\r\n",
    "$GPRMC,010448.00,A,2307.5867125,N,11321.9300875,E,000.020,026.6,180914,-0.0,W,A*0A\r\n",
    "$GPRMC,235953.80,V,0000.0000000,N,00000.0000000,E,000.000,000.0,311205,-0.0,W,N*15\r\n",
    "$GPRMC,,V,,,,,,,,,,N*53\r\n",
};

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && gprmc_is(g_lines[i]))
        {
            int32_t rs = gprmc_parser(&g_gprmc,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf("valid   = %d\n",g_gprmc.valid);
            printf("utc_ms  = %d\n",g_gprmc.utc_ms);
            printf("speed   = %f\n",g_gprmc.speed);
            printf("heading = %f\n",g_gprmc.heading);
            printf("year    = %d\n",g_gprmc.year);
            printf("month   = %d\n",g_gprmc.month);
            printf("day     = %d\n",g_gprmc.day);
            printf("}\n");
        }
        else
        {
            printf("[%d] is not a RMC line.\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}

#endif // __TEST_GPRMC_C__
