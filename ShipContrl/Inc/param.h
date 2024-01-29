// param/param.h
// 2013-10-29 14:48:10
// yys@system-intergration-department

#ifndef __FILE_APP_PARAM_H__
#define __FILE_APP_PARAM_H__

#include <stdint.h>

//

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    struct
    {
        uint32_t working_mode; // 工作模式，主板算航向，还是自己算航向。
        uint8_t baud;         // 外部输入差分的波特率
        uint8_t freq;
        uint16_t reserve_1[1]; //原预留uin32_t的字节，上面baud与freq使用了2字节，改为uint16_t yys 13/12/31
        int64_t output_type;  // 输出类型，按位标识。
        double base_latitude; // 基站纬度。
        double base_longitude;// 基站经度。
        double base_altitude; // 基站海拔。
        //
        uint32_t reserve_2[6];
    } gnss; // 刚好8字节对齐。
    //
    struct
    {
        float gyr_offset[3];
        float gyr_gain[3];
        float acc_offset[3];
        float acc_gain[3];
        //
        uint32_t reserve[4];
    } sensor; // 刚好8字节对齐。
    //
    struct
    {
        uint8_t register_code[20]; // 注册码。
        //
        uint32_t reserve[3];
    } register_; // 刚好8字节对齐。
    //
    struct
    {
        float roll_base;  // 横滚角零点。
        float pitch_base; // 俯仰角零点。
        //
        uint32_t reserve[6];
    } ins; // 刚好8字节对齐。
} param_t;
#pragma pack(pop)

int32_t   param_init(void);           // 初始化参数模块。
param_t * param_get(void);            // 获取RAM中的参数，可以修改。
const param_t * param_on_flash(void); // 获取Flash中的参数，不可以修改。
int32_t   param_write_to_flash(void); // 把RAM中的参数写到Flash里。
void      param_clear_flash(void);    // 清除Flash中的参数。
void      param_print(void);          // 打印参数。
//
int32_t   param_auto_save(void);      // 自动保存，相当于param_write_to_flash，但可以屏蔽。
void      param_ignore_auto_save(int32_t yes); // 如果yes!=0就允许自动保存，否则忽略自动保存。

// 使用方法
//
// 1.读参数
//      //上电后初始化。
//      param_init();
//         ...
//      //使用ram_param控制程序行为。
//      param_t * ram_param = param_get();
//         ...
//
// 2.写参数
//      //上电后初始化。
//      param_init();
//         ...
//      //修改ram_param里的值。
//      param_t * ram_param = param_get();
//         ...
//      //把RAM里的值写入到Flash里。
//      param_write_to_flash();

#endif // __FILE_APP_PARAM_H__
