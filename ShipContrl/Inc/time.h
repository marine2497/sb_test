// time.h
// Xuan Yong-jun@System-int
// 2012-11-3 12:01:00

#ifndef __FILE_TIME_H__
#define __FILE_TIME_H__

#include <stdint.h>
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"


void     time_init(void);
uint64_t time_nowUs(void);
uint32_t time_nowMs(void);
void     time_waitUs(uint32_t delay);
void     time_waitMs(uint32_t delay);
void     time_pwm_init(void);
void Time_pwm_setCompare(uint16_t val1,uint16_t val2,uint16_t val3,uint16_t val4);
void Time_pwm_setPrescaler(uint16_t val1,uint16_t val2,uint16_t val3,uint16_t val4);
#define TIME_EXECUTE_PERIODICALLY_BEGIN(time_ms) \
{\
    static uint32_t target = 0;\
    if(target <= time_nowMs())\
    {\
        target = time_nowMs() + time_ms;
#define TIME_EXECUTE_PERIODICALLY_END() \
    }\
}

#endif // __FILE_TIME_H__
