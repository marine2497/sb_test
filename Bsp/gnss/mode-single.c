// gnss/mode-single.c
// 2013-11-29 15:46:30
// yys@system-intergration-department

// 单主板单天线。

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

void mode_single_init(gnss_t * gnss)
{
    g_gnss = gnss;
}


int32_t mode_single_line_master(const char *line)
{
    if(gpgga_is(line))
    {
        gpgga_parser(&(g_gnss->master_gpgga),line);
        //
        if( !(register_is_invalid_or_expried() || boot_des_is_invalid())
            && output_is_bit_set(output_current_type(),OUTPUT_BIT_MASTER_GPGGA))
            printf(line);
        //
        output_triger(g_gnss);
        //
        return 0;
    }
    if(bestxyza_is(line))
    {
        bestxyza_parser(&(g_gnss->master_bestxyza),line);
        //
        return 0;
    }
    if(gprmc_is(line))
    {
        gprmc_parser(&(g_gnss->master_gprmc),line);
        //
        return 0;
    }
    if(refstation_is(line))
    {
        refstation_parser(&(g_gnss->master_refstation),line);
        output_triger_refstation(g_gnss);
        //
        return 0;
    }
    //
    return 1;
}

// 行被处理就返回0，没处理过就返回1。
int32_t mode_single_line_rover(const char *line)
{
    return 1;
}
