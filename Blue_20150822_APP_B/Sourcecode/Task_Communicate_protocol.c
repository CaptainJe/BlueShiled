/***************************************************************************
 * Name:Task_Communicate_protocol.c 				                                                
 *											
 * Function:	Communicate protocol with Andriod APP, 
							Focused on L1 layer;  L0 Layer was applicated in low lever
							"bsp_Usart.c" ; L2 layer was abstracted as keys.
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#include "Task_Communicate_protocol.h"
#include "bsp_Delay.h"
u8 L0_RxBuffer[20]={0};
u8 L0_RxBuffer_Temp[20]={0};

u8 L0_TxBuffer[20]={0};
u8 L0_TxBuffer_Temp[20]={0};

u8 L1_RxBuffer[2100]={0}; //2097
u8 L1_TxBuffer[2100]={L1_HEADER_MAGIC,L1_HEADER_VERSION,0x00}; //2097

u8 *L2_TxBuffer=L1_TxBuffer+L1_HEADER_SIZE;
u8 *L2_RxBuffer=L1_RxBuffer+L1_HEADER_SIZE;
u8 *L2_TxKeyBuffer=L1_TxBuffer+L1_HEADER_SIZE+L2_HEADER_SIZE;

u8 SD_IN=1;
u8 L1_ACK_Buffer[20]={L1_HEADER_MAGIC,L1_HEADER_VERSION,0x00};
L1_RCV_STATUS L1_RcvSta=L1_IDLE;
L1_TX_STATUS L1_TxSta=L1_TX_IDLE;

L1Header_t L1_RxHeader,L1_TxHeader;
L2DataHeader_t L2_RxHeader,L2_TxHeader;

extern xQueueHandle usart1_RxQue,usart1_TxQue,L1_TxQue;
extern xQueueHandle	L0_RxQue,L0_TxQue;

extern u8 BlinkStepR,BlinkStepG,BlinkStepB,BeepStep;

u8 ACCESS_STATUS=0;
u8 L1_Resend_Times=0;
u8 L1_Tx_Sempaphore=0x01;
L2_KEY L2_RxKeys[10],L2_TxKeys[10];
u8 L2_TxKeys_Number=0;
int32_t Crp_Sta = AES_SUCCESS;
u8 Decrp_Buffer[32]={0};
u8 Encrp_Buffer[32]={0};
uint32_t Crp_Length=0;
uint32_t RNG_Number=0;
uint32_t KN_Rcv=0;
u16 Tx_SequenceID=1;
u8 Delay_20Bytes=70;
extern u8 timesbeeped,timesledrblinked,timesledgblinked,timesledbblinked;
u8 WAKEUPSEMA_RCV=0;
u8 WAKEUPSEMA_SEND=1;
extern u8 SDIF_Read[2048];
u8 Battery_Test[2]={0};
extern double Battery_Voltage;
extern u16 FilteredADC;
extern u8 STX_Received;
extern u8 Version_Received;
u16 MCU_ReadLength=0;
extern u16 SleepTime;
void BT_Reset(void);
u8 *controlkey=0;
u8 controlkeyindex=0;
extern u8 ControlNeedStop;
extern u16 ledrticks,beepticks,ledgticks,ledbticks;

const u8 chCRCHTable[] =                                 // CRC ??????
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40
};

const u8 chCRCLTable[] =                                 // CRC ??????
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
0x41, 0x81, 0x80, 0x40
};
uint32_t GetRandNumber(void)
{
	srand(xTaskGetTickCount());
	return rand();
}
u16 CRC16(u8 *puchMsgg,u16 usDataLen)
{
	u8 uchCRCHi=0xFF;
	u8 uchCRCLo=0xFF;
	u16 uIndex;
	while(usDataLen--)
	{
		uIndex=uchCRCHi^*puchMsgg++;
		uchCRCHi=uchCRCLo^chCRCHTable[uIndex];
		uchCRCLo=chCRCLTable[uIndex];
		
	}
	return ((uchCRCHi<<8)|uchCRCLo);
}

void L1_RESPOND(RCV_STATUS RCV_Sta)
{	
	u8 Buffer_index=0;
	L1_ACK_Buffer[6]=L1_RxHeader.sequence_id>>8;
	L1_ACK_Buffer[7]=L1_RxHeader.sequence_id&0xff;
	if(RCV_Sta==RCV_SUCCESS)
	{
		L1_ACK_Buffer[1]=0x10;

	}
	else
	{
		L1_ACK_Buffer[1]=0x20;
	}
	L1_TxSta=L1_TX_SENDING;				
	for(Buffer_index=0;Buffer_index<20;Buffer_index++)
	{
		xQueueSendToBack(L0_TxQue,&L1_ACK_Buffer[Buffer_index],portMAX_DELAY);
		USART_ITConfig( USART2, USART_IT_TXE,ENABLE ); //Give signal to usart handler to start a 2o bytes' data transfer
	}
		ForDelay_nms(Delay_20Bytes);
  L1_TxSta=L1_TX_IDLE;				
}

void L1_Pack(void)
{
		  static u8 Key_index=0;
		  static u16 Buffer_index=0;
	    
			u16 CRC_Tx=0;
	    SleepTime=0;
			/////////////////////////////////////////Fill_L2_Header/////////////////////////////////////////

			L2_TxBuffer[L2_HEADER_VERSION_POS]= L2_HEADER_VERSION;
			
			///////////////////////////////////////FILL_L2_KEYS////////////////////////////////////////////
			for(Key_index=0,Buffer_index=0;Key_index<L2_TxKeys_Number;Key_index++)
			{
				L2_TxKeyBuffer[Buffer_index++]= L2_TxKeys[Key_index].L2_KEY;
				L2_TxKeyBuffer[Buffer_index++]= L2_TxKeys[Key_index].L2_KEY_Header.data>>8;
				L2_TxKeyBuffer[Buffer_index++]= L2_TxKeys[Key_index].L2_KEY_Header.data&0xff;
				memcpy(&L2_TxKeyBuffer[Buffer_index],L2_TxKeys[Key_index].L2_KEY_Value,L2_TxKeys[Key_index].L2_KEY_Header.key_header_bit_field.v_length);  //copy key value to L1_Buffer
				Buffer_index+=L2_TxKeys[Key_index].L2_KEY_Header.key_header_bit_field.v_length;
			}
			L1_TxHeader.payload_len=L2_HEADER_SIZE+Buffer_index;
			///////////////////////////////////////////Fill_L1_Header///////////////////////////////////////	
			
			L1_TxBuffer[L1_PAYLOAD_LENGTH_HIGH_BYTE_POS]= L1_TxHeader.payload_len>>8;
			L1_TxBuffer[L1_PAYLOAD_LENGTH_LOW_BYTE_POS]=	L1_TxHeader.payload_len&0xff;
			CRC_Tx=CRC16(L2_TxBuffer,L1_TxHeader.payload_len);
			L1_TxHeader.crc16=CRC_Tx;
			L1_TxBuffer[L1_HEADER_CRC16_HIGH_BYTE_POS]=CRC_Tx>>8;
			L1_TxBuffer[L1_HEADER_CRC16_LOW_BYTE_POS]=CRC_Tx&0xff;
						if(L2_TxBuffer[L2_HEADER_CMDID_POS]==COMMAND_NOTIFY||L2_TxBuffer[L2_HEADER_CMDID_POS]==COMMAND_PING)
			{
				
				L1_TxHeader.sequence_id=32769;
				L1_TxBuffer[L1_HEADER_SEQ_ID_HIGH_BYTE_POS]= 32769>>8;
				L1_TxBuffer[L1_HEADER_SEQ_ID_LOW_BYTE_POS]=	32769&0xff;
				
			}
			
			else
			
		 {
				L1_TxHeader.sequence_id=Tx_SequenceID;
				L1_TxBuffer[L1_HEADER_SEQ_ID_HIGH_BYTE_POS]= Tx_SequenceID>>8;
				L1_TxBuffer[L1_HEADER_SEQ_ID_LOW_BYTE_POS]=	Tx_SequenceID&0xff;
				Tx_SequenceID++;	
				if(Tx_SequenceID>32767)
				{
					Tx_SequenceID=0;
				}		
			}			
}

void L2_RxKeys_Handle(u8 KeyNum)
{
	float Freq=0;
	u16 Cycle=0;
	SleepTime=0;
	switch(L2_RxHeader.cmd_ID)
	{
		case COMMAND_TEST:
//			L2_TxKeys[0].L2_KEY=KEY_T_RESPOND;
//		  L2_TxKeys[2].L2_KEY_Value=SDIF_Read;
//			L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=513;
//			L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
//			L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_T_TRANS;
//			L2_TxKeys_Number=1;
////			while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
//			xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
		  
		  BT_Reset();
			break;
		case COMMAND_ACCESS_REQ:
			switch(L2_RxKeys[0].L2_KEY){
				case KEY_ACCESS_REQ:
					Crypto_DeInit();
          Crp_Sta= STM32_AES_ECB_Decrypt( L2_RxKeys[0].L2_KEY_Value, L2_RxKeys[0].L2_KEY_Header.key_header_bit_field.v_length, Key, Decrp_Buffer,  //Decrp and get the 4 byte an
                            &Crp_Length);
					RNG_Number = GetRandNumber();   //??
					
					Decrp_Buffer[4]=RNG_Number>>24;               //put 4 byte kn at the end of an 
					Decrp_Buffer[5]=RNG_Number>>16;
					Decrp_Buffer[6]=RNG_Number>>8;
					Decrp_Buffer[7]=RNG_Number&0xff;
				
          Crp_Sta= STM32_AES_ECB_Encrypt( Decrp_Buffer, 32, Key, Encrp_Buffer,  //encrop the an + kn 
                            &Crp_Length);
				
					L2_TxKeys[0].L2_KEY=KEY_ACCESS_RESPONSE;
					L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=L2_ACCESS_RESPONSE_KEYVALUE_LENGTH;
				  L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
				  L2_TxKeys[0].L2_KEY_Value=Encrp_Buffer;
					L2_TxKeys_Number=1;
					L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_ACCESS_REQ;
				  ACCESS_STATUS=1;
//					while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
					xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);		

					break;
				default:
					break;
			}
			break;
		case COMMAND_DEVICE_INFO:
				 if(L2_RxKeys[0].L2_KEY==KEY_ACCESS_TOKEN) // first key must be access token?
			{
				KN_Rcv=(L2_RxKeys[0].L2_KEY_Value[0]<<24|L2_RxKeys[0].L2_KEY_Value[1]<<16|L2_RxKeys[0].L2_KEY_Value[2]<<8|L2_RxKeys[0].L2_KEY_Value[3]);
				if(KN_Rcv==RNG_Number) //if kn not right, do nothing
				{
					switch(L2_RxKeys[1].L2_KEY)
					{
						case KEY_INFO_Req:
								L2_TxKeys[0].L2_KEY=KEY_INFO_Manufacturer_Name;
								L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=char_ManuName_Length;
						    L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[0].L2_KEY_Value=(u8 *)char_ManuName;
						
								L2_TxKeys[1].L2_KEY=KEY_INFO_Model_Number;
								L2_TxKeys[1].L2_KEY_Header.key_header_bit_field.v_length=char_ModelNum_Length;
						    L2_TxKeys[1].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[1].L2_KEY_Value=(u8 *)char_ModelNum;
						
						    L2_TxKeys[2].L2_KEY=KEY_INFO_Serial_Number;
								L2_TxKeys[2].L2_KEY_Header.key_header_bit_field.v_length=char_SN_Length;
						    L2_TxKeys[2].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[2].L2_KEY_Value=(u8 *)char_SN;
						
						    L2_TxKeys[3].L2_KEY=KEY_INFO_Hardware_Rev;
								L2_TxKeys[3].L2_KEY_Header.key_header_bit_field.v_length=char_HardRev_Length;
						    L2_TxKeys[3].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[3].L2_KEY_Value=(u8 *)char_HardRev;
						
								L2_TxKeys[4].L2_KEY=KEY_INFO_Firmware_Rev;
								L2_TxKeys[4].L2_KEY_Header.key_header_bit_field.v_length=char_FirmRev_Length;
								L2_TxKeys[4].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[4].L2_KEY_Value=(u8 *)char_FirmRev;
								
								L2_TxKeys[5].L2_KEY=KEY_INFO_Software_Rev;
								L2_TxKeys[5].L2_KEY_Header.key_header_bit_field.v_length=char_SoftRev_Length;
								L2_TxKeys[5].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[5].L2_KEY_Value=(u8 *)char_SoftRev;
	
						    L2_TxKeys[6].L2_KEY=KEY_INFO_Battery_Level;
								L2_TxKeys[6].L2_KEY_Header.key_header_bit_field.v_length=2;
								L2_TxKeys[6].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								Battery_Test[0]=0x00;
						    Battery_Test[1]=Battery_Level;						
								L2_TxKeys[6].L2_KEY_Value=(u8 *)Battery_Test;
								//L2_TxKeys[6].L2_KEY_Value=(u8 *)&Battery_Level;
			
								L2_TxKeys[7].L2_KEY=KEY_INFO_System_ID;
								L2_TxKeys[7].L2_KEY_Header.key_header_bit_field.v_length=char_SysID_Length;
								L2_TxKeys[7].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[7].L2_KEY_Value=(u8 *)char_SysID;					
								
								L2_TxKeys[8].L2_KEY=KEY_IST_IN;
								L2_TxKeys[8].L2_KEY_Header.key_header_bit_field.v_length=1;
								L2_TxKeys[8].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								//SD_IN=SD_Init();	
								L2_TxKeys[8].L2_KEY_Value=&SD_IN;
								
								L2_TxKeys_Number=9;	
								L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_DEVICE_INFO;
//								while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
								xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);		
							break;
				
				    case KEY_INFO_Battery_Test:
							  L2_TxKeys[0].L2_KEY=KEY_INFO_Battery_Level;
								L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=2;
						    L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
//						    Battery_Test[0]=FilteredADC>>8;
//						    Battery_Test[1]=FilteredADC&0x00ff;
//								Battery_Test[0]=Battery_Voltage_Test>>8;
//						    Battery_Test[1]=Battery_Voltage_Test&0x00ff;
						    Battery_Test[0]=0x00;
						    Battery_Test[1]=Battery_Level;						
						
								L2_TxKeys[0].L2_KEY_Value=(u8 *)Battery_Test;
						
								L2_TxKeys_Number=1;	
								L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_DEVICE_INFO;
//								while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
								xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
						    
							break;
					  default:
							break;
					}
				} 
				else
				{
					Ble_Discon();
					//printf("AT+B DISCON\r");
				}
			}
			break;
		case COMMAND_DEVICE_SETTING:
      if(L2_RxKeys[0].L2_KEY==KEY_ACCESS_TOKEN) // first key must be access token?
			{
				KN_Rcv=(L2_RxKeys[0].L2_KEY_Value[0]<<24|L2_RxKeys[0].L2_KEY_Value[1]<<16|L2_RxKeys[0].L2_KEY_Value[2]<<8|L2_RxKeys[0].L2_KEY_Value[3]);
				if(KN_Rcv==RNG_Number) //if kn not right, do nothing
				{
					switch(L2_RxKeys[1].L2_KEY)
					{
						case KEY_SETTING_QUERY:
								L2_TxKeys[0].L2_KEY=KEY_DEVICE_NAME_SET;
								L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=char_SysID_Length;
						    L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[0].L2_KEY_Value=(u8 *)char_SysID;
						
								L2_TxKeys[1].L2_KEY=KEY_DEVICE_BATTERY_THRESHOLD;
								L2_TxKeys[1].L2_KEY_Header.key_header_bit_field.v_length=1;
						    L2_TxKeys[1].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[1].L2_KEY_Value=(u8 *)&Battery_Threshold;
						
						    L2_TxKeys[2].L2_KEY=KEY_KEY_SOUND_SET;
								L2_TxKeys[2].L2_KEY_Header.key_header_bit_field.v_length=1;
						    L2_TxKeys[2].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
								L2_TxKeys[2].L2_KEY_Value=(u8 *)&Key_Sound;
						
								L2_TxKeys_Number=3;	
								L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_DEVICE_SETTING;
//								while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
								xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);		
							break;
						case KEY_DEVICE_NAME_SET:
							char_SysID_Length=L2_RxKeys[1].L2_KEY_Header.key_header_bit_field.v_length;
						  memcpy(char_SysID,L2_RxKeys[1].L2_KEY_Value,char_SysID_Length);
						  bsp_SysinfoSave();
							break;
						case KEY_DEVICE_BATTERY_THRESHOLD:
							memcpy(&Battery_Threshold,L2_RxKeys[1].L2_KEY_Value,1);
						  bsp_SysinfoSave();
						//Save to flash????
							break;
						case KEY_KEY_SOUND_SET:
							memcpy(&Key_Sound,L2_RxKeys[1].L2_KEY_Value,1);
						  bsp_SysinfoSave();
							break;
						case KEY_DELAY_SET:
							Delay_20Bytes = L2_RxKeys[1].L2_KEY_Value[0];
				      break;
					  default:
							break;
					}
				} 
        else
				{
					Ble_Discon();
					//printf("AT+B DISCON\r");
				}
			}
			break;
		case COMMAND_DEVICE_CONTROL:
//						if(L2_RxKeys[0].L2_KEY==KEY_ACCESS_TOKEN) // first key must be access token?
//			{
//				KN_Rcv=(L2_RxKeys[0].L2_KEY_Value[0]<<24|L2_RxKeys[0].L2_KEY_Value[1]<<16|L2_RxKeys[0].L2_KEY_Value[2]<<8|L2_RxKeys[0].L2_KEY_Value[3]);
//				if(KN_Rcv==RNG_Number) //if kn not right, do nothing
//				{
					switch(L2_RxKeys[1].L2_KEY)
					{
						case KEY_STOP_CONTROL:
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
							break;
						case KEY_REDLED_MODE:
							  LEDRed_APPCon.mode=L2_RxKeys[1].L2_KEY_Value[0]>>4;
						    LEDRed_APPCon.blink_mode=L2_RxKeys[1].L2_KEY_Value[0]&0x0f;
						    if(LEDRed_APPCon.mode==LED_MODE_BLINK&&LEDRed_APPCon.blink_mode!=0)
								{
									timesledrblinked=0;//clear beeped times to eanble beep 
								}		
						    if(L2_RxKeys[1].L2_KEY_Value[2]!=0)
						    {
									Freq=(float)L2_RxKeys[1].L2_KEY_Value[1]/L2_RxKeys[1].L2_KEY_Value[2];
									Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;     //STEP need to reset???
								}
									LEDRed_APPCon.blink_cycle=Cycle;
									LEDRed_APPCon.ledoff();
									LEDRed_APPCon.statusnow=LED_STATUS_OFF;
								  BlinkStepR=0;
									LEDRed_Act=&LEDRed_APPCon;
								
						  break;
						case KEY_GREENLED_MODE:
							  LEDGreen_APPCon.mode=L2_RxKeys[1].L2_KEY_Value[0]>>4;
								LEDGreen_APPCon.blink_mode=L2_RxKeys[1].L2_KEY_Value[0]&0x0f;	
                if(LEDGreen_APPCon.mode==LED_MODE_BLINK&&LEDGreen_APPCon.blink_mode!=0)
								{
									timesledgblinked=0;//clear beeped times to eanble beep 
								}						
						    if(L2_RxKeys[1].L2_KEY_Value[2]!=0)
								{
									Freq=(float)L2_RxKeys[1].L2_KEY_Value[1]/L2_RxKeys[1].L2_KEY_Value[2];
									Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;
								}
									LEDGreen_APPCon.blink_cycle=Cycle;
									LEDGreen_APPCon.ledoff();
									LEDGreen_APPCon.statusnow=LED_STATUS_OFF;
								  BlinkStepG=0;
									LEDGreen_Act=&LEDGreen_APPCon;
								
							break;
						case KEY_BLUELED_MODE:
							  LEDBlue_APPCon.mode=L2_RxKeys[1].L2_KEY_Value[0]>>4;
						    LEDBlue_APPCon.blink_mode=L2_RxKeys[1].L2_KEY_Value[0]&0x0f;
						    if(LEDBlue_APPCon.mode==LED_MODE_BLINK&&LEDBlue_APPCon.blink_mode!=0)
								{
									timesledbblinked=0;//clear beeped times to eanble beep 
								}
								if(L2_RxKeys[1].L2_KEY_Value[2]!=0)
								{
									Freq=(float)L2_RxKeys[1].L2_KEY_Value[1]/L2_RxKeys[1].L2_KEY_Value[2];
									Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;
								}
									LEDBlue_APPCon.blink_cycle=Cycle;
									LEDBlue_APPCon.ledoff();
									LEDBlue_APPCon.statusnow=LED_STATUS_OFF;
								  BlinkStepB=0;
									LEDBlue_Act=&LEDBlue_APPCon;
								
							
							break;
						case KEY_BUZZER_MODE:
							  BUZZ_APPCon.mode=L2_RxKeys[1].L2_KEY_Value[0]>>4;
						    BUZZ_APPCon.beep_mode=L2_RxKeys[1].L2_KEY_Value[0]&0x0f;
						    if(BUZZ_APPCon.mode==BUZZ_MODE_BEEP&&BUZZ_APPCon.beep_mode!=0)
								{
									timesbeeped=0;//clear beeped times to eanble beep 
								}
								if(L2_RxKeys[1].L2_KEY_Value[2]!=0)
								{
									Freq=(float)L2_RxKeys[1].L2_KEY_Value[1]/L2_RxKeys[1].L2_KEY_Value[2];
									Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;
								}
									BUZZ_APPCon.beep_cycle=Cycle;
									BUZZ_APPCon.buzzoff();
									BUZZ_APPCon.statusnow=BUZZ_STATUS_OFF;
								  BeepStep=0;
									BUZZ_Act=&BUZZ_APPCon;
							break;
						case KEY_START_CONTROL:
							controlkeyindex=0;
						  controlkey=L2_RxKeys[1].L2_KEY_Value;
						  ControlNeedStop=1;
							while(((controlkey[0])==KEY_REDLED_MODE||(controlkey[0])==KEY_GREENLED_MODE||(controlkey[0])==KEY_BLUELED_MODE||(controlkey[0])==KEY_BUZZER_MODE)&&controlkeyindex<4)
							{
								switch(controlkey[0])
								{
									 case KEY_REDLED_MODE:
											LEDRed_APPCon.mode=controlkey[3]>>4;
											LEDRed_APPCon.blink_mode=controlkey[3]&0x0f;
											if(LEDRed_APPCon.mode==LED_MODE_BLINK&&LEDRed_APPCon.blink_mode!=0)
											{
												timesledrblinked=0;//clear beeped times to eanble beep 
											}		
											if(controlkey[5]!=0)
											{
												Freq=(float)controlkey[4]/controlkey[5];
												Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;     //STEP need to reset???
											}
												LEDRed_APPCon.blink_cycle=Cycle;
												LEDRed_APPCon.ledoff();
												LEDRed_APPCon.statusnow=LED_STATUS_OFF;
												BlinkStepR=0;
												LEDRed_Act=&LEDRed_APPCon;
											  ledrticks=0;	
											  controlkeyindex++;
								        controlkey+=6;
										continue;
									 case KEY_GREENLED_MODE:
											LEDGreen_APPCon.mode=controlkey[3]>>4;
											LEDGreen_APPCon.blink_mode=controlkey[3]&0x0f;	
											if(LEDGreen_APPCon.mode==LED_MODE_BLINK&&LEDGreen_APPCon.blink_mode!=0)
											{
												timesledgblinked=0;//clear beeped times to eanble beep 
											}						
											if(controlkey[5]!=0)
											{
												Freq=(float)controlkey[4]/controlkey[5];
												Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;
											}
												LEDGreen_APPCon.blink_cycle=Cycle;
												LEDGreen_APPCon.ledoff();
												LEDGreen_APPCon.statusnow=LED_STATUS_OFF;
												BlinkStepG=0;
												LEDGreen_Act=&LEDGreen_APPCon;
											  ledgticks=12;
										    controlkeyindex++;
								        controlkey+=6;
										continue;
									case KEY_BLUELED_MODE:
										LEDBlue_APPCon.mode=controlkey[3]>>4;
										LEDBlue_APPCon.blink_mode=controlkey[3]&0x0f;
										if(LEDBlue_APPCon.mode==LED_MODE_BLINK&&LEDBlue_APPCon.blink_mode!=0)
										{
											timesledbblinked=0;//clear beeped times to eanble beep 
										}
										if(controlkey[5]!=0)
										{
											Freq=(float)controlkey[4]/controlkey[5];
											Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;
										}
											LEDBlue_APPCon.blink_cycle=Cycle;
											LEDBlue_APPCon.ledoff();
											LEDBlue_APPCon.statusnow=LED_STATUS_OFF;
											BlinkStepB=0;
											LEDBlue_Act=&LEDBlue_APPCon;
										  ledbticks=24;
										  controlkeyindex++;
								      controlkey+=6;
									
									continue;
								case KEY_BUZZER_MODE:
											BUZZ_APPCon.mode=controlkey[3]>>4;
											BUZZ_APPCon.beep_mode=controlkey[3]&0x0f;
											if(BUZZ_APPCon.mode==BUZZ_MODE_BEEP&&BUZZ_APPCon.beep_mode!=0)
											{
												timesbeeped=0;//clear beeped times to eanble beep 
											}
											if(controlkey[5]!=0)
											{
												Freq=(float)controlkey[4]/controlkey[5];
												Cycle=((float)1/Freq)*100;  //cycle from s to n 10 ms;
											}
												BUZZ_APPCon.beep_cycle=Cycle;
												BUZZ_APPCon.buzzoff();
												BUZZ_APPCon.statusnow=BUZZ_STATUS_OFF;
												BeepStep=0;
												BUZZ_Act=&BUZZ_APPCon;
											  controlkeyindex++;
								        controlkey+=6;
										continue;
									default:
										break;
								}
								
							}
							break;
						default:
							break;
					}
//				} 
//				else
//				{
//					Ble_Discon();
//					//printf("AT+B DISCON\r");
//				}
			//}
			break;

		case COMMAND_DEVICE_LOG:
			break;
		case COMMAND_FIRMWARE_UPDATE:
			break;
		case COMMAND_T_TRANS:
			if(L2_RxKeys[0].L2_KEY==KEY_ACCESS_TOKEN) // first key must be access token?
			{
				//KN_Rcv=(L2_RxKeys[0].L2_KEY_Value[0]<<24|L2_RxKeys[0].L2_KEY_Value[1]<<16|L2_RxKeys[0].L2_KEY_Value[2]<<8|L2_RxKeys[0].L2_KEY_Value[3]);
				//if(KN_Rcv==RNG_Number) //if kn not right, do nothing
				{
					switch(L2_RxKeys[1].L2_KEY)
					{
						case KEY_T_COMMAND:
						{
							switch(L2_RxKeys[1].L2_KEY_Value[0]) //T COMMAND CODE
							{
								case KEY_T_Connect:	
									bsp_T_Write(L2_RxKeys[1].L2_KEY_Value+1,512);
									L2_TxKeys[0].L2_KEY_Value=bsp_T_Read(512);
									L2_TxKeys[0].L2_KEY=KEY_T_RESPOND;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=513;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_T_TRANS;
									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									break;
							
								case KEY_T_Discon:
									bsp_T_Write(L2_RxKeys[1].L2_KEY_Value+1,512);
									L2_TxKeys[0].L2_KEY_Value=bsp_T_Read(512);
									L2_TxKeys[0].L2_KEY=KEY_T_RESPOND;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=513;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_T_TRANS;
									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									break;
							
								case KEY_T_GetSN:
									bsp_T_Write(L2_RxKeys[1].L2_KEY_Value+1,512);
									L2_TxKeys[0].L2_KEY_Value=bsp_T_Read(512);
									L2_TxKeys[0].L2_KEY=KEY_T_RESPOND;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=513;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_T_TRANS;
									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									break;
							
								case KEY_T_Write:
									bsp_T_Write(L2_RxKeys[1].L2_KEY_Value+1,2048);
									L2_TxKeys[0].L2_KEY_Value=bsp_T_Read(512);
									L2_TxKeys[0].L2_KEY=KEY_T_RESPOND;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=513;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_T_TRANS;
									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									break;
								case KEY_T_Read:
									bsp_T_Write(L2_RxKeys[1].L2_KEY_Value+1,512);
		              L2_TxKeys[0].L2_KEY_Value=bsp_T_Read(2048);
									L2_TxKeys[0].L2_KEY=KEY_T_RESPOND;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=1025;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
						
								  L2_TxKeys[0].L2_KEY_Value[1024]=L2_TxKeys[0].L2_KEY_Value[0];			// byte 0 means succeed or not
								  L2_TxKeys[0].L2_KEY_Value+=1024; // move pointer to 1024 so send the back 1024 bytes
									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_T_TRANS;
									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									break;
								default:
									break;
							}
						}
					}
				} 
//				else
//				{
//					Ble_Discon();
//				}
			}
			break;
	  case COMMAND_MCU_ROM:
			if(L2_RxKeys[0].L2_KEY==KEY_ACCESS_TOKEN) // first key must be access token?
			{
				KN_Rcv=(L2_RxKeys[0].L2_KEY_Value[0]<<24|L2_RxKeys[0].L2_KEY_Value[1]<<16|L2_RxKeys[0].L2_KEY_Value[2]<<8|L2_RxKeys[0].L2_KEY_Value[3]);
				if(KN_Rcv==RNG_Number) //if kn not right, do nothing
				{
					switch(L2_RxKeys[1].L2_KEY)
					{
						case KEY_MCU_COMMAND:
						{
							switch(L2_RxKeys[1].L2_KEY_Value[0]) //MCU_ROM_COMMAND_CODE
							{
								case KEY_MCU_Read:	
									MCU_ReadLength=L2_RxKeys[1].L2_KEY_Value[2]*256+L2_RxKeys[1].L2_KEY_Value[3];
									L2_TxKeys[0].L2_KEY_Value=bsp_MCU_Read(L2_RxKeys[1].L2_KEY_Value[1],MCU_ReadLength);
									L2_TxKeys[0].L2_KEY=KEY_MCU_RESPOND;
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=MCU_ReadLength+1; //+1byte Resultcode
									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_MCU_ROM;
									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									break;
//								  L2_TxKeys[0].L2_KEY_Value=bsp_MCU_Read(L2_RxKeys[1].L2_KEY_Value[1]);
//									L2_TxKeys[0].L2_KEY=KEY_MCU_RESPOND;
//									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=513;
//									L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
//									L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_MCU_ROM;
//									L2_TxKeys_Number=1;
//									while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
//									xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
//									break;
							
								case KEY_MCU_Write:
									//bsp_T_Write(L2_RxKeys[1].L2_KEY_Value+1,512);
								  if(L2_RxKeys[1].L2_KEY_Header.key_header_bit_field.v_length>1)
									{
										L2_TxKeys[0].L2_KEY_Value=bsp_MCU_Write(L2_RxKeys[1].L2_KEY_Value+2,L2_RxKeys[1].L2_KEY_Value[1],L2_RxKeys[1].L2_KEY_Header.key_header_bit_field.v_length-2);
										L2_TxKeys[0].L2_KEY=KEY_MCU_RESPOND;
										L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.v_length=1;
										L2_TxKeys[0].L2_KEY_Header.key_header_bit_field.reserve=L2_KEY_HEADER_RESERVE;
										L2_TxBuffer[L2_HEADER_CMDID_POS]=COMMAND_MCU_ROM;
										L2_TxKeys_Number=1;
//										while(L1_TxSta!=L1_TX_IDLE);  //wait until L1 tx idle , need  timeout ?????????????????????????
										xQueueSendToBack(L1_TxQue,&WAKEUPSEMA_SEND,portMAX_DELAY);	
									}
									break;
								default:
									break;
							}
						}
					}
				} 
				else
				{
					Ble_Discon();
					//printf("AT+B DISCON\r");
				}
			}
			break;
		default:
			break;
	}
	L1_RcvSta=L1_IDLE; 
}


void L1_Unpack(void)   //maxim 10 keys
{
	u16 CRC_Cal=0;
	
	u8* p_L2_Buffer=L2_RxBuffer;
	u16 Buffer_index=0,Key_index=0;
  u16 Tx_index;
	u8 Usart_index;
	SleepTime=0;
	if(L1_RxHeader.magic==0xAB)
	{	
		if(L1_RxHeader.version.version_def.ack_flag==1) //ack package
		{
			if(L1_RxHeader.version.version_def.err_flag==1) //err,resend
			{
				L1_Resend_Times++;
				if(L1_Resend_Times>4)
				{
					Ble_Discon();
					//printf("AT+B DISCON\r");//disconnect blue tooth and wait app to reconnect;
				}
				else
				{
//					while(L1_TxSta!=L1_TX_IDLE);  //wait for idle
					L1_TxSta=L1_TX_SENDING;				
					for(Tx_index=0;Tx_index-L1_HEADER_SIZE<L1_TxHeader.payload_len;)	//resend;
					{
							memcpy(L0_TxBuffer_Temp,&L1_TxBuffer[Tx_index],20);
							Tx_index+=20;
							for(Usart_index=0;Usart_index<20;Usart_index++)
							{
								xQueueSendToBack(L0_TxQue,&L0_TxBuffer_Temp[Usart_index],portMAX_DELAY);
								USART_ITConfig( USART2, USART_IT_TXE,ENABLE ); //Give signal to usart handler to start a 2o bytes' data transfer
							}
				      ForDelay_nms(Delay_20Bytes);
					}	
					L1_TxSta=L1_TX_IDLE;				
				}
			}
			else
			{
				L1_Resend_Times=0;
			}
		}
		
		  else if(L1_RxHeader.version.version_def.err_flag==1) //err,resend
			{
				L1_Resend_Times++;
				if(L1_Resend_Times>4)
				{
					Ble_Discon();
					//printf("AT+B DISCON\r");//disconnect blue tooth and wait app to reconnect;
				}
				else
				{
//					while(L1_TxSta!=L1_TX_IDLE);  //wait for idle
					L1_TxSta=L1_TX_SENDING;				
					for(Tx_index=0;Tx_index-L1_HEADER_SIZE<L1_TxHeader.payload_len;)	//resend;
					{
							memcpy(L0_TxBuffer_Temp,&L1_TxBuffer[Tx_index],20);
							Tx_index+=20;
							for(Usart_index=0;Usart_index<20;Usart_index++)
							{
								xQueueSendToBack(L0_TxQue,&L0_TxBuffer_Temp[Usart_index],portMAX_DELAY);
								USART_ITConfig( USART2, USART_IT_TXE,ENABLE ); //Give signal to usart handler to start a 2o bytes' data transfer
							}
				      ForDelay_nms(Delay_20Bytes);
					}	
					L1_TxSta=L1_TX_IDLE;				
				}
			}
		else //data package
		{
			CRC_Cal=CRC16(L2_RxBuffer,L1_RxHeader.payload_len);
			if(CRC_Cal==L1_RxHeader.crc16) //CRC ok
			{
				L1_RESPOND(RCV_SUCCESS);
				L2_RxHeader.cmd_ID = *p_L2_Buffer++;
				L2_RxHeader.version = *p_L2_Buffer++;  //just do this now in fact the high 4 bits
				L2_RxHeader.key = *p_L2_Buffer++;
				L2_RxHeader.key_header.data = (((u16)*(p_L2_Buffer)<<8) + *(p_L2_Buffer+1));   // just do this now in fact the low 10 bits
				p_L2_Buffer+=2;
				
				L2_RxKeys[0].L2_KEY = L2_RxHeader.key;
				L2_RxKeys[0].L2_KEY_Header.data= L2_RxHeader.key_header.data;
				p_L2_Buffer=L2_RxBuffer+2;
				for(Key_index=0;(Buffer_index<(L1_RxHeader.payload_len-L2_HEADER_SIZE)&&Key_index<10);)
				{
					L2_RxKeys[Key_index].L2_KEY = *(p_L2_Buffer+Buffer_index);
					Buffer_index++;
					L2_RxKeys[Key_index].L2_KEY_Header.data = (((u16)*(p_L2_Buffer+Buffer_index)<<8) + *(p_L2_Buffer+Buffer_index+1)); 
					Buffer_index+=2;
					L2_RxKeys[Key_index].L2_KEY_Value = p_L2_Buffer+Buffer_index;
					Buffer_index+=L2_RxKeys[Key_index].L2_KEY_Header.key_header_bit_field.v_length;
					Key_index++;
				}
				L2_RxKeys_Handle(Key_index);
			}
			
		else
		{
//			while(L1_TxSta!=L1_TX_IDLE);
			L1_RESPOND(RCV_FAIL);
		}
	}
	}
		else
	{
//		while(L1_TxSta!=L1_TX_IDLE);
		L1_RESPOND(RCV_FAIL);
	}
	L1_RcvSta=L1_IDLE; 
}

void USART_L0_RXTask(void * pvParameters)
{
	u16 Rx_index=0;
	portBASE_TYPE XSTATUS;
		for(;;)
	{
		/////////////////////////////////////////////////////RCV/////////////////////////////////////////////////////////
			XSTATUS=xQueueReceive(L0_RxQue,L0_RxBuffer_Temp,portMAX_DELAY);
			if(XSTATUS==pdPASS)
			{
				SleepTime=0;
				if(L1_RcvSta==L1_IDLE)  //if IDLE, fisrt L0 packet will contain the L1 Header
				{
					Rx_index=0;
					L1_RxHeader.magic=*(L0_RxBuffer_Temp+L1_HEADER_MAGIC_POS);
					L1_RxHeader.version.value=*(L0_RxBuffer_Temp+L1_HEADER_PROTOCOL_VERSION_POS);
					L1_RxHeader.payload_len=((u16)*(L0_RxBuffer_Temp+L1_PAYLOAD_LENGTH_HIGH_BYTE_POS)<<8)+*(L0_RxBuffer_Temp+L1_PAYLOAD_LENGTH_LOW_BYTE_POS); 
			
					L1_RxHeader.crc16=((u16)*(L0_RxBuffer_Temp+L1_HEADER_CRC16_HIGH_BYTE_POS)<<8)+*(L0_RxBuffer_Temp+ L1_HEADER_CRC16_LOW_BYTE_POS); 
					L1_RxHeader.sequence_id=((u16)*(L0_RxBuffer_Temp+L1_HEADER_SEQ_ID_HIGH_BYTE_POS)<<8)+*(L0_RxBuffer_Temp+L1_HEADER_SEQ_ID_LOW_BYTE_POS);
					if(L1_RxHeader.payload_len>2097) // exceed max length
					{
//					  while(L1_TxSta!=L1_TX_IDLE);
						L1_RESPOND(RCV_FAIL);
					}
					else
					L1_RcvSta=L1_RCVING;
				}
				/*******************************NEED TIMEOUT CHECK?*************************************************/
				
				if(L1_RcvSta==L1_RCVING)
				{
					  memcpy(&L1_RxBuffer[Rx_index],L0_RxBuffer_Temp,20);  //move 20 bytes data to L1_RxBuffer
						Rx_index+=20;
						if((Rx_index-L1_HEADER_SIZE)>=L1_RxHeader.payload_len)   //end of L1 packet
						{
							Rx_index=0;
							L1_RcvSta=L1_UNPACKING;  //unpacking now block rcv
							STX_Received=0;
							Version_Received=0;
							L1_Unpack();
						}
				
				}
		}		
	}
}

void USART_L0_TXTask(void * pvParameters)
{
  u16 Tx_index=0,Usart_index=0;
	static u8 mx;
	portBASE_TYPE XSTATUS;
	for(;;)
	{
	/////////////////////SEND////////////////////////////////////////////
		XSTATUS=xQueueReceive(L1_TxQue,&mx,portMAX_DELAY);
		if(XSTATUS==pdPASS)
		{
			  SleepTime=0;
			  L1_TxSta=L1_TX_SENDING;
				L1_Pack();
				for(Tx_index=0;Tx_index-L1_HEADER_SIZE<L1_TxHeader.payload_len;)	
				{
					memcpy(L0_TxBuffer_Temp,&L1_TxBuffer[Tx_index],20);
					Tx_index+=20;
					for(Usart_index=0;Usart_index<20;Usart_index++)
					{
					  xQueueSendToBack(L0_TxQue,&L0_TxBuffer_Temp[Usart_index],portMAX_DELAY);
						USART_ITConfig( USART2, USART_IT_TXE,ENABLE ); //Give signal to usart handler to start a 2o bytes' data transfer
					}
						ForDelay_nms(Delay_20Bytes);
					
				}		
        L1_TxSta=L1_TX_IDLE;				
				
			}
			
		}
}



