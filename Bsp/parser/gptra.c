// gptra.c
// 2014-9-17 10:57:03
// yys@system-intergration-department

// 解析GPTRA

#include "gptra.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gptra_is(const char *line)
{
    return strncmp(line,"$GPTRA,",7) == 0;
}

int32_t gptra_parser(gptra_t *gptra,const char *line)
{
    memset(gptra,0,sizeof(*gptra));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 10 || strcmp(section[1-1],"$GPTRA") != 0)
        return -1;
    //
    // 计算结构体成员的值
    gptra->utc_ms   = parser_str2utc(section[2-1]);
    gptra->heading  = atof(section[3-1]);
    gptra->pitch    = atof(section[4-1]);
    gptra->roll     = atof(section[5-1]);
    gptra->valid    = atoi(section[6-1]);
    gptra->satNo    = atoi(section[7-1]);
    gptra->diff_age = atof(section[8-1]);
    //
    return 0;
}

//#define __TEST_GPTRA_C__
#ifdef __TEST_GPTRA_C__

#include <stdio.h>

gptra_t g_gptra;
const char *g_lines[] =
{
    "$GPTRA,025330.41,186.76,002.35,001.11,5,15,8.80,0000*46\r\n",
    "$GPTRA,025330.633,186.91,003.99,020.20,5,16,0.00,000*49\r\n",
    "$GPTRA,025330.8,187.01,005.59,003.03,5,16,0.55,000*45\r\n",
    "$GPTRA,025331.,187.16,007.30,000.00,5,16,0.00,00*47\r\n",
    "$GPTRA,025331.20,187.16,008.77,000.00,5,16,0.00,0000*49\r\n",
};

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && gptra_is(g_lines[i]))
        {
            int32_t rs = gptra_parser(&g_gptra,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,g_lines[i],rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf("  utc_ms   = %d\n",g_gptra.utc_ms  );
            printf("  heading  = %f\n",g_gptra.heading );
            printf("  pitch    = %f\n",g_gptra.pitch   );
            printf("  roll     = %f\n",g_gptra.roll    );
            printf("  valid    = %d\n",g_gptra.valid   );
            printf("  satNo    = %d\n",g_gptra.satNo   );
            printf("  diff_age = %f\n",g_gptra.diff_age);
            printf("}\n");
        }
        else
        {
            printf("[%d] is not a GPTRA line.\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}

#endif // __TEST_GPGGA_C__

