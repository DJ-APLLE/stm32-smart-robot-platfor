/**
  ******************************************************************************
  * @file           : control_car.c
  * @brief          : 车辆运动控制实现文件
  * @details        : 包含四个车轮的速度控制和车辆运动方向控制
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 车轮连接说明:
  * - 车轮A: PWM PE5(TIM9_CH1), PE6(TIM9_CH2)
  * - 车轮B: PWM PC6(TIM8_CH1), PC7(TIM8_CH2)
  * - 车轮C: PWM PC8(TIM8_CH3), PC9(TIM8_CH4)
  * - 车轮D: PWM PB6(TIM4_CH1), PB7(TIM4_CH2)
  *
  * 车轮布局示意:
  *        B ---- C
  *        |      |
  *        A ---- D
  *        (车头朝向)
  *
  ******************************************************************************
  */

#include "control_car.h"

/* ============================================ */
/*              单个车轮速度控制                  */
/* ============================================ */

/**
  * @brief  设置车轮A速度
  * @param  speed_forward: 前进速度 (0-1000)
  * @param  speed_backward: 后退速度 (0-1000)
  * 
  * @details 车轮A位于车辆左后位置
  *          使用TIM9通道1控制前进，通道2控制后退
  */
void Set_WheelA_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    // 速度范围限制 (0-1000)
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    // 设置前进和后退PWM值
    TIM_SetCompare1(TIM9, speed_forward);   // PE5 (TIM9_CH1) - 前进
    TIM_SetCompare2(TIM9, speed_backward);  // PE6 (TIM9_CH2) - 后退
}

/**
  * @brief  设置车轮B速度
  * @param  speed_forward: 前进速度 (0-1000)
  * @param  speed_backward: 后退速度 (0-1000)
  * 
  * @details 车轮B位于车辆左前位置
  *          使用TIM8通道1控制前进，通道2控制后退
  */
void Set_WheelB_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    TIM_SetCompare1(TIM8, speed_forward);   // PC6 (TIM8_CH1) - 前进
    TIM_SetCompare2(TIM8, speed_backward);  // PC7 (TIM8_CH2) - 后退
}

/**
  * @brief  设置车轮C速度
  * @param  speed_forward: 前进速度 (0-1000)
  * @param  speed_backward: 后退速度 (0-1000)
  * 
  * @details 车轮C位于车辆右前位置
  *          使用TIM8通道3控制前进，通道4控制后退
  */
void Set_WheelC_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    TIM_SetCompare3(TIM8, speed_forward);   // PC8 (TIM8_CH3) - 前进
    TIM_SetCompare4(TIM8, speed_backward);  // PC9 (TIM8_CH4) - 后退
}

/**
  * @brief  设置车轮D速度
  * @param  speed_forward: 前进速度 (0-1000)
  * @param  speed_backward: 后退速度 (0-1000)
  * 
  * @details 车轮D位于车辆右后位置
  *          使用TIM4通道1控制前进，通道2控制后退
  */
void Set_WheelD_Speed(uint16_t speed_forward, uint16_t speed_backward) {
    if(speed_forward > 1000) speed_forward = 1000;
    if(speed_backward > 1000) speed_backward = 1000;
    
    TIM_SetCompare1(TIM4, speed_forward);   // PB6 (TIM4_CH1) - 前进
    TIM_SetCompare2(TIM4, speed_backward);  // PB7 (TIM4_CH2) - 后退
}

/* ============================================ */
/*              通用车轮速度设置                  */
/* ============================================ */

/**
  * @brief  设置指定车轮的速度
  * @param  wheel: 车轮类型 (WHEEL_A, WHEEL_B, WHEEL_C, WHEEL_D)
  * @param  speed_forward: 前进速度 (0-1000)
  * @param  speed_backward: 后退速度 (0-1000)
  */
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

/* ============================================ */
/*              车辆运动控制                      */
/* ============================================ */

/**
  * @brief  停止所有车轮
  * @details 将所有车轮的前进和后退速度都设置为0
  */
void Stop_All_Wheels(void) {
    Set_WheelA_Speed(0, 0);
    Set_WheelB_Speed(0, 0);
    Set_WheelC_Speed(0, 0);
    Set_WheelD_Speed(0, 0);
}

/**
  * @brief  车辆运动控制
  * @param  direction: 运动方向
  * @param  speed: 运动速度 (0-1000)
  * 
  * @details 根据指定方向控制四个车轮的速度，实现不同的运动模式
  * 
  * @note   车轮布局:
  *         ```
  *              B ---- C
  *              |      |
  *              A ---- D
  *         ```
  * 
  *         运动方向说明:
  *         - FORWARD:      前进（四轮同向）
  *         - BACKWARD:     后退（四轮同向）
  *         - LEFT:         左平移（对角线车轮反向）
  *         - RIGHT:        右平移（对角线车轮反向）
  *         - ROTATE_LEFT:  原地左旋（左右轮反向）
  *         - ROTATE_RIGHT: 原地右旋（左右轮反向）
  */
void Move(MoveDirection direction, uint16_t speed) 
{
    // 速度范围限制
    if(speed > 1000) speed = 1000;
    
    switch(direction) 
	{
        case FORWARD:
            // 前进：四个车轮同时向前转动
            Set_WheelA_Speed(speed, 0);
            Set_WheelB_Speed(speed, 0);
            Set_WheelC_Speed(speed, 0);
            Set_WheelD_Speed(speed, 0);
            break;
            
        case BACKWARD:
            // 后退：四个车轮同时向后转动
            Set_WheelA_Speed(0, speed);
            Set_WheelB_Speed(0, speed);
            Set_WheelC_Speed(0, speed);
            Set_WheelD_Speed(0, speed);
            break;
            
        case LEFT:
            // 左平移：对角线车轮反向运动
            // A(后左)后退, B(前左)前进
            // C(前右)前进, D(后右)后退
            Set_WheelA_Speed(0, speed);  // A后退
            Set_WheelB_Speed(speed, 0);  // B前进
            Set_WheelC_Speed(speed, 0);  // C前进
            Set_WheelD_Speed(0, speed);  // D后退
            break;
            
        case RIGHT:
            // 右平移：对角线车轮反向运动
            // A(后左)前进, B(前左)后退
            // C(前右)后退, D(后右)前进
            Set_WheelA_Speed(speed, 0);  // A前进
            Set_WheelB_Speed(0, speed);  // B后退
            Set_WheelC_Speed(0, speed);  // C后退
            Set_WheelD_Speed(speed, 0);  // D前进
            break;
            
        case ROTATE_LEFT:
            // 原地左旋：左侧车轮后退，右侧车轮前进
            // A(后左)后退, B(前左)前进
            // C(前右)后退, D(后右)前进
            Set_WheelA_Speed(0, speed);  // A后退
            Set_WheelB_Speed(speed, 0);  // B前进
            Set_WheelC_Speed(0, speed);  // C后退
            Set_WheelD_Speed(speed, 0);  // D前进
            break;
            
        case ROTATE_RIGHT:
            // 原地右旋：左侧车轮前进，右侧车轮后退
            // A(后左)前进, B(前左)后退
            // C(前右)前进, D(后右)后退
            Set_WheelA_Speed(speed, 0);  // A前进
            Set_WheelB_Speed(0, speed);  // B后退
            Set_WheelC_Speed(speed, 0);  // C前进
            Set_WheelD_Speed(0, speed);  // D后退
            break;
    }
}
