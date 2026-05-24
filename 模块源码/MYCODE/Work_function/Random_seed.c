#include "Random_seed.h"

// 初始化硬件随机数发生器
void RNG_Init(void) {
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
    
    // 等待RNG就绪
    while(!RNG_GetFlagStatus(RNG_FLAG_DRDY));
}

// 获取随机种子
uint32_t Get_Random_Seed(void) {
    // 等待数据就绪
    while(!RNG_GetFlagStatus(RNG_FLAG_DRDY));
    return RNG_GetRandomNumber();
}

