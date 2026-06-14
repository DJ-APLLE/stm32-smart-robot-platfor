/**
  ******************************************************************************
  * @file           : ALL_Hardware_init.c
  * @brief          : 所有硬件模块初始化
  * @details        : 包含系统所有硬件模块的初始化函数
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 此文件负责初始化系统中所有硬件模块
  * 初始化顺序按照硬件依赖关系进行
  *
  ******************************************************************************
  */

#include "includes.h"

/**
  * @brief  独立看门狗初始化函数声明
  * @note   看门狗用于监控系统运行状态，防止系统死机
  */
void Iwdg_Init(void);

/**
  * @brief  初始化所有硬件模块
  * 
  * @details 此函数按照以下顺序初始化所有硬件模块:
  * 
  *          1. ALL_WHEEL_PWM_INIT()   - 初始化所有电机PWM定时器
  *          2. Usart1_Init(115200)     - 初始化调试串口(115200波特率)
  *          3. Bluetooth_Usart3_Init(9600) - 初始化蓝牙串口(9600波特率)
  *          4. DWT_Init()              - 初始化DWT计时器(用于精确延时)
  *          5. Sr04_Init()             - 初始化超声波测距模块
  *          6. Servo_PB0_Init()        - 初始化舵机控制
  *          7. Iwdg_Init()             - 初始化独立看门狗
  *          8. Led_Init()              - 初始化LED指示灯
  *          9. RNG_Init()              - 初始化随机数生成器
  *          10. I2C_Configuration()    - 初始化I2C总线
  *          11. OLED_Init()            - 初始化OLED显示屏
  *          12. srand(Get_Random_Seed()) - 设置随机数种子
  * 
  * @note   初始化顺序非常重要，某些模块依赖其他模块已初始化
  */
void ALL_HARDWARE_INIT()
{
	// 1. 初始化电机PWM输出定时器
	ALL_WHEEL_PWM_INIT();    // 配置所有车轮的PWM信号（TIM定时器初始化及使能）

	// 2. 初始化调试串口
	Usart1_Init(115200);        // 调试串口初始化（PA9-TX, PA10-RX）

	// 3. 初始化蓝牙通信串口
	Bluetooth_Usart3_Init(9600); // 蓝牙串口初始化（PB10-TX, PB11-RX）

	// 4. 初始化DWT计时器
	DWT_Init();					 // DWT计时器初始化（用于高精度延时）

	// 5. 初始化超声波模块
	Sr04_Init();				 // 超声波测距模块初始化

	// 6. 初始化舵机
	Servo_PB0_Init();			 // 舵机初始化（PB0-TIM3_CH3）

	// 7. 初始化独立看门狗
	Iwdg_Init();                 // 独立看门狗初始化（防止系统死机）

	// 8. 初始化LED指示灯
	Led_Init();                  // LED指示灯初始化

	// 9. 初始化随机数生成器
	RNG_Init();                  // 硬件随机数生成器初始化

	// 10. 初始化I2C总线
	I2C_Configuration();         // I2C总线配置（用于OLED和EEPROM）

	// 11. 初始化OLED显示屏
	OLED_Init();                 // OLED显示屏初始化

	// 12. 设置随机数种子
    srand(Get_Random_Seed());    // 使用硬件RNG生成随机种子
}

/**
  * @brief  独立看门狗(IWDG)初始化
  * 
  * @details 独立看门狗使用内部32kHz LSI时钟，配置为3秒超时
  *          如果系统在3秒内没有喂狗，看门狗会复位系统
  * 
  * @note   计算公式: 超时时间 = ReloadValue * Prescaler / LSI_Frequency
  *         本例: 375 * 256 / 32000 = 3秒
  */
void Iwdg_Init(void)
{
	// 1. 取消写保护，允许访问看门狗寄存器
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	// 2. 设置预分频器为256
	//    LSI时钟为32kHz，分频后为32000/256 = 125Hz
	IWDG_SetPrescaler(IWDG_Prescaler_256);

	// 3. 设置重装载值为375
	//    超时时间 = 375 / 125 = 3秒
    IWDG_SetReload(375);
	
	// 4. 使能看门狗
	IWDG_Enable();

	// 5. 立即喂狗（首次喂狗）
	IWDG_ReloadCounter();
}
