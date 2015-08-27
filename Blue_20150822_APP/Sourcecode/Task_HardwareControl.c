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
#include "Task_HardwareControl.h"

struct led LEDRed={LED_MODE_OFF,LED_STATUS_OFF,LED_BLINK_CONTINUE,200,bsp_LED1On,bsp_LED1Off};
struct led LEDGreen={LED_MODE_OFF,LED_STATUS_OFF,LED_BLINK_CONTINUE,200,bsp_LED2On,bsp_LED2Off};
struct led LEDBlue={LED_MODE_BLINK,LED_STATUS_OFF,0,100,bsp_LED0On,bsp_LED0Off};
struct led LEDRed_APPCon={LED_MODE_OFF,LED_STATUS_OFF,LED_BLINK_CONTINUE,200,bsp_LED1On,bsp_LED1Off};
struct led LEDGreen_APPCon={LED_MODE_OFF,LED_STATUS_OFF,LED_BLINK_CONTINUE,100,bsp_LED2On,bsp_LED2Off};
struct led LEDBlue_APPCon={LED_MODE_OFF,LED_STATUS_OFF,LED_BLINK_CONTINUE,100,bsp_LED0On,bsp_LED0Off};

struct led* LEDRed_Act=&LEDRed;
struct led* LEDGreen_Act=&LEDGreen;
struct led* LEDBlue_Act=&LEDBlue;

struct buzz BUZZ={ BUZZ_MODE_OFF,BUZZ_STATUS_OFF,BUZZ_BEEP_CONTINUE,100,bsp_BUZZOn,bsp_BUZZOff};
struct buzz BUZZ_APPCon={ BUZZ_MODE_OFF,BUZZ_STATUS_OFF,BUZZ_BEEP_CONTINUE,200,bsp_BUZZOn,bsp_BUZZOff};

struct buzz* BUZZ_Act=&BUZZ;

extern u8 PowerOn_Released;
extern u8 STX_Received;
extern u8 L0_RxCount;
extern u8 UDISK_CONNECTED,UDISK_SUSPENDED;
extern u8 EXIT_FromRTC;
u8 Blank_Buffer[11]={0x00};
u8 MCU_JUSTEXITStop_Flag=0;
u8 BlinkStepR=0,BlinkStepG=0,BlinkStepB=0,BeepStep=0;
u8 BT_NameFlag=1;
u8 BT_Machine=1;
u16 ledrticks=0,beepticks=0,ledgticks=0,ledbticks=0,batteryticks=0,buttonticks=0,onoffticks=0,rcvticks=0,BTticks=0,accesstick=0,BT_ADV_Time=0,ExitClear_Time=0,SleepTime=0;;
u8 timesbeeped=0,timesledrblinked=0,timesledgblinked=0,timesledbblinked=0;
u16 BT_Timeout=0;
u8 Battery_Alarmed=0;
u8 ButtonControl=0;
u8 LEDRControl=0;
u8 ButtonFirstRelease=0;
extern u8 BT_IsReady;
extern L2_KEY L2_TxKeys[10];
extern u8 L2_TxKeys_Number;
extern u8 *L2_TxBuffer;
extern xQueueHandle L1_TxQue;
extern u8 L1_Tx_Sempaphore;
extern u8 WAKEUPSEMA_SEND;
extern L1_TX_STATUS L1_TxSta;
extern L1_RCV_STATUS L1_RcvSta;
extern u8 ACCESS_STATUS;
extern void SetSysClock(void);
u8 MCU_STOP_Flag=0;
u8 ControlNeedStop=0;
u16 BTtimes=0;
extern void SDIO_Register_Deinit(void);
extern u8 BT_INIT_FINISHED;
extern u8  BT_SLDN_FINISHED;
extern u8  BT_RST_FINISHED;
extern u8  BT_ADV_FINISHED;
extern u8  BT_STATE_FINISHED;
extern u8  BT_ADDRESS_FINISHED;
u8 USB_INIT_FINISHED=0;
char BT_ADDRESS[12];
FIL file_blue;
UINT blue_bw;
u8 blue_res;
u8 BT_State_ascii;
u8 BT_Initialed=0;
extern u8 BT_CMDBuffer_Index;
extern u8 BTCMD_State;
extern u8 BT_Respond;
extern u8 bsp_ChangeAPPtoRun(u8 App_Select);

void BT_Reset(void)
{
	Ble_Discon();
	bsp_BTPWROff();
	BT_IsReady=0;
	BT_State=BT_STATE_Standby;
	BT_State_old=BT_STATE_Standby;
	BT_CMDBuffer_Index=0;
	BTCMD_State=0;
	BT_Respond=0;
	STX_Received=0;
	BT_INIT_FINISHED=0;
	BT_SLDN_FINISHED=0;
	BT_RST_FINISHED=0;
	BT_ADV_FINISHED=0;
	BT_STATE_FINISHED=0;
	BT_ADDRESS_FINISHED=0;
	BT_Machine=1;
	BT_Initialed=0;
	ForDelay_nms(3000);
	bsp_BTPWRInit();
	bsp_BTPWROn();
	ForDelay_nms(500);
  USART2_Init();
}
void bsp_USB_Shutdown(void)
{

//	memset((u8*)(0x50000000),0x00,0x10f*sizeof(u8));
	  RCC_AHB2PeriphResetCmd( RCC_AHB2Periph_OTG_FS, ENABLE);//
	  RCC_AHB2PeriphClockCmd( RCC_AHB2Periph_OTG_FS, DISABLE) ; 	
}


void bsp_EnterStop(void)
{

	 GPIO_InitTypeDef GPIO_InitStructure;
	 EXTI_InitTypeDef EXTI_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	 //USB_OTG_DCTL_TypeDef mydctl;
	
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	//RCC_AHB2PeriphClockCmd( RCC_AHB2Periph_OTG_FS, ENABLE) ; 
  //RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_OTG_FS,ENABLE);
	
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_15;//|GPIO_Pin_2;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOA,&GPIO_InitStructure);
	 //GPIO_SetBits(GPIOA,GPIO_Pin_2);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_15);
	
	
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
	                             GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOA,&GPIO_InitStructure);
	 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;   //SWDIO
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	 GPIO_Init(GPIOA,&GPIO_InitStructure);
	 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;   //SWCLK
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
	 GPIO_Init(GPIOA,&GPIO_InitStructure);
	
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_14;//|GPIO_Pin_13
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOB,&GPIO_InitStructure);
	 GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3);
   GPIO_SetBits(GPIOB,GPIO_Pin_14);
//	 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOB,&GPIO_InitStructure);
	 
	 	
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOC,&GPIO_InitStructure);
	 GPIO_ResetBits(GPIOC,GPIO_Pin_15);
	 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	 GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOH,&GPIO_InitStructure);
	 GPIO_ResetBits(GPIOH,GPIO_Pin_1);
	 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	 GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOH,&GPIO_InitStructure);
	 
	
	 
	 USART_Cmd(USART2, DISABLE); 
	 ADC_Cmd(ADC1, DISABLE);	 
	 SDIO_DeInit();
	 SDIO_Register_Deinit();
	 TIM_Cmd(TIM2,DISABLE);
	 DMA_Cmd(DMA2_Stream3,DISABLE);
	 
	 bsp_USB_Shutdown();
	 //mydctl.b.sftdiscon=1;
	 //USB_OTG_MODIFY_REG32(&USB_OTG_Core.regs.DREGS->DCTL,0,mydctl.d32);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);
	/* Configure EXTI line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//??????????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //????????
	NVIC_Init(&NVIC_InitStructure);
//	
	

	 USART_DeInit(USART2);
	 //EXTI_DeInit();
   ADC_DeInit();	 
	 TIM_DeInit(TIM2);
	 DMA_DeInit(DMA2_Stream3);
	 USBD_DeInit(&USB_OTG_Core);
	   NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
     NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
     NVIC_Init(&NVIC_InitStructure);  
     NVIC_DisableIRQ(OTG_FS_IRQn);
		 
		   NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	  NVIC_DisableIRQ(SDIO_IRQn);

	 RCC_AHB2PeriphClockCmd( RCC_AHB2Periph_OTG_FS, DISABLE) ; 	
	 
	 
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, DISABLE);	
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, DISABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, DISABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, DISABLE);
	 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,DISABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,DISABLE);
	 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,DISABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);
	 //RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_OTG_FS,DISABLE);
	
	//vTaskSuspendAll();
	//PWR_WakeUpPinCmd(ENABLE);
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);	
	//PWR_EnterSTANDBYMode();
	
	//xTaskResumeAll();
}

void bsp_ExitStop(void)
{	  
	SetSysClock();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	bsp_GpioInit();
  bsp_SDPWROn();
	bsp_OTGPwrOff();
	bsp_ChargeOn();
	USART2_Init_withoutTx();
	bsp_AdcInit();
	SD_Init();
  //bsp_FSInit();
	RCC_AHB2PeriphResetCmd( RCC_AHB2Periph_OTG_FS, DISABLE);
	USBD_Init(&USB_OTG_Core,        
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_MSC_cb, 
            &USRD_cb);
	USB_MS.USB_Mode_Changed=0;
	USB_MS.USB_Mode_Select=0;
	USB_MS.USB_Mode_Pre=0;	
  /* Infinite loop */
	 if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  Button.press_type=BUTTON_NONE;
	//xTaskResumeAll();
}

void EXTI3_IRQHandler(void)
{
		 EXTI_InitTypeDef EXTI_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	  EXTI_ClearITPendingBit(EXTI_Line3);
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//??????????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;    //????????
	NVIC_Init(&NVIC_InitStructure);
}

void BSP_Task(void * pvParameters)
{  

	portTickType xLastWakeTime;
	xLastWakeTime=xTaskGetTickCount();

	
  while (1)
  {
		  if(BT_ADV_FINISHED&&USB_INIT_FINISHED==0)
			{
									
					USBD_Init(&USB_OTG_Core,        
										USB_OTG_FS_CORE_ID,
										&USR_desc,
										&USBD_MSC_cb, 
										&USRD_cb);
					USB_MS.USB_Mode_Changed=0;
					USB_MS.USB_Mode_Select=0;
					USB_MS.USB_Mode_Pre=0;
				  USB_INIT_FINISHED=1;
			}
		
		  if(
				(LEDRed_APPCon.mode==LED_MODE_BLINK&&LEDRed_APPCon.blink_mode!=0&&timesledrblinked>=LEDRed_APPCon.blink_mode) && 
				(LEDGreen_APPCon.mode==LED_MODE_BLINK&&LEDGreen_APPCon.blink_mode!=0&&timesledgblinked>=LEDGreen_APPCon.blink_mode) &&
				(LEDBlue_APPCon.mode==LED_MODE_BLINK&&LEDBlue_APPCon.blink_mode!=0&&timesledbblinked>=LEDBlue_APPCon.blink_mode) &&
				(BUZZ_APPCon.mode==BUZZ_MODE_BEEP&&BUZZ_APPCon.beep_mode!=0&&timesbeeped>=BUZZ_APPCon.beep_mode)&&ControlNeedStop
			  )
			{
								LEDRed.statusnow=LED_STATUS_OFF;
						    LEDGreen.statusnow=LED_STATUS_OFF;
						    LEDBlue.statusnow=LED_STATUS_OFF;
						    BUZZ.statusnow=BUZZ_STATUS_OFF;
						    LEDRed.ledoff();
						    LEDGreen.ledoff();
						    LEDBlue.ledoff();
						    BUZZ.buzzoff();
						    BeepStep=0;
						    BlinkStepR=0;
						    BlinkStepG=0;
						    BlinkStepB=0;
								LEDRed_Act=&LEDRed;
						    LEDGreen_Act=&LEDGreen;
								LEDBlue_Act=&LEDBlue;
								BUZZ_Act=&BUZZ;
				        ControlNeedStop=0;
			}
		
		  if(ButtonControl==0)  //Do not control buzz when button sound need to display
			{
    		switch(BUZZ_Act->mode)
			{
				case BUZZ_MODE_ON:
					if(BUZZ_Act->statusnow==BUZZ_STATUS_OFF)
					{	
						BUZZ_Act->buzzon();
						BUZZ_Act->statusnow=BUZZ_STATUS_ON;
					}
					break;
				case BUZZ_MODE_OFF:
					if(BUZZ_Act->statusnow==BUZZ_STATUS_ON)
					{
						BUZZ_Act->buzzoff();
						BUZZ_Act->statusnow=BUZZ_STATUS_OFF;
					}
					break;
				case BUZZ_MODE_BEEP:
					beepticks++;	
					{
						if(BUZZ_Act->beep_mode==BUZZ_BEEP_CONTINUE) ///////////////////////////////////Continue Beep//////////////////
	          {
							 if(BeepStep==0)
							 {
								beepticks=0;
								BUZZ_Act->buzzon();
								BUZZ_Act->statusnow=BUZZ_STATUS_ON;
								BeepStep++;
							 }
							 else if(BeepStep==1)
							 {
									if(beepticks>=BUZZ_Act->beep_cycle/2)  
									{
										beepticks=0;
										BUZZ_Act->buzzoff();
										BeepStep++;
									}
							 }
							 else if(BeepStep==2)
							 {
								 	if(beepticks>=BUZZ_Act->beep_cycle)  
									{
                   // printf("AT+B ADV 1\r");
										BeepStep=0;
									}
							 }
						}
						///////////////////////////////////Times Beep//////////////////
						else
						{
							 if(timesbeeped<BUZZ_Act->beep_mode)
							 {
								 if(BeepStep==0)
								 {
									beepticks=0;
									BUZZ_Act->buzzon();
									BUZZ_Act->statusnow=BUZZ_STATUS_ON;
									BeepStep++;
								 }
								 else if(BeepStep==1)
								 {
											if(beepticks>=BUZZ_Act->beep_cycle/2)  
											{
												beepticks=0;
												BUZZ_Act->buzzoff();
												BeepStep++;
											}
									 }
								 else if(BeepStep==2)
								 {
										if(beepticks>=BUZZ_Act->beep_cycle)  
										{
										 // printf("AT+B ADV 1\r");
											BeepStep=0;
											timesbeeped++;
										}
								 }
								 
							 }
						}
          }
					break;
				
				default:
					break;
			}
		}
		
		if(LEDRControl==0)
		{
		  switch(LEDRed_Act->mode)
			{
				case LED_MODE_ON:
					if(LEDRed_Act->statusnow==LED_STATUS_OFF)
					{
						LEDRed_Act->ledon();
						LEDRed_Act->statusnow=LED_STATUS_ON;
					}
					break;
				case LED_MODE_OFF:
					if(LEDRed_Act->statusnow==LED_STATUS_ON)
					{
						LEDRed_Act->ledoff();
						LEDRed_Act->statusnow=LED_STATUS_OFF;
					}
					break;
				case LED_MODE_BLINK:
					 ledrticks++;
					{
						if(LEDRed_Act->blink_mode==LED_BLINK_CONTINUE)///////////////////////////////////Continue blink//////////////////
						{
							 if(BlinkStepR==0)
							 {
								
								LEDRed_Act->ledoff();
								LEDRed_Act->statusnow=LED_STATUS_OFF;
								BlinkStepR++;
							 }
							 else if(BlinkStepR==1)
							 {
									if(ledrticks>=(LEDRed_Act->blink_cycle-5))  
									{
										LEDRed_Act->ledon();
										LEDRed_Act->statusnow=LED_STATUS_ON;
										BlinkStepR++;
									}
							 }
							 else if(BlinkStepR==2)
							 {
									if(ledrticks>=LEDRed_Act->blink_cycle)  
									{
										BlinkStepR=0;
										ledrticks=0;
									}
							 }
						}
						else 
						{
							 if(timesledrblinked<LEDRed_Act->blink_mode)
							 {
								 	 if(BlinkStepR==0)
									 {
										
										LEDRed_Act->ledoff();
										LEDRed_Act->statusnow=LED_STATUS_OFF;
										BlinkStepR++;
									 }
									 else if(BlinkStepR==1)
									 {
											if(ledrticks>=(LEDRed_Act->blink_cycle-5))  
											{
												LEDRed_Act->ledon();
												LEDRed_Act->statusnow=LED_STATUS_ON;
												BlinkStepR++;
											}
									 }
									 else if(BlinkStepR==2)
									 {
											if(ledrticks>=LEDRed_Act->blink_cycle)  
											{
												BlinkStepR=0; 
												ledrticks=0;
												LEDRed_Act->ledoff();
										    LEDRed_Act->statusnow=LED_STATUS_OFF;
												timesledrblinked++;
											}
									 }
									
							 }
						}
						
					}
					break;
				default:
					break;
			}  
		}
			
			switch(LEDGreen_Act->mode)/////////////////////////////////LEDGreen////////////////////////////////////
			{
				case LED_MODE_ON:
					if(LEDGreen_Act->statusnow==LED_STATUS_OFF)
					{
						LEDGreen_Act->ledon();
						LEDGreen_Act->statusnow=LED_STATUS_ON;
					}
					break;
				case LED_MODE_OFF:
					if(LEDGreen_Act->statusnow==LED_STATUS_ON)
					{
						LEDGreen_Act->ledoff();
						LEDGreen_Act->statusnow=LED_STATUS_OFF;
					}
					break;
				case LED_MODE_BLINK:
					ledgticks++;
					{
						if(LEDGreen_Act->blink_mode==LED_BLINK_CONTINUE)///////////////////////////////////Continue blink//////////////////
						{
							 if(BlinkStepG==0)
							 {
								
								LEDGreen_Act->ledoff();
								LEDGreen_Act->statusnow=LED_STATUS_OFF;
								BlinkStepG++;
							 }
							 else if(BlinkStepG==1)
							 {
									if(ledgticks>=(LEDGreen_Act->blink_cycle-5))  
									{
										LEDGreen_Act->ledon();
										LEDGreen_Act->statusnow=LED_STATUS_ON;
										BlinkStepG++;
									}
							 }
							 else if(BlinkStepG==2)
							 {
									if(ledgticks>=LEDGreen_Act->blink_cycle)  
									{
										BlinkStepG=0;
										ledgticks=0;
									}
							 }
						}
						else 
						{
							 if(timesledgblinked<LEDGreen_Act->blink_mode)
							 {
								 	 if(BlinkStepG==0)
									 {
										
										LEDGreen_Act->ledoff();
										LEDGreen_Act->statusnow=LED_STATUS_OFF;
										BlinkStepG++;
									 }
									 else if(BlinkStepG==1)
									 {
											if(ledgticks>=(LEDGreen_Act->blink_cycle-5))  
											{
												LEDGreen_Act->ledon();
												LEDGreen_Act->statusnow=LED_STATUS_ON;
												BlinkStepG++;
											}
									 }
									 else if(BlinkStepG==2)
									 {
											if(ledgticks>=LEDGreen_Act->blink_cycle)  
											{
												BlinkStepG=0;
												ledgticks=0;
												LEDGreen_Act->ledoff();
										    LEDGreen_Act->statusnow=LED_STATUS_OFF;
												timesledgblinked++;
											}
									 }
									 
							 }
						}
						
					}
					break;
				default:
					break;
			}  
			
			switch(LEDBlue_Act->mode)/////////////////////////////////LEDBlue////////////////////////////////////
			{
				case LED_MODE_ON:
					if(LEDBlue_Act->statusnow==LED_STATUS_OFF)
					{
						LEDBlue_Act->ledon();
						LEDBlue_Act->statusnow=LED_STATUS_ON;
					}
					break;
				case LED_MODE_OFF:
					if(LEDBlue_Act->statusnow==LED_STATUS_ON)
					{
						LEDBlue_Act->ledoff();
						LEDBlue_Act->statusnow=LED_STATUS_OFF;
					}
					break;
				case LED_MODE_BLINK:
					ledbticks++;
					{
						if(LEDBlue_Act->blink_mode==LED_BLINK_CONTINUE)///////////////////////////////////Continue blink//////////////////
						{
							 if(BlinkStepB==0)
							 {
								
								LEDBlue_Act->ledoff();
								LEDBlue_Act->statusnow=LED_STATUS_OFF;
								BlinkStepB++;
							 }
							 else if(BlinkStepB==1)
							 {
									if(ledbticks>=(LEDBlue_Act->blink_cycle-5))  
									{
										LEDBlue_Act->ledon();
										LEDBlue_Act->statusnow=LED_STATUS_ON;
										BlinkStepB++;
									}
							 }
							 else if(BlinkStepB==2)
							 {
									if(ledbticks>=LEDBlue_Act->blink_cycle)  
									{
										BlinkStepB=0;
										ledbticks=0;
									}
							 }
						}
						else 
						{
							 if(timesledbblinked<LEDBlue_Act->blink_mode)
							 {
								 	 if(BlinkStepB==0)
									 {
										
										LEDBlue_Act->ledoff();
										LEDBlue_Act->statusnow=LED_STATUS_OFF;
										BlinkStepB++;
									 }
									 else if(BlinkStepB==1)
									 {
											if(ledbticks>=(LEDBlue_Act->blink_cycle-5))  
											{
												LEDBlue_Act->ledon();
												LEDBlue_Act->statusnow=LED_STATUS_ON;
												BlinkStepB++;
											}
									 }
									 else if(BlinkStepB==2)
									 {
											if(ledbticks>=LEDBlue_Act->blink_cycle)  
											{
												BlinkStepB=0;
												ledbticks=0;
												LEDBlue_Act->ledoff();
										    LEDBlue_Act->statusnow=LED_STATUS_OFF;
												timesledbblinked++;
											}
									 }
									 
							 }
						}
						
					}
						
					
					break;
				default:
					break;
			}  
			if(BT_State_old!=BT_State)
			{
				  BT_State_old=BT_State;
				  BT_State_ascii=BT_State+'0';
					blue_res=f_open(&file_blue, "0:/Blue_State.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
					blue_res=f_write(&file_blue,&BT_State_ascii,1,&blue_bw);
					blue_res=f_close(&file_blue);
			}
		  if(BT_State!=BT_STATE_Connected&&BT_State!=BT_STATE_Advertising&&BT_Initialed==0)
			{
				LEDRed.ledon();
				
				LEDRed.statusnow=LED_STATUS_ON;
				switch (BT_Machine){
					case 0:
						  printf("AT+B STATE\r");
					    BT_Machine=1;
					    break;
					
					case 1:
	            BT_IsReady=0;
					//printf("AT+B DORMANT\r");
//					    bsp_WakeUpBT_High();//high to wake up
//					    ForDelay_nms(200);
//					    bsp_WakeUpBT_Low();  //low to exit sleep
//					    BTticks++;
//						   if(BTticks>50)
					      
					      //bsp_WakeUpBT_Low();
					     if(BT_INIT_FINISHED==1)
							 { 
                //printf("AT+B RST\r");
								printf("AT+B GLBD\r");
					      BT_Machine=2;
							 }
							 else
							 {
								 BT_Timeout++;
								 if(BT_Timeout>=100)
								 {
									 BT_Timeout=0;
									 //BT_HardReset();
									 BT_Machine=1;
								 }
							 }

							 
					 
					    break;
					case 2:

							 
							 //if(BT_RST_FINISHED==1)
					     if(BT_ADDRESS_FINISHED==1)
							 { 
								 //printf("AT+B SLDN ABKEY\r");
								 printf("AT+B SLDN ABKEY%s\r",BT_ADDRESS);
								 //printf("AT+B SLDN ABKEY_001583D25B00\r");
								 //printf("AT+B SLDN ABKEY001583D25B00\r");
							   BT_Machine=3;   //>0.5s no respond than re send;
							 }
							 else
							 {
								 BT_Timeout++;
								 if(BT_Timeout>=100)
								 {
									 BT_Timeout=0;
									 //BT_HardReset();
									  BT_Machine=1;
								 }
							 }
						 break;
					case 3:
//						BTticks++;
//						if(BTticks>20)
//						{
//							BTticks=0;
//							//printf("AT+B STATE\r");
//							BT_Machine=3;
//						}
					
					    if(BT_SLDN_FINISHED==1)
							{
								printf("AT+B ADV 1\r");
								BT_Machine=4;
							}
								 else
							 {
								 BT_Timeout++;
								 if(BT_Timeout>=100)
								 {
									 BT_Timeout=0;
									 //BT_HardReset();
									  BT_Machine=1;
								 }
							 }
					  break;
					case 4:
						if(BT_State==BT_STATE_Advertising||BT_State==BT_STATE_Connected)
						{
							BT_Machine=4;
							BT_ADV_Time=0;
						}
						else
//						{
//							  BTticks++;
//						    if(BTticks>20)
//							  {
//										 BTticks=0;
//										 printf("AT+B ADV 1\r");
//										 BT_ADV_Time++;
//							   }
//								if(BT_ADV_Time>5) //5 times try no result then reset bt
//									{
//										//bsp_BTPwrOff();
//					          ForDelay_nms(200);
//					          //bsp_BTPwrOn();
//										BT_ADV_Time=0;
//										BT_Machine=0;
//									}
						
						
//							}
						  if(BT_ADV_FINISHED==1)
							{
								printf("AT+B ADV 1\r");
								BT_Machine=4;
							}
								 else
							 {
								 BT_Timeout++;
								 if(BT_Timeout>=100)
								 {
									 BT_Timeout=0;
									 //BT_HardReset();
									  BT_Machine=1;
								 }
							 }
						
						 break;
					case 5:
						 BT_Machine=0;
						 break;
			   }
			}
			else
			{
				BT_Initialed=1;
			}
			
			batteryticks++;
			if(batteryticks>=200) //every 2 sec get batter level
			{
				batteryticks=0;
				//bsp_EnterStop();
				Battery_Level=Get_Battery();
				if(Battery_Level<=Battery_Threshold)
				{
					if(Battery_Alarmed==0)
					{
						Battery_Alarmed=1;
						timesbeeped=0;
						BUZZ.mode=BUZZ_MODE_BEEP;
						BUZZ.beep_mode=BUZZ_BEEP_ONCE;
						beepticks=0;
						BUZZ.buzzoff();
						BUZZ.statusnow=BUZZ_STATUS_OFF;
					}
					LEDRed.mode=LED_MODE_BLINK;
				}
				else
				{
					LEDRed.mode=LED_MODE_OFF;
					Battery_Alarmed=0;
				}
				if(Battery_Level<1)
				{
//					Battery_ShutDown_Times++;
//				}
//				if(Battery_ShutDown_Times>2)
//				{
						  LEDRed.statusnow=LED_STATUS_OFF;
							LEDGreen.statusnow=LED_STATUS_OFF;
							LEDBlue.statusnow=LED_STATUS_OFF;
							BUZZ.statusnow=BUZZ_STATUS_OFF;
							LEDRed.ledoff();
							LEDGreen.ledoff();
							LEDBlue.ledoff();
							BUZZ.buzzoff();
							onoffticks=0;
					    Ble_Discon();
							//printf("AT+B DISCON\r"); //disconnect before pwroff
							//ForDelay_nms(2000);
							bsp_MCUPWROff();//turn off power 
					    bsp_BTPWROff();
							while(1)
							{
									;
							}
				}

			}
			
			
			if(BT_State!=0x03) //not connected
			{
				accesstick=0;
			}
			else
			{
				if(ACCESS_STATUS==0) //not accessed
				{
					accesstick++;
					if(accesstick>=1000)  //>= 10s,no access then disconnect
					{
						accesstick=0;
						Ble_Discon();
						//printf("AT+B DISCON\r");
						//printf("AT+B RST\r");
					}
			  }
				else
				{
					accesstick=0;
				}
			}
			
				
			if(Button.press_type!=BUTTON_NONE)
			{				
        if(Button.press_type==BUTTON_DOUBLE_PRESS)//&&ACCESS_STATUS==1)
        {
					 bsp_ChangeAPPtoRun(1);
					 L2_TxKeys[0].L2_KEY=KEY_KEYBACK;
					 L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=1;
					 L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
					 L2_TxKeys[0].L2_KEY_Value=&Button.press_type;
					 L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_DEVICE_KEYBACK;
					 L2_TxKeys_Number=1;
//					 while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
					 xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
					 buttonticks=0;
					 Button.press_type=BUTTON_NONE;
				}
				else if(Button.press_type==BUTTON_SINGLE_PRESS) //>0.5 sec no double click will send a single button 
				{
						 buttonticks++;
						 if(buttonticks>=50)//&&ACCESS_STATUS==1)
						 {		
								 L2_TxKeys[0].L2_KEY=KEY_KEYBACK;
								 L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=1;
								 L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								 L2_TxKeys[0].L2_KEY_Value=&Button.press_type;
								 L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_DEVICE_KEYBACK;
							   L2_TxKeys_Number=1;
//								 while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
								 xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
								 buttonticks=0;
								 Button.press_type=BUTTON_NONE;
						 }
					 
				}
			}			

				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==1)//Button.press_state==BUTTON_PRESSED )
				{
					ButtonFirstRelease=1;
					if(PowerOn_Released==1)
					{
						onoffticks++;
	          LEDRControl=1;
						LEDRed.ledon();
						LEDRed.statusnow=LED_STATUS_ON;
						
						if(Key_Sound)
						{
							ButtonControl=1;
						  BUZZ.buzzon();
							BUZZ.statusnow=BUZZ_STATUS_ON;
						}
						
						if(onoffticks>=300)// >=3 sec,then turn off
						{
							LEDRed.statusnow=LED_STATUS_OFF;
							LEDGreen.statusnow=LED_STATUS_OFF;
							LEDBlue.statusnow=LED_STATUS_OFF;
							BUZZ.statusnow=BUZZ_STATUS_OFF;
						
							onoffticks=0;
							Ble_Discon();
			
//							while(BT_State==BT_STATE_Connected);
//							{
//								printf("AT+B STATE\r"); //disconnect before pwroff
//								ForDelay_nms(200);
//							}
							bsp_BTPWROff();
							bsp_MCUPWROff();//turn off power 
							LEDRed.ledoff();
							LEDGreen.ledoff();
							LEDBlue.ledoff();  // led blue on to waste the rest power
							BUZZ.buzzoff();
							
							while(1)
							{
									;
							}
						}
					}
				}
				else
				{
					
					if(PowerOn_Released==0)
					{
						PowerOn_Released=1;
					}	
					if(ButtonFirstRelease==1)
          {
						LEDRControl=0;
					  BlinkStepR=1;//turn off led
					
					  LEDRed.ledoff();
					  LEDRed.statusnow=LED_STATUS_OFF;					
						if(Key_Sound)
						{
								ButtonControl=0;
								BUZZ.buzzoff();
								BUZZ.statusnow=BUZZ_STATUS_OFF;
						}
				  }
					onoffticks=0;
					ButtonFirstRelease=0;			
				}	
				
				if(L1_RcvSta==L1_RCVING)
				{
					rcvticks++;
					if(rcvticks>=300) //>=2s, it means a bad command
					{
						rcvticks=0;
						L1_RcvSta=L1_IDLE;  //timeout 
						
					}
				}
				else
				{
					rcvticks=0;
				}
//				if(UDISK_CONNECTED==0)
//        SleepTime++;
				if(SleepTime>=1000)
				{
					 SleepTime=0;
           MCU_STOP_Flag=1;
        	 bsp_EnterStop();
				}
        
				if(MCU_STOP_Flag==1)
				{
					while(EXIT_FromRTC)
					{
//						bsp_GpioInit();
//						bsp_LED1On();
//						ForDelay_nms(200);
						//IWDG_ReloadCounter();
						EXIT_FromRTC=0;
						bsp_EnterStop();
					}
					MCU_STOP_Flag=0;
			    bsp_ExitStop();
					//printf("AT+B STATE\r000000000");
					MCU_JUSTEXITStop_Flag=1;
				}
       
				if(MCU_JUSTEXITStop_Flag==1)
				{				

					
						ExitClear_Time++;
						if(ExitClear_Time>=50) //0.5s clear
						{
							ExitClear_Time=0;
							MCU_JUSTEXITStop_Flag=0;
							STX_Received=0;
							L1_RcvSta=L1_IDLE;
							L0_RxCount=0;
							
							memcpy(&L2_TxBuffer[L2_HEADER_CMDID_POS+1],Blank_Buffer,11);
							L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_NOTIFY;
							L2_TxKeys_Number=0;
	//					  while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
							xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
						
					  }
						
				}

			/* This task is handled periodically, each 10 mS */
    vTaskDelayUntil(&xLastWakeTime,10);
	  }
		
  }



	

