/**
  ******************************************************************************
  * @file           : includes.h
  * @brief          : AI智能摄像赛车项目主头文件
  * @details        : 包含所有必要的头文件引用和全局变量声明
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 此头文件是项目的核心头文件，包含了所有模块的头文件引用
  * 所有源文件应包含此头文件以获取完整的项目定义
  *
  ******************************************************************************
  */

#ifndef _INCLUDES_H
#define _INCLUDES_H

/* ============================================ */
/*              STM32 HAL库头文件                */
/* ============================================ */
#include "stm32f4xx.h"           // STM32F4系列标准库
#include "stm32f4xx_rng.h"       // 随机数生成器驱动
#include "sys.h"                  // 系统级定义和初始化
#include "usart.h"                // 串口驱动
#include "stdio.h"                // 标准输入输出
#include "string.h"               // 字符串操作
#include "stdlib.h"               // 标准库函数

/* ============================================ */
/*              FreeRTOS头文件                   */
/* ============================================ */
#include "FreeRTOS.h"             // FreeRTOS核心头文件
#include "task.h"                 // 任务管理

/* ============================================ */
/*              硬件驱动头文件                    */
/* ============================================ */
#include "led.h"                  // LED驱动
#include "usart.h"                // 串口驱动（调试串口+蓝牙串口）
#include "motor.h"                // 电机驱动
#include "sr04.h"                 // 超声波模块驱动
#include "servo.h"                // 舵机驱动
#include "i2c_ee.h"               // I2C EEPROM驱动
#include "OLED_I2C.h"             // OLED显示屏驱动

/* ============================================ */
/*              业务功能头文件                    */
/* ============================================ */
#include "control_car.h"          // 车辆控制核心逻辑
#include "hand_control.h"         // 手动控制模式
#include "auto_mode.h"            // 自动控制模式
#include "Random_seed.h"          // 随机数种子生成

/* ============================================ */
/*              函数声明                          */
/* ============================================ */

/**
  * @brief  初始化所有硬件模块
  * @note   在main函数中调用，完成系统所有硬件的初始化
  */
void ALL_HARDWARE_INIT(void);

/**
  * @brief  创建FreeRTOS任务
  * @note   在硬件初始化完成后调用，启动任务调度器
  */
void Create_task(void);

/* ============================================ */
/*              全局变量声明                      */
/* ============================================ */

/**
  * @defgroup 全局控制变量
  * @{
  */

extern uint16_t g_hand_move_speed;    // 手动模式移动速度 (0-999)
extern uint16_t g_auto_move_speed;    // 自动模式移动速度 (0-999)
extern int g_avoid_distance;          // 避障距离阈值 (单位: cm)
extern int g_control_flag;            // 控制模式标志
                                      //   0: 停止模式
                                      //   1: 自动模式
                                      //   2: 手动模式
extern volatile int g_hand_flag;      // 手动控制标志 (1表示有新指令)
extern volatile int g_cmd_move;       // 手动控制指令
                                      //   0: FORWARD 前进
                                      //   1: BACKWARD 后退
                                      //   2: LEFT 左转
                                      //   3: RIGHT 右转
                                      //   4: ROTATE_LEFT 原地左旋
                                      //   5: ROTATE_RIGHT 原地右旋

extern volatile int g_ask_cmd_flag;   // 串口指令标志 (1表示有新指令)
extern volatile int g_ask_cmd_move;   // 串口指令移动类型

/** @} */

#endif /* _INCLUDES_H */
