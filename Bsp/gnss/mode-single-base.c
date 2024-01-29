// gnss/mode-single-base.c
// 2014-2-11 09:46:30
// YYS@system-intergration-department

// 单块主板作基准站的模式。

#include <string.h>
#include <stdio.h>
#include "mode-single.h"
#include "parser/gpgga.h"
#include "parser/bestxyza.h"
#include "parser/gprmc.h"
#include "output/output.h"
#include "security/register.h"
#include "security/boot-des.h"

static gnss_t * g_gnss = 0;

void mode_single_base_init(gnss_t * gnss)
{
    g_gnss = gnss;
}

// 行被处理就返回0，没处理过就返回1。
int32_t mode_single_base_line_master(const char *line)
{

    printf(line);//直接打印每一行以便于知道基站坐标设置是否正确
    if(gpgga_is(line))
    {
        gpgga_parser(&(g_gnss->master_gpgga),line);
//        if( !(register_is_invalid_or_expried() || boot_des_is_invalid())
//            && output_is_bit_set(output_current_type(),OUTPUT_BIT_MASTER_GPGGA))
//        printf(line);

        output_triger(g_gnss);

        return 0;
    }


    return 1;
}

// 行被处理就返回0，没处理过就返回1。
int32_t mode_single_base_line_rover(const char *line)
{
    return 1;
}
