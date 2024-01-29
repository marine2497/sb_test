// headinga.c
// 2014-9-19 15:43:43
// yys@system-intergration-department

#include "parser.h"
#include "headinga.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t headinga_is(const char *line)
{
    return (strncmp(line,"#HEADING3A,",11) == 0) || (strncmp(line,"#HEADINGA,",10) == 0);
}

// #HEADINGA,COM1,0,77.0,FINESTEERING,1481,418557.000,00000000,3663,36137;
// SOL_COMPUTED,L1_INT,5.913998127,75.566444397,-0.152066842,0.0,0.104981117,
// 0.222061798,"AAAA",13,10,10,0,0,00,0,11*481a5bab
int32_t headinga_parser(headinga_t *headinga,const char *line)
{
    memset(headinga,0,sizeof(*headinga));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 28 || (strcmp(section[1-1],"#HEADING3A") != 0 && strcmp(section[1-1],"#HEADINGA") != 0))
        return -1;
    //
    // 计算结构体成员的值
    headinga->gps_week          = atoi(section[6-1]);
    headinga->gps_ms            = (uint32_t)(atof(section[7-1]) * 1000 + 0.5);
    headinga->base_line_length  = atof(section[13-1]);
    headinga->heading           = atof(section[14-1]);
    headinga->pitch             = atof(section[15-1]);
    headinga->sol_n             = atoi(section[21-1]);
    //
    headinga->sol_type = 0;
    if(strcmp(section[11-1],"SOL_COMPUTED") == 0)
    {
        static const struct
        {
            int32_t type_i;
            const char * type_str;
        } s_tab[] =
        {
            { 1, "SINGLE"       },
            { 5, "L1_FLOAT"     },
            { 5, "NARROW_FLOAT" },
            { 4, "L1_INT"       },
            { 4, "NARROW_INT"   },
        };
        for(uint32_t i=0;i<sizeof(s_tab)/sizeof(s_tab[0]);i++)
        {
            if(strcmp(section[12-1],s_tab[i].type_str) == 0)
            {
                headinga->sol_type = s_tab[i].type_i;
                break;
            }
        }
    }
    //
    return 0;
}

// #define __TEST_HEADINGS_C__
#ifdef __TEST_HEADINGS_C__

#include <stdio.h>
#include "parser.c"

const char * g_lines[] = 
{
    "#HEADINGA,COM1,0,77.0,FINESTEERING,1481,418557.000,00000000,3663,36137;"
        "SOL_COMPUTED,L1_INT,5.913998127,75.566444397,-0.152066842,0.0,0.104981117,"
        "0.222061798,\"AAAA\",13,10,10,0,0,00,0,11*481a5bab",
    "#HEADING3A,COM1,0,18.5,FINESTEERING,1810,461639.400,00000000,d3de,12916;"
        "SOL_COMPUTED,NARROW_FLOAT,-1.000000000,95.242324829,-37.760620117,0.000,1.367507935,"
        "1.408322811,\"U43H\",17,14,14,13,04,00,30,33*b98dfbcd\r\n",
    "#HEADING3A,COM1,0,30.0,FINESTEERING,1810,461694.600,00000000,d3de,12916;"
        "SOL_COMPUTED,NARROW_FLOAT,-1.000000000,96.810913086,-36.319824219,0.000,1.207882643,"
        "1.386836886,\"U43H\",15,8,8,8,04,00,00,33*6fb8ab0a\r\n",
    "#HEADING3A,COM1,0,78.5,UNKNOWN,0,26.800,004c0000,d3de,12916;"
        "INSUFFICIENT_OBS,NONE,0.000000000,0.000000000,0.000000000,0.000,0.000000000,"
        "0.000000000,\"\",0,0,0,0,00,00,00,00*cc1773d2\r\n",
    "#HEADING3A,COM1,0,37.0,FINESTEERING,1810,462721.600,00040000,d3de,12916;"
        "SOL_COMPUTED,NARROW_INT,-1.000000000,181.997253418,66.470512390,0.000,53.901287079,"
        "28.283653259,\"U43H\",16,13,13,9,04,00,10,23*a0689d87\r\n",
};

headinga_t g_headinga;

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && headinga_is(g_lines[i]))
        {
            int32_t rs = headinga_parser(&g_headinga,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf(" base_line = %d\n",g_headinga.gps_week);
            printf(" base_line = %d\n",g_headinga.gps_ms);
            printf(" base_line = %f\n",g_headinga.base_line_length);
            printf(" heading   = %f\n",g_headinga.heading);
            printf(" pitch     = %f\n",g_headinga.pitch);
            printf(" sol_n     = %d\n",g_headinga.sol_n);
            printf(" sol_type  = %d\n",g_headinga.sol_type);
            printf("}\n");
        }
        else
        {
            printf("[%d] is not a HEADINGA line.\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}

#endif // __TEST_HEADINGS_C__
