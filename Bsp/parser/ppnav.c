// ppnav.c
// 2013-12-10 9:15:25

// 解析PPNAV

#include "ppnav.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t ppnav_is(const char *line)
{
		if(strncmp(line,"$PPNAV,",7) == 0)
		{
			printf("%s",line);
			return 1;
		}
    else
		 return 0;
		//return strncmp(line,"$PPNAV,",7) == 0 ;
}

int32_t ppnav_parser(ppnav_t *ppnav,const char *line)
{

    //$GPGGA,091827.00,2307.58813,N,11321.93129,E,2,08,1.0,32.6,M,-5.2,M,7.0,0129*6C
	//$PPNAV,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>,<14>
    uint8_t comm_cnt = 0, dat, j = 0;
    char heading[8]="0000.0", pitch[8]="0000.0", roll[8]="00.00",flag[8] = "1";

    memset((uint8_t *)ppnav,0,sizeof(ppnav_t));

    int32_t count_down = 256; // 最长256个字节，防止传入非字符串导致死循环。
    for(const char * ch = line;*ch != '\0' && count_down-- > 0;ch ++)
    {
        dat = *ch;

        if (dat == '\n')
            break;

        if (dat == ',')
        {
            comm_cnt++;
            j = 0;
            continue;
        }

        switch (comm_cnt)
        {
        case 8://roll
        	roll[j++] = dat;
        	roll[j] = '\0';
            break;

        case 9://pitch
            pitch[j++] = dat;
            pitch[j] = '\0';
            break;

        case 10://heading
        	heading[j++] = dat;
        	heading[j] = '\0';
            break;

        case 14://flag
        	flag[j++] = dat;
        	flag[j] = '\0';
            break;
        default:
            break;
        }
    }/* end for */
    ppnav->heading = atof(heading);
    ppnav->pitch = atof(pitch);
    ppnav->roll = atof(roll);
    ppnav->init_flag = atoi(flag);

    return 0;
}

// #define __TEST_PPPNAV_C__
#ifdef __TEST_PPPNAV_C__

#include <stdio.h>

ppnav_t g_ppnav;
const char *g_line = "$PPNAV,,,,,,,,-0.89,-3.01,-66.61,,,,1*6F\r\n";

int main(void)
{
    if(ppnav_is(g_line))
    {
        ppnav_parser(&g_ppnav,g_line);
        printf("heading=%f\n",g_ppnav.heading);
        printf("pitch  =%f\n",g_ppnav.pitch);
        printf("roll   =%f\n",g_ppnav.roll);
        printf("flag   =%d\n",g_ppnav.init_flag);
    }
    //
    return 0;
}

#endif // __TEST_PPPNAV_C__
