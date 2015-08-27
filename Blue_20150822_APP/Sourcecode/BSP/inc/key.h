#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "list.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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
void KEY_Init(void);	//IO��ʼ��		    
#endif
