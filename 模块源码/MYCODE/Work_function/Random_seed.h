#ifndef __RANDOM_SEED_H
#define __RANDOM_SEED_H

#include "includes.h"

#include "stm32f4xx_rng.h"


// 获取随机种子
uint32_t Get_Random_Seed(void);
// 初始化硬件随机数发生器
void RNG_Init(void);

#endif