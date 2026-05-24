#include "led.h"


/************************************
引脚说明：

LED0连接在PF9
PF9输出低电平（0），灯亮；
PF9输出高电平（1），灯灭；

LED0连接在PF9
LED1连接在PF10
LED2连接在PE13
LED3连接在PE14


*************************************/

void Led_Init(void)
{
	//结构体变量
	GPIO_InitTypeDef	GPIO_InitStruct;

	//使能GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	/*
	//RCC->AHB1ENR |= RCC_AHB1Periph_GPIOF;
	
	//RCC->AHB1ENR |= (0x01<<5);    //第5位置1
	//打开GPIOF组时钟
	//RCC_AHB1ENR |= (0x01<<5);    //第5位置1
	
	//RCC_AHB1ENR |= RCC_AHB1Periph_GPIOF;
	*/
	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9|GPIO_Pin_10;	//引脚
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;//模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP; //推挽
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_25MHz; //速度
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//上拉
	
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_13|GPIO_Pin_14;	//引脚
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;//模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP; //推挽
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_25MHz; //速度
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//上拉
	
	
	
	GPIO_Init(GPIOE, &GPIO_InitStruct);


	
	LED0(1);
	LED1(1);
	LED2(1);
	LED3(1);
	

}