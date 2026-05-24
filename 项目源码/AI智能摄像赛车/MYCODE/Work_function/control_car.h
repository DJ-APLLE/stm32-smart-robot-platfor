#ifndef _CONTROL_CAR_H
#define _CONTROL_CAR_H
#include "includes.h"

// 轮子定义
typedef enum {
    WHEEL_A = 0,
    WHEEL_B,
    WHEEL_C,
    WHEEL_D
} WheelType;

// 运动方向
typedef enum {
    FORWARD = 0,
    BACKWARD,
    LEFT,			 // 左平移
    RIGHT,
    ROTATE_LEFT,     // 原地左转
    ROTATE_RIGHT,    // 原地右转
	NO_CMD
} MoveDirection;

// 初始化函数
void MecanumWheel_Init(void);

// 设置单个轮子的速度和方向
void Set_Wheel_Speed(WheelType wheel, uint16_t speed_forward, uint16_t speed_backward);

// 停止所有轮子
void Stop_All_Wheels(void);

// 运动控制函数
void Move(MoveDirection direction, uint16_t speed);


#endif
