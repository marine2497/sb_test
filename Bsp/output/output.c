// output/output.c
// 2013-11-29 19:14:14




#include "output.h"
#include "globle.h"
#include "parser/parser.h"
#include "parser/gphpd.h"
#include "parser/gphdt.h"
#include "parser/test.h"
#include "ins-engine/engine.h"
#include <stdio.h>
#include <string.h>
#include "math/interface.h"
#include "param/param.h"
#include "com/respond.h"
#include "security/register.h"
#include "security/boot-des.h"
#include "ins-engine/engine.h"
#include "time.h"

static uint64_t g_output_type = 0;
static int32_t g_output_silent = 0;


typedef void (*output_fn_t)(const gnss_t *gnss);

static uint64_t output_get_type(void);
static void output_to_ins(double x,double y,int32_t fix,float heading,float speed);
static void output_to_ins_2(double x,double y,int32_t fix,float dx,float dy,float speed);
static int32_t output_indicator_of_xingwang(int32_t master, int32_t rover);
static void output_mode_divided_type_south(const gnss_t *gnss);
static void output_mode_coupling_type_south(const gnss_t *gnss);
static void output_mode_divided_type_xingwang(const gnss_t *gnss);
static void output_mode_coupling_type_xingwang(const gnss_t *gnss);
static void output_mode_coupling_ins_type_xingwang(const gnss_t *gnss);
static void output_mode_single_type_xingwang(const gnss_t *gnss);
static void output_mode_single_ins_type_xingwang(const gnss_t *gnss);
static void output_mode_coupling_type_gphdt(const gnss_t *gnss);
static void output_mode_single_type_0x0400(const gnss_t *gnss);
static void output_mode_single_base_south(const gnss_t *gnss);
static void output_mode_single2ant_type_south(const gnss_t *gnss);
static void output_mode_single2ant_type_0x0400(const gnss_t *gnss);
static void output_mode_single2ant_type_xingwang(const gnss_t *gnss);
static void output_mode_all_type_test(const gnss_t *gnss);

void output_init(void)
{
    g_output_type = output_get_type();
    g_output_silent = 0;
}

void output_check_event(void)
{
    TIME_EXECUTE_PERIODICALLY_BEGIN(10000)
    {
        output_triger_refstation(gnss_get());
    } TIME_EXECUTE_PERIODICALLY_END()
}

uint64_t output_get_type(void)
{
    uint64_t type = param_get()->gnss.output_type;
    //
    struct
    {
        int32_t bit;
        const char *desc;
    } bit_desc[] =
    {
        { OUTPUT_BIT_SOUTH          , "AB57"},
        { OUTPUT_BIT_XING_WANG      , "GPHPD"},
        { OUTPUT_BIT_MASTER_GPGGA   , "GPGGA"},
        { OUTPUT_BIT_COMPASS_GPHDT  , "GPHDT"},
        { OUTPUT_BIT_MASTER_GPGSV   , "GPGSV"},
        { OUTPUT_BIT_D5EA_0x0400    , "0x0400"},
        { OUTPUT_BIT_TEST           , "TEST"},
    };
    //
    int32_t has = 0;
    for(int32_t i=0;i<sizeof(bit_desc)/sizeof(bit_desc[0]);i++)
    {
        if(output_is_bit_set(type,bit_desc[i].bit))
        {
            printf("output: com1 >> %s\r\n",bit_desc[i].desc);
            has = 1;
        }
    }
    if(! has)
        printf("output: No output.\r\n");
    //
    return type;
}

uint64_t output_current_type(void)
{
    return g_output_type;
}

int32_t output_set_type(uint64_t new_type)
{
    g_output_type = new_type; // 停止输出。
    param_get()->gnss.output_type = new_type;
    if(param_auto_save() != 0)
        return -1;
    //
    return 0;
}

void output_test(int32_t yes)
{
    if(yes)
        g_output_type = OUTPUT_BIT_VALUE_TEST;
    else
        g_output_type = param_get()->gnss.output_type;
}


void output_to_ins(double x,double y,int32_t fix,float heading,float speed)
{
    float h = heading * (3.1415926f/180.f);
    //
    float vel[2] = 
    {
        speed * sinf(h),
        speed * cosf(h)
    };
    //
    double xy[] = {x,y};
    engine_input_gnss(xy,fix,vel);
}


void output_to_ins_2(double x,double y,int32_t fix,float dx,float dy,float speed)
{
    float d_len = sqrtf(dx*dx + dy*dy);
    if(d_len < 0.01f)
        return;
    //
    float d_len_rev = 1.0f / d_len;
    //
    float vel[2] = 
    {
        speed * d_len_rev * dx,
        speed * d_len_rev * dy
    };
    //
    double xy[] = {x,y};
    engine_input_gnss(xy,fix,vel);
}


void output_silent(int32_t yes)
{
    g_output_silent = yes;
}

int32_t output_indicator_of_xingwang(int32_t master, int32_t rover)
{
    if(master == rover)
        return rover;
    //
    static int32_t s_tab[] = {0, 1, 2, 5, 6, 4 };
    for(uint32_t i=0; i<sizeof(s_tab)/sizeof(s_tab[0]); i++)
    {
        if(master == s_tab[i] || rover == s_tab[i])
            return s_tab[i];
    }
    //
    return 0;
}


void output_triger(const gnss_t *gnss)
{
  
    if(register_is_invalid_or_expried() || boot_des_is_invalid())
        return;

    struct
    {
        uint32_t working_mode;
        uint32_t bit;
        output_fn_t output_fn;
    } mode_bit[] =
    {
        { MODE_WORK_DIVIDED         , OUTPUT_BIT_SOUTH         , output_mode_divided_type_south       },
        { MODE_WORK_DIVIDED         , OUTPUT_BIT_XING_WANG     , output_mode_divided_type_xingwang    },
        { MODE_WORK_COUPLING        , OUTPUT_BIT_SOUTH         , output_mode_coupling_type_south      },
        { MODE_WORK_COUPLING_INS    , OUTPUT_BIT_SOUTH         , output_mode_coupling_type_south      },
        { MODE_WORK_COUPLING        , OUTPUT_BIT_XING_WANG     , output_mode_coupling_type_xingwang   },
        { MODE_WORK_COUPLING_INS    , OUTPUT_BIT_XING_WANG     , output_mode_coupling_ins_type_xingwang   },
        { MODE_WORK_SINGLE          , OUTPUT_BIT_XING_WANG     , output_mode_single_type_xingwang     },
        { MODE_WORK_COUPLING        , OUTPUT_BIT_COMPASS_GPHDT , output_mode_coupling_type_gphdt      },//电子罗经gphdt报文
        { MODE_WORK_COUPLING_INS    , OUTPUT_BIT_COMPASS_GPHDT , output_mode_coupling_type_gphdt      },//电子罗经gphdt报文
        { MODE_WORK_SINGLE_INS      , OUTPUT_BIT_XING_WANG     , output_mode_single_ins_type_xingwang },//sm39带ins,gphpd报文
        { MODE_WORK_SINGLE          , OUTPUT_BIT_D5EA_0x0400   , output_mode_single_type_0x0400       },//沈阳低精度主板单天线方式
        { MODE_WORK_SINGLE_BASE     , OUTPUT_BIT_SOUTH         , output_mode_single_base_south        },
        { MODE_WORK_DIVIDED         , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
        { MODE_WORK_COUPLING        , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
        { MODE_WORK_COUPLING_INS    , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
        { MODE_WORK_SINGLE          , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
        { MODE_WORK_SINGLE_INS      , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
        { MODE_WORK_SINGLE_2ANTENNA , OUTPUT_BIT_SOUTH         , output_mode_single2ant_type_south    },
        { MODE_WORK_SINGLE_2ANTENNA , OUTPUT_BIT_D5EA_0x0400   , output_mode_single2ant_type_0x0400   },
        { MODE_WORK_SINGLE_2ANTENNA , OUTPUT_BIT_XING_WANG     , output_mode_single2ant_type_xingwang },
        { MODE_WORK_SINGLE_2ANTENNA , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
        { MODE_WORK_SINGLE_2ANTENNA_INS , OUTPUT_BIT_SOUTH         , output_mode_single2ant_type_south    },
        { MODE_WORK_SINGLE_2ANTENNA_INS , OUTPUT_BIT_D5EA_0x0400   , output_mode_single2ant_type_0x0400   },
        { MODE_WORK_SINGLE_2ANTENNA_INS , OUTPUT_BIT_XING_WANG     , output_mode_single2ant_type_xingwang },
        { MODE_WORK_SINGLE_2ANTENNA_INS , OUTPUT_BIT_TEST          , output_mode_all_type_test            },
    };
    //
    for(int32_t i=0;i<sizeof(mode_bit)/sizeof(mode_bit[0]);i++)
    {
        if(mode_bit[i].working_mode == gnss->working_mode &&
            output_is_bit_set(g_output_type,mode_bit[i].bit) )
        {
            mode_bit[i].output_fn(gnss);
        }
    }
}

void output_triger_refstation(const gnss_t *gnss)
{

    if(register_is_invalid_or_expried() || boot_des_is_invalid())
        return;
    //
    if(g_output_silent || !output_is_bits_set(g_output_type,OUTPUT_BIT_VALUE_SOUTH|OUTPUT_BIT_VALUE_D5EA_0x0400))
        return;

    body_refstation_0x0d04_t body;
    //
    const param_t *param = param_get();
    if(gnss->master_refstation.valid)
    {
        long double la=0,lo=0,al=0;
        XYZ2BLH(gnss->master_refstation.x,gnss->master_refstation.y,gnss->master_refstation.z,&la,&lo,&al);
        body.latitude   = la * 180/3.14159265359;
        body.longitude  = lo * 180/3.14159265359;
        body.altitude   = al;
        body.valid =  1;
    }
    else if(param->gnss.base_longitude != 0)
    {
        body.latitude   = param->gnss.base_latitude;
        body.longitude  = param->gnss.base_longitude;
        body.altitude   = param->gnss.base_altitude;
        body.valid =  2;
    }
    else
    {
        body.latitude   = 0;
        body.longitude  = 0;
        body.altitude   = 0;
        body.valid =  0;
    }
    //
    respond_output_refstation(&body);
}

void output_mode_divided_type_south(const gnss_t *gnss)
{
    body_output_0x0100_t body;
    memset(&body,0,sizeof(body));
    //
    body.hour             = (gnss->master_gpgga.utc_ms / (60*60*1000) ) % 24;
    body.minute           = (gnss->master_gpgga.utc_ms / (60*1000)    ) % 60;
    body.second           = (gnss->master_gpgga.utc_ms / (1000)       ) % 60;
    body.millisecond      = (gnss->master_gpgga.utc_ms                ) % 1000;
    body.rover_latitude   = gnss->rover_gpgga.latitude;
    body.rover_longitude  = gnss->rover_gpgga.longitude;
    body.rover_altitude   = gnss->rover_gpgga.altitude;
    body.master_latitude  = gnss->master_gpgga.latitude;
    body.master_longitude = gnss->master_gpgga.longitude;
    body.master_altitude  = gnss->master_gpgga.altitude;
    body.speed            = gnss->master_gprmc.speed * 3.6f;
    body.pitch            = engine_get_pitch() * (180.0f/3.1415926f);
    body.roll             = engine_get_roll()  * (180.0f/3.1415926f);
    body.rover_nosv       = gnss->rover_gpgga.nosv;
    body.master_nosv      = gnss->master_gpgga.nosv;
    body.rover_valid      = gnss->rover_gpgga.valid;
    body.master_valid     = gnss->master_gpgga.valid;
    //
    if(gnss->master_gpgga.valid != 0 && gnss->rover_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        double rx = 0;
        double ry = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        covertBLToXY(gnss->rover_gpgga.latitude,gnss->rover_gpgga.longitude,&rx,&ry);
        //
        float dx = rx - mx;
        float dy = ry - my;

        output_to_ins_2(mx,my,gnss->master_gpgga.valid,dx,dy,gnss->master_gprmc.speed);
    }
    //
    body_output_0x0d03_t body_infor;
    body_infor.rover_nosv       = gnss->rover_gpgga.nosv;
    body_infor.master_nosv      = gnss->master_gpgga.nosv;
    body_infor.rover_valid      = gnss->rover_gpgga.valid;
    body_infor.master_valid     = gnss->master_gpgga.valid;
    body_infor.rover_age        = gnss->rover_gpgga.diff_age;
    body_infor.master_age       = gnss->master_gpgga.diff_age;
    body_infor.rover_hdop       = gnss->rover_gpgga.hdop;
    body_infor.master_hdop      = gnss->master_gpgga.hdop;
    //
    if(! g_output_silent)
    {
        respond_output_0x0100(&body);
        respond_output_ox0d03(&body_infor);
        //
        if(gnss->master_gpgga.utc_ms % (30*1000) == 0)
        {
            datetime_t now;
            datetime_fromWeekSecond(&now,gnss->master_bestxyza.gps_week,gnss->master_bestxyza.gps_ms * 1e-3);
            respond_sync_time(&now);
        }
    }
}

void output_mode_coupling_type_south(const gnss_t *gnss)
{
    body_output_0x0200_t body;
    memset(&body,0,sizeof(body));
    //
    body.year             = gnss->master_gprmc.year;
    body.month            = gnss->master_gprmc.month;
    body.day              = gnss->master_gprmc.day;
    body.hour             = (gnss->master_gprmc.utc_ms / (60*60*1000) ) % 24;
    body.minute           = (gnss->master_gprmc.utc_ms / (60*1000)    ) % 60;
    body.second           = (gnss->master_gprmc.utc_ms / (1000)       ) % 60;
    body.millisecond      = (gnss->master_gprmc.utc_ms                ) % 1000;
    body.master_latitude  = gnss->master_gpgga.latitude;
    body.master_longitude = gnss->master_gpgga.longitude;
    body.master_altitude  = gnss->master_gpgga.altitude;
    body.speed            = gnss->master_gprmc.speed * 3.6f;
    body.heading_rmc      = gnss->master_gprmc.heading;
    body.pitch_ins        = engine_get_pitch() * (180.0f/3.1415926f);
    body.roll_ins         = engine_get_roll()  * (180.0f/3.1415926f);
    body.master_nosv      = gnss->master_gpgga.nosv;
    body.master_hdop_10   = gnss->master_gpgga.hdop * 10;
    body.master_diff_age  = gnss->master_gpgga.diff_age;
    body.master_gga_valid = gnss->master_gpgga.valid;
    body.master_rmc_valid = gnss->master_gprmc.valid;
    //
    switch(gnss->oem_type)
    {
    case OEM_OEM615:
        body.heading_valid    = (gnss->rover_headinga_update_time + 2000 > time_nowMs()) ? 1 : 0;
        body.heading          = gnss->rover_headinga.heading;
        body.pitch_gnss       = gnss->rover_headinga.pitch;
        body.rover_nosv       = gnss->rover_headinga.sol_n;
        body.rover_valid      = gnss->rover_headinga.sol_type;
        body.base_line_length = gnss->rover_headinga.base_line_length;
        break;
    case OEM_K501:
    case OEM_K505:
        body.heading_valid    = (gnss->master_gptra_update_time + 2000 > time_nowMs()) ? 1 : 0;
        body.heading          = gnss->master_gptra.heading;
        body.pitch_gnss       = gnss->master_gptra.pitch;
        body.rover_nosv       = gnss->master_gptra.satNo;
        body.rover_valid      = gnss->master_gptra.valid;
        body.base_line_length = 0;
        break;
    default :
        break;
    }

    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,body.heading,gnss->master_gpgga.valid,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        respond_output_0x0200(&body);
    }
}

void output_mode_divided_type_xingwang(const gnss_t *gnss)
{
    gphpd_t gphpd;
    memset(&gphpd,0,sizeof(gphpd));
    //
    // 可以直接取的。
    gphpd.gpsweek   = gnss->master_bestxyza.gps_week;
    gphpd.gpstime   = gnss->master_bestxyza.gps_ms * 1e-3;
    gphpd.ve        = gnss->master_bestxyza.ve;
    gphpd.vn        = gnss->master_bestxyza.vn;
    gphpd.vu        = gnss->master_bestxyza.vu;
    gphpd.ae        = gnss->master_bestxyza.ae;
    gphpd.an        = gnss->master_bestxyza.an;
    gphpd.au        = gnss->master_bestxyza.au;
    gphpd.nsv1      = gnss->master_gpgga.nosv;
    gphpd.nsv2      = gnss->rover_gpgga.nosv;
    gphpd.latitude  = gnss->master_gpgga.latitude;
    gphpd.longitude = gnss->master_gpgga.longitude;
    gphpd.altitude  = gnss->master_gpgga.altitude;
    gphpd.roll      = engine_get_roll() * (180.0f/3.1415926f);
    gphpd.valid     = output_indicator_of_xingwang(gnss->master_gpgga.valid, gnss->rover_gpgga.valid);
    //
    // 用三联的投影算“X”、“Y”、“Z”。
    {
        if(gnss->master_gpgga.valid == 0)
        {
            gphpd.x = gphpd.y = gphpd.z = 0;
        }
        else
        {
            double x = 0;
            double y = 0;
            BL2XY_sanlian(gnss->base_longitude,gnss->master_gpgga.longitude,gnss->master_gpgga.latitude,&x,&y);
            gphpd.x = x - gnss->base_x;
            gphpd.y = y - gnss->base_y;
            gphpd.z = gnss->master_gpgga.altitude - gnss->base_z;
        }
    }
    //
    // 用高斯投影算“航向”、“基线”和“俯仰”。
    {
        if(gnss->master_gpgga.valid == 0 || gnss->rover_gpgga.valid == 0)
        {
            gphpd.baseline = 0;
            gphpd.heading = 0;
            gphpd.pitch = 0;
        }
        else
        {
            double mx = 0;
            double my = 0;
            double mz = gnss->master_gpgga.altitude;
            double rx = 0;
            double ry = 0;
            double rz = gnss->rover_gpgga.altitude;
            covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
            covertBLToXY(gnss->rover_gpgga.latitude,gnss->rover_gpgga.longitude,&rx,&ry);
            //
            //rx += 100.0f*1/2.f;
            //ry += 100.0f*1.73f/2.f;
            //rz += 100.0f*1;
            //
            // 投影后的坐标必须用double，但相对距离可以用float。
            float dx = rx - mx;
            float dy = ry - my;
            float dz = rz - mz;
            //
            gphpd.baseline = sqrtf(dx*dx + dy*dy);
            if(fabs(gphpd.baseline) < 0.01)
                gphpd.pitch = 0;
            else
                gphpd.pitch = atanf((rz-mz) / gphpd.baseline) * (180.0f/3.1415926f);
            gphpd.heading = (atan2f(dx,dy) * (180.0f/3.1415926f));
            if(gphpd.heading < 0)
                gphpd.heading += 360.0f;
            //
            // 把航向和速度信息发送到惯导模块。
            output_to_ins(mx,my,gnss->master_gpgga.valid,gphpd.heading,gnss->master_gprmc.speed);
        }
    }
    //
    if(! g_output_silent)
    {
        static char line[256];
        gphpd_print(line,&gphpd);
        printf("%s",line);
    }
}

void output_mode_coupling_type_xingwang(const gnss_t *gnss)
{
    gphpd_t gphpd;
    memset(&gphpd,0,sizeof(gphpd));
    //
    // 可以直接取的。
    gphpd.gpsweek   = gnss->master_bestxyza.gps_week;
    gphpd.gpstime   = gnss->master_bestxyza.gps_ms * 1e-3;
    gphpd.ve        = gnss->master_bestxyza.ve;
    gphpd.vn        = gnss->master_bestxyza.vn;
    gphpd.vu        = gnss->master_bestxyza.vu;
    gphpd.ae        = gnss->master_bestxyza.ae;
    gphpd.an        = gnss->master_bestxyza.an;
    gphpd.au        = gnss->master_bestxyza.au;
    gphpd.nsv1      = gnss->master_gpgga.nosv;
    gphpd.nsv2      = gnss->rover_gpgga.nosv;
    gphpd.latitude  = gnss->master_gpgga.latitude;
    gphpd.longitude = gnss->master_gpgga.longitude;
    gphpd.altitude  = gnss->master_gpgga.altitude;
    gphpd.roll      = engine_get_roll() * (180.0f/3.1415926f);
    //
    // 用三联的投影算“X”、“Y”、“Z”。
    {
        if(gnss->master_gpgga.valid == 0)
        {
            gphpd.x = gphpd.y = gphpd.z = 0;
        }
        else
        {
            double x = 0;
            double y = 0;
            BL2XY_sanlian(gnss->base_longitude,gnss->master_gpgga.longitude,gnss->master_gpgga.latitude,&x,&y);
            gphpd.x = x - gnss->base_x;
            gphpd.y = y - gnss->base_y;
            gphpd.z = gnss->master_gpgga.altitude - gnss->base_z;
        }
    }
    //
    if(gnss->master_gpgga.valid == 0 || gnss->rover_gpgga.valid == 0)
    {
        gphpd.pitch     = 0;
        gphpd.heading   = 0;
        gphpd.baseline  = 0;
        gphpd.valid     = 0;
    }
    else
    {
        switch(gnss->oem_type)
        {
        case OEM_OEM615:
            gphpd.pitch     = gnss->rover_headinga.pitch;
            gphpd.heading   = gnss->rover_headinga.heading;
            gphpd.baseline  = gnss->rover_headinga.base_line_length \
                                * fabs(cosf(gnss->rover_headinga.pitch * (3.1415926f/180.0f)));
            gphpd.valid     = output_indicator_of_xingwang(gnss->master_gpgga.valid,gnss->rover_headinga.sol_type);
            break;
        case OEM_K501:
        case OEM_K505:
            gphpd.pitch     = gnss->master_gptra.pitch;
            gphpd.heading   = gnss->master_gptra.heading;
            gphpd.baseline  = 0;
            gphpd.valid     = output_indicator_of_xingwang(gnss->master_gpgga.valid,gnss->master_gptra.valid);
            break;
        default :
            break;
        }
    }
    //
    // 把航向和速度信息发送到惯导模块。
    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gphpd.heading,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        static char line[256];
        gphpd_print(line,&gphpd);
        printf("%s",line);
    }
}

// 协同测向模式，输出星网格式。
void output_mode_coupling_ins_type_xingwang(const gnss_t *gnss)
{
    gphpd_t gphpd;
    memset(&gphpd,0,sizeof(gphpd));
    //
    // 可以直接取的。
    gphpd.gpsweek   = gnss->master_bestxyza.gps_week;
    gphpd.gpstime   = gnss->master_bestxyza.gps_ms * 1e-3;
    gphpd.ve        = gnss->master_bestxyza.ve;
    gphpd.vn        = gnss->master_bestxyza.vn;
    gphpd.vu        = gnss->master_bestxyza.vu;
    gphpd.ae        = gnss->master_bestxyza.ae;
    gphpd.an        = gnss->master_bestxyza.an;
    gphpd.au        = gnss->master_bestxyza.au;
    gphpd.nsv1      = gnss->master_gpgga.nosv;
    gphpd.nsv2      = gnss->rover_gpgga.nosv;
    gphpd.latitude  = gnss->master_gpgga.latitude;
    gphpd.longitude = gnss->master_gpgga.longitude;
    gphpd.altitude  = gnss->master_gpgga.altitude;
    gphpd.roll      = engine_get_roll() * (180.0f/3.1415926f);
    //
    // 用三联的投影算“X”、“Y”、“Z”。
    {
        if(gnss->master_gpgga.valid == 0)
        {
            gphpd.x = gphpd.y = gphpd.z = 0;
        }
        else
        {
            double x = 0;
            double y = 0;
            BL2XY_sanlian(gnss->base_longitude,gnss->master_gpgga.longitude,gnss->master_gpgga.latitude,&x,&y);
            gphpd.x = x - gnss->base_x;
            gphpd.y = y - gnss->base_y;
            gphpd.z = gnss->master_gpgga.altitude - gnss->base_z;
        }
    }
    //
    int32_t heading_valid = 0;
    if(gnss->master_gpgga.valid == 0)
    {
        gphpd.pitch     = 0;
        gphpd.heading   = 0;
        gphpd.baseline  = 0;
        gphpd.valid     = 0;
    }
    else
    {
        switch(gnss->oem_type)
        {
        case OEM_OEM615:
            gphpd.pitch     = gnss->rover_headinga.pitch;
            gphpd.heading   = gnss->rover_headinga.heading;
            gphpd.baseline  = gnss->rover_headinga.base_line_length \
                                * fabs(cosf(gnss->rover_headinga.pitch * (3.1415926f/180.0f)));
            heading_valid   = gnss->rover_headinga.sol_type;
            break;
        case OEM_K501:
        case OEM_K505:
            gphpd.pitch     = gnss->master_gptra.pitch;
            gphpd.heading   = gnss->master_gptra.heading;
            gphpd.baseline  = 0;
            heading_valid   = gnss->master_gptra.valid;
            break;
        default :
            break;
        }
    }
    if(gnss->master_gpgga.valid == 6 || (gnss->master_gpgga.valid >=4 && heading_valid == 0))
    {
        gphpd.pitch     = engine_get_pitch() * (180.0f/3.1415926f);
        gphpd.heading   = gnss->ins_ppnav.heading;
        gphpd.baseline  = 0;
        gphpd.valid     = 6;
    }
    else
    {
        gphpd.valid = output_indicator_of_xingwang(gnss->master_gpgga.valid, heading_valid);
    }
    //
    // 把航向和速度信息发送到惯导模块。
    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gphpd.heading,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        static char line[256];
        gphpd_print(line,&gphpd);
        printf("%s",line);
    }
}



void output_mode_coupling_type_gphdt(const gnss_t *gnss)
{
    gphdt_t gphdt;
    memset(&gphdt,0,sizeof(gphdt));
    //
    gphdt.heading = gnss->rover_headinga.heading;
    if(! g_output_silent)
    {
        static char line[256];
        gphdt_print(line,&gphdt);
        printf("%s",line);
    }
}


static void output_mode_single_type_xingwang(const gnss_t *gnss)
{
    gphpd_t gphpd;
    memset(&gphpd,0,sizeof(gphpd));
    //
    // 可以直接取的。
    gphpd.gpsweek   = gnss->master_bestxyza.gps_week;
    gphpd.gpstime   = gnss->master_bestxyza.gps_ms * 1e-3;
    gphpd.ve        = gnss->master_bestxyza.ve;
    gphpd.vn        = gnss->master_bestxyza.vn;
    gphpd.vu        = gnss->master_bestxyza.vu;
    gphpd.ae        = gnss->master_bestxyza.ae;
    gphpd.an        = gnss->master_bestxyza.an;
    gphpd.au        = gnss->master_bestxyza.au;
    gphpd.nsv1      = gnss->master_gpgga.nosv;
    gphpd.nsv2      = 0;
    gphpd.latitude  = gnss->master_gpgga.latitude;
    gphpd.longitude = gnss->master_gpgga.longitude;
    gphpd.altitude  = gnss->master_gpgga.altitude;
    gphpd.valid     = gnss->master_gpgga.valid;
    gphpd.roll      = engine_get_roll()  * (180.0f/3.1415926f);
    gphpd.pitch     = engine_get_pitch() * (180.0f/3.1415926f);
    gphpd.heading   = gnss->master_gprmc.heading;
    gphpd.baseline  = 0;
    //
    // 用三联的投影算“X”、“Y”、“Z”。
    {
        if(gnss->master_gpgga.valid == 0)
        {
            gphpd.x = gphpd.y = gphpd.z = 0;
        }
        else
        {
            double x = 0;
            double y = 0;
            BL2XY_sanlian(gnss->base_longitude,gnss->master_gpgga.longitude,gnss->master_gpgga.latitude,&x,&y);
            gphpd.x = x - gnss->base_x;
            gphpd.y = y - gnss->base_y;
            gphpd.z = gnss->master_gpgga.altitude - gnss->base_z;
        }
    }
   
    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gphpd.heading,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        static char line[256];
        gphpd_print(line,&gphpd);
        printf("%s",line);
    }
}



static void output_mode_single_ins_type_xingwang(const gnss_t *gnss)
{
    gphpd_t gphpd;
    memset(&gphpd,0,sizeof(gphpd));
    //
    // 可以直接取的。
    gphpd.gpsweek   = gnss->master_bestxyza.gps_week;
    gphpd.gpstime   = gnss->master_bestxyza.gps_ms * 1e-3;
    gphpd.ve        = gnss->master_bestxyza.ve;
    gphpd.vn        = gnss->master_bestxyza.vn;
    gphpd.vu        = gnss->master_bestxyza.vu;
    gphpd.ae        = gnss->master_bestxyza.ae;
    gphpd.an        = gnss->master_bestxyza.an;
    gphpd.au        = gnss->master_bestxyza.au;
    gphpd.nsv1      = gnss->master_gpgga.nosv;
    gphpd.nsv2      = 0;
    gphpd.latitude  = gnss->master_gpgga.latitude;
    gphpd.longitude = gnss->master_gpgga.longitude;
    gphpd.altitude  = gnss->master_gpgga.altitude;
    gphpd.valid     = gnss->master_gpgga.valid;


    
    if(gnss->ins_ppnav.init_flag % 2 == 1)
    {
    	gphpd.roll      = gnss->ins_ppnav.roll;
		gphpd.pitch     = gnss->ins_ppnav.pitch;
		gphpd.heading   = gnss->ins_ppnav.heading;
		gphpd.baseline  = 0;
    }
    else
    {
		gphpd.roll      = engine_get_roll()  * (180.0f/3.1415926f);
		gphpd.pitch     = engine_get_pitch() * (180.0f/3.1415926f);
		gphpd.heading   = gnss->master_gprmc.heading;
		gphpd.baseline  = 0;
    }

    {
        if(gnss->master_gpgga.valid == 0)
        {
            gphpd.x = gphpd.y = gphpd.z = 0;
        }
        else
        {
            double x = 0;
            double y = 0;
            BL2XY_sanlian(gnss->base_longitude,gnss->master_gpgga.longitude,gnss->master_gpgga.latitude,&x,&y);
            gphpd.x = x - gnss->base_x;
            gphpd.y = y - gnss->base_y;
            gphpd.z = gnss->master_gpgga.altitude - gnss->base_z;
        }
    }

    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gphpd.heading,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        static char line[256];
        gphpd_print(line,&gphpd);
        printf("%s",line);
    }
}

void output_mode_single_type_0x0400(const gnss_t *gnss)
{
    body_output_0x0400_t  body;
    memset(&body,0,sizeof(body));
    //
    body.year             = gnss->master_gprmc.year;
    body.month            = gnss->master_gprmc.month;
    body.day              = gnss->master_gprmc.day;
    body.hour             = (gnss->master_gprmc.utc_ms / (60*60*1000) ) % 24;
    body.minute           = (gnss->master_gprmc.utc_ms / (60*1000)    ) % 60;
    body.second           = (gnss->master_gprmc.utc_ms / (1000)       ) % 60;
    body.ms               = (gnss->master_gprmc.utc_ms                ) % 1000;
    body.latitude         = gnss->master_gpgga.latitude;
    body.longitude        = gnss->master_gpgga.longitude;
    body.altitude         = gnss->master_gpgga.altitude;
    body.speed            = gnss->master_gprmc.speed * 3.6f;
    body.heading          = gnss->master_gprmc.heading;
    body.valid            = gnss->master_gpgga.valid;
    //
    body.roll             = engine_get_roll()  * (180.0f/3.1415926f);
    body.pitch            = engine_get_pitch() * (180.0f/3.1415926f);
    //
    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gnss->master_gprmc.heading,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        respond_output_0x0400(&body);
    }
}



void output_mode_single_base_south(const gnss_t *gnss)
{

    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gnss->master_gprmc.heading,gnss->master_gprmc.speed);
    }
    printf("$STHBASE,lat:%0.7f,longit:%0.7f,alt:%0.3f,satnum:%d,fix:%d\r\n",
    		gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,gnss->master_gpgga.altitude,
    		gnss->master_gpgga.nosv,gnss->master_gpgga.valid);
}

void output_mode_single2ant_type_south(const gnss_t *gnss)
{
    body_output_0x0200_t body;
    memset(&body,0,sizeof(body));
    //
    body.year             = gnss->master_gprmc.year;
    body.month            = gnss->master_gprmc.month;
    body.day              = gnss->master_gprmc.day;
    body.hour             = (gnss->master_gprmc.utc_ms / (60*60*1000) ) % 24;
    body.minute           = (gnss->master_gprmc.utc_ms / (60*1000)    ) % 60;
    body.second           = (gnss->master_gprmc.utc_ms / (1000)       ) % 60;
    body.millisecond      = (gnss->master_gprmc.utc_ms                ) % 1000;
    body.master_latitude  = gnss->master_gpgga.latitude;
    body.master_longitude = gnss->master_gpgga.longitude;
    body.master_altitude  = gnss->master_gpgga.altitude;
    body.speed            = gnss->master_gprmc.speed * 3.6f;
    body.heading_rmc      = gnss->master_gprmc.heading;
    body.pitch_ins        = engine_get_pitch() * (180.0f/3.1415926f);
    body.roll_ins         = engine_get_roll()  * (180.0f/3.1415926f);
    body.master_nosv      = gnss->master_gpgga.nosv;
    body.master_hdop_10   = gnss->master_gpgga.hdop * 10;
    body.master_diff_age  = gnss->master_gpgga.diff_age;
    body.master_gga_valid = gnss->master_gpgga.valid;
    body.master_rmc_valid = gnss->master_gprmc.valid;
    //
    switch(gnss->oem_type)
    {
    case OEM_OEM615:
        body.heading_valid    = (gnss->master_headinga_update_time + 2000 > time_nowMs()) ? 1 : 0;
        body.heading          = gnss->master_headinga.heading;
        body.pitch_gnss       = gnss->master_headinga.pitch;
        body.rover_nosv       = gnss->master_headinga.sol_n;
        body.rover_valid      = gnss->master_headinga.sol_type;
        body.base_line_length = gnss->rover_headinga.base_line_length;
        break;
    case OEM_K501:
    case OEM_K505:
        body.heading_valid    = (gnss->master_gptra_update_time + 2000 > time_nowMs()) ? 1 : 0;
        body.heading          = gnss->master_gptra.heading;
        body.pitch_gnss       = gnss->master_gptra.pitch;
        body.rover_nosv       = gnss->master_gptra.satNo;
        body.rover_valid      = gnss->master_gptra.valid;
        body.base_line_length = 0;
        break;
    default :
        break;
    }

    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,body.heading,gnss->master_gpgga.valid,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        respond_output_0x0200(&body);
    }
}

void output_mode_single2ant_type_0x0400(const gnss_t *gnss)
{
    body_output_0x0400_t  body;
    memset(&body,0,sizeof(body));
    //
    body.year             = gnss->master_gprmc.year;
    body.month            = gnss->master_gprmc.month;
    body.day              = gnss->master_gprmc.day;
    body.hour             = (gnss->master_gprmc.utc_ms / (60*60*1000) ) % 24;
    body.minute           = (gnss->master_gprmc.utc_ms / (60*1000)    ) % 60;
    body.second           = (gnss->master_gprmc.utc_ms / (1000)       ) % 60;
    body.ms               = (gnss->master_gprmc.utc_ms                ) % 1000;
    body.latitude         = gnss->master_gpgga.latitude;
    body.longitude        = gnss->master_gpgga.longitude;
    body.altitude         = gnss->master_gpgga.altitude;
    body.speed            = gnss->master_gprmc.speed * 3.6f;
    body.valid            = gnss->master_gpgga.valid;
    //
    body.roll             = engine_get_roll()  * (180.0f/3.1415926f);
    //
    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gnss->master_gprmc.heading,gnss->master_gprmc.speed);
        //
        switch(gnss->oem_type)
        {
        case OEM_OEM617:
            body.heading    = gnss->master_headinga.heading;
            body.pitch      = gnss->master_headinga.pitch;
            break;
        case OEM_K528:
            body.heading   = gnss->master_gptra.heading;
            body.pitch     = gnss->master_gptra.pitch;
            break;
        }
    }
    //
    if(! g_output_silent)
    {
        respond_output_0x0400(&body);
    }
}

void output_mode_single2ant_type_xingwang(const gnss_t *gnss)
{
    gphpd_t gphpd;
    memset(&gphpd,0,sizeof(gphpd));

    gphpd.gpsweek   = gnss->master_bestxyza.gps_week;
    gphpd.gpstime   = gnss->master_bestxyza.gps_ms * 1e-3;
    gphpd.ve        = gnss->master_bestxyza.ve;
    gphpd.vn        = gnss->master_bestxyza.vn;
    gphpd.vu        = gnss->master_bestxyza.vu;
    gphpd.ae        = gnss->master_bestxyza.ae;
    gphpd.an        = gnss->master_bestxyza.an;
    gphpd.au        = gnss->master_bestxyza.au;
    gphpd.nsv1      = gnss->master_gpgga.nosv;
    gphpd.latitude  = gnss->master_gpgga.latitude;
    gphpd.longitude = gnss->master_gpgga.longitude;
    gphpd.altitude  = gnss->master_gpgga.altitude;
    gphpd.roll      = engine_get_roll() * (180.0f/3.1415926f);

    {
        if(gnss->master_gpgga.valid == 0)
        {
            gphpd.x = gphpd.y = gphpd.z = 0;
        }
        else
        {
            double x = 0;
            double y = 0;
            BL2XY_sanlian(gnss->base_longitude,gnss->master_gpgga.longitude,gnss->master_gpgga.latitude,&x,&y);
            gphpd.x = x - gnss->base_x;
            gphpd.y = y - gnss->base_y;
            gphpd.z = gnss->master_gpgga.altitude - gnss->base_z;
        }
    }
    //
    switch(gnss->oem_type)
    {
    case OEM_OEM617:
        gphpd.pitch     = gnss->master_headinga.pitch;
        gphpd.heading   = gnss->master_headinga.heading;
        gphpd.baseline  = gnss->master_headinga.base_line_length \
                            * fabs(cosf(gnss->master_headinga.pitch * (3.1415926f/180.0f)));
        gphpd.nsv2      = gnss->master_headinga.sol_n;
        gphpd.valid     = output_indicator_of_xingwang(gnss->master_gpgga.valid,gnss->master_headinga.sol_type);
        break;
    case OEM_K528:
        gphpd.pitch     = gnss->master_gptra.pitch;
        gphpd.heading   = gnss->master_gptra.heading;
        gphpd.baseline  = 0;
        gphpd.nsv2      = gnss->master_gptra.satNo;
        gphpd.valid     = output_indicator_of_xingwang(gnss->master_gpgga.valid,gnss->master_gptra.valid);
        break;
    }
    //
    // 把航向和速度信息发送到惯导模块。
    if(gnss->master_gpgga.valid != 0)
    {
        double mx = 0;
        double my = 0;
        covertBLToXY(gnss->master_gpgga.latitude,gnss->master_gpgga.longitude,&mx,&my);
        output_to_ins(mx,my,gnss->master_gpgga.valid,gphpd.heading,gnss->master_gprmc.speed);
    }
    //
    if(! g_output_silent)
    {
        static char line[256];
        gphpd_print(line,&gphpd);
        printf("%s",line);
    }
}

void output_mode_all_type_test(const gnss_t *gnss)
{
    test_t test;
    memset(&test,0,sizeof(test));
    //
    strncpy(test.model, G_BOOT_INFO->model, 31);
    strncpy(test.oem_type  , G_BOOT_INFO->board1sn, 2);
    strncpy(test.oem_type+2, G_BOOT_INFO->board2sn, 2);
    strncpy(test.working_mode,gnss_get_working_mode_string(),sizeof(test.working_mode));
    //
    test.sys_time       = time_nowMs();
    test.utc_year       = gnss->master_gprmc.year;
    test.utc_month      = gnss->master_gprmc.month;
    test.utc_day        = gnss->master_gprmc.day;
    test.utc_ms_of_day  = gnss->master_gprmc.utc_ms;
    test.gps_week       = gnss->master_bestxyza.gps_week;
    test.gps_ms         = gnss->master_bestxyza.gps_ms;
    //
    test.speed          = gnss->master_gprmc.speed;
    test.heading        = gnss->master_gprmc.heading;
    test.yaw            = 0;
    test.pitch          = 0;
    test.roll           = engine_get_roll() * (180.0f/3.1415926f);
    //
    test.master.latitude  = gnss->master_gpgga.latitude;
    test.master.longitude = gnss->master_gpgga.longitude;
    test.master.altitude  = gnss->master_gpgga.altitude;
    test.master.sol_n     = gnss->master_gpgga.nosv;
    test.master.sol_type  = gnss->master_gpgga.valid;
    test.master.diff_age  = gnss->master_gpgga.diff_age;
    test.master.hdop      = gnss->master_gpgga.hdop;
    //
    test.rover.latitude   = gnss->rover_gpgga.latitude;
    test.rover.longitude  = gnss->rover_gpgga.longitude;
    test.rover.altitude   = gnss->rover_gpgga.altitude;
    test.rover.sol_n      = gnss->rover_gpgga.nosv;
    test.rover.sol_type   = gnss->rover_gpgga.valid;
    test.rover.diff_age   = gnss->rover_gpgga.diff_age;
    test.rover.hdop       = gnss->rover_gpgga.hdop;
    //
    if(gnss->working_mode != MODE_WORK_COUPLING && gnss->working_mode != MODE_WORK_SINGLE_2ANTENNA && gnss->working_mode != MODE_WORK_SINGLE_2ANTENNA_INS)
    {
        test.pitch          = engine_get_pitch() * (180.0f/3.1415926f);
    }
    switch(gnss->oem_type)
    {
    case OEM_OEM615:
    case OEM_OEM617:
        if(gnss->working_mode == MODE_WORK_COUPLING)
        {
            test.yaw            = gnss->rover_headinga.heading;
            test.pitch          = gnss->rover_headinga.pitch;
            test.base_line      = gnss->rover_headinga.base_line_length \
                                   * fabs(cosf(gnss->rover_headinga.pitch * (3.1415926f/180.0f)));
            test.rover.sol_n    = gnss->rover_headinga.sol_n;
            test.rover.sol_type = gnss->rover_headinga.sol_type;
        }
        if(gnss->working_mode == MODE_WORK_SINGLE_2ANTENNA || gnss->working_mode == MODE_WORK_SINGLE_2ANTENNA_INS)
        {
            test.yaw            = gnss->master_headinga.heading;
            test.pitch          = gnss->master_headinga.pitch;
            test.base_line      = gnss->master_headinga.base_line_length \
                                   * fabs(cosf(gnss->master_headinga.pitch * (3.1415926f/180.0f)));
            test.rover.sol_n    = gnss->master_headinga.sol_n;
            test.rover.sol_type = gnss->master_headinga.sol_type;
        }
        break;
    case OEM_K501:
    case OEM_K505:
    case OEM_K528:
        if(gnss->working_mode == MODE_WORK_COUPLING || gnss->working_mode == MODE_WORK_SINGLE_2ANTENNA || gnss->working_mode == MODE_WORK_SINGLE_2ANTENNA_INS)
        {
            test.yaw            = gnss->master_gptra.heading;
            test.pitch          = gnss->master_gptra.pitch;
            test.rover.sol_n    = gnss->master_gptra.satNo;
            test.rover.sol_type = gnss->master_gptra.valid;
            test.rover.diff_age = gnss->master_gptra.diff_age;
        }
        break;
    }
    //
    test_print(g_parser_print_buffer,sizeof(g_parser_print_buffer),&test);
    printf("%s",g_parser_print_buffer);
}

unsigned char f_NMEA_CheckNum(char *s)
{
	unsigned sum = 0;
	if(*s == '$')
		s++;
	while(*s != '*' && *s)
	{
		sum ^= *s;
		s++;
	}
	return sum;
}

void f_NMEA_Output(char *s,int len)
{ 
	
	unsigned char  o_Checnum;
  char char_data[200];   
  memset(char_data,0,200); 	
	memcpy(char_data,s,len);
  //len=sprintf(char_data,"$PPNFO,%.3f,%.9f,%.9f,%.3f,%.2f,%.2f,%.2f,%.3f,%d,%d,%d,%d,%.2f,%.2f,%.2f,%d,%x",\
  sensor_data.o_Timer,sensor_data.o_Latitude,sensor_data.o_Longitude,sensor_data.o_Altitude,\
  sensor_data.o_Roll,sensor_data.o_Pitch,sensor_data.o_Head,sensor_data.o_Length,sensor_data.o_Mag_pre,\
  sensor_data.o_Acc_pre,sensor_data.o_Head_pre,sensor_data.o_Position_pre,sensor_data.o_Roll_g,\
  sensor_data.o_Pitch_g,sensor_data.o_Head_g,sensor_data.o_Cal_pro,sensor_data.o_Flag);
  
  o_Checnum=f_NMEA_CheckNum(s);  
  sprintf(char_data+len,"*%02x\r\n",o_Checnum);               
  printf("%s",char_data); 
}

