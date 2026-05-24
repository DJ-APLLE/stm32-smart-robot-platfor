#ifndef _MOTOR_H
#define _MOTOR_H

#include "includes.h"


// 宏定义：简化PWM占空比计算
#define PWM_PERIOD  999    // 周期值，决定PWM频率为1kHz
#define SPEED_TO_PULSE(speed) ((speed) * PWM_PERIOD / 100)  // 将速度(0-100)转换为脉冲值

void ALL_WHEEL_PWM_INIT(void);
	

#endif
