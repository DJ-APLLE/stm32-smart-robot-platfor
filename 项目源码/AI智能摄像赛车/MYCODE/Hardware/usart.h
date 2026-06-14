#ifndef __USART_H
#define __USART_H

#include "includes.h"

/* ============================================ */
/*              外部函数声明                      */
/* ============================================ */

/**
  * @brief  USART1初始化（支持DMA发送）
  * @param  myBaudRate: 波特率
  * @note   PA9 - USART1_TX (连接DMA2_Stream7_Channel4)
  *         PA10 - USART1_RX
  */
void Usart1_Init(uint32_t myBaudRate);

/**
  * @brief  蓝牙串口USART3初始化
  * @param  myBaudRate: 波特率
  * @note   PB10 - USART3_TX
  *         PB11 - USART3_RX
  */
void Bluetooth_Usart3_Init(uint32_t myBaudRate);

/**
  * @brief  USART1 DMA发送数据
  * @param  data: 要发送的数据指针
  * @param  len: 数据长度
  * @return 发送是否成功
  */
uint8_t USART1_DMA_Send(uint8_t *data, uint16_t len);

/**
  * @brief  USART1 DMA发送字符串
  * @param  str: 要发送的字符串指针
  */
void USART1_DMA_SendString(const char *str);

/* ============================================ */
/*              外部变量声明                      */
/* ============================================ */

extern uint8_t g_uart1_tx_buffer[256];      // DMA发送缓冲区
extern volatile uint8_t g_uart1_tx_busy;    // DMA发送忙标志

#endif
