#include "includes.h"

void Iwdg_Init(void);


void ALL_HARDWARE_INIT()
{
	
	
	ALL_WHEEL_PWM_INIT();    // 轮子用的全部引脚和TIM初始化总函数
	Usart1_Init(115200);        // 语音接口
	Bluetooth_Usart3_Init(9600); // 蓝牙引脚初始化
	DWT_Init();					 //	DWT外设初始化
	Sr04_Init();				 // 超声波模块初始化
	Servo_PB0_Init();			 // 舵机初始化
	Iwdg_Init();
	Led_Init();
	RNG_Init();
	I2C_Configuration();
	OLED_Init();
    srand(Get_Random_Seed());
}



// 喂狗初始化
void Iwdg_Init(void)
{
	//取消寄存器写保护：
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	//设置独立看门狗的预分频系数，确定时钟:125HZ
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	//设置看门狗重装载值为375，确定溢出时间为3秒
    //计算：375 × 256 / 32000 = 3秒
    IWDG_SetReload(375);
	
	//使能看门狗
	IWDG_Enable();
	//应用程序喂狗:
	IWDG_ReloadCounter();
}
