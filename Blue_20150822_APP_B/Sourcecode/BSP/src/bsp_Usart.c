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
#include "bsp_Usart.h"

		/*  Pinout & Signal
	     |Group 1             |
	     |--------------------|
			 |                    | 
			 |MCU_TX---USART2--PA2| 
			 |MCU_RX---USART2--PA3|                             
	     ---------------------|
	*/
extern xQueueHandle L0_RxQue,L0_TxQue,L1_TxQue,usart1_TxQue;
u8 BT_CMDBufferIndex=0;
char BT_CMDBufferRx[40]={0};

//xSemaphoreHandle xSemaphore
extern u8 L0_RxBuffer[20];
extern u8 L0_TxBuffer[20];
extern char BT_ADDRESS[20];
u8 DTS=0;
u8 L0_RxCount=0;
u8 L0_TxCount=0;
extern u8 WAKEUPSEMA_SEND;
extern u8 WAKEUPSEMA_RCV;
extern u8 BT_Machine;
extern u16 SleepTime;
extern u8 L1_TxSta;
extern u8 *L2_TxBuffer;
extern u8 L2_TxKeys_Number;
extern u8 ACCESS_STATUS;
u8 BT_IsReady=0;
u8 BT_State=BT_STATE_Standby;
u8 BT_State_old=BT_STATE_Standby;
u8 BT_CMDBuffer_Index=0;
u8 BTCMD_State=0;
u8 BT_Respond=0;
u8 STX_Received=0;
u8 Version_Received=0;
u8  BT_INIT_FINISHED=0;
u8  BT_SLDN_FINISHED=0;
u8  BT_RST_FINISHED=0;
u8  BT_ADV_FINISHED=0;
u8  BT_STATE_FINISHED=0;
u8  BT_ADDRESS_FINISHED=0;

u8 APPTest_Buffer[2097]={0xab,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x00};
extern u8 Blank_Buffer[11];
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;     
	return ch;
}
#endif 

void USART2_Init(void)
{


	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	

	//usart1_TxQue=xQueueCreate(64,sizeof(u8));
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
 	 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 
	
	//USART1_TX   PA.9 PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
  USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure); 
  USART_Cmd(USART2, ENABLE); 
	USART_ClearFlag(USART2, USART_FLAG_TC);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_TXE, DISABLE);


	//Usart1 NVIC 
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	
	
}

void USART2_Init_withoutTx(void)
{


	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	

	//usart1_TxQue=xQueueCreate(64,sizeof(u8));
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
 	 
	//GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 
	
	//USART1_TX   PA.9 PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
  USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure); 
  USART_Cmd(USART2, ENABLE); 
	USART_ClearFlag(USART2, USART_FLAG_TC);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_TXE, DISABLE);


	//Usart1 NVIC 
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	
	
}

void USART2_IRQHandler(void)    //User debug
{
	u8 rcv;
  portBASE_TYPE usart1_Que_Woken=pdFALSE;
	  SleepTime=0;

		if(USART_GetITStatus(USART2,USART_IT_TXE)!=RESET)
		{					

			USART_ClearITPendingBit(USART1,USART_IT_TXE);
			if( xQueueReceiveFromISR( L0_TxQue, &DTS, &usart1_Que_Woken ) == pdTRUE )
			{
				bsp_LED2On();
	      LEDGreen.statusnow=LED_STATUS_ON;
				/* A character was retrieved from the queue so can be sent to the
				THR now. */
					USART_SendData( USART2,DTS);	
			}
			else
			{
				USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
			}
		}
				
			
		
		
		if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
		{
				USART_ClearITPendingBit(USART2,USART_IT_RXNE);
				rcv=USART_ReceiveData(USART2);   //Save to L0 Buffer, Send to L1 every 20 bytes.
//				bsp_LED2On();
//	      LEDGreen.statusnow=LED_STATUS_ON;
			  switch(BTCMD_State)  //if it's a BTCMD deal it;
				{
					case BTCMD_NONE:
						if(rcv=='A')
							BTCMD_State=BTCMD_A;
						else
							BTCMD_State=BTCMD_NONE;
						break;
					case BTCMD_A:
								if(rcv=='T')
							BTCMD_State=BTCMD_T;
						else
							BTCMD_State=BTCMD_NONE;
						break;
					case BTCMD_T:
								if(rcv=='-')
							BTCMD_State=BTCMD_E;
						else
							BTCMD_State=BTCMD_NONE;
						break;
					case BTCMD_E:
							if(rcv=='B')
							{
								L0_RxCount=0;
								BTCMD_State=BTCMD_WAITR;
								//L0_RxCount=0;// if it's a BTCMD then clear L0 Rx Data"AT-B"
							}
						else
							BTCMD_State=BTCMD_NONE;
						break;
	
					case BTCMD_WAITR:
						BT_CMDBufferRx[BT_CMDBufferIndex++]=rcv;
						if(rcv=='\r')
						{
							L0_RxCount=0;
							BT_CMDHandler();
							BT_CMDBufferIndex=0;
							BTCMD_State=BTCMD_NONE;
						}
				}
				
				if(STX_Received!=1)
				{
						if(rcv==0xAB)
						{
							STX_Received=1;
				      L0_RxBuffer[L0_RxCount++]=rcv;
						}
						else if(rcv==0xaf) //wakeup byte
						{
							memcpy(&L2_TxBuffer[L2_HEADER_CMDID_POS+1],Blank_Buffer,11);
							L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_PING;
							L2_TxKeys_Number=0;
							//while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
							xQueueSendToBackFromISR(L1_TxQue,&WAKEUPSEMA_SEND,&usart1_Que_Woken);	
						}
				}
				else
				{
					if(Version_Received!=1)
					{
						
						if(rcv!=0xAB)
						{
							Version_Received=1;
							L0_RxBuffer[L0_RxCount++]=rcv;
						}
					
					}
					else
					{
						L0_RxBuffer[L0_RxCount++]=rcv;
						if(L0_RxCount>=20)
						{
							L0_RxCount=0;
							xQueueSendToBackFromISR(L0_RxQue,L0_RxBuffer,&usart1_Que_Woken);
						}
					}
				}	
			}	
		portEND_SWITCHING_ISR(usart1_Que_Woken);

}

void BT_CMDHandler(void)
{
	BT_IsReady=1;
	if(BT_CMDBufferRx[0]==0x20&&BT_CMDBufferRx[1]==0x53&&BT_CMDBufferRx[2]==0x54&&BT_CMDBufferRx[3]==0x41&&
		 BT_CMDBufferRx[4]==0x54&&BT_CMDBufferRx[5]==0x45&&BT_CMDBufferRx[6]==0x20)
	{
		 BT_State=BT_CMDBufferRx[7]-0x30;
		 BT_STATE_FINISHED=1;
		 if(BT_State==0x03)
		 {
			 BT_Machine=0; //connected then clear BT machine
		 }
		 else if(BT_State==0x01)
		 {
			 ACCESS_STATUS=0;
		 }
		 else if(BT_State==0x00)
		 {
			 BT_INIT_FINISHED=1;
		 }
	}
	
	else if(BT_CMDBufferRx[0]==' '&&BT_CMDBufferRx[1]=='S'&&BT_CMDBufferRx[2]=='L'&&BT_CMDBufferRx[3]=='D'&&
		 BT_CMDBufferRx[4]=='N'&&BT_CMDBufferRx[5]==' ')
	{
		 BT_SLDN_FINISHED=1;
	}
	
	else if(BT_CMDBufferRx[0]==' '&&BT_CMDBufferRx[1]=='R'&&BT_CMDBufferRx[2]=='S'&&BT_CMDBufferRx[3]=='T'&&
		 BT_CMDBufferRx[4]==' ')
	{
		 BT_RST_FINISHED=1;
	}
	else if(BT_CMDBufferRx[0]==' '&&BT_CMDBufferRx[1]=='A'&&BT_CMDBufferRx[2]=='D'&&BT_CMDBufferRx[3]=='V'&&
		 BT_CMDBufferRx[4]==' ')//&&BT_CMDBufferRx[5]==' '&&BT_CMDBufferRx[6]=='0')
	{
		 BT_ADV_FINISHED=1;
	}
	else if(BT_CMDBufferRx[0]==' '&&BT_CMDBufferRx[1]=='G'&&BT_CMDBufferRx[2]=='L'&&BT_CMDBufferRx[3]=='B'&&
		 BT_CMDBufferRx[4]=='D'&&BT_CMDBufferRx[5]==' '&&BT_CMDBufferRx[6]=='0'&&BT_CMDBufferRx[7]==',')//&&BT_CMDBufferRx[5]==' '&&BT_CMDBufferRx[6]=='0')
	{
		 memcpy(BT_ADDRESS,&BT_CMDBufferRx[9],12);
		 //BT_ADDRESS[12]='\0';
		 BT_ADDRESS_FINISHED=1;
	}
}

