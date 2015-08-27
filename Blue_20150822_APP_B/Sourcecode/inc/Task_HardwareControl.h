/***************************************************************************
 * Name:Task_HardwareControl.c 				                                                
 *											
 * Function:	Basic Hardware control task, including LED,BUTTON,BUZZER,
	            USB and peripherals' power control.
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#ifndef Task_bsp_H
#define Task_bsp_H
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_Gpio.h"
#include "usb_bsp.h"
#include "bsp_Usart.h"
#include "bsp_Adc.h"
#include "Task_Communicate_protocol.h"


#define LED_MODE_OFF 0
#define LED_MODE_ON 1
#define LED_MODE_BLINK 2
#define LED_BLINK_ONCE 1
#define LED_BLINK_CONTINUE 0
#define LED_STATUS_ON 1
#define LED_STATUS_OFF 0

#define BUZZ_STATUS_OFF 0
#define BUZZ_STATUS_ON 1
#define BUZZ_MODE_OFF 0
#define BUZZ_MODE_ON 1
#define BUZZ_MODE_BEEP  2
#define BUZZ_BEEP_ONCE  1
#define BUZZ_BEEP_CONTINUE 0

#define APPCON_STOP 0
#define APPCON_START 1

struct led{
	u8 mode;
	u8 statusnow;
	u8 blink_mode;
	u16 blink_cycle; //cycle n *10 ms.
	void (*ledon)(void);
	void (*ledoff)(void);
};

struct buzz{
	u8 mode;
  u8 statusnow;
	u16 beep_mode;
	float beep_cycle; //cycle n*10 ms.
	void (*buzzon)();
	void (*buzzoff)();
};


void BSP_Task(void * pvParameters);
void USB_Task(void * pvParameters);
extern struct led LEDRed,LEDGreen,LEDBlue;
extern struct led LEDRed_APPCon,LEDGreen_APPCon,LEDBlue_APPCon;
extern struct buzz BUZZ,BUZZ_APPCon;
extern struct led* LEDRed_Act,*LEDGreen_Act,*LEDBlue_Act;
extern struct buzz* BUZZ_Act;
#endif
