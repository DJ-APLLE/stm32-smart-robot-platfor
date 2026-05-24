#include "IRQ.h"
#include "includes.h"

// 全局变量
/*
说明：g_control_flag == 0 为无任何控制模式 执行所有轮子停止操作：Stop_All_Wheels()
                     == 1 则为自动模式   执行 hand_control_car(u8 rec_signal[])
					 == 2 为手动控制模式 执行 car_auto_mode()
*/
int g_control_flag    =   0;
volatile int g_cmd_move ;
volatile int g_hand_flag = 0 ;
uint16_t g_hand_move_speed = 600;
uint16_t g_auto_move_speed = 400;


// 串口接收状态变量 - 移到全局作用域
volatile uint8_t g_rxflag =  0;
volatile uint8_t g_count  =  0;
volatile uint8_t g_rec_signal[6]  = {0};

volatile uint8_t g_rec_ask[6]  = {0};
volatile int g_ask_cmd_flag = 0 ;
volatile int g_ask_cmd_move = 0 ;
volatile uint8_t g_ask_count  =  0;

//每接收一个字节都会进行中断
void USART1_IRQHandler(void)
{
	//判断串口接收标志位的标志位
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		//清空标志位，方便接收一位数据
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		//接受数据
		g_rec_ask[g_ask_count++] = USART_ReceiveData(USART1);
			if(g_rec_ask[0] == '0')
			{
				GPIO_ToggleBits(GPIOF,GPIO_Pin_10);        // 指示灯;
				g_control_flag = 0;         // 标志
				g_hand_flag = 0;
				Stop_All_Wheels();
			}
			else if(g_rec_ask[0] == '1')
			{
				GPIO_ToggleBits(GPIOE,GPIO_Pin_13);        // 指示灯;
				g_control_flag = 1;         // 自动标志
				car_auto_mode();			// 开启自动模式函数
			}
			else if(g_rec_ask[0] == '2')
			{
				GPIO_ToggleBits(GPIOE,GPIO_Pin_13);        // 指示灯;
				g_control_flag = 2; 		// 手动标志
				g_hand_flag = 0;
				Stop_All_Wheels();
			}
			else if(g_rec_ask[0] >= '3' && g_rec_ask[0] <= '8')
			{
				GPIO_ToggleBits(GPIOE,GPIO_Pin_13);        // 指示灯;
				g_ask_cmd_flag = 1;
				g_ask_cmd_move = g_rec_ask[0] - '0';
			}
			g_ask_count = 0; //下一数据从g_buffer[0]开始存储
			memset(g_rec_ask, 0, sizeof(g_rec_ask));
		
	}	
}


// 通过usart3发送数据出去（即给手机）
void usart3_send_str(const char *pbuf)
{
    const char *p = pbuf;
    
    // 检测当前p的指针有效性
    while(p && *p)
    {
        USART_SendData(USART3, *p++);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
}

void USART3_IRQHandler(void)
{
    // 判断串口接收标志位的标志位
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        // 清空标志位，方便接收一位数据
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		// 接受数据
		g_rec_signal[g_count++] = USART_ReceiveData(USART3);
		
		// 如果收到数据为':'表示收到结束符
		if(g_rec_signal[g_count-1] == ':')
		{
			
			if(strcmp(g_rec_signal, "a:")==0)
			{
				g_control_flag = 1;         // 自动标志
				car_auto_mode();			// 开启自动模式函数
			}
			else if(strcmp(g_rec_signal, "h:")==0)
			{
				g_control_flag = 2; 		// 手动标志
				g_hand_flag = 0;
				Stop_All_Wheels();
			}                              
			else if(g_control_flag == 1) //自动控制模式下
			{
				if(strcmp(g_rec_signal, "):")==0)
				{
					g_auto_move_speed += 50;
					if(g_auto_move_speed > 999)
						g_auto_move_speed = 999;
					Move(FORWARD, g_auto_move_speed);   
				}
				else if(strcmp(g_rec_signal, "(:")==0)
				{
					g_auto_move_speed -= 50;
					if(g_auto_move_speed < 0)
						g_auto_move_speed = 0;
					Move(FORWARD, g_auto_move_speed);   
				}
			}
			else if(g_control_flag == 2) //手动控制模式下
			{
				
				if(g_rec_signal[1] == ':' && g_rec_signal[0] >= '0' && g_rec_signal[0] <= '5')
				{
					g_hand_flag = 1;
					g_cmd_move = g_rec_signal[0] - '0';
				}
				else if(strcmp(g_rec_signal, "+:") == 0)
				{
					g_hand_move_speed += 50;
					if(g_hand_move_speed > 999)
						g_hand_move_speed = 999;
				}
				else if(strcmp(g_rec_signal, "-:") == 0)
				{
					g_hand_move_speed -= 50;
					if(g_hand_move_speed < 0)
						g_hand_move_speed = 0;
				}	
			}	
			else
				;
			g_count = 0;  // 下一数据从rec_signal[0]开始存储
			memset(g_rec_signal, 0, sizeof(g_rec_signal)); //清空
		}
    }    
}

