#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f4xx.h"
#include "includes.h"


void Delay_Init(void);
//void delay_us(int nus);
void delay_ms(int nms);
void delay_s(int ns);

#endif
