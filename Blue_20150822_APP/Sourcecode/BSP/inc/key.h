#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "list.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//按键输入驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
#define KEY_RELEASED 0
#define KEY_PRESSED 1
extern TickType_t Key_Tick,Key_PressTime;
struct key{
	u8 press_state;
	u8 press_type;
	TickType_t Key_Tick;
	TickType_t Key_PressTime;
};
extern struct key KEY;
void KEY_Init(void);	//IO初始化		    
#endif
