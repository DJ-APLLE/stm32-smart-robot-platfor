#include "includes.h"


int main(void)
{
	SystemInit();		   // 系统初始化
	ALL_HARDWARE_INIT();  // 初始化全部硬件
	Create_task();       // 创建任务	

}

