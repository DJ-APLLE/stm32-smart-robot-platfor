/**
  ******************************************************************************
  * @file           : usart.c
  * @brief          : 串口驱动实现文件（支持DMA发送优化）
  * @details        : 包含USART1和USART3的初始化及DMA发送功能
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * USART1: 调试串口（PA9-TX, PA10-RX）- 使用DMA发送
  * USART3: 蓝牙串口（PB10-TX, PB11-RX）- 中断接收
  *
  * DMA配置:
  * - USART1_TX 使用 DMA2_Stream7_Channel4
  * - 数据传输方向: 内存到外设
  * - 模式: 正常模式（非循环）
  *
  ******************************************************************************
  */

#include "includes.h"

/* ============================================ */
/*              DMA发送相关定义                   */
/* ============================================ */

#define USART1_TX_DMA_STREAM    DMA2_Stream7
#define USART1_TX_DMA_CHANNEL   DMA_Channel_4

uint8_t g_uart1_tx_buffer[256];  // DMA发送缓冲区
volatile uint8_t g_uart1_tx_busy = 0;  // DMA发送忙标志

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
    x = x; 
} 

/* ============================================ */
/*              DMA发送完成中断处理               */
/* ============================================ */

/**
  * @brief  DMA2 Stream7 中断处理函数
  * @details 处理USART1 DMA发送完成中断
  */
void DMA2_Stream7_IRQHandler(void)
{
    // 检查DMA传输完成中断标志
    if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
    {
        // 清除中断标志
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
        
        // 清除发送忙标志
        g_uart1_tx_busy = 0;
    }
}

/* ============================================ */
/*              USART1 DMA发送函数               */
/* ============================================ */

/**
  * @brief  USART1 DMA发送数据
  * @param  data: 要发送的数据指针
  * @param  len: 数据长度
  * @return 发送是否成功
  */
uint8_t USART1_DMA_Send(uint8_t *data, uint16_t len)
{
    // 等待上一次发送完成
    while(g_uart1_tx_busy);
    
    // 复制数据到发送缓冲区
    memcpy((void*)g_uart1_tx_buffer, data, len);
    
    // 设置发送忙标志
    g_uart1_tx_busy = 1;
    
    // 停止DMA传输（确保状态干净）
    DMA_Cmd(USART1_TX_DMA_STREAM, DISABLE);
    
    // 设置DMA传输参数
    DMA_SetCurrDataCounter(USART1_TX_DMA_STREAM, len);
    
    // 启动DMA传输
    DMA_Cmd(USART1_TX_DMA_STREAM, ENABLE);
    
    return 1;
}

/**
  * @brief  USART1 DMA发送字符串
  * @param  str: 要发送的字符串指针
  */
void USART1_DMA_SendString(const char *str)
{
    if(str == NULL) return;
    
    // 计算字符串长度
    uint16_t len = strlen(str);
    
    // 使用DMA发送
    USART1_DMA_Send((uint8_t*)str, len);
}

/* ============================================ */
/*              printf重定向函数                  */
/* ============================================ */

/**
  * @brief  printf输出函数重定向（使用DMA发送）
  * @param  ch: 要发送的字符
  * @param  f: 文件指针（未使用）
  * @return 发送的字符
  */
int fputc(int ch, FILE *f)
{     
    // 等待上一次发送完成
    while(g_uart1_tx_busy);
    
    // 设置发送忙标志
    g_uart1_tx_busy = 1;
    
    // 停止DMA传输
    DMA_Cmd(USART1_TX_DMA_STREAM, DISABLE);
    
    // 设置DMA传输长度为1
    DMA_SetCurrDataCounter(USART1_TX_DMA_STREAM, 1);
    
    // 直接写入发送数据寄存器（单字符优化）
    DMA2_Stream7->M0AR = (uint32_t)&ch;
    
    // 启动DMA传输
    DMA_Cmd(USART1_TX_DMA_STREAM, ENABLE);
    
    return ch;
}

/* ============================================ */
/*              USART1初始化（带DMA支持）          */
/* ============================================ */

/**
  * @brief  USART1初始化（支持DMA发送）
  * @param  myBaudRate: 波特率
  * @note   PA9 - USART1_TX (连接DMA2_Stream7_Channel4)
  *         PA10 - USART1_RX
  */
void Usart1_Init(uint32_t myBaudRate)
{
    GPIO_InitTypeDef 	GPIO_InitStructure;
    USART_InitTypeDef	USART_InitStruct;
    NVIC_InitTypeDef    NVIC_InitStructure;
    DMA_InitTypeDef     DMA_InitStructure;
	
    // ========== 使能时钟 ==========
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  // 使能DMA2时钟

    // ========== 配置GPIO ==========
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 		
	
    // 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);   // PA9 -> USART1_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);  // PA10 -> USART1_RX
	
    // ========== 配置USART ==========
    USART_InitStruct.USART_BaudRate		= myBaudRate;
    USART_InitStruct.USART_Mode			= USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStruct.USART_Parity		= USART_Parity_No;
    USART_InitStruct.USART_StopBits		= USART_StopBits_1;
    USART_InitStruct.USART_WordLength	= USART_WordLength_8b;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);
	
    // ========== 配置DMA发送 ==========
    DMA_DeInit(USART1_TX_DMA_STREAM);
    
    DMA_InitStructure.DMA_Channel = USART1_TX_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;  // 外设地址: USART1数据寄存器
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_uart1_tx_buffer;  // 内存地址: 发送缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  // 方向: 内存到外设
    DMA_InitStructure.DMA_BufferSize = 256;  // 缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度: 字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // 内存数据宽度: 字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // 正常模式（非循环）
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  // 中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  // 禁用FIFO
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(USART1_TX_DMA_STREAM, &DMA_InitStructure);
    
    // 使能USART1的DMA发送
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
    // ========== 配置DMA中断 ==========
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能DMA传输完成中断
    DMA_ITConfig(USART1_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
	
    // ========== 配置USART接收中断 ==========
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 使能串口
    USART_Cmd(USART1, ENABLE);
}

/* ============================================ */
/*              USART3初始化（蓝牙串口）          */
/* ============================================ */

/**
  * @brief  蓝牙串口USART3初始化
  * @param  myBaudRate: 波特率
  * @note   PB10 - USART3_TX
  *         PB11 - USART3_RX
  */
void Bluetooth_Usart3_Init(uint32_t myBaudRate)
{
    GPIO_InitTypeDef 	GPIO_InitStructure;
    USART_InitTypeDef	USART_InitStruct;
    NVIC_InitTypeDef    NVIC_InitStructure;
	
    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // GPIO初始化配置
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 		
	
    // 设置GPIO复用映射
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
    USART_InitStruct.USART_BaudRate		= myBaudRate;
    USART_InitStruct.USART_Mode			= USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStruct.USART_Parity		= USART_Parity_No;
    USART_InitStruct.USART_StopBits		= USART_StopBits_1;
    USART_InitStruct.USART_WordLength	= USART_WordLength_8b;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStruct);
	
    // 配置中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
    // 使能接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // 使能串口
    USART_Cmd(USART3, ENABLE);
}
