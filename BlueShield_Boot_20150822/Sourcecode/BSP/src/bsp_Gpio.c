/***************************************************************************
 * Name:bsp_Gpio.c 				                                                
 *											
 * Function:	config gpio to control basic peripherals.
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#include "bsp_Gpio.h" 
	/*  Pinout & Signal
      PA0-----MCU_Wakeup_BT          PB0-----LED1(Red)
			PA1-----OTG_EN                 PB1-----LED2(Green)
			PA2-----MCU_TX                 PB3-----BUZZ
			PA3-----MCU_RX                 PB4-----SD_D0
			PA4-----BAT_ADC                PB5-----SD_D3
			PA6-----SD_CMD                 PB6-----PWR_BTN_SW
			PA8-----SD_D1                  PB7-----MCU_PWR_ON
			PA9-----SD_D2                  PB8-----CHARGING
			PA10----USB_ID                 PB9-----CHG_EN
			PA11----USB_DM                 PB13----PWR_BT
			PA12----USB_DP                 PB14----SD_EN
			PA15----LED0(Blue)             PB15----SD_CLK
	*/
struct BUTN Button={0,0};
u8 PowerOn_Released=0;
void bsp_GpioInit(void)
{   
  GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
//  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;    //100MHz+SetBits  sink in ADC;   100Mhz+dontSetBits OK     //50Mhz+DontSet   sink in ADC;    50Mhz+SetBits OK
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = //GPIO_Pin_0| //wakeup bt
	                              GPIO_Pin_1| //otg_en
	                              GPIO_Pin_15; //led0
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|     //led1
	                              GPIO_Pin_1|     //led2
	                              GPIO_Pin_3|     //buzz
	                              GPIO_Pin_7|     //mcu_pwr_on
	                              GPIO_Pin_9|     //charg_en  low on
	                              //GPIO_Pin_13|    //pwr_bt    low on
																GPIO_Pin_14;    //sd_en     low on
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_7);
	GPIO_SetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_9);  //GPIO_Pin_13|  pwr_bt not able to control now
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|
	                              GPIO_Pin_2|  //mcu_tx
	                              GPIO_Pin_3|	 //mcu_rx
	                              GPIO_Pin_4|	 //bt_adc      																
																GPIO_Pin_6|  //sd_cmd
	                              GPIO_Pin_8|  //sd_d1
	                              GPIO_Pin_9|  //sd_d2
																GPIO_Pin_10|  //usb_id
	                              GPIO_Pin_11|  //usb_dm
	                              GPIO_Pin_12;  //usb_dp
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|  //sd_d0
																GPIO_Pin_5|  //sd_d3
	                              GPIO_Pin_6|  //pwr_btn_sw
	                              GPIO_Pin_8|  //charging
																GPIO_Pin_15; //sd_clk
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//BSP_BTN
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource6);
	/* Configure EXTI line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Startup with High ,and wait for low to end startup.
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//??????????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //????????
	NVIC_Init(&NVIC_InitStructure);
	
}


void EXTI9_5_IRQHandler(void)
{

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(EXTI_Line6);
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==1)
	{	
		Button.press_state=BUTTON_PRESSED;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	}
	
	else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0)
	{	
		Button.press_state=BUTTON_RELEASED;
		if (PowerOn_Released==0) //ensure this is the realse action of power on
		{
			PowerOn_Released=1;
		}
		else
		{
				;
		}
		
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
	}


}



void bsp_LED0On(void) 			{GPIO_SetBits(GPIOA,GPIO_Pin_15);}
void bsp_LED0Off(void)			{GPIO_ResetBits(GPIOA,GPIO_Pin_15);}

void bsp_LED1On(void)  	    {GPIO_SetBits(GPIOB,GPIO_Pin_0);}
void bsp_LED1Off(void)      {GPIO_ResetBits(GPIOB,GPIO_Pin_0);}

void bsp_LED2On(void)  	    {GPIO_SetBits(GPIOB,GPIO_Pin_1);}
void bsp_LED2Off(void)      {GPIO_ResetBits(GPIOB,GPIO_Pin_1);}


void bsp_SDPWROn(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	ForDelay_nms(5);
}
void bsp_SDPWROff(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
	//f_mount(&fatfs_SDCARD,NULL,1 ); //–∂‘ÿŒƒº˛œµÕ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);//SDIO ±÷” πƒ‹

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_6; 	//PC8,9,10,11,12∏¥”√π¶ƒ‹ ‰≥ˆ	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//∏¥”√π¶ƒ‹
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100M
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//…œ¿≠
  GPIO_Init(GPIOA, &GPIO_InitStructure);// PC8,9,10,11,12∏¥”√π¶ƒ‹ ‰≥ˆ

	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//PD2∏¥”√π¶ƒ‹ ‰≥ˆ
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
}


