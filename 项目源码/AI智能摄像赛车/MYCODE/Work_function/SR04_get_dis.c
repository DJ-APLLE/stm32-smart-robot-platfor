/**
  ******************************************************************************
  * @file           : SR04_get_dis.c
  * @brief          : 超声波传感器(SR04)距离测量实现
  * @details        : 使用HC-SR04超声波模块进行距离测量
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * HC-SR04连接说明:
  * - Trig引脚: PA2 (输出触发信号)
  * - Echo引脚: PA3 (输入回波信号)
  * 
  * 工作原理:
  * 1. 发送至少10us的高电平触发信号
  * 2. 模块自动发送8个40kHz的方波
  * 3. 检测回波信号，计算高电平持续时间
  * 4. 距离 = 时间(us) / 58 (单位: cm)
  * 
  * 计算公式说明:
  * - 声速约为340m/s = 0.034cm/us
  * - 往返时间 = 2 * 距离 / 声速
  * - 距离 = (时间 * 声速) / 2 = 时间 * 0.017 cm/us
  * - 时间(us) / 58 ≈ 时间 * 0.01724 ≈ 距离(cm)
  *
  ******************************************************************************
  */

#include "sr04.h"

/**
  * @brief  获取超声波传感器测量距离
  * @retval 测量距离（单位：cm），返回1000表示超时
  * 
  * @details 测量流程:
  *          1. 发送10us高电平触发信号
  *          2. 等待Echo引脚变为高电平（开始计时）
  *          3. 等待Echo引脚变为低电平（结束计时）
  *          4. 计算距离 = 计时时间 / 58
  * 
  * @note   使用TIM5作为高精度计时器
  *         超时时间设置为23200us，对应最大测量距离约400cm
  */
int Get_Sr04_Value(void)
{
    uint32_t timeout = 23200;  // 超时时间(us)，对应最大测量距离约400cm
    uint32_t start_time, end_time;
    
    // 发送触发信号
    PAout(2) = 0;              // Trig引脚先拉低
    delay_us(8);               // 稳定低电平
    PAout(2) = 1;              // Trig引脚拉高
    delay_us(20);              // 保持高电平至少10us
    PAout(2) = 0;              // Trig引脚拉低
    
    // 重置TIM5计数器
    TIM5->CNT = 0;

    // 等待Echo引脚变为高电平（开始接收回波）
    start_time = TIM5->CNT;
    while(PAin(3) == 0) {
        if(TIM5->CNT - start_time > timeout) 
            return 1000;  // 超时返回1000
    }

    // 使能TIM5开始计时
    TIM_Cmd(TIM5, ENABLE);    
    TIM5->CNT = 0;  // 重置计数器
    
    // 等待Echo引脚变为低电平（回波接收完毕）
    start_time = TIM5->CNT;
    while(PAin(3) == 1) {
        if(TIM5->CNT - start_time > timeout) 
            return 1000;  // 超时返回1000
    }

    // 获取计时结果
    end_time = TIM5->CNT;

    // 关闭TIM5
    TIM_Cmd(TIM5, DISABLE);
    
    // 计算距离：距离(cm) = 时间(us) / 58
    // 声速340m/s = 0.034cm/us，往返路程需除以2
    return (int)(end_time / 58);
}
