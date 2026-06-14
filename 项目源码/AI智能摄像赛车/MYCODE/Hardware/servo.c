/**
  ******************************************************************************
  * @file           : servo.c
  * @brief          : 舵机驱动实现文件
  * @details        : 包含舵机角度控制的初始化和角度设置函数
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 舵机连接说明:
  * - 使用TIM12通道1，对应PB14引脚
  * - TIM12挂载在APB1总线，时钟频率84MHz
  * 
  * 舵机控制参数:
  * - PWM频率: 50Hz (周期20ms)
  * - 角度范围: 0度 ~ 180度
  * - 脉冲宽度: 0.5ms ~ 2.5ms
  * - 对应关系: 
  *   - 0度: 0.5ms脉冲
  *   - 90度: 1.5ms脉冲
  *   - 180度: 2.5ms脉冲
  *
  ******************************************************************************
  */

#include "includes.h"

/* ============================================ */
/*              舵机角度参数定义                  */
/* ============================================ */

/**
  * @brief 舵机PWM控制参数
  * 
  * 舵机控制原理:
  * - PWM周期: 20ms (50Hz)
  * - 占空比范围: 2.5% ~ 12.5%
  * - 对应角度: 0度 ~ 180度
  * 
  * 计算公式:
  *   TIM_Period = (SystemCoreClock / Prescaler) / Frequency - 1
  *   TIM_Pulse = TIM_Period * DutyCycle
  * 
  * 对于TIM12 (84MHz):
  *   Prescaler = 8399, TIM_Period = 19999
  *   时钟频率 = 84MHz / (8399+1) = 10kHz
  *   PWM周期 = (19999+1) / 10kHz = 20ms
  */

/* ============================================ */
/*              舵机初始化函数                    */
/* ============================================ */

/**
  * @brief  舵机PWM初始化
  * @details 使用TIM12通道1，对应PB14引脚
  *          配置PWM频率为50Hz，用于控制标准舵机
  * 
  * @note   配置步骤:
  *         1. 使能GPIOB和TIM12时钟
  *         2. 配置GPIO为复用功能模式
  *         3. 设置GPIO复用映射到TIM12
  *         4. 配置TIM12定时器参数 (20ms周期)
  *         5. 配置PWM输出模式
  *         6. 使能定时器和预装载
  */
void Servo_PB0_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);  // TIM12时钟
    
    // 2. 配置GPIO为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);  // PB14 -> TIM12_CH1
    
    // 4. 配置TIM12定时器 (84MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 8399;       // 8400分频得到10kHz
    TIM_TimeBaseStructure.TIM_Period = 19999;        // PWM周期20ms(50Hz)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
    
    // 5. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1500;            // 初始位置1.5ms脉冲(90度)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 6. 初始化通道1
    TIM_OC1Init(TIM12, &TIM_OCInitStructure);
    
    // 7. 使能预装载
    TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM12, ENABLE);
    
    // 8. 使能定时器
    TIM_Cmd(TIM12, ENABLE);
}

/* ============================================ */
/*              舵机角度设置函数                  */
/* ============================================ */

/**
  * @brief  设置舵机角度（物理角度）
  * @param  angle: 物理角度值 (0 ~ 180度)
  * 
  * @details 将物理角度转换为对应的PWM脉冲宽度
  *          脉冲宽度范围: 0.5ms ~ 2.5ms
  *          计算公式: pulse = 500 + angle * (2000 / 180)
  */
void Set_Servo_Angle_Physical(uint16_t angle)
{
    uint16_t pulse;
    
    // 角度范围限制
    if(angle > 180)
        angle = 180;
    
    // 计算脉冲宽度 (0.5ms = 500us, 2.5ms = 2500us)
    // 每度对应: (2500 - 500) / 180 = 11.11us/度
    pulse = 500 + (angle * 2000 / 180);
    
    // 设置PWM脉冲值
    TIM_SetCompare1(TIM12, pulse);
}

/**
  * @brief  设置舵机角度（逻辑角度）
  * @param  angle: 逻辑角度值 (-90 ~ +90度)
  * 
  * @details 将逻辑角度转换为物理角度后设置舵机
  *          逻辑角度0对应物理角度90度（中位）
  *          转换公式: physical = logical + 90
  * 
  * @note   逻辑角度便于控制扫描方向：
  *         - -90度: 最左边
  *         - 0度: 中间位置
  *         - +90度: 最右边
  */
void Set_Servo_Angle(float angle)
{
    uint16_t physical_angle;
    
    // 逻辑角度范围限制
    if(angle < -90.0f)
        angle = -90.0f;
    if(angle > 90.0f)
        angle = 90.0f;
    
    // 转换为物理角度
    physical_angle = (uint16_t)(angle + 90.0f);
    
    // 设置物理角度
    Set_Servo_Angle_Physical(physical_angle);
}
