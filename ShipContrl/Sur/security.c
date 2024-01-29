// 安全模块。
// 负责检查DES码、注册码和操作看门狗。

#include "security.h"
#include "time.h"
#include "stm32f10x.h"


void security_check_event(void)
{
   
    //
    // 喂狗。
//    TIME_EXECUTE_PERIODICALLY_BEGIN(1000)
//    {
//        IWDG_ReloadCounter();
//    } TIME_EXECUTE_PERIODICALLY_END();
}

//
// 配置看门狗。
void security_setup_dog(float timeout_s)
{
    if(timeout_s < 0.01f)
        timeout_s = 0.01f;
    if(timeout_s > 32)
        timeout_s = 32;
    //
//    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//    IWDG_SetPrescaler(IWDG_Prescaler_256);
//    IWDG_SetReload((uint16_t)(timeout_s * 32000 / 256));
//    IWDG_Enable();
}
