#include "sr04.h"

int Get_Sr04_Value(void)
{
    u32 timeout = 23200;  // 超时时间（us）
    u32 start_time = 0, end_time = 0;
    
    // 触发信号
    PAout(2) = 0;
    delay_us(8);
    PAout(2) = 1;
    delay_us(20); // 至少10us
    PAout(2) = 0;
    
    // 设置定时器的CNT为0（修改为TIM5）
    TIM5->CNT = 0;
	// 使能定时器开始计数（修改为TIM5）
    TIM_Cmd(TIM5, ENABLE);    
    
    while(PAin(3) == 0) {
        if(TIM5->CNT - start_time > timeout) return 1000;  // 超时
    }
	// 等待PA3高电平到来（不使用软件延时）
    start_time = TIM5->CNT;
    while(PAin(3) == 1) {
        if(TIM5->CNT - start_time > timeout) return 1000;  // 超时
    }

    // 获取定时器CNT值（修改为TIM5）
    end_time = TIM5->CNT;

    // 关闭定时器（修改为TIM5）
    TIM_Cmd(TIM5, DISABLE);
    
    // 通过公式计算出超声波测量距离    
    return (end_time-start_time) / 58;  // 距离（cm） = 时间（us） / 58
}

