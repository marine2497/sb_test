
#include "time.h"
#include "stm32f10x.h"
extern uint32_t SystemCoreClock ;
static struct 
{
    volatile uint32_t msPeriod;    // 整周期的时间，ms�?    
    uint32_t ticksPerUs;  // 每us等于的tick数�?    
    uint32_t ticksPerMs;  // 每ms等于的tick数�?    
    uint32_t msPerPeriod; // 每周期的ms数�?
}time;

// 初始化时间�?
void time_init(void)
{
    uint32_t systick_freq = SystemCoreClock  ;
    time.msPeriod = 0;
    time.ticksPerUs = systick_freq / 1e6;
    time.ticksPerMs = systick_freq / 1e3;
    time.msPerPeriod = 1;
    //SystemCoreClock
    SysTick_Config(systick_freq/(1000/time.msPerPeriod));//72000000
   // SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);    
}

// SysTick中断�?
void SysTick_Handler(void)
{        
    time.msPeriod += time.msPerPeriod;
}

// 获取当前时间，us�?
uint64_t time_nowUs(void)
{
    __set_PRIMASK(1);
    uint64_t v = time.msPeriod * (uint64_t)1000 + (SysTick->LOAD - SysTick->VAL) / time.ticksPerUs;
    __set_PRIMASK(0);
    return v;
}

// 获取当前时间，ms�?
uint32_t time_nowMs(void)
{
    __set_PRIMASK(1);
    uint32_t v = time.msPeriod + (SysTick->LOAD - SysTick->VAL) / time.ticksPerMs;
    __set_PRIMASK(0);
    return v;
}

// 延时delay us，delay>=4时才准确�?
void time_waitUs(uint32_t delay)
{
    uint64_t target = time_nowUs() + delay - 2;
    while(time_nowUs() <= target)
        ; // 空循环�?
}

// 延时delay ms�?
void time_waitMs(uint32_t delay)
{
  time_waitUs(delay * 1000);
}
void Delay(uint32_t delay)
{
  uint32_t i;
  for(i=0;i<delay;i++)
    for(i=0;i<10000;i++)
      for(i=0;i<10000;i++);
}

uint16_t CCR1_Val = 1500;//
uint16_t CCR2_Val = 1500;//
uint16_t CCR3_Val = 1500;//
uint16_t CCR4_Val = 1500;//



void time_pwm_init(void)//pwm
{

   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   TIM_OCInitTypeDef  TIM_OCInitStructure;
   uint16_t PrescalerValue = 0;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

/* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TIM3CLK frequency is set to SystemCoreClock (Hz), to get TIM3 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TIM3 is running at 36 KHz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
                                                  = 1 MHz / 20000 = 50Hz
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 19999;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM4, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM4, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

  TIM_OC4Init(TIM4, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM4, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}
void Time_pwm_setCompare(uint16_t val1,uint16_t val2,uint16_t val3,uint16_t val4)
{
    TIM_SetCompare1(TIM4,val1);
    TIM_SetCompare2(TIM4,val2);
    TIM_SetCompare3(TIM4,val3);
    TIM_SetCompare4(TIM4,val4);
}
void Time_pwm_setPrescaler(uint16_t val1,uint16_t val2,uint16_t val3,uint16_t val4)
{
    TIM_SetIC1Prescaler(TIM4,val1);
    TIM_SetIC2Prescaler(TIM4,val2);
    TIM_SetIC3Prescaler(TIM4,val3);
    TIM_SetIC4Prescaler(TIM4,val4);
}