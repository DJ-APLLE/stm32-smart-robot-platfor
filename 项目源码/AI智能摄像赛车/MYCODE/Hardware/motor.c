/**
  ******************************************************************************
  * @file           : motor.c
  * @brief          : 电机驱动实现文件
  * @details        : 包含四个车轮的PWM控制初始化和配置
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 电机连接说明:
  * - 车轮A: PE5(TIM9_CH1), PE6(TIM9_CH2)
  * - 车轮B: PC6(TIM8_CH1), PC7(TIM8_CH2)
  * - 车轮C: PC8(TIM8_CH3), PC9(TIM8_CH4)
  * - 车轮D: PB6(TIM4_CH1), PB7(TIM4_CH2)
  *
  * TIM定时器时钟频率:
  * - TIM9 (APB2): 168MHz
  * - TIM8 (APB2): 168MHz  
  * - TIM4 (APB1): 84MHz
  *
  ******************************************************************************
  */

#include "includes.h"

/* ============================================ */
/*              电机硬件连接定义                  */
/* ============================================ */

/**
  * @brief 电机车轮与定时器通道对应关系
  * 
  * | 车轮 | TIM定时器 | 通道1(PWM+) | 通道2(PWM-) | GPIO端口 |
  * |------|-----------|-------------|-------------|----------|
  * | A    | TIM9      | CH1         | CH2         | PE5/PE6  |
  * | B    | TIM8      | CH1         | CH2         | PC6/PC7  |
  * | C    | TIM8      | CH3         | CH4         | PC8/PC9  |
  * | D    | TIM4      | CH1         | CH2         | PB6/PB7  |
  */

/* ============================================ */
/*              车轮A PWM初始化                  */
/* ============================================ */

/**
  * @brief  车轮A PWM初始化
  * @details 使用TIM9通道1和通道2，对应PE5和PE6引脚
  *          TIM9挂载在APB2总线，时钟频率168MHz
  * 
  * @note   配置步骤:
  *         1. 使能GPIOE和TIM9时钟
  *         2. 配置GPIO为复用功能模式
  *         3. 设置GPIO复用映射到TIM9
  *         4. 配置TIM9定时器参数
  *         5. 配置PWM输出模式
  *         6. 使能定时器和预装载
  */
void WHEEL_A_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  // GPIOE时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);   // TIM9时钟
    
    // 2. 配置GPIO为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // 3. 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);  // PE5 -> TIM9_CH1
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_TIM9);  // PE6 -> TIM9_CH2
    
    // 4. 配置TIM9定时器 (168MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 167;       // 168分频得到1MHz
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;  // PWM周期1ms(1kHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    
    // 5. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;               // 初始占空比0%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 6. 初始化通道1和通道2
    TIM_OC1Init(TIM9, &TIM_OCInitStructure);  // 通道1 (PE5)
    TIM_OC2Init(TIM9, &TIM_OCInitStructure);  // 通道2 (PE6)
    
    // 7. 使能预装载
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);
    
    // 8. 使能自动重装载预装载
    TIM_ARRPreloadConfig(TIM9, ENABLE);
    
    // 9. 使能定时器
    TIM_Cmd(TIM9, ENABLE);
}

/* ============================================ */
/*              车轮B PWM初始化                  */
/* ============================================ */

/**
  * @brief  车轮B PWM初始化
  * @details 使用TIM8通道1和通道2，对应PC6和PC7引脚
  *          TIM8挂载在APB2总线，时钟频率168MHz
  *          TIM8是高级定时器，需要配置刹车和死区
  */
void WHEEL_B_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;  // 刹车和死区配置
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);   // TIM8时钟
    
    // 2. 配置GPIO为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 3. 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);  // PC6 -> TIM8_CH1
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);  // PC7 -> TIM8_CH2
    
    // 4. 配置TIM8定时器 (168MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 167;       // 168分频得到1MHz
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;  // PWM周期1ms(1kHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    
    // 5. 配置刹车和死区（高级定时器特有）
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = 5;                           // 死区时间5个时钟周期
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
    
    // 6. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 7. 初始化通道1和通道2
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);  // 通道1 (PC6)
    TIM_OC2Init(TIM8, &TIM_OCInitStructure);  // 通道2 (PC7)
    
    // 8. 使能高级定时器PWM输出
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    
    // 9. 使能预装载
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
}

/* ============================================ */
/*              车轮C PWM初始化                  */
/* ============================================ */

/**
  * @brief  车轮C PWM初始化
  * @details 使用TIM8通道3和通道4，对应PC8和PC9引脚
  *          与车轮B共享TIM8定时器
  */
void WHEEL_C_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    
    // 1. 使能时钟（与车轮B相同，无需重复使能）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    
    // 2. 配置GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 3. 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);  // PC8 -> TIM8_CH3
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);  // PC9 -> TIM8_CH4
    
    // 4. 配置TIM8定时器（与车轮B相同）
    TIM_TimeBaseStructure.TIM_Prescaler = 167;
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    
    // 5. 配置刹车和死区
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = 5;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
    
    // 6. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 7. 初始化通道3和通道4
    TIM_OC3Init(TIM8, &TIM_OCInitStructure);  // 通道3 (PC8)
    TIM_OC4Init(TIM8, &TIM_OCInitStructure);  // 通道4 (PC9)
    
    // 8. 使能高级定时器PWM输出
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    
    // 9. 使能预装载
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
}

/* ============================================ */
/*              车轮D PWM初始化                  */
/* ============================================ */

/**
  * @brief  车轮D PWM初始化
  * @details 使用TIM4通道1和通道2，对应PB6和PB7引脚
  *          TIM4挂载在APB1总线，时钟频率84MHz
  */
void WHEEL_D_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   // TIM4时钟
    
    // 2. 配置GPIO为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);  // PB6 -> TIM4_CH1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);  // PB7 -> TIM4_CH2
    
    // 4. 配置TIM4定时器 (84MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 83;        // 84分频得到1MHz
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;  // PWM周期1ms(1kHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 5. 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 6. 初始化通道1和通道2
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);  // 通道1 (PB6)
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);  // 通道2 (PB7)
    
    // 7. 使能预装载
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

/* ============================================ */
/*              初始化所有车轮                    */
/* ============================================ */

/**
  * @brief  初始化所有车轮的PWM输出
  * @details 依次调用四个车轮的初始化函数
  */
void ALL_WHEEL_PWM_INIT(void)
{
    WHEEL_A_PWM_INIT();
    WHEEL_B_PWM_INIT();
    WHEEL_C_PWM_INIT();
    WHEEL_D_PWM_INIT();
}
