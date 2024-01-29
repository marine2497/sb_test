
#include <stdint.h>
#include "time.h"
#include "mode-test.h"
#include "ppnfo.h"
//#include "parser/gpgga.h"
//#include "parser/bestxyza.h"
#include "output.h"
#include <string.h>
#include <stdio.h>


static gnss_t * g_gnss = 0;

static int32_t mode_test_trigger_output(int32_t index);

void mode_test_init(gnss_t * gnss)
{
    g_gnss = gnss;
}

// 行被处理就返�?，没处理过就返回1�?
int32_t mode_test_line_master(const char *line)
{
    if(ppnfo_is(line) && ppnfo_parser(&(g_gnss->ins_ppnfo),line) == 0)
    {
        g_gnss->ins_ppnfo_update_time = time_nowMs();
        printf(line);
        
        return 0;
    }     
    return 1;
}

// 行被处理就返�?，没处理过就返回1�?
int32_t mode_test_line_rover(const char *line)
{
    if(gpgga_is(line) && gpgga_parser(&(g_gnss->rover_gpgga),line) == 0)
    {
        g_gnss->rover_gpgga_update_time = time_nowMs();
        //
        mode_test_trigger_output(3);
        //
        return 0;
    }
    //
    return 1;
}

int32_t mode_test_trigger_output(int32_t index)
{
    static const uint32_t TRIGGER_INTERVAL_MAX_AGE = 1000; 
    static uint32_t s_last_trigger_time = 0; // 上次触发输出的时间�?
    if(time_nowMs() >= s_last_trigger_time+TRIGGER_INTERVAL_MAX_AGE)
    {
        output_triger(g_gnss);
        s_last_trigger_time = time_nowMs();
        //
        return 0;
    }

    uint32_t ms = g_gnss->rover_gpgga.utc_ms  % 1000;
    if(  ms != g_gnss->master_gpgga.utc_ms    % 1000 ||
         ms != g_gnss->master_gprmc.utc_ms    % 1000 ||
         ms != g_gnss->master_bestxyza.gps_ms % 1000 )
        return 2;
    //

    static uint32_t ms_last = 0;
    if(ms == 0 && ms_last == 0)
    {
        if(index == 0)
            s_last_trigger_time = 0;
        return 3;
    }
    ms_last = ms;
    //
    gnss_check_message_age(g_gnss); // 把失效的报文清零�?    
    output_triger(g_gnss);
    s_last_trigger_time = time_nowMs();
    //
    //printf("t %d\n",index);
    //
    return 0;
}
