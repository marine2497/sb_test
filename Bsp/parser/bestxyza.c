// bestxyza.c
// 2013-11-29 15:11:18
// yys@system-intergration-department

// 解析BESTXYZA

#include "bestxyza.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t bestxyza_is(const char *line)
{
    return strncmp(line,"#BESTXYZA,",10) == 0;
}

int32_t bestxyza_parser(bestxyza_t *bestxyza,const char *line)
{
    memset(bestxyza,0,sizeof(*bestxyza));
    //+
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 39 || strcmp(section[1-1],"#BESTXYZA") != 0)
        return -1;
    //
    // 计算结构体成员的值
    bestxyza->gps_week = atoi(section[6-1]);
    bestxyza->gps_ms   = (uint32_t)(atof(section[7-1]) * 1000 + 0.5);
    bestxyza->ve    = atof(section[21-1]);
    bestxyza->vn    = atof(section[22-1]);
    bestxyza->vu    = atof(section[23-1]);
    bestxyza->ae    = atof(section[24-1]);
    bestxyza->an    = atof(section[25-1]);
    bestxyza->au    = atof(section[26-1]);
    //
    return 0;
}

// #define __TEST_BESTXYZA_C__
#ifdef __TEST_BESTXYZA_C__

#include <stdio.h>
#include "parser.c"

bestxyza_t g_bestxyza;

const char * g_lines[] = 
{
    /*
    "#BESTXYZA,COM1,0,27.0,FINESTEERING,1811,182587.200,00000000,bde7,12916;"
        "SOL_COMPUTED,SINGLE,-2327465.3323,5387370.3859,2489596.9273,2.1031,3.4714,2.1015,"
        "SOL_COMPUTED,DOPPLER_VELOCITY,-0.0159,-0.0283,-0.0138,0.2514,0.4149,0.2512,"
        "\"\",0.150,0.000,0.000,21,21,21,0,0,02,10,11*3b0e9240\r\n",
    "#BESTXYZA,COM1,0,27.0,FINESTEERING,1811,182587.200,00000000,bde7,12916;"
        "SOL_COMPUTED,SINGLE,-2327465.3323,5387370.3859,2489596.9273,2.1031,3.4714,2.1015,"
        "SOL_COMPUTED,DOPPLER_VELOCITY,-0.0159,-0.0283,-0.0138,0.2514,0.4149,0.2512,"
        "\"\",0.150,0.000,0.000,21,21,21,0,0,02,10,11*3b0e9241\r\n", // error
    "#BESTXYZA,COM1,0,24.0,FINESTEERING,1811,182641.800,00000000,bde7,12916;"
        "SOL_COMPUTED,SINGLE,-2327449.0398,5387352.9791,2489602.7408,2.1263,3.4942,2.1016,"
        "SOL_COMPUTED,DOPPLER_VELOCITY,0.0190,0.0187,-0.0141,0.2541,0.4176,0.2512,"
        "\"\",0.150,0.000,0.000,21,21,21,0,0,02,10,11*bd9eba71\r\n",
    "#BESTXYZA,COM1,0,24.0,FINESTEERING,1811,182642.000,00000000,bde7,12916;"
        "SOL_COMPUTED,SINGLE,-2327449.0607,5387353.0099,2489602.7578,2.1261,3.4942,2.1016,"
        "SOL_COMPUTED,DOPPLER_VELOCITY,-0.0004,-0.0038,-0.0053,0.2541,0.4176,0.2512,"
        "\"\",0.150,0.000,0.000,21,21,21,0,0,02,10,11*99df07d6\r\n",
    "#BESTXYZA,COM1,0,76.0,UNKNOWN,0,6.800,004c0000,bde7,12916;"
        "INSUFFICIENT_OBS,NONE,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,"
        "INSUFFICIENT_OBS,NONE,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,"
        "\"\",0.000,0.000,0.000,0,0,0,0,0,00,00,00*0d5f640d\r\n",
    "#BESTXYZA,COM2,0,60.0,UNKNOWN,1356,000008.600,00000000,0000,1114;"
        "COLD_START,NONE,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,"
        "COLD_START,NONE,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,"
        "\"0000\",0.000,0.000,29.000,0,0,0,0,0,0,0,0*8bfd6989\r\n",
    "\r\n",
    */
    //
    // K501
    "#BESTXYZA,COM1,0,60.0,FINESTEERING,1811,271214.000,00000000,0000,1114;SOL_COMPUTED,NARROW_FLOAT,-2327475.9677,5387383.0917,2489618.7825,0.1423,0.2893,0.1051,SOL_COMPUTED,DOPPLER_VELOCITY,-0.0059,0.0228,0.0081,1.0718,1.6250,0.7624,\"0000\",0.000,4.000,2345.000,16,11,16,16,0,0,0,75*52bd1d50\r\n",
    "#BESTXYZA,COM1,0,60.0,FINESTEERING,1811,271213.700,00000000,0000,1114;SOL_COMPUTED,NARROW_FLOAT,-2327475.9743,5387383.0988,2489618.7858,0.1425,0.2894,0.1051,SOL_COMPUTED,DOPPLER_VELOCITY,-0.0109,0.0424,0.0140,0.9150,1.4948,0.6506,\"0000\",0.000,3.000,2342.000,17,12,17,17,0,0,0,75*0c1d4a70\r\n",
    //
    // K528
    "#BESTXYZA,COM1,0,35.5,FINESTEERING,1811,271477.200,00000000,bde7,12916;SOL_COMPUTED,SINGLE,-2327474.1670,5387389.0412,2489621.2546,2.7502,4.7745,2.1982,SOL_COMPUTED,DOPPLER_VELOCITY,0.0026,0.0137,-0.0047,0.3287,0.5707,0.2627,\"\",0.150,0.000,0.000,18,15,15,15,0,06,30,33*3dc89d1e\r\n",
    "#BESTXYZA,COM1,0,35.5,FINESTEERING,1811,271477.400,00000000,bde7,12916;SOL_COMPUTED,SINGLE,-2327474.1538,5387389.0074,2489621.2492,2.7501,4.7744,2.1981,SOL_COMPUTED,DOPPLER_VELOCITY,0.0271,-0.0422,0.0069,0.3287,0.5706,0.2627,\"\",0.150,0.000,0.000,17,15,15,15,0,06,30,33*08755ad4\r\n"
};

int main(void)
{
    for(uint32_t i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(parser_check_line(g_lines[i]) == 0 && bestxyza_is(g_lines[i]))
        {
            int32_t rs = bestxyza_parser(&g_bestxyza,g_lines[i]);
            if(rs != 0)
            {
                printf("parser [%d] error, %d\n",i,rs);
                continue;
            }
            printf("[%d] \"%s\" : {\n",i,g_lines[i]);
            printf("  gps_week = %d\n",g_bestxyza.gps_week );
            printf("  gps_ms   = %d\n",g_bestxyza.gps_ms   );
            printf("  ve       = %f\n",g_bestxyza.ve );
            printf("  vn       = %f\n",g_bestxyza.vn );
            printf("  vu       = %f\n",g_bestxyza.vu );
            printf("  ae       = %f\n",g_bestxyza.ae );
            printf("  an       = %f\n",g_bestxyza.an );
            printf("  au       = %f\n",g_bestxyza.au );
            printf("}\n");
        }
        else
        {
            printf("[%d] is not a BASTXYZA line.\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}
             
#endif // __TEST_BESTXYZA_C__

