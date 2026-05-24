#include "servo.h"
#include "includes.h"
/************************************
引脚说明：
舵机信号连接在PB0
TIM3_CH3 -- APB2总线，84MHz
************************************/

void Servo_PB0_Init(void)
{
    GPIO_InitTypeDef          GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;

    // 使能TIM8和GPIOC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    // 配置PC6为复用功能
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;        // 引脚6
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;      // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输出速率
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  // 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    
    // 映射PB0到TIM3_CH3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

    // 定时器配置：20ms周期（50Hz）
    TIM_TimeBaseStructure.TIM_Prescaler     = (84-1);       // 84分频，得到1MHz计数时钟
    TIM_TimeBaseStructure.TIM_Period        = (20000-1);    // 1MHz计数，20ms周期（20000us）
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 配置PWM通道3
    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;     // PWM模式1
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse        = 1500;                // 
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;  // 高电平有效
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    
    // 使能预装载寄存器
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    // 使能定时器和主输出
    TIM_Cmd(TIM3, ENABLE);
   
}
/**
  * @brief  设置舵机角度
  * @param  angle: 舵机角度 (0-180度)
  * @retval 无
  */
//void Set_Servo_Angle(float angle)
//{
//    // 角度限幅 (0-180度范围)
//    if(angle < 0) angle = 0;
//    if(angle > 180) angle = 180;
//    
//    // 将角度转换为脉冲宽度 (500-2500μs)
//    uint32_t pulse_width = 500 + (uint32_t)(angle * (2000.0f / 180.0f));
//    
//    // 设置捕获比较寄存器值 (TIM3通道3)
//    TIM_SetCompare3(TIM3, pulse_width);
//}

void Set_Servo_Angle(float logic_angle)
{
    float physical_angle;
    uint32_t pulse_width;
    
    // 逻辑角度限幅 (-90°~90°)
    if (logic_angle < -90) logic_angle = -90;
    if (logic_angle > 90) logic_angle = 90;
    
    // 逻辑角度转物理角度：physical = logic + 90
    physical_angle = logic_angle + 90;
    
    // 物理角度限幅 (0°~180°，预留5°安全余量)
    if (physical_angle < 5) physical_angle = 5;
    if (physical_angle > 175) physical_angle = 175;
    
    // 将物理角度转换为脉冲宽度 (500-2500μs，对应0°-180°)
    pulse_width = 500 + (uint32_t)(physical_angle * (2000.0f / 180.0f));
    
    // 设置捕获比较寄存器值 (TIM3通道3)
    TIM_SetCompare3(TIM3, pulse_width);
}

/**
  * @brief  设置舵机脉冲宽度 (微秒)
  * @param  pulse_us: 脉冲宽度 (500-2500μs)
  * @retval 无
  */
void Set_Servo_Pulse(uint16_t pulse_us)
{
    // 脉冲宽度限幅 (500-2500μs)
    if(pulse_us < 500) pulse_us = 500;
    if(pulse_us > 2500) pulse_us = 2500;
    
    // 设置捕获比较寄存器值
    TIM_SetCompare3(TIM3, pulse_us);
}

