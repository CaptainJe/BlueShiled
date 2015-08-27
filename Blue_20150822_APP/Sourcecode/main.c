/**
  ******************************************************************************
  * @file    Template/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "list.h"
#include "Task_HardwareControl.h"
#include "Task_Communicate_protocol.h"
#include "bsp_Usart.h"
#include "bsp_Sdio.h"
#include "bsp_Gpio.h"
#include "usb_bsp.h"
#include "usb_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_msc_core.h"
#include "Task_HardwareControl.h"
#include "bsp_AesEcb256.h"
#include "bsp_FileSystem.h"
#include "bsp_Delay.h"
#include "bsp_RTC.h"
xTaskHandle                   BSP_Task_Handle;
xTaskHandle                   USART_Task_Handle;
xTaskHandle                   USB_Task_Handle;
xTaskHandle										USART_L0_RXTask_Handle;
xTaskHandle										USART_L0_TXTask_Handle;
xTaskHandle										USART_L1_RXTask_Handle;
xTaskHandle										USART_L1_TXTask_Handle;
xQueueHandle L0_RxQue,L0_TxQue,L1_TxQue,usart1_TxQue;
u8 res=0;
int i=0;
extern L1Header_t L1_RxHeader,L1_TxHeader;
extern u8 bsp_ChangeAPPtoRun(u8 App_Select);
extern void MD5_Test(void);
//extern USB_OTG_CORE_HANDLE USB_OTG_Core;
//extern vu8 USB_STATUS_REG;		//USB״̬
/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f429_439xx.s) before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  
  /* Add your application code here */	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//	
//  bsp_PowerOnInit();
//  ForDelay_nms(3000);// >3 sec ,then turn on MCU power
//  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==1) //still pressed
//  {
//		bsp_MCUPWROn();//turn on MCU
//  }
	
		NVIC_SetVectorTable (NVIC_VectTab_FLASH, 0x20000);
	__enable_irq();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	MD5_Test();
  bsp_BTPWRInit();
  bsp_BTPWROn();
	ForDelay_nms(500);
	bsp_GpioInit();

  bsp_SDPWROn();
	
	bsp_OTGPwrOff();
	
	//while(1);
	bsp_ChargeOn();
	
  bsp_ChangeAPPtoRun(1);
	USART2_Init();
	
	bsp_AdcInit();
  res=bsp_FSInit();

	bsp_SysinfoInit();
//	AES_Test();
  RTC_Config();
	IDog_Init();

	
//	while(1);
  /* Infinite loop */
	 if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
	L0_RxQue=xQueueCreate(64,20*sizeof(u8));  //used for L0 20 byte data trans
	L0_TxQue=xQueueCreate(128,sizeof(u8));
	L1_TxQue=xQueueCreate(64,sizeof(u8));
  xTaskCreate(BSP_Task,
             (char const*)"BSP",
              512,
              NULL,
              2,
              &BSP_Task_Handle);
	xTaskCreate(USART_L0_RXTask,
             (char const*)"L0RX",
              512,
              NULL,
              3,
              &USART_L0_RXTask_Handle);
	xTaskCreate(USART_L0_TXTask,
             (char const*)"L0TX",
              512,
              NULL,
              3,
              &USART_L0_TXTask_Handle);

/*	xTaskCreate(USART_L1_RXTask,
             (char const*)"L1RX",
              64,
              NULL,
              1,
              &USART_L1_RXTask_Handle);
						 */
//	xTaskCreate(USART_L1_TXTask,
//             (char const*)"L1TX",
//              64,
//              NULL,
//              2,
//              &USART_L1_TXTask_Handle);

//  xTaskCreate(USART_Task,
//							(char const*)"USART",
//							128,
//							NULL,
//							3,
//							&USART_Task_Handle);
							
//   xTaskCreate(USB_Task,
//							(char const*)"HDP",
//							128,
//							NULL,
//							1,
//							&USB_Task_Handle);
  vTaskStartScheduler();
}






#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
