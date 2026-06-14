/**
  ******************************************************************************
  * @file           : IRQ.c
  * @brief          : 中断处理实现文件
  * @details        : 包含串口中断处理和全局变量定义
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 此文件包含中断处理函数实现和全局控制变量定义
  * 
  ******************************************************************************
  */

#include "IRQ.h"
#include "includes.h"

/* ============================================ */
/*              全局变量定义                      */
/* ============================================ */

/**
  * @brief 控制模式标志
  * @note  g_control_flag 的取值:
  *        - 0: 停止模式 - 执行停止命令 Stop_All_Wheels()
  *        - 1: 自动模式 - 执行 car_auto_mode()
  *        - 2: 手动模式 - 执行 hand_control_car()
  */
int g_control_flag = 0;

volatile int g_cmd_move;       // 手动控制指令
volatile int g_hand_flag = 0;  // 手动控制标志 (1表示有新指令)
uint16_t g_hand_move_speed = 600;  // 手动模式速度 (默认600)
uint16_t g_auto_move_speed = 400;  // 自动模式速度 (默认400)

/**
  * @brief 蓝牙串口接收缓冲区
  * @note  用于存储从蓝牙模块接收到的数据
  */
volatile uint8_t g_rxflag = 0;       // 接收标志
volatile uint8_t g_count = 0;        // 接收计数
volatile uint8_t g_rec_signal[6] = {0};  // 接收缓冲区

/**
  * @brief 调试串口接收缓冲区
  * @note  用于存储从调试串口接收到的数据
  */
volatile uint8_t g_rec_ask[6] = {0};     // 调试串口接收缓冲区
volatile int g_ask_cmd_flag = 0;         // 调试串口指令标志
volatile int g_ask_cmd_move = 0;         // 调试串口指令
volatile uint8_t g_ask_count = 0;        // 调试串口接收计数

/* ============================================ */
/*              USART1中断处理                    */
/* ============================================ */

/**
  * @brief  USART1中断处理函数
  * @details 处理调试串口(USART1)的数据接收
  *          支持通过串口命令控制小车模式和动作
  * 
  * @note   命令格式（单字符命令）:
  *         - '0': 停止模式
  *         - '1': 自动模式
  *         - '2': 手动模式
  *         - '3'-'8': 动作指令（配合自动模式使用）
  */
void USART1_IRQHandler(void)
{
	// 检查是否发生接收中断
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		// 清除中断标志位
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		// 读取接收到的数据
		g_rec_ask[g_ask_count++] = USART_ReceiveData(USART1);

		// 根据接收到的第一个字符判断命令
		switch(g_rec_ask[0])
		{
			case '0':  // 停止模式
				GPIO_ToggleBits(GPIOF, GPIO_Pin_10);  // 切换指示灯
				g_control_flag = 0;         // 设置控制模式为停止
				g_hand_flag = 0;
				Stop_All_Wheels();         // 停止所有车轮
				break;

			case '1':  // 自动模式
				GPIO_ToggleBits(GPIOE, GPIO_Pin_13);  // 切换指示灯
				g_control_flag = 1;         // 设置控制模式为自动
				car_auto_mode();            // 启动自动模式逻辑
				break;

			case '2':  // 手动模式
				GPIO_ToggleBits(GPIOE, GPIO_Pin_13);  // 切换指示灯
				g_control_flag = 2;         // 设置控制模式为手动
				g_hand_flag = 0;
				Stop_All_Wheels();         // 先停止
				break;

			case '3': case '4': case '5': case '6': case '7': case '8':
				// 动作指令
				GPIO_ToggleBits(GPIOE, GPIO_Pin_13);  // 切换指示灯
				g_ask_cmd_flag = 1;
				g_ask_cmd_move = g_rec_ask[0] - '0';  // 转换为数字
				break;

			default:
				break;
		}

		// 重置接收计数和缓冲区
		g_ask_count = 0;
		memset((void*)g_rec_ask, 0, sizeof(g_rec_ask));
	}	
}

/* ============================================ */
/*              USART3相关函数                    */
/* ============================================ */

/**
  * @brief  通过USART3发送字符串
  * @param  pbuf: 要发送的字符串指针
  * @note   用于通过蓝牙模块发送数据
  */
void usart3_send_str(const char *pbuf)
{
    const char *p = pbuf;
    
    // 检查指针有效性并发送每个字符
    while(p && *p)
    {
        USART_SendData(USART3, *p++);
        // 等待发送完成
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
}

/**
  * @brief  USART3中断处理函数
  * @details 处理蓝牙串口(USART3)的数据接收
  *          支持通过蓝牙命令控制小车模式和速度
  * 
  * @note   命令格式（以冒号':'结尾）:
  *         - "a:": 切换到自动模式
  *         - "h:": 切换到手动模式
  *         - "0:" 到 "5:": 手动控制方向指令
  *         - "+:": 增加速度
  *         - "-:": 减少速度
  *         - "(:": 自动模式减速
  *         - "):": 自动模式加速
  */
void USART3_IRQHandler(void)
{
    // 检查是否发生接收中断
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        // 清除中断标志位
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

        // 读取接收到的数据
        g_rec_signal[g_count++] = USART_ReceiveData(USART3);

        // 判断是否接收到命令结束符 ':'
        if(g_rec_signal[g_count-1] == ':')
        {
            // 根据接收到的命令执行相应操作
            if(strcmp((const char*)g_rec_signal, "a:") == 0)
            {
                // 切换到自动模式
                g_control_flag = 1;
                car_auto_mode();
            }
            else if(strcmp((const char*)g_rec_signal, "h:") == 0)
            {
                // 切换到手动模式
                g_control_flag = 2;
                g_hand_flag = 0;
                Stop_All_Wheels();
            }
            else if(g_control_flag == 1)
            {
                // 自动模式下的速度调节
                if(strcmp((const char*)g_rec_signal, "):") == 0)
                {
                    // 增加自动模式速度
                    g_auto_move_speed += 50;
                    if(g_auto_move_speed > 999)
                        g_auto_move_speed = 999;
                    Move(FORWARD, g_auto_move_speed);
                }
                else if(strcmp((const char*)g_rec_signal, "(:") == 0)
                {
                    // 减少自动模式速度
                    if(g_auto_move_speed < 50)
                        g_auto_move_speed = 0;
                    else
                        g_auto_move_speed -= 50;
                    Move(FORWARD, g_auto_move_speed);
                }
            }
            else if(g_control_flag == 2)
            {
                // 手动模式下的控制
                if(g_rec_signal[1] == ':' && g_rec_signal[0] >= '0' && g_rec_signal[0] <= '5')
                {
                    // 方向控制指令
                    g_hand_flag = 1;
                    g_cmd_move = g_rec_signal[0] - '0';
                }
                else if(strcmp((const char*)g_rec_signal, "+:") == 0)
                {
                    // 增加手动模式速度
                    g_hand_move_speed += 50;
                    if(g_hand_move_speed > 999)
                        g_hand_move_speed = 999;
                }
                else if(strcmp((const char*)g_rec_signal, "-:") == 0)
                {
                    // 减少手动模式速度
                    if(g_hand_move_speed < 50)
                        g_hand_move_speed = 0;
                    else
                        g_hand_move_speed -= 50;
                }
            }

            // 重置接收计数和缓冲区
            g_count = 0;
            memset((void*)g_rec_signal, 0, sizeof(g_rec_signal));
        }
    }    
}
