// parser/refstation.c
// 2014-7-2 15:14:35
// yys@system-intergration-department

#include "refstation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int32_t refstation_is(const char *line)
{
    return strncmp(line,"#REFSTATIONA,",13) == 0;
}

// #REFSTATIONA,COM1,0,60.0,FINESTEERING,1799,285410.000,00000000,0000,1114;00000000,0.000,0.000,0.000,0,4,"0006"*a2a8f90b
int32_t refstation_parser(refstation_t *refstation,const char *line)
{
    uint8_t comm_cnt = 0, j = 0;
    char status[20]={0}, x[20]={0}, y[20]={0}, z[20]={0}, health[20]={0};

    int32_t count_down = 256; // 最长256个字节，防止传入非字符串导致死循环。
    for(const char * ch = line;*ch != '\0' && count_down-- > 0;ch ++)
    {
        char dat = *ch;

        if (dat == ',' || dat == ';')
        {
            comm_cnt++;
            j = 0;
            continue;
        }

        if (dat == '\n')
            break;

        switch (comm_cnt)
        {
        case 10:
            status[j++] = dat;
            status[j] = '\0';
            break;
        case 11:
            x[j++] = dat;
            x[j] = '\0';
            break;
        case 12:
            y[j++] = dat;
            y[j] = '\0';
            break;
        case 13:
            z[j++] = dat;
            z[j] = '\0';
            break;
        case 14:
            health[j++] = dat;
            health[j] = '\0';
            break;
        default:
            break;
        }
    }
    char *err = 0;
    refstation->status = strtoul(status,&err,16);
    if(*status == '\0' || (err != 0 && *err != '\0'))
        refstation->status = 0x01; // 如果status字段为空，或者不是一个十六进制数，则置为1，即invalid。
    refstation->x = strtod(x,0);
    refstation->y = strtod(y,0);
    refstation->z = strtod(z,0);
    refstation->health = atoi(health);
    refstation->valid = (refstation->status & 0x01) ? 0 : 1;
    if(refstation->x == 0 && refstation->y == 0 && refstation->z == 0)
        refstation->valid = 0;

    return 0;
}

//#define __TEST_REFSTATION_C__
#ifdef __TEST_REFSTATION_C__

#include <stdio.h>

refstation_t g_refstation;

const char *g_lines[] = 
{
    "#REFSTATIONA,COM1,0,60.0,FINESTEERING,1799,285410.000,00000000,0000,1114;00000000,0.000,0.000,0.000,0,4,\"0006\"*a2a8f90b\r\n",
    "#REFSTATIONA,COM1,0,60.0,FINESTEERING,1799,285410.000,00000000,0000,1114;03e00F00,1.003,2.340,3.200,1,4,\"0006\"*a2a8f90b\r\n",
    "#REFSTATIONA,COM1,0,60.0,FINESTEERING,1799,285410.000,00000000,0000,1114;,1.003,2.340,3.200,1,4,\"0006\"*a2a8f90b\r\n",
    "#REFSTATIONA,COM1,0,60.0,FINESTEERING,1799,285410.000,00000000,0000,1114;kyu,1.003,2.340,3.200,1,4,\"0006\"*a2a8f90b\r\n",
    "#REFSTATIONA,COM1,0,66.5,FINESTEERING,1364,490401.124,80000000,4e46,2310;00000000,-1634532.443,-3664608.907,4942482.713,0,RTCA,\"AAAA\"*1e2a0508"
};

int main(void)
{
    for(int i=0;i<sizeof(g_lines)/sizeof(g_lines[0]);i++)
    {
        if(refstation_is(g_lines[i]))
        {
            refstation_parser(&g_refstation,g_lines[i]);
            printf("[%d].status=0x%08X\n",i,g_refstation.status);
            printf("[%d].x=%f\n",i,g_refstation.x);
            printf("[%d].y=%f\n",i,g_refstation.y);
            printf("[%d].z=%f\n",i,g_refstation.z);
            printf("[%d].health=%d\n",i,g_refstation.health);
        }
        else
        {
            printf("bad line. %d\n",i);
        }
        printf("\n");
    }
    //
    return 0;
}

#endif // __TEST_REFSTATION_C__
