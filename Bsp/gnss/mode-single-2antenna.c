// gnss/mode-single.c
// 2014-9-11 10:27:01
// yys@system-intergration-department

// 单主板双天线模式。
// 和单主板单天线模式公用一个CPLD状态。

#include <string.h>
#include <stdio.h>
#include "time.h"
#include "mode-single-2antenna.h"
#include "output/output.h"
#include "security/register.h"
#include "security/boot-des.h"

static gnss_t * g_gnss = 0;

int32_t mode_single_2antenna_trigger_output(int32_t index);

void mode_single_2antenna_init(gnss_t * gnss)
{
    g_gnss = gnss;
}

// 行被处理就返回0，没处理过就返回1。
int32_t mode_single_2antenna_line_master(const char *line)
{
    if(gpgga_is(line) && gpgga_parser(&(g_gnss->master_gpgga),line)==0)
    {
        g_gnss->master_gpgga_update_time = time_nowMs();
        //
        if( !(register_is_invalid_or_expried() || boot_des_is_invalid())
            && output_is_bit_set(output_current_type(),OUTPUT_BIT_MASTER_GPGGA))
            printf(line);
        //
        mode_single_2antenna_trigger_output(0);
        //
        return 0;
    }
    if(bestxyza_is(line) && bestxyza_parser(&(g_gnss->master_bestxyza),line)==0)
    {
        g_gnss->master_bestxyza_update_time = time_nowMs();
        //
        mode_single_2antenna_trigger_output(1);
        //
        return 0;
    }
    if(gprmc_is(line) && gprmc_parser(&(g_gnss->master_gprmc),line)==0)
    {
        g_gnss->master_gprmc_update_time = time_nowMs();
        //
        mode_single_2antenna_trigger_output(2);
        //
        return 0;
    }
    if(refstation_is(line) && refstation_parser(&(g_gnss->master_refstation),line)==0)
    {
        g_gnss->master_refstation_update_time = time_nowMs();
        //
        output_triger_refstation(g_gnss);
        //
        return 0;
    }
    if(headinga_is(line) && headinga_parser(&(g_gnss->master_headinga),line)==0)
    {
        g_gnss->master_headinga_update_time = time_nowMs();
        //
        mode_single_2antenna_trigger_output(3);
        //
        return 0;
    }
    if(gptra_is(line) && gptra_parser(&(g_gnss->master_gptra),line)==0)
    {
        g_gnss->master_gptra_update_time = time_nowMs();
        //
        mode_single_2antenna_trigger_output(4);
        //
        return 0;
    }
    //
    return 1;
}

// 行被处理就返回0，没处理过就返回1。
int32_t mode_single_2antenna_line_rover(const char *line)
{
    return 1;
}

int32_t mode_single_2antenna_trigger_output(int32_t index)
{
#if 1
    if(index == 0)
    {
        gnss_check_message_age(g_gnss); // 把失效的报文清零。
        output_triger(g_gnss);
    }
#else
    static const uint32_t TRIGGER_INTERVAL_MAX_AGE = 1000; 
    static uint32_t s_last_trigger_time = 0; 

    if(time_nowMs() >= s_last_trigger_time+TRIGGER_INTERVAL_MAX_AGE)
    {
        output_triger(g_gnss);
        s_last_trigger_time = time_nowMs();
        //
        return 0;
    }

    uint32_t ms = (g_gnss->master_headinga_update_time > g_gnss->master_gptra_update_time) ? 
        g_gnss->master_headinga.gps_ms % 1000:
        g_gnss->master_gptra.utc_ms % 1000;
    if(  ms != g_gnss->master_gpgga.utc_ms    % 1000 ||
         ms != g_gnss->master_gprmc.utc_ms    % 1000 ||
         ms != g_gnss->master_bestxyza.gps_ms % 1000)
        return 2;

    static uint32_t ms_last = 0;
    if(ms == 0 && ms_last == 0)
    {
        if(index == 0)
            s_last_trigger_time = 0;
        return 3;
    }
    ms_last = ms;
    //
    gnss_check_message_age(g_gnss); 
    output_triger(g_gnss);
    s_last_trigger_time = time_nowMs();
    //
    //printf("t %d\n",index);
    //
#endif
    return 0;
}
