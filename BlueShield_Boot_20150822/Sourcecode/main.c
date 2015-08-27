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
#include "bsp_Sdio.h"
#include "bsp_Gpio.h"
#include "bsp_FileSystem.h"
#include "bsp_Delay.h"
#include "flash_if.h"
#include "command.h"
#include "bsp_Adc.h"
/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
	
u8 res=0;
pFunction Jump_To_Application;
uint32_t JumpAddress;
NVIC_InitTypeDef NVIC_InitStructure;
extern void SDIO_Register_Deinit(void);
u8 ADCindex=0;
u16 Battery_Level_Boot=0;
u32 APPLICATION_ADDRESS_toRun=0;
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f429_439xx.s) before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  
  /* Add your application code here */	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	bsp_GpioInit();	
	bsp_OTGPwrOff();
	bsp_ChargeOn();
	bsp_AdcInit();
	ForDelay_nms(500);
	for(ADCindex=0;ADCindex<5;ADCindex++)
	{
		Battery_Level_Boot=Get_Battery();
	}
	while(Battery_Level_Boot<3536)  //if Battery_level too low, than do not start up
	{
		Battery_Level_Boot=Get_Battery();
	}
//	if(Battery_Level_Boot<3578)  //if Battery_level too low, than do not start up
//	{
//		while(1)
//		{
//			;
//		}
//	}
  bsp_SDPWROn();
	//bsp_LED1On();  //red led on
  ForDelay_nms(2000);// >3 sec ,then turn on MCU power
	//ForDelay_nms(1000);
  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==1) //still pressed
  {
		bsp_MCUPWROn();//turn on MCU
  }
	FLASH_If_FlashUnlock();

  res=bsp_FSInit();
  bsp_LED2On();
	if(res) //NO file or SD init failed, then jump to APP directly
	{
   res=f_mount(NULL,"0:/",1 );
	 SDIO_DeInit();
	 SDIO_Register_Deinit();
	 DMA_Cmd(DMA2_Stream3,DISABLE);
	 DMA_DeInit(DMA2_Stream3);	 
	 NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority =6;		//子优先级3
	 NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//IRQ通道使能
	 NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	 NVIC_DisableIRQ(SDIO_IRQn);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,DISABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);
   bsp_SDPWROff();
	 if(*(u8*)(ADDR_FLASH_SECTOR_3))
	 {
			APPLICATION_ADDRESS_toRun=APPLICATION_ADDRESS_A;
	 }
	 else
	 {
			APPLICATION_ADDRESS_toRun=APPLICATION_ADDRESS_A;
   } 
			 __disable_irq();
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS_toRun + 4);
			Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS_toRun);
			Jump_To_Application();
	}
	
	else //find image bin and start to upgrade
	{
		    bsp_LED2On();
		    if(COMMAND_DOWNLOAD()) //download failed
				{
					bsp_LED2Off();
					NVIC_SystemReset();
				}
				else  //download succeed,then delet bin file and jump to app
				{
					bsp_LED2Off();
					f_unlink("0:/Akey.bin");
					__disable_irq();
//					JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
//					Jump_To_Application = (pFunction) JumpAddress;
//				/* Initialize user application's Stack Pointer */
//					__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
//					Jump_To_Application();
					bsp_MCUPWROff();
				}
	}


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
