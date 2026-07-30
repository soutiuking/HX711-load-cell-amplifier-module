/**
 *******************************************************************************
 * @file    delay.c
 * @brief   延时函数 - STM32 HAL版本
 * @version v1.0
 *
 * @note    微秒延时使用SysTick实现，毫秒延时调用HAL_Delay
 *******************************************************************************
 */
#include "delay.h"

// 微秒延时（HAL库SysTick实现）
void Delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t reload = SysTick->LOAD;
    ticks = nus * (SystemCoreClock / 1000000);
    SysTick->LOAD = ticks;
    SysTick->VAL = 0;
    SysTick->CTRL |= 1;
    while((SysTick->CTRL & 0x10) == 0);
    SysTick->CTRL = 0;
    SysTick->LOAD = reload;
}

// 毫秒延时（调用HAL库）
void Delay_ms(uint32_t nms)
{
    HAL_Delay(nms);
}
