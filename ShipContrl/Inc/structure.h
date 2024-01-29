// com/structure.h


#ifndef __FILE_COM_STRUCTURE_H__
#define __FILE_COM_STRUCTURE_H__

#include "stm32f10x.h"
// 以下结构体都是1字节对齐。
#pragma pack(push)
#pragma pack(1)

// 量产工具写入的BootInfo。
typedef struct
{
    char boot_flag[32];//boot标记，固定为SouthGnss，通过此字符串判断boot文件是否正确
    char boot_ver[32];//boot版本信息，编译boot文件时产生 		
    char sn[32];//机身号，由PC量产工具写入       		             
    char pn[32];//硬件版本号，由PC量产工具写入 		
    char hid[32];//硬件标识，由PC量产工具写入  		
    char brand[32];//品牌，由PC量产工具写入  	
    char model[32];//型号，由PC量产工具写入 	
    char product_date[32];//生产日期，由PC量产工具写入 		  		
    char build_date[32];//软件编译时间
    char board1sn[32];
    char board2sn[32];
    char des[16];
} boot_info_t;

// 返回版本信息命令的Body。
typedef struct
{
    char boot_ver[32];      //boot版本信息，编译boot文件时产生
    char app_ver[32];       //软件版本，由SOFT_VERSION宏控制。
    char sn[32];            //机身号，由PC量产工具写入
    char pn[32];            //硬件版本号，由PC量产工具写入
    char hid[32];           //硬件标识，由PC量产工具写入
    char brand[32];         //品牌，由PC量产工具写入
    char model[32];         //型号，由PC量产工具写入
    char product_date[32];  //生产日期，由PC量产工具写入
    char board1sn[32];
    char board2sn[32];
} body_version_t;

// 查询注册状态命令的Body。
typedef struct
{
    char     sn[32];
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint8_t  regist_code[20];
} body_register_query_t;

// 设置基站命令的Body。
typedef struct
{
    double latitude;
    double longitude;
    double altitude;
} body_set_base_t;

// 输出命令的Body。
typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    //
    //rover 前天线经纬度
    double rover_latitude;
    double rover_longitude;
    double rover_altitude;
    //
    //master 后天线经纬度
    double master_latitude;
    double master_longitude;
    double master_altitude; 
    //
    float speed;//速度 speed;
    float pitch;//俯仰 pitch : x轴，[-90,90)
    float roll;//横滚  roll  : y轴，[-180,180)
    uint8_t rover_nosv;//前天线可视卫星数
    uint8_t master_nosv;//后天线可视卫星数
    //
    uint8_t rover_valid;//前天线解状态
    uint8_t master_valid;//后天线解状态
} body_output_0x0100_t;


// 输出命令的Body,add by yys 2014.6.24
typedef struct
{

    uint8_t rover_nosv;//前天线可视卫星数
    uint8_t master_nosv;//后天线可视卫星数
    //
    uint8_t rover_valid;//前天线解状态
    uint8_t master_valid;//后天线解状态
    
    uint8_t rover_age;//前天线差分延时
    uint8_t master_age;//后天线差分延时
    
    float rover_hdop;//前天线hdop
    float master_hdop;//后天线hdop
    
} body_output_0x0d03_t;

// 输出命令的Body。
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;             // UTC 小时
    uint8_t  minute;           // UTC 分钟
    uint8_t  second;           // UTC 秒
    uint16_t millisecond;      // UTC 毫秒
    double   master_latitude;  // 后天线纬度
    double   master_longitude; // 后天线经度
    double   master_altitude;  // 后天线高程
    float    base_line_length; // 基线长，米。
    float    speed;            // 速度
    float    heading;          // 航向角
    float    heading_rmc;      // 行驶角
    float    pitch_gnss;       // 主板算的俯仰角
    float    pitch_ins;        // 传感器算的俯仰角
    float    roll_ins;         // 传感器算的横滚角
    uint8_t  master_nosv;      // 后天线可视卫星数
    uint8_t  master_hdop_10;   // 后天线HDOP*10
    uint8_t  master_diff_age;  // 后天线差分延迟
    uint8_t  master_gga_valid; // 后天线定位状态
    uint8_t  master_rmc_valid; // 后天线速度状态
    uint8_t  heading_valid;    // 航向状态
    uint8_t  rover_nosv;       // 前天线可视卫星数
    uint8_t  rover_valid;      // 前天线定位状态
} body_output_0x0200_t;

// 输出命令的Body。
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint16_t ms;
    double   latitude;  // 后天线纬度
    double   longitude; // 后天线经度
    double   altitude;  // 后天线高程
    float    speed;            // 速度
    float    heading;          // 航向角
    float    pitch;            // 俯仰角
    float    roll;             // 横滚角
    uint8_t  valid;     // 后天线定位状态
} body_output_0x0400_t;

// 传感器参数，0x0402和0x0403命令的Body。
typedef struct
{
    uint32_t id;
    float gyr_offset[3];
    float gyr_gain[3];
    float acc_offset[3];
    float acc_gain[3];
} body_sensor_param_t;

// 传感器原始数据。
typedef struct
{
    uint32_t uid;
    int32_t  omg_raw[3];
    int32_t  spf_raw[3];
} body_sensor_raw_t;

// 响应同步时间。
typedef struct
{
    uint16_t ms;
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
} body_sync_time_t;

typedef struct
{
    double latitude;
    double longitude;
    double altitude;
    uint8_t valid;
} body_refstation_0x0d04_t;

// 以上结构体需要紧缩。
#pragma pack(pop)

// 下面的结构体不需要紧缩。

#endif // __FILE_COM_STRUCTURE_H__
