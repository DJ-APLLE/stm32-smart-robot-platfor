#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"


#define RCC_AHB1ENR  	*((volatile unsigned int *)(0x40023800+0x30)) //值强制转换为地址，通过解引用，访问地址空间


#define  LED0(x)  x?GPIO_SetBits(GPIOF, GPIO_Pin_9):GPIO_ResetBits(GPIOF, GPIO_Pin_9)
#define  LED1(x)  x?GPIO_SetBits(GPIOF, GPIO_Pin_10):GPIO_ResetBits(GPIOF, GPIO_Pin_10)
#define  LED2(x)  x?GPIO_SetBits(GPIOE, GPIO_Pin_13):GPIO_ResetBits(GPIOE, GPIO_Pin_13)
#define  LED3(x)  x?GPIO_SetBits(GPIOE, GPIO_Pin_14):GPIO_ResetBits(GPIOE, GPIO_Pin_14)


//#define GPIOF_MODER	  	*((volatile unsigned int *)(0x40021400+0x00))
//#define GPIOF_OTYPER  	*((volatile unsigned int *)(0x40021400+0x04))
//#define GPIOF_OSPEEDR	*((volatile unsigned int *)(0x40021400+0x08))
//#define GPIOF_PUPDR    	*((volatile unsigned int *)(0x40021400+0x0C))
//#define GPIOF_ODR       *((volatile unsigned int *)(0x40021400+0x14))

void Led_Init(void);

#endif
