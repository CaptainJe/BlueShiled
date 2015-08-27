/***************************************************************************
 * Name:bsp_FileSystem.c 				                                                
 *											
 * Function:  SD & USB HOST File system manage
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#include "bsp_FileSystem.h"

FATFS fatfs_SDCARD,fatfs_USBDISK;
FIL file_sdif,file_read,file_write,file_info,file_test;
unsigned int bw,br;
const u8 Akey_infoBuffer[]="Manufacturer Nmae:AKEY\r\n"
													  "Model Number     :AB008\r\n"
														"Serial Number    :081F01200001\r\n"
														"Hardware Rev     :1.1.5\r\n"
														"Firmware Rev     :1.1.5\r\n"
														"Software Rev     :NONE\r\n"
														"Battery Threshold:010\r\n"
														"KeySound         :ON \r\n"
                            "System ID        :ABKEY\r\n";
char char_ManuName[]="AKEY";
char char_ModelNum[]="AB008";
char char_SN[]="081f01200001";
char char_HardRev[]="1.1.6"; //20150723 1.1.4
char char_FirmRev[]="1.1.6";
char char_SoftRev[]="0.0.1";
char char_SysID[]="ABKEY";
extern u8 SD_IN;
u8 SDIF_Read[2049];
u8 MCU_ROM_Read[513];
u8 MCU_ROM_Write[1024+16];
u8 MCU_WriteStatus=0;
u8 MCU_ReadStatus=0;
u8 char_SysID_Length=5;
u8 Key_Sound=0;
u8 Battery_Threshold=10;
u8 SDTEST_BlankBuffer[512]=
{0x00};
u8 SDTEST_WriteBuffer[512]=
{0x06,0x0F,0X19,0X16,0X1F,0X16,0X0C,0X0B,0X1F,0X1F,0X18,0X0E,0X1A,0X19,0X03,0X02,
 0X1F,0X0C,0X04,0X12,0X16,0X0F,0X05,0X0D,0X00,0X02,0X00,0X00,0X01,0X02,0X00,0X01,
 0X00,0X00,0X00,0X00,0X00};
u8 SDTEST_ReadBuffer[512]=
 {0x01,0x02,0x03,0x04,0x05};
struct SYSINFO Sysinfo={char_ManuName,char_ModelNum,char_SN,char_HardRev,char_FirmRev,char_SoftRev,10,1,char_SysID};
extern u16 SleepTime;

//void SD_TEST(void)
//{
//	u8 res;
//	UINT bw,index;
//	res=f_mount( &fatfs_SDCARD,"0:/",1 );
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
//	res=f_write(&file_sdif,SDTEST_WriteBuffer,512,&bw);
//	res=f_close(&file_sdif);
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING |FA_WRITE);
//	res=f_write(&file_sdif,SDTEST_WriteBuffer,512,&bw);
//	res=f_close(&file_sdif);
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING | FA_READ);
//	index=0;
//	while(SDTEST_ReadBuffer[31]!=0x04)
//	{
//		index++;
//		res=f_read(&file_sdif,SDTEST_ReadBuffer,512,&bw);
//	}
//	f_close(&file_sdif);      
//	
//	
//}
u8 bsp_ChangeAPPtoRun(u8 App_Select)
{
  u8 status=0;
	u8 Flash_status=0;
	FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	status=FLASH_EraseSector(FLASH_Sector_3 ,VoltageRange_3);//VCC=2.7~3.6V之间!!

	if(status==FLASH_COMPLETE)
	{
   __set_PRIMASK(1);  
	status=FLASH_ProgramByte(App_ToRun_Flag,App_Select);  //SysID
   __set_PRIMASK(0);  
	}
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁	
	if(status==FLASH_COMPLETE)
	{
		Flash_status=0;
	}
	else
	{
		Flash_status=status;
	}
	return Flash_status;
		
}
u8 bsp_FSInit(void)
{
	u8 res=0;
	//SD_IN=SD_Init();
	res=f_mount(&fatfs_SDCARD,"0:/",1 );
			//		bsp_LED2On();
	   //   LEDGreen.statusnow=LED_STATUS_ON;
	if(res) return 1;
	
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//	if(res) return 2;
//	f_close(&file_sdif);
	return 0;	
}
void bsp_SysinfoInit(void)
{
	  u8 index=0;
	  SleepTime=0;

    char_SysID_Length=*(u8*)(ADDR_FLASH_SECTOR_7+1024);
	  if(char_SysID_Length==0xff)
		{
			//Sysinformation not set yet, write default info to flash
			bsp_SysinfoSave();
		}

			for(index=0;index<char_SysID_Length;index++)
			{
				char_SysID[index]=*(u8*)(ADDR_FLASH_SECTOR_7+1024+index+1);
			}
		  Battery_Threshold=*(u8*)(ADDR_FLASH_SECTOR_7+1024+9);
      Key_Sound=*(u8*)(ADDR_FLASH_SECTOR_7+1024+10);
}

u8* bsp_MCU_Write(u8* writebuffer,u8 blockid,u16 writelength)
{	
	
	u8 status=0;
	u16 index=0;
	SleepTime=0;
	__set_PRIMASK(1);  
	memcpy(MCU_ROM_Write,(u8*)ADDR_FLASH_SECTOR_7,1024+16); //在擦除之前，先把数据读出来放在内存buffer
	__set_PRIMASK(0);  
	
  if(blockid==1)
	{
		memcpy(MCU_ROM_Write,writebuffer,writelength); //将512byte数据写入第一个block区域
	}
	if(blockid==2)
	{
		memcpy(MCU_ROM_Write+512,writebuffer,writelength); //将512byte数据写入第二个block区域
	}
	
	FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	status=FLASH_EraseSector(FLASH_Sector_7 ,VoltageRange_3);//VCC=2.7~3.6V之间!!

	if(status==FLASH_COMPLETE)
	{
   __set_PRIMASK(1);  
		  for(index=0;index<(1024+16);index++)
		{
			status=FLASH_ProgramByte(ADDR_FLASH_SECTOR_7+index,MCU_ROM_Write[index]);  //SysID
		}
   __set_PRIMASK(0);  
	}
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁	
	if(status==FLASH_COMPLETE)
	{
		MCU_WriteStatus=0;
	}
	else
	{
		MCU_WriteStatus=status;
	}
	return &MCU_WriteStatus;
		
}

u8* bsp_MCU_Read(u8 blockid,u16 readlength)
{
	__set_PRIMASK(1);  
	SleepTime=0;
	if(blockid==1)
	{
		memcpy(&MCU_ROM_Read[1],(u8*)ADDR_FLASH_SECTOR_7,readlength);
	}
	if(blockid==2)
	{
		memcpy(&MCU_ROM_Read[1],(u8*)(ADDR_FLASH_SECTOR_7+512),readlength);
	}
	MCU_ROM_Read[0]=0;
	__set_PRIMASK(0);  
	return MCU_ROM_Read;
}

u8 bsp_Firmware_Erase(u8 APP_Select)
{
	u8 status=0;
	FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	if(APP_Select)
	status=FLASH_EraseSector(FLASH_Sector_6 ,VoltageRange_3);//VCC=2.7~3.6V之间!!
	else
	status=FLASH_EraseSector(FLASH_Sector_5 ,VoltageRange_3);//VCC=2.7~3.6V之间!!
	
	MCU_WriteStatus=status;
	return MCU_WriteStatus;

}
u8* bsp_Firmware_Write(u8* writebuffer,u32 writeaddress,u16 writelength)
{	
	
	u8 status=0;
	u16 index=0;
	SleepTime=0;
  FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
   __set_PRIMASK(1);  
		  for(index=0;index<writelength;index++)
		{
			status=FLASH_ProgramByte(writeaddress+index,writebuffer[index]);  
		}
   __set_PRIMASK(0);  

	
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁	
	if(status==FLASH_COMPLETE)
	{
		MCU_WriteStatus=0;
	}
	else
	{
		MCU_WriteStatus=status;
	}
	return &MCU_WriteStatus;
		
}
//u8* bsp_MCU_Read(u8 blockid)
//{
//	if(blockid==1)
//	{
//		memcpy(&MCU_ROM_Read[1],(u8*)ADDR_FLASH_SECTOR_7,512);
//	}
//	if(blockid==2)
//	{
//		memcpy(&MCU_ROM_Read[1],(u8*)(ADDR_FLASH_SECTOR_7+512),512);
//	}
//	MCU_ROM_Read[0]=0;
//	return MCU_ROM_Read;
//}
u8 bsp_T_Write(u8* writebuffer,u16 writelength)
{
	u8 res=0;
	SleepTime=0;
	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
	res=f_write(&file_sdif,writebuffer,writelength,&bw);
	res=f_close(&file_sdif);
	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING |FA_WRITE);
	res=f_write(&file_sdif,writebuffer,writelength,&bw);
	res=f_close(&file_sdif);
	
	//test 20150731
	res=f_open(&file_test, "0:/test.txt",FA_OPEN_ALWAYS |FA_WRITE);
	res=f_write(&file_test,writebuffer,writelength,&bw);
	res=f_close(&file_test);
	
	
	if (res)
		return res;
	else 
		return 0;
}

u8* bsp_T_Read(u16 readlength)
{
	u8 res=0;
	u16 index=0;
	UINT br=0;
	SleepTime=0;
  res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
	res=f_read(&file_sdif,&SDIF_Read[1],readlength,&br);
	f_close(&file_sdif); 
	if (res)
	{	
		 SDIF_Read[0]=res;	
	}
	else 
	{	
		SDIF_Read[0]=0;
	}
		if(br<readlength)
		{ 
			for(index=br+1;index<readlength+1;index++)
			{
				SDIF_Read[index]=0;
			}
	  }	
		return SDIF_Read;

}

void bsp_SysinfoSave(void)
{
	u8 status=0;
	u16 index=0;
	SleepTime=0;
	memcpy(MCU_ROM_Write,(u8*)ADDR_FLASH_SECTOR_7,1024+16); //?????,???????????buffer
	memcpy(MCU_ROM_Write+1024,&char_SysID_Length,1); 
	memcpy(MCU_ROM_Write+1024+1,char_SysID,char_SysID_Length); 
	memcpy(MCU_ROM_Write+1024+9,&Battery_Threshold,1);
	memcpy(MCU_ROM_Write+1024+10,&Key_Sound,1);
	
	FLASH_Unlock();									//?? 
  FLASH_DataCacheCmd(DISABLE);//FLASH????,????????
 	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	status=FLASH_EraseSector(FLASH_Sector_7 ,VoltageRange_3);//VCC=2.7~3.6V??!!

	if(status==FLASH_COMPLETE)
	{

		  for(index=0;index<(1024+16);index++)
		{
			status=FLASH_ProgramByte(ADDR_FLASH_SECTOR_7+index,MCU_ROM_Write[index]);  //SysID
		}

	}
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH????,??????
	FLASH_Lock();//??	
	if(status==FLASH_COMPLETE)
	{
		MCU_WriteStatus=0;
	}
	else
	{
		MCU_WriteStatus=status;
	}
//	FLASH_Unlock();									//?? 
//  FLASH_DataCacheCmd(DISABLE);//FLASH????,????????
// 	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
//	status=FLASH_EraseSector(FLASH_Sector_7 ,VoltageRange_3);//VCC=2.7~3.6V??!!

//	if(status==FLASH_COMPLETE)
//	{

//			status=FLASH_ProgramByte(ADDR_FLASH_SECTOR_7+1024,char_SysID_Length);//????
//		  for(index=0;index<char_SysID_Length;index++)
//		{
//			status=FLASH_ProgramByte(ADDR_FLASH_SECTOR_7+1024+index+1,char_SysID[index]);  //SysID
//		}
//		  status=FLASH_ProgramByte(ADDR_FLASH_SECTOR_7+1024+9,Battery_Threshold);
//		  status=FLASH_ProgramByte(ADDR_FLASH_SECTOR_7+1024+10,Key_Sound);
//		  

//	}
//  FLASH_DataCacheCmd(ENABLE);	//FLASH????,??????
//	FLASH_Lock();//??
}


