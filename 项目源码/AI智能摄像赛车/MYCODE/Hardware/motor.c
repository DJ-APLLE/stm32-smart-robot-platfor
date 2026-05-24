#include "includes.h"
/*
麦克纳姆轮 A	PWM 	PE5(TIM9_CH1)、 PE6(TIM9_CH2)
麦克纳姆轮 B	PWM	    PC6(TIM8_CH1)、 PC7(TIM8_CH2)
麦克纳姆轮 C	PWM 	PC8(TIM8_CH3)、 PC9(TIM8_CH4)
麦克纳姆轮 D	PWM 	PB6(TIM4_CH1)、 PB7(TIM4_CH2)
*/
/*
 * 麦克纳姆轮A初始化：使用TIM9通道1和通道2，对应引脚PE5和PE6
 * TIM9挂载在APB2总线，时钟频率168MHz
 */
void WHEEL_A_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  // GPIOE时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);   // TIM9时钟
    
    // 2. 配置GPIO引脚为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // 3. 配置引脚复用映射
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);  // PE5 -> TIM9_CH1
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_TIM9);  // PE6 -> TIM9_CH2
    
    // 4. 配置TIM9时基 (168MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 167;       // 168分频，得到1MHz时钟
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;  // PWM周期1ms（频率1kHz）
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

/*
 * 麦克纳姆轮B初始化：使用TIM8通道1和通道2，对应引脚PC6和PC7
 * TIM8挂载在APB2总线，时钟频率168MHz
 */
void WHEEL_B_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;  // 刹车和死区配置结构体
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);   // TIM8时钟
    
    // 2. 配置GPIO引脚为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 3. 配置引脚复用映射
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);  // PC6 -> TIM8_CH1
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);  // PC7 -> TIM8_CH2
    
    // 4. 配置TIM8时基 (168MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 167;       // 168分频，得到1MHz时钟
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;  // PWM周期1ms（频率1kHz）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    
    // 5. 配置刹车和死区时间（高级定时器特有）
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;       // 运行模式死区使能
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;       // 空闲模式死区使能
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;          // 锁定级别
    TIM_BDTRInitStructure.TIM_DeadTime = 5;                           // 死区时间（5个时钟周期）
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;              // 禁用刹车功能
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High; // 刹车输入极性
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
    
    // 6. 配置PWM模式（与TIM9类似）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 7. 初始化通道1和通道2
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);  // 通道1 (PC6)
    TIM_OC2Init(TIM8, &TIM_OCInitStructure);  // 通道2 (PC7)
    
    // 8. 使能高级定时器主输出（高级定时器特有）
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    
    // 9. 使能预装载和自动重装载
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
}


/*
 * 麦克纳姆轮C初始化：使用TIM8通道3和通道4，对应引脚PC8和PC9
 * TIM8挂载在APB2总线，时钟频率168MHz
 */
void WHEEL_C_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;  // 刹车和死区配置结构体
    
    // 1. 使能时钟（同轮B）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    
    // 2. 配置GPIO引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 3. 配置引脚复用映射
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);  // PC8 -> TIM8_CH3
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);  // PC9 -> TIM8_CH4
    
    // 4. 配置TIM8时基（同轮B）
    TIM_TimeBaseStructure.TIM_Prescaler = 167;
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    
    // 5. 配置刹车和死区时间（高级定时器特有）
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = 5;                           // 死区时间（5个时钟周期）
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
    
    // 6. 配置PWM模式（同轮B）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 7. 初始化通道3和通道4
    TIM_OC3Init(TIM8, &TIM_OCInitStructure);  // 通道3 (PC8)
    TIM_OC4Init(TIM8, &TIM_OCInitStructure);  // 通道4 (PC9)
    
    // 8. 使能高级定时器主输出（高级定时器特有）
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    
    // 9. 使能预装载和自动重装载（同轮B）
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM8, ENABLE);
    TIM_Cmd(TIM8, ENABLE);
}
/*
 * 麦克纳姆轮D初始化：使用TIM4通道1和通道2，对应引脚PB6和PB7
 * TIM4挂载在APB1总线，时钟频率84MHz（假设APB1为84MHz）
 */
void WHEEL_D_PWM_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   // TIM4时钟
    
    // 2. 配置GPIO引脚为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 配置引脚复用映射
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);  // PB6 -> TIM4_CH1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);  // PB7 -> TIM4_CH2
    
    // 4. 配置TIM4时基 (84MHz时钟)
    TIM_TimeBaseStructure.TIM_Prescaler = 83;        // 84分频，得到1MHz时钟
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;  // PWM周期1ms（频率1kHz）
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
    
    // 7. 使能预装载和自动重装载
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}




void ALL_WHEEL_PWM_INIT(void)
{
    WHEEL_A_PWM_INIT();
    WHEEL_B_PWM_INIT();
    WHEEL_C_PWM_INIT();
    WHEEL_D_PWM_INIT();
}





































