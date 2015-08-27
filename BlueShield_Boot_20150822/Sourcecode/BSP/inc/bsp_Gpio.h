/***************************************************************************
 * Name:bsp_Gpio.h 				                                                
 *											
 * Function:	config gpio to control basic peripherals.
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#ifndef bsp_Gpio_H
#define bsp_Gpio_H	 
#include "stm32f4xx.h" 
#include "bsp_Delay.h"
#include "ff.h"
#include "bsp_FileSystem.h"
	/*  Pinout & Signal
			LED0-------------PA15
			LED1-------------PB0
			LED2-------------PB1
			BUZZER-----------PB3
			PWR_BTN_SW-------PB6
			MCU_PWR_ON-------PB7
			CHARGING---------PB8
			MCU_WAKIUP_BT----PA0
			PWR_BT-----------PB13
	*/
#define bsp_MCUPWROn()    GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define bsp_MCUPWROff()   GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define bsp_WakeUpBT_High()  GPIO_SetBits(GPIOA,GPIO_Pin0)
#define bsp_WakeUpBt_Low()	 GPIO_ResetBist(GPIOA,GPIO_Pin0)

#define bsp_ChargeOn()   GPIO_ResetBits(GPIOB,GPIO_Pin_9);
#define bsp_ChargeOff()  GPIO_SetBits(GPIOB,GPIO_Pin_9);

#define bsp_OTGPwrOn()  GPIO_SetBits(GPIOA,GPIO_Pin_1);
#define bsp_OTGPwrOff() GPIO_ResetBits(GPIOA,GPIO_Pin_1);


#define BUZZ_FREQ 119 //  1Khz~~479  2Khz~~239  4khz~~119  5khz~~96
#define BUTTON_RELEASED 0
#define BUTTON_PRESSED 1
#define BUTTON_NONE 0
#define BUTTON_SINGLE_PRESS 1
#define BUTTON_DOUBLE_PRESS 2

void bsp_LED0On(void);  	  
void bsp_LED0Off(void);			

void bsp_LED1On(void);  	  
void bsp_LED1Off(void);     

void bsp_LED2On(void);  	  
void bsp_LED2Off(void);       

void bsp_BUZZOn(void);  	  
void bsp_BUZZOff(void);    

void bsp_SDPWROn(void) ;
void bsp_SDPWROff(void) ;
extern struct BUTN Button;

struct BUTN{
	u8 press_state;
	u8 press_type;
};
void bsp_GpioInit(void);
#endif















