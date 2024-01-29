
#include "gnss.h"
#include "oem_o.h"
#include "globle.h"
#include <stdio.h>
#include <string.h>
#include "uart2.h"
#include "uart3.h"
#include "time.h"
#include "parser.h"
#include "getline.h"
#include "mode-divided.h"
#include "mode-coupling.h"
#include "mode-single.h"
#include "mode-single-ins.h"
#include "mode-single-base.h"
#include "mode-single-2antenna.h"
//#include "interface.h"
#include "param.h"

//static gnss_t g_gnss; // 用于存放解析结果。
gnss_t g_gnss; // 用于存放解析结果。
static gnss_output_line_t g_output_line_mode = GNSS_OUTPUT_LINE_NONE; // 是否转发主板来的数据，用于调试。

static getline_t g_getline_2; // 用于分别从主板来的数据中提取行。
static getline_t g_getline_3;
static char g_line_2[2048];
static char g_line_3[2048];

static const struct
{
    uint32_t mode;
    uint8_t cpld;
    uint8_t need_auto_save;
    const char * desc;
} g_mode_cpld_table[] = 
{
    { MODE_UNDEFINE             , UNDEFINE                    , 1 , "undefine"            },
    { MODE_DIRECT_MASTER        , DIRECT_OEM1                 , 0 , "direct-master"       },
    { MODE_DIRECT_ROVER         , DIRECT_OEM2                 , 0 , "direct-rover"        },
    { MODE_DIRECT_INS           , DIRECT_INS                  , 0 , "direct-ins"          },
    { MODE_WORK_DIVIDED         , SM36_CPU_CAL_HEADING_NO_INS , 1 , "work-divided"        },
    { MODE_WORK_COUPLING        , SM36_OEM_CAL_HEADING_NO_INS , 1 , "work-coupling"       },
    { MODE_WORK_COUPLING_INS    , SM36_OEM_CAL_HEADING_INS    , 1 , "work-coupling-ins"   },
    { MODE_WORK_SINGLE          , SM39_NO_INS                 , 1 , "work-single"         },
    { MODE_WORK_SINGLE_INS      , SM39_INS                    , 1 , "work-single-ins"     },
    { MODE_WORK_SINGLE_BASE     , SM39_BASE                   , 1 , "work-single-base"    },
    { MODE_WORK_SINGLE_2ANTENNA , SM39_NO_INS                 , 1 , "work-single-2antenna"},
    { MODE_WORK_SINGLE_2ANTENNA_INS , SM39_INS                , 1 , "work-single-2antenna-ins"},
};

// 内部函数声明。
static uint32_t gnss_get_oem_type(void);
static uint32_t gnss_get_working_mode(void);
static void gnss_on_getline_2(char *line,int32_t length,void *param);
static void gnss_on_getline_3(char *line,int32_t length,void *param);

// 初始化gnss模块。
// 会很忙等待，而且会很长时间。
void gnss_init(void)
{   
}

// 复位主板。
int32_t gnss_reset(void)
{   
    return 0;
}

const gnss_t *gnss_get(void)
{
    return &g_gnss;
}

// 检查事件，在main函数的主循环里不断调用。
void gnss_check_event(void)
{
    char buffer[32];
    //

    if(uart2_receiveValidBufferSize() > 0)
    {
        int32_t len = uart2_readReceiveBuffer(buffer,sizeof(buffer));
        getline_input(&g_getline_2,buffer,len);
    }
    if(uart3_receiveValidBufferSize() > 0)
    {
        int32_t len = uart3_readReceiveBuffer(buffer,sizeof(buffer));
        getline_input(&g_getline_3,buffer,len);
    }
}


uint32_t gnss_get_working_mode(void)
{
    uint32_t mode = param_get()->gnss.working_mode;
    //
    for(int32_t i=0;i<sizeof(g_mode_cpld_table)/sizeof(g_mode_cpld_table[0]);i++)
    {
        if(mode == g_mode_cpld_table[i].mode)
        {
            printf("gnss: The current mode is %s\r\n",g_mode_cpld_table[i].desc);
            return mode;
        }
    }
    //
    printf("gnss: The current mode is undefine.\r\n");
    return MODE_UNDEFINE;
}

const char * gnss_get_working_mode_string(void)
{
    uint32_t mode = param_get()->gnss.working_mode;
    //
    for(int32_t i=0;i<sizeof(g_mode_cpld_table)/sizeof(g_mode_cpld_table[0]);i++)
    {
        if(mode == g_mode_cpld_table[i].mode)
            return g_mode_cpld_table[i].desc;
    }
    //
    return g_mode_cpld_table[0].desc;
}

int32_t gnss_set_mode(uint32_t mode)
{
    //
    for(uint32_t i=0;i<sizeof(g_mode_cpld_table)/sizeof(g_mode_cpld_table[0]);i++)
    {
        if(g_mode_cpld_table[i].mode == mode)
        {
            param_get()->gnss.working_mode = mode;
            cpld_set(g_mode_cpld_table[i].cpld);
            if(g_mode_cpld_table[i].need_auto_save)
                param_auto_save();
            return 0;
        }
    }
    return -1;
}

void gnss_restore_mode(void)
{
    gnss_set_mode(param_on_flash()->gnss.working_mode);
}

uint32_t gnss_get_oem_type(void)
{
    struct
    {
        const char *  id;
        uint32_t       type;
        const char *  desc;
    } table[] =
    {
        { "11" , OEM_OEM615 , "N-OEM615"},
        { "12" , OEM_OEM615 , "N-OEM615"},
        { "18" , OEM_OEM615 , "N-OEM615"},//novatel三星，2013/12/27 yys加入
        { "14" , OEM_K501   , "B-K501"},
        { "10" , OEM_K501   , "B-K502"},
        { "15" , OEM_K501   , "B-K503"},
        { "16" , OEM_K505   , "B-K505"},
        { "21" , OEM_K505   , "B-K508"},
        { "09" , OEM_K501   , "T-BD920"},
        { "86" , OEM_BD910  , "T-BD910"},
        { "24" , OEM_OEM617 , "N-OEM617"},
        { "25" , OEM_K528   , "B-K528"},
        
    };
    
    //特殊情况的主板先在此处进行处理
    if(strncmp(G_BOOT_INFO->model,"SM37",4) == 0) return TRI_67650_10;
    //
    for(uint32_t i=0;i<sizeof(table)/sizeof(table[0]);i++)
    {
        if(strncmp(G_BOOT_INFO->sn+7,table[i].id,2) == 0)
        {
            //printf("gnss: OEM type is %s\r\n",table[i].desc);
            return table[i].type;
        }
    }
    //
    printf("gnss: OEM type is unknow.\r\n");
    return OEM_UNKNOW;
}


void gnss_on_getline_2(char *line,int32_t length,void *param)
{
    int32_t is_line_handled = 0;
    if(parser_check_line(line) == 0)
    {

        switch(g_gnss.working_mode)
        {
        case MODE_WORK_DIVIDED :
            if(mode_divided_line_master(line) == 0)
                is_line_handled = 1;
            break;
        case MODE_WORK_COUPLING :
        case MODE_WORK_COUPLING_INS :
            if(mode_coupling_line_master(line) == 0)
                is_line_handled = 1;
            break;
        case MODE_WORK_SINGLE :
            if(mode_single_line_master(line) == 0)
                is_line_handled = 1;
            break;
        case MODE_WORK_SINGLE_INS:
            if(mode_single_ins_line_master(line) == 0)
                 is_line_handled = 1;
            break;
        case MODE_WORK_SINGLE_BASE:
            if(mode_single_base_line_master(line) == 0)
                is_line_handled = 1;
            break;
        case MODE_WORK_SINGLE_2ANTENNA:
        case MODE_WORK_SINGLE_2ANTENNA_INS:
            if(mode_single_2antenna_line_master(line) == 0)
                is_line_handled = 1;
            break;

        default:break;
        }
    }
    //
    // 根据转发状态和行是否被解析，判断是否应该打印主板发来的行。
    if(g_output_line_mode == GNSS_OUTPUT_LINE_ALL ||
        (g_output_line_mode == GNSS_OUTPUT_LINE_IGNORE && !is_line_handled))
        printf(" 2=> %s",line);
}


void gnss_on_getline_3(char *line,int32_t length,void *param)
{
    int32_t is_line_handled = 0;
    if(parser_check_line(line) == 0)
    {

        switch(g_gnss.working_mode)
        {
        case MODE_WORK_DIVIDED :
            if(mode_divided_line_rover(line) == 0)
                is_line_handled = 1;
            break;
        case MODE_WORK_COUPLING :
        case MODE_WORK_COUPLING_INS :
            if(mode_coupling_line_rover(line) == 0)
                is_line_handled = 1;
            break;
        }
    }

    if(g_output_line_mode == GNSS_OUTPUT_LINE_ALL ||
        (g_output_line_mode == GNSS_OUTPUT_LINE_IGNORE && !is_line_handled))
        printf(" 3=> %s",line);
}


int32_t gnss_set_base(double base_latitude,double base_longitude,double base_altitude)
{
    if(base_latitude < -90 || base_latitude > 90
        || base_longitude <= 0 || base_longitude > 360
        || base_altitude < -1e5 || base_altitude > 1e5)
    {
        return -1;
    }
    //
    param_t * p = param_get();
    p->gnss.base_latitude  = base_latitude;
    p->gnss.base_longitude = base_longitude;
    p->gnss.base_altitude  = base_altitude;
    param_auto_save();
    //
    g_gnss.base_longitude = p->gnss.base_longitude;
    BL2XY_sanlian(g_gnss.base_longitude,p->gnss.base_longitude,p->gnss.base_latitude,&(g_gnss.base_x),&(g_gnss.base_y));
    g_gnss.base_z = p->gnss.base_altitude;
    //
    return 0;
}


int32_t gnss_get_current_datetime(datetime_t *dt)
{
    if(g_gnss.master_gpgga.nosv < 4)
        return -1;
    //
    if(g_gnss.master_gprmc.valid)
    {
        uint32_t utc_ms = g_gnss.master_gprmc.utc_ms;
        datetime_set(dt,
            g_gnss.master_gprmc.year, g_gnss.master_gprmc.month,  g_gnss.master_gprmc.day,
            (utc_ms / (60*60*1000)) % 24,
            (utc_ms / (60*1000))    % 60,
            (utc_ms % (60*1000))    * 1e-3f);
        datetime_utc2beijing(dt);
        return 0;
    }
    else if(g_gnss.master_bestxyza.gps_week != 0 && g_gnss.master_bestxyza.gps_ms != 0)
    {
        datetime_fromWeekSecond(dt,g_gnss.master_bestxyza.gps_week,g_gnss.master_bestxyza.gps_ms * 1e-3f);
        datetime_utc2beijing(dt);
        //
        return 0;
    }
    //
    return -1;
}


void gnss_output_line(gnss_output_line_t mode)
{
    g_output_line_mode = mode;
}

void gnss_check_message_age(gnss_t * gnss)
{
    const uint32_t time_min = time_nowMs() - 2000;
    //
    if(gnss->master_gpgga_update_time < time_min)
        memset(&(gnss->master_gpgga),0,sizeof(gnss->master_gpgga));
    if(gnss->master_bestxyza_update_time < time_min)
        memset(&(gnss->master_bestxyza),0,sizeof(gnss->master_bestxyza));
    if(gnss->master_gprmc_update_time < time_min)
        memset(&(gnss->master_gprmc),0,sizeof(gnss->master_gprmc));
    if(gnss->master_refstation_update_time < time_min)
        memset(&(gnss->master_refstation),0,sizeof(gnss->master_refstation));
    if(gnss->master_headinga_update_time < time_min)
        memset(&(gnss->master_headinga),0,sizeof(gnss->master_headinga));
    if(gnss->master_gptra_update_time < time_min)
        memset(&(gnss->master_gptra),0,sizeof(gnss->master_gptra));
    //
    if(gnss->rover_gpgga_update_time < time_min)
        memset(&(gnss->rover_gpgga),0,sizeof(gnss->rover_gpgga));
    if(gnss->rover_headinga_update_time < time_min)
        memset(&(gnss->rover_headinga),0,sizeof(gnss->rover_headinga));
    //
    if(gnss->ins_ppnav_update_time < time_min)
        memset(&(gnss->ins_ppnav),0,sizeof(gnss->ins_ppnav));
}
