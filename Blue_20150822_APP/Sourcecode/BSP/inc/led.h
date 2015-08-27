#ifndef led_H
#define led_H	 
#include "stm32f4xx.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED端口定义
//#define LED0 PFout(9)	// DS0
//#define LED1 PFout(10)	// DS1
void LEDR_ON(void);

void LEDR_OFF(void);

void LEDR_TOGGLE(void) ;

void LEDG_ON(void) ;

void LEDG_OFF(void);

void LEDG_TOGGLE(void);

void LED_Init(void);//初始化		 				    
#endif

















