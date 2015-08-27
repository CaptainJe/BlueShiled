#include "key.h"

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

//按键初始化函数
struct key KEY={0,0,0,0};
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	/* Enable SYSCFG's APB interface clock */ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Configure PA0 pin in input mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/* Connect EXTI Line0 to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	/* Configure EXTI line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//??????????
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //????????
	NVIC_Init(&NVIC_InitStructure);
} 

void EXTI4_IRQHandler(void)
{

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_ClearITPendingBit(EXTI_Line4);
	
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0)
	{	
	KEY.press_state=KEY_PRESSED;
	KEY.Key_Tick=xTaskGetTickCountFromISR();
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	}
	
	else if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==1)
	{	
	KEY.press_state=KEY_RELEASED;
	KEY.Key_PressTime=xTaskGetTickCountFromISR()-KEY.Key_Tick;
	if(KEY.Key_PressTime<=20)
	{
	;
	}
	else if(KEY.Key_PressTime<=500)
	{
  KEY.press_type=1;
	}
	else if(KEY.Key_PressTime<=2000)
	{
		KEY.press_type=2;
	}
	else 
	{
		KEY.press_type=3;
	}	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	}

}
















