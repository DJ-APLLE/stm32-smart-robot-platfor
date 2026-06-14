/**
  ******************************************************************************
  * @file           : hand_control.c
  * @brief          : 手动控制模式实现文件
  * @details        : 处理蓝牙串口发送的手动控制指令
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 手动控制指令格式:
  * - "0:": 前进
  * - "1:": 后退
  * - "2:": 左平移
  * - "3:": 右平移
  * - "4:": 原地左旋
  * - "5:": 原地右旋
  *
  ******************************************************************************
  */

#include "hand_control.h"

/**
  * @brief  手动控制车辆运动
  * @param  rec_signal: 接收到的控制指令字符串
  * 
  * @details 根据蓝牙串口接收到的指令控制车辆运动
  *          每个动作执行1秒后自动停止
  */
void hand_control_car(u8 rec_signal[])
{
	if(strcmp((const char*)rec_signal, "0:") == 0)
	{
		Move(FORWARD, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有车轮运动
	}
	if(strcmp((const char*)rec_signal, "1:") == 0)
	{
		Move(BACKWARD, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有车轮运动
	}
	if(strcmp((const char*)rec_signal, "2:") == 0)
	{
		Move(LEFT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有车轮运动
	}
	if(strcmp((const char*)rec_signal, "3:") == 0)
	{
		Move(RIGHT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有车轮运动
	}
	if(strcmp((const char*)rec_signal, "4:") == 0)
	{
		Move(ROTATE_LEFT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有车轮运动
	}
	if(strcmp((const char*)rec_signal, "5:") == 0)
	{
		Move(ROTATE_RIGHT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有车轮运动
	}
}
