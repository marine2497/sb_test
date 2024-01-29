// gphpr.c
// 2013-12-3 14:15:00
// yys@system-intergration-department

#include "gphpr.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gphpr_is(const char *line)
{
    return strncmp(line,"$GPHPR,",7) == 0;
}

// $GPHPR,070901.00,090.10,000.20,000.00,4,14,1.00,0004*42
int32_t gphpr_parser(gphpr_t *gphpr,const char *line)
{
    memset(gphpr,0,sizeof(*gphpr));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 10 || strcmp(section[1-1],"$GPHPR") != 0)
        return -1;
    //
    // 计算结构体成员的值
    gphpr->utc_ms       = parser_str2utc(section[2-1]);
    gphpr->heading      = atof(section[3-1]);
    gphpr->pitch        = atof(section[4-1]);
    //
    return 0;
}

// #define __TEST_GPHPR_C__
#ifdef __TEST_GPHPR_C__

#include <stdio.h>
#include "parser.c"

gphpr_t g_gphpr;

const char * g_lines[] = 
{
    "$GPHPR,070901.00,090.10,000.20,000.00,4,14,1.00,0004*42",
    "$GPHPR,070901.00,090.10,000.20,000.00,4,14,1.00,0004*43",
    "$GPHPR,070901.11,090.10,000.20,000.00,4,14,1.00,0004*42",
};

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && gphpr_is(g_lines[i]))
        {
            int32_t rs = gphpr_parser(&g_gphpr,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf("  utc_ms  = %d\n",g_gphpr.utc_ms  );
            printf("  heading = %f\n",g_gphpr.heading );
            printf("  pitch   = %f\n",g_gphpr.pitch   );
            printf("}\n");
        }
        else
        {
            printf("[%d] is not a GPHPR line.\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}
             
#endif // __TEST_GPHPR_C__

