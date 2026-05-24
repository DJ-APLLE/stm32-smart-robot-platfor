#include "hand_control.h"

void hand_control_car(u8 rec_signal[])
{
	if(strcmp(rec_signal, "0:")==0)
	{
		Move(FORWARD, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有轮子运动
	}
	if(strcmp(rec_signal, "1:")==0)
	{
		Move(BACKWARD, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有轮子运动
	}
	if(strcmp(rec_signal, "2:")==0)
	{
		Move(LEFT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有轮子运动
	}
	if(strcmp(rec_signal, "3:")==0)
	{
		Move(RIGHT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有轮子运动
	}
	if(strcmp(rec_signal, "4:")==0)
	{
		Move(ROTATE_LEFT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有轮子运动
	}
	if(strcmp(rec_signal, "5:")==0)
	{
		Move(ROTATE_RIGHT, g_hand_move_speed);
		vTaskDelay(1000);
		Stop_All_Wheels();  // 停止所有轮子运动
	}
}







