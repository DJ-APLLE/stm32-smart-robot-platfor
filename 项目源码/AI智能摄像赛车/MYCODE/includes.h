#ifndef _INCLUDES_H
#define _INCLUDES_H


#include "stm32f4xx.h"
#include "stm32f4xx_rng.h"
#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "task.h"


#include "led.h"
#include "usart.h"
#include "motor.h"
#include "sr04.h"
#include "servo.h"

#include "i2c_ee.h"
#include "OLED_I2C.h"


#include "control_car.h"
#include "hand_control.h"
#include "auto_mode.h"
#include "Random_seed.h"


void ALL_HARDWARE_INIT();//初始化全部硬件

void Create_task();  //创建任务的函数





extern uint16_t g_hand_move_speed;
extern uint16_t g_auto_move_speed;
extern int g_avoid_distance;
extern int g_control_flag;
extern volatile int g_hand_flag;
extern volatile int g_cmd_move;


extern volatile int g_ask_cmd_flag;
extern volatile int g_ask_cmd_move;

#endif
