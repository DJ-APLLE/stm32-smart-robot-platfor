#include "sr04.h"


/************************************
引脚说明

PA2做为普通输出
PA3做为普通输入
PA2 – TRIG（触发信号）  
PA3 – ECHO（回响信号）
************************************/

// 初始化DWT计数器
void DWT_Init(void) {
    // 使能DWT外设
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // 使能CYCCNT寄存器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    // 清零计数器
    DWT->CYCCNT = 0;
}

// 微秒级延时函数
void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    
    while ((DWT->CYCCNT - start) < cycles);
}
// 毫秒级延时函数
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        delay_us(1000);
    }
}
void Sr04_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
    
    // 初始化DWT计数器
    DWT_Init();
    
    // 1、使能定时器时钟（修改为TIM5）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);    
    // 时钟初始化
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);    
    
    // TRIG
    GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_2; 
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_25MHz;
    GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ECHO
    GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_3;     
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;    
    GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);    
    
    // 配置TIM5（32位定时器）
    TIM_TimeBaseInitStruct.TIM_Prescaler    = 84-1;         // 84分频 84MHZ/84 = 1MHZ 计1个数用1us
    TIM_TimeBaseInitStruct.TIM_Period       = 0xFFFFFFFF;   // 32位最大值，约42.9秒
    TIM_TimeBaseInitStruct.TIM_CounterMode  = TIM_CounterMode_Up;    // 向上计数
    TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1;            // 分频因子
    // 2、初始化定时器，配置ARR,PSC
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
            
    // 不使能定时器
    TIM_Cmd(TIM5, DISABLE);    
}

