/**
  ******************************************************************************
  * @file           : Creat_task.c
  * @brief          : FreeRTOS任务创建与实现
  * @details        : 包含所有FreeRTOS任务的创建和任务函数实现
  * @author         : 
  * @date           : 
  ******************************************************************************
  * @attention
  *
  * 此文件包含FreeRTOS任务的定义和实现
  * 任务优先级说明（数值越大优先级越高）:
  *   - task0: 优先级7 - 最高（看门狗喂狗）
  *   - task2: 优先级5 - 自动模式控制
  *   - task1: 优先级4 - 手动模式控制
  *   - task5: 优先级3 - 串口指令处理
  *   - task3: 优先级2 - 舵机扫描
  *   - task4: 优先级1 - OLED显示（最低）
  *
  ******************************************************************************
  */

#include "includes.h"

/* ============================================ */
/*              全局变量定义                      */
/* ============================================ */

/**
  * @brief 避障距离阈值（单位：cm）
  * @note  当超声波检测距离小于此值时，自动模式会触发避障行为
  */
int g_avoid_distance = 30;

/**
  * @defgroup 任务句柄
  * @{
  */
TaskHandle_t task0_handle = NULL;  // 看门狗喂狗任务
TaskHandle_t task1_handle = NULL;  // 手动模式任务
TaskHandle_t task2_handle = NULL;  // 自动模式任务
TaskHandle_t task3_handle = NULL;  // 舵机扫描任务
TaskHandle_t task4_handle = NULL;  // OLED显示任务
TaskHandle_t task5_handle = NULL;  // 串口指令任务
/** @} */

/* ============================================ */
/*              任务函数声明                      */
/* ============================================ */

/**
  * @defgroup 任务函数声明
  * @{
  */
static void task0(void* pvParameters);  // 看门狗喂狗任务
static void task1(void* pvParameters);  // 手动控制模式任务
static void task2(void* pvParameters);  // 自动控制模式任务
static void task3(void* pvParameters);  // 舵机扫描任务
static void task4(void* pvParameters);  // OLED显示任务
static void task5(void* pvParameters);  // 串口指令处理任务
/** @} */

/* ============================================ */
/*              任务创建函数                      */
/* ============================================ */

/**
  * @brief  创建所有FreeRTOS任务并启动调度器
  * 
  * @details 此函数按照优先级从高到低创建以下任务:
  * 
  *          | 任务 | 优先级 | 功能描述 | 栈大小 |
  *          |------|--------|----------|--------|
  *          | task0 | 7 | 看门狗喂狗（最高优先级） | 256 |
  *          | task2 | 5 | 自动模式控制 | 256 |
  *          | task1 | 4 | 手动模式控制 | 256 |
  *          | task5 | 3 | 串口指令处理 | 256 |
  *          | task3 | 2 | 舵机扫描 | 256 |
  *          | task4 | 1 | OLED显示（最低优先级） | 256 |
  * 
  * @note   创建完成后调用 vTaskStartScheduler() 启动任务调度器
  */
void Create_task()
{
    // 创建任务（按优先级从高到低）
    xTaskCreate(task0, "task0", 256, NULL, 7, &task0_handle); // 看门狗任务（最高优先级）
    xTaskCreate(task1, "task1", 256, NULL, 4, &task1_handle); // 手动控制任务
    xTaskCreate(task2, "task2", 256, NULL, 5, &task2_handle); // 自动控制任务
    xTaskCreate(task3, "task3", 256, NULL, 2, &task3_handle); // 舵机扫描任务
    xTaskCreate(task4, "task4", 256, NULL, 1, &task4_handle); // OLED显示任务
    xTaskCreate(task5, "task5", 256, NULL, 3, &task5_handle); // 串口指令任务
	
	// 启动任务调度器（此函数不会返回）
    vTaskStartScheduler();
}

/* ============================================ */
/*              任务函数实现                      */
/* ============================================ */

/**
  * @brief  任务0 - 看门狗喂狗任务
  * @param  pvParameters: 任务参数（未使用）
  * 
  * @details 此任务以最高优先级运行，负责定期喂狗
  *          运行频率：1秒一次
  *          主要功能：
  *          1. 喂看门狗（防止系统复位）
  *          2. 切换指示灯状态（指示系统正常运行）
  */
static void task0(void* pvParameters)
{
    // 任务主循环
    while(1)
    {
		// 喂狗：必须在看门狗超时前调用此函数
		IWDG_ReloadCounter();
		
		// 切换LED指示灯状态（PF9）
		GPIO_ToggleBits(GPIOF, GPIO_Pin_9);
		
        // 延时1秒（降低CPU占用）
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
  * @brief  任务1 - 手动控制模式任务
  * @param  pvParameters: 任务参数（未使用）
  * 
  * @details 此任务处理手动控制模式下的车辆运动
  *          运行频率：约100Hz（10ms延时）
  *          当 g_control_flag == 2 且 g_hand_flag == 1 时执行
  * 
  * @note   支持的运动指令:
  *         - FORWARD: 前进
  *         - BACKWARD: 后退
  *         - LEFT: 左转
  *         - RIGHT: 右转
  *         - ROTATE_LEFT: 原地左旋
  *         - ROTATE_RIGHT: 原地右旋
  */
static void task1(void* pvParameters)
{
    while(1)
    {
		// 检查是否为手动模式且有新指令
		if(g_control_flag == 2 && g_hand_flag == 1)
		{
			// 根据指令执行相应动作
			switch(g_cmd_move)
			{
				case FORWARD:
					Move(FORWARD, g_hand_move_speed);
					break;
				case BACKWARD:
					Move(BACKWARD, g_hand_move_speed);
					break;
				case LEFT:
					Move(LEFT, g_hand_move_speed);
					break;
				case RIGHT:
					Move(RIGHT, g_hand_move_speed);
					break;
				case ROTATE_LEFT:
					Move(ROTATE_LEFT, g_hand_move_speed);
					break;
				case ROTATE_RIGHT:
					Move(ROTATE_RIGHT, g_hand_move_speed);
					break;
			}
			// 清除指令标志
			g_hand_flag = 0;
		}
		
		// 短延时后再次检查（高优先级轮询）
		vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
  * @brief  任务2 - 自动控制模式任务
  * @param  pvParameters: 任务参数（未使用）
  * 
  * @details 此任务实现自动避障逻辑
  *          运行频率：约16.7Hz（60ms延时）
  *          主要功能：
  *          1. 通过超声波传感器获取距离
  *          2. 判断是否需要避障（距离 < 避障阈值）
  *          3. 执行避障动作（随机左转或右转）
  *          4. 继续前进
  * 
  * @note   避障算法:
  *         - 使用硬件RNG生成随机数决定转向方向
  *         - 为避免连续同一方向转向，记录上次转向方向
  *         - 10%概率随机转向，90%概率与上次相反
  */
static void task2(void* pvParameters)
{
    static uint8_t last_direction = 0;  // 记录上次转向方向，避免连续同向
    
    while(1)
    {
        uint16_t distance;
        
        // 非自动模式时直接跳过
        if (g_control_flag != 1) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        
        // 获取超声波距离（单位：cm）
        distance = Get_Sr04_Value();
        
        // 判断是否需要避障
        if (distance <= g_avoid_distance) 
		{
            // 使用硬件RNG生成随机数，实现智能避障算法
            uint32_t random_num = Get_Random_Seed();
            uint8_t random_turn;
            
            // 10%概率完全随机转向，90%概率与上次相反
            if (random_num % 10 == 0) 
			{
                random_turn = random_num % 2;
            } 
			else 
			{
                random_turn = !last_direction;
            }
            last_direction = random_turn;
            
            // 执行转向动作
            if (random_turn) {
                Move(LEFT, g_auto_move_speed);   // 左转
            } else {
                Move(RIGHT, g_auto_move_speed);  // 右转
            }
            
            // 转向持续时间（1.5秒）
            vTaskDelay(pdMS_TO_TICKS(1500));
            
            // 转向后继续前进
            Move(FORWARD, g_auto_move_speed);
            vTaskDelay(pdMS_TO_TICKS(150));
        } 
		else 
		{
            // 距离安全，继续前进
            Move(FORWARD, g_auto_move_speed);
        }
        
        // 主循环延时（控制检测频率）
        vTaskDelay(pdMS_TO_TICKS(60));
    }
}

/**
  * @brief  任务3 - 舵机扫描任务
  * @param  pvParameters: 任务参数（未使用）
  * 
  * @details 此任务控制舵机进行角度扫描
  *          扫描范围：-45度 到 +45度（逻辑角度）
  *          扫描步长：15度
  *          运行频率：由扫描周期决定
  * 
  * @note   逻辑角度与物理角度的关系:
  *         - 逻辑角度: -90度 ~ +90度
  *         - 物理角度: 0度 ~ 180度
  *         - 转换公式: physical = logical + 90
  */
static void task3(void* pvParameters)
{
    float logic_angle;  // 逻辑角度 (-90度 ~ +90度)
    
    // 初始化：设置舵机到中位
    Set_Servo_Angle(0);
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while(1)
    {
        // 非自动模式时返回中位并等待
        if (g_control_flag != 1) {
            Set_Servo_Angle(0);
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        
        // 向上扫描：-45度到+45度
        for(logic_angle = -45; logic_angle <= 45; logic_angle += 15)
        {
            // 检查模式是否改变
            if (g_control_flag != 1) break;
            
            Set_Servo_Angle(logic_angle);
            vTaskDelay(pdMS_TO_TICKS(30));
        }
        
        vTaskDelay(pdMS_TO_TICKS(80));
        
        // 向下扫描：+45度到-45度
        for(logic_angle = 45; logic_angle >= -45; logic_angle -= 15)
        {
            // 检查模式是否改变
            if (g_control_flag != 1) break;
            
            Set_Servo_Angle(logic_angle);
            vTaskDelay(pdMS_TO_TICKS(30));
        }
        
        vTaskDelay(pdMS_TO_TICKS(80));
    }
}

/**
  * @brief  任务4 - OLED显示任务
  * @param  pvParameters: 任务参数（未使用）
  * 
  * @details 此任务负责OLED显示屏的内容更新
  *          运行频率：2秒更新一次
  *          显示内容：
  *          1. 标题（固定显示）
  *          2. 当前模式（自动/手动）
  *          3. 当前速度
  */
static void task4(void* pvParameters)
{
    char buf[36] = {0};
    uint8_t display_mode = 0; // 0: 显示速度, 1: 显示其他信息
    
    while(1)
    {
        // 清屏
        OLED_CLS();
        
        // 显示标题
        OLED_ShowCN(30, 0, 0);       // 显示中文标题
        OLED_ShowStr(50, 0, "A8888", 2);
        
        // 显示当前模式
        if(g_control_flag == 1) {
            OLED_ShowStr(26, 3, "Mode: Auto", 1);
        } else {
            OLED_ShowStr(26, 3, "Mode: Manual", 1);
        }
        
        // 显示速度信息
        if(display_mode == 0) {
            if(g_control_flag == 1)
                snprintf(buf, sizeof(buf), "Speed: %d%%", g_auto_move_speed);
            else
                snprintf(buf, sizeof(buf), "Speed: %d%%", g_hand_move_speed);
            OLED_ShowStr(26, 5, (uint8_t*)buf, 1);
        }
        
        // 延时2秒
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 切换显示模式
        display_mode = !display_mode;
    }
}

/**
  * @brief  任务5 - 串口指令处理任务
  * @param  pvParameters: 任务参数（未使用）
  * 
  * @details 此任务处理通过调试串口(USART1)发送的动作指令
  *          运行频率：约100Hz（10ms延时）
  *          当 g_ask_cmd_flag == 1 时执行指令
  * 
  * @note   指令格式:
  *         - '3'-'8' 对应动作指令（需要减3转换为内部指令码）
  */
static void task5(void* pvParameters)
{
    while(1)
    {
		// 检查是否有串口指令
		if(g_ask_cmd_flag == 1)
		{
			// 解析并执行指令（指令码需要减3）
			switch(g_ask_cmd_move - 3)
			{
				case FORWARD:
					Move(FORWARD, g_auto_move_speed);
					break;
				case BACKWARD:
					Move(BACKWARD, g_auto_move_speed);
					break;
				case LEFT:
					Move(RIGHT, g_auto_move_speed);   // 注意：此处可能存在逻辑问题
					break;
				case RIGHT:
					Move(LEFT, g_auto_move_speed);    // 注意：此处可能存在逻辑问题
					break;
				case ROTATE_LEFT:
					Move(ROTATE_LEFT, g_auto_move_speed);
					break;
				case ROTATE_RIGHT:
					Move(ROTATE_RIGHT, g_auto_move_speed);
					break;
				default:
					break;
			}
			// 清除指令标志
			g_ask_cmd_flag = 0;
			g_ask_cmd_move = 0;
		}
		// 短延时后再次检查
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
