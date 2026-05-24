#include "control_car.h"
/*
麦克纳姆轮 A	PWM 	PE5(TIM9_CH1)、 PE6(TIM9_CH2)
麦克纳姆轮 B	PWM	    PC6(TIM8_CH1)、 PC7(TIM8_CH2)
麦克纳姆轮 C	PWM 	PC8(TIM8_CH3)、 PC9(TIM8_CH4)
麦克纳姆轮 D	PWM 	PB6(TIM4_CH1)、 PB7(TIM4_CH2)
*/
// 设置轮子A的速度
void Set_WheelA_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    // 确保速度在0-1000范围内
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    // 设置前进和后退PWM值
    TIM_SetCompare1(TIM9, speed_forward);   // PE5 (TIM9_CH1)
    TIM_SetCompare2(TIM9, speed_backward);  // PE6 (TIM9_CH2)
}

// 设置轮子B的速度
void Set_WheelB_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    TIM_SetCompare1(TIM8, speed_forward);   // PC6 (TIM8_CH1)
    TIM_SetCompare2(TIM8, speed_backward);  // PC7 (TIM8_CH2)
}

// 设置轮子C的速度
void Set_WheelC_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    TIM_SetCompare3(TIM8, speed_forward);    // PC8 (TIM8_CH3)
    TIM_SetCompare4(TIM8, speed_backward);  // PC9 (TIM8_CH4)
}

// 设置轮子D的速度
void Set_WheelD_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    TIM_SetCompare1(TIM4, speed_forward);    // PB6 (TIM4_CH1)
    TIM_SetCompare2(TIM4, speed_backward);   // PB7 (TIM4_CH2)
}

// 设置单个轮子的速度和方向
void Set_Wheel_Speed(WheelType wheel, uint16_t speed_forward, uint16_t speed_backward) {
    switch(wheel) {
        case WHEEL_A:
            Set_WheelA_Speed(speed_forward, speed_backward);
            break;
        case WHEEL_B:
            Set_WheelB_Speed(speed_forward, speed_backward);
            break;
        case WHEEL_C:
            Set_WheelC_Speed(speed_forward, speed_backward);
            break;
        case WHEEL_D:
            Set_WheelD_Speed(speed_forward, speed_backward);
            break;
    }
}

// 停止所有轮子
void Stop_All_Wheels(void) {
    Set_WheelA_Speed(0, 0);
    Set_WheelB_Speed(0, 0);
    Set_WheelC_Speed(0, 0);
    Set_WheelD_Speed(0, 0);
}

// 运动控制函数
void Move(MoveDirection direction, uint16_t speed) 
{
    // 确保速度在有效范围内
    if(speed > 1000) speed = 1000;
    
    switch(direction) 
	{
        case FORWARD:
            // 前进：所有轮子向前转动
            Set_WheelA_Speed(speed, 0);
            Set_WheelB_Speed(speed, 0);
            Set_WheelC_Speed(speed, 0);
            Set_WheelD_Speed(speed, 0);
            break;
            
        case BACKWARD:
            // 后退：所有轮子向后转动
            Set_WheelA_Speed(0, speed);
            Set_WheelB_Speed(0, speed);
            Set_WheelC_Speed(0, speed);
            Set_WheelD_Speed(0, speed);
            break;
            
        case LEFT:
            // 向左平移：
            // A轮：向后，B轮：向前
            // C轮：向前，D轮：向后
            Set_WheelA_Speed(0, speed);  // A轮后退
            Set_WheelB_Speed(speed, 0);  // B轮前进
            Set_WheelC_Speed(speed, 0);  // C轮前进
            Set_WheelD_Speed(0, speed);  // D轮后退
            break;
            
        case RIGHT:
            // 向右平移：
            // A轮：向前，B轮：向后
            // C轮：向后，D轮：向前
            Set_WheelA_Speed(speed, 0);  // A轮前进
            Set_WheelB_Speed(0, speed);  // B轮后退
            Set_WheelC_Speed(0, speed);  // C轮后退
            Set_WheelD_Speed(speed, 0);  // D轮前进
            break;
            
        case ROTATE_LEFT:
            // 原地左转（逆时针）：
            // A轮：向后，B轮：向前
            // C轮：向后，D轮：向前
            Set_WheelA_Speed(0, speed);  // A轮后退
            Set_WheelB_Speed(speed, 0);  // B轮前进
            Set_WheelC_Speed(0, speed);  // C轮后退
            Set_WheelD_Speed(speed, 0);  // D轮前进
            break;
            
        case ROTATE_RIGHT:
            // 原地右转（顺时针）：
            // A轮：向前，B轮：向后
            // C轮：向前，D轮：向后
            Set_WheelA_Speed(speed, 0);  // A轮前进
            Set_WheelB_Speed(0, speed);  // B轮后退
            Set_WheelC_Speed(speed, 0);  // C轮前进
            Set_WheelD_Speed(0, speed);  // D轮后退
            break;
    }
}
