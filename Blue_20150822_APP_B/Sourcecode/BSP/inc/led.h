#ifndef led_H
#define led_H	 
#include "stm32f4xx.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED�˿ڶ���
//#define LED0 PFout(9)	// DS0
//#define LED1 PFout(10)	// DS1
void LEDR_ON(void);

void LEDR_OFF(void);

void LEDR_TOGGLE(void) ;

void LEDG_ON(void) ;

void LEDG_OFF(void);

void LEDG_TOGGLE(void);

void LED_Init(void);//��ʼ��		 				    
#endif

















