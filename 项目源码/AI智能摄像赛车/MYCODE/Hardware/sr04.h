#ifndef __SR04_H
#define __SR04_H


#include "includes.h"

void Sr04_Init(void);
void DWT_Init(void);
void delay_us(uint32_t us);// Î¢Ãë¼¶ÑÓÊ±º¯Êý
void delay_ms(uint32_t ms);
int Get_Sr04_Value(void);

#endif