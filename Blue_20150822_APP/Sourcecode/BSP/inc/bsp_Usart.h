/***************************************************************************
 * bso_Usart.c 				                                                
 *											
 * USART LowLayer config to communicate with BT
 *                                                                                      																		
 * Prepared: Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#ifndef USARTDRI_H
#define USARTDRI_H
#include "stm32f4xx.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "task.h"
#include "list.h"
#include "bsp_Gpio.h"

#define BT_STATE_Ready 0
#define BT_STATE_Advertising 1
#define BT_STATE_Connected 3
#define BT_STATE_Disconnecting 4
#define BT_STATE_Standby 5

#define BTCMD_NONE 0
#define BTCMD_A 1
#define BTCMD_T 2
#define BTCMD_E 3
#define BTCMD_B 4
#define BTCMD_R 5
#define BTCMD_WAITR 6

extern u8 BT_State;
extern u8 BT_State_old;
void	USART2_Init(void);
void USART2_Init_withoutTx(void);
void BT_CMDHandler(void);
#endif

