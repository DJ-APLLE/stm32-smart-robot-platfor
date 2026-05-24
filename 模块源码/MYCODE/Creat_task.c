#include "includes.h"

int g_avoid_distance = 30; 
/* 
创建任务的 函数
任务说明：
任务0：保留（要执行看门狗任务，超时没有喂狗则重启系统（复位））
任务2：小车接收超声波数据，自动避障
任务3：控制舵机转动扫描周围环境
*/ 

// 任务句柄
 TaskHandle_t task0_handle = NULL;
 TaskHandle_t task1_handle = NULL;
 TaskHandle_t task2_handle = NULL;
 TaskHandle_t task3_handle = NULL;
 TaskHandle_t task4_handle = NULL;
 TaskHandle_t task5_handle = NULL;

// 任务函数声明
static void task0(void* pvParameters);  // 保留任务
static void task1(void* pvParameters);  // 手动控制模式任务（）
static void task2(void* pvParameters);  // 自动避障
static void task3(void* pvParameters);  // 舵机控制(舵机带动超声波测距)
static void task4(void* pvParameters);  // OLED显示
static void task5(void* pvParameters);  // 语音控制小车

//configMAX_PRIORITIES
void Create_task()
{
    // 修正任务创建
	xTaskCreate( task0, "task0",  256, NULL, 7, &task0_handle); // 保留任务（看门狗），最高优先级
    xTaskCreate( task1, "task1",  256, NULL, 4, &task1_handle); // 执行手动控制任务
    xTaskCreate( task2, "task2",  256, NULL, 5, &task2_handle); // 自动避障任务
    xTaskCreate( task3, "task3",  256, NULL, 2, &task3_handle); // 舵机控制任务，最低优先级
	xTaskCreate( task4, "task4",  256, NULL, 1, &task4_handle); // OLED显示
	xTaskCreate( task5, "task5",  256, NULL, 3, &task5_handle); // 语音控制小车
	
	/* 开启任务调度器 */
    vTaskStartScheduler();
	
}



// 任务0：
static void task0(void* pvParameters)
{
    // 此任务保留，执行操作（看门狗）
    while(1)
    {
		//应用程序喂狗:(初始化后的看门狗3秒内要喂狗)
		IWDG_ReloadCounter();
		GPIO_ToggleBits(GPIOF,GPIO_Pin_9);        // 指示灯
        vTaskDelay(1000);                     // 低频率运行，减少CPU占用
    }
}


// 任务1：手动控制模式任务（）
static void task1(void* pvParameters)
{
    // 
    while(1)
    {
		if(g_control_flag == 2 && g_hand_flag == 1)
		{
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
			g_hand_flag = 0;
		}
		vTaskDelay(pdMS_TO_TICKS(10)); // 低优先级轮询
    }
}


// 任务2：自动模式避障
static void task2(void* pvParameters)
{
    BaseType_t xHigherPriorityTaskWoken;
    static uint8_t last_direction = 0;  // 记录上次转向方向，避免连续同向转向
    
    while(1)
    {
        
        uint16_t distance;
        
        // 非自动模式直接跳过
        if (g_control_flag != 1) {
            vTaskDelay(pdMS_TO_TICKS(20));  // 短延时，快速响应模式切换
            continue;
        }
        
        // 读取距离传感器数据（假设函数为非阻塞式）
        distance = Get_Sr04_Value();
        
        // 避障触发（距离小于阈值）
        if (distance <= g_avoid_distance) 
		{
            // 利用硬件RNG生成随机数实现随机转向算法（优先交替方向，加入10%概率随机转向）
            uint32_t random_num = Get_Random_Seed();  // 调用之前定义的获取随机种子函数，这里实际可获取随机数
            uint8_t random_turn;
            if (random_num % 10 == 0) 
			{
                random_turn = random_num % 2;
            } 
			else 
			{
                random_turn = !last_direction;
            }
            last_direction = random_turn;
            
            // 执行转向
            if (random_turn) {
                Move(LEFT, g_auto_move_speed);  // 左转
            } else {
                Move(RIGHT, g_auto_move_speed); // 右转
            }
            
            // 转向持续时间（优化延时为任务通知）
            vTaskDelay(pdMS_TO_TICKS(1500));  // 转向300ms
            
            // 恢复前进
            Move(FORWARD, g_auto_move_speed);
            vTaskDelay(pdMS_TO_TICKS(150));  // 前进150ms
        } 
		else 
		{
            // 无障碍物时正常前进
            Move(FORWARD, g_auto_move_speed);
        }
        
        // 短周期检测（提高反应速度）
        vTaskDelay(pdMS_TO_TICKS(60));  // 60ms检测一次
    }
}

// 任务3：转动舵机（-90°~90°逻辑角度扫描）
static void task3(void* pvParameters)
{
    float logic_angle;  // 逻辑角度（-90°~90°）
    
    // 舵机初始化
    
    Set_Servo_Angle(0);     // 初始位置设为逻辑0°（物理90°）
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while(1)
    {
        // 非自动模式直接跳过
        if (g_control_flag != 1) {
            Set_Servo_Angle(0);  // 回到中间位置
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        
        // 正向扫描：-90°到90°（从左到右）
        for(logic_angle = -45; logic_angle <= 45; logic_angle += 15)
        {
            // 检查模式切换
            if (g_control_flag != 1) break;
            
            Set_Servo_Angle(logic_angle);
            vTaskDelay(pdMS_TO_TICKS(30));  // 每个角度停留50ms
        }
        
        vTaskDelay(pdMS_TO_TICKS(80));  // 扫描间隔
        
        // 反向扫描：90°到-90°（从右到左）
        for(logic_angle = 45; logic_angle >= -45; logic_angle -= 15)
        {
            // 检查模式切换
            if (g_control_flag != 1) break;
            
            Set_Servo_Angle(logic_angle);
            vTaskDelay(pdMS_TO_TICKS(30));
        }
        
        vTaskDelay(pdMS_TO_TICKS(80));  // 扫描间隔
    }
}
// 任务4：OLED显示任务
static void task4(void* pvParameters)
{
    char buf[36] = {0};
    uint8_t display_mode = 0; // 0: 显示速度, 1: 显示电池电量
    
    while(1)
    {
        // 清屏
        OLED_CLS();
        
        // 显示标题
		
			OLED_ShowCN(30,0,0);//测试显示中文
			OLED_ShowStr(50, 0, "A8888", 2);
        //OLED_ShowStr(20, 0, "Smart Car", 2);
        
        // 显示控制模式
        if(g_control_flag == 1) {
            OLED_ShowStr(26, 3, "Mode: Auto", 1);
        } else {
            OLED_ShowStr(26, 3, "Mode: Manual", 1);
        }
        
        // 根据显示模式切换内容
        if(display_mode == 0) {
            // 显示速度
            if(g_control_flag == 1)
                snprintf(buf, sizeof(buf), "Speed: %d%%", g_auto_move_speed);
            else
                snprintf(buf, sizeof(buf), "Speed: %d%%", g_hand_move_speed);
            OLED_ShowStr(26, 5, buf, 1);
        } 
        
       
        
       
        // 每2秒切换一次显示内容
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 切换显示模式
        display_mode = !display_mode;
    }
}
//// 任务4：OLED显示任务
//static void task4(void* pvParameters)
//{
//    while(1)
//	{
////		OLED_Fill(0xFF);//全屏点亮
////		vTaskDelay(2000);
////		OLED_Fill(0x00);//全屏灭
////		vTaskDelay(2000);
//		for(int i = 0; i < 5; i++)
//		{
//			OLED_ShowCN(22+i*16,0,i);//测试显示中文
//		}
////		vTaskDelay(2000);
////		OLED_ShowStr(0,3,"HelTec Automation",1);//测试6*8字符
////		OLED_ShowStr(0,4,"Hello Tech",2);				//测试8*16字符
////		vTaskDelay(2000);
////		OLED_CLS();//清屏
////		OLED_OFF();//测试OLED休眠
////		vTaskDelay(2000);
////		OLED_ON();//测试OLED休眠后唤醒
//		char buf[36] = {0};
//		if(g_control_flag == 1)
//			sprintf(buf,"SPEED:%d",g_auto_move_speed);
//		else
//			sprintf(buf,"SPEED:%d",g_hand_move_speed);
//		OLED_ShowStr(0,4, buf, sizeof(buf));
//		vTaskDelay(600);		
//	}
//}

// 任务5：语音控制小车
static void task5(void* pvParameters)
{
    while(1)
	{
		if(g_ask_cmd_flag == 1)
		{
			switch(g_ask_cmd_move - 3)
			{
				case FORWARD:
					Move(FORWARD, g_auto_move_speed);
					break;
				case BACKWARD:
					Move(BACKWARD, g_auto_move_speed);
					break;
				case LEFT:
					Move(RIGHT, g_auto_move_speed);   //由于开始没有调试 左转右转是反的
					break;
				case RIGHT:
					Move(LEFT, g_auto_move_speed);    //由于开始没有调试 左转右转是反的
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
			g_ask_cmd_flag = 0;
			g_ask_cmd_move = 0;
		}
		vTaskDelay(pdMS_TO_TICKS(10)); // 低优先级轮询
	}
}



