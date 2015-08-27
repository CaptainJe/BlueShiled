/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "stdio.h"
#include "bsp_FileSystem.h"
#include "Task_HardwareControl.h"
#include "bsp_Delay.h"
//u8 SDTEST_BlankBuffer[512]=
//{0x00};
//u8 SDTEST_WriteBuffer[512]=
//{0x06,0x0F,0X19,0X16,0X1F,0X16,0X0C,0X0B,0X1F,0X1F,0X18,0X0E,0X1A,0X19,0X03,0X02,
// 0X1F,0X0C,0X04,0X12,0X16,0X0F,0X05,0X0D,0X00,0X02,0X00,0X00,0X01,0X02,0X00,0X01,
// 0X00,0X00,0X00,0X00,0X00};
//u8 SDTEST_ReadBuffer[512]=
// {0x01,0x02,0x03,0x04,0x05};
//void SD_TEST(void)
//{
//	u8 res;
//	UINT bw,index;
//	FIL file_sdif;
//	
//  printf("SDTEST_ReadBuffer Org:\r\n");
//	for(index=0;index<48;index++)
//	{
//			printf("0x%02x  ",SDTEST_ReadBuffer[index]);
//			if(index%16==15)
//				printf("\r\n");
//	}
//	res=f_mount( &fatfs_SDCARD,"0:/",1 );
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
//	printf("Writting Data...\r\n");
//	res=f_write(&file_sdif,SDTEST_WriteBuffer,512,&bw);
//	res=f_close(&file_sdif);
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING |FA_WRITE);
//	res=f_write(&file_sdif,SDTEST_WriteBuffer,512,&bw);
//	res=f_close(&file_sdif);
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING | FA_READ);
//	printf("Reading Data...\r\n");
//	index=0;
//	while(SDTEST_ReadBuffer[31]!=0x04)
//	{
//		index++;
//		res=f_read(&file_sdif,SDTEST_ReadBuffer,512,&bw);
//		printf("Read the %d times\r\n",index);
//	}
//		for(index=0;index<48;index++)
//	{
//			printf("0x%02x  ",SDTEST_ReadBuffer[index]);
//			if(index%16==15)
//				printf("\r\n");
//	}
//	f_close(&file_sdif);      
//	
//	
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING |FA_WRITE);        //Clear sdif
//	res=f_write(&file_sdif,SDTEST_BlankBuffer,512,&bw);
//	res=f_close(&file_sdif);
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING | FA_READ);
//	printf("Reading Data...\r\n");
//	res=f_read(&file_sdif,SDTEST_ReadBuffer,512,&bw);
//			for(index=0;index<48;index++)
//	{
//			printf("0x%02x  ",SDTEST_ReadBuffer[index]);
//			if(index%16==15)
//				printf("\r\n");
//	}
//	res=f_close(&file_sdif);
//	
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING |FA_WRITE);        //retest a gain use just one write cycle
//	res=f_write(&file_sdif,SDTEST_WriteBuffer,512,&bw);
//	res=f_close(&file_sdif);
//	res=f_open(&file_sdif, "0:/sdif.dat",FA_OPEN_EXISTING | FA_READ);
//	printf("Reading Data...\r\n");
//	res=f_read(&file_sdif,SDTEST_ReadBuffer,512,&bw);
//			for(index=0;index<48;index++)
//	{
//			printf("0x%02x  ",SDTEST_ReadBuffer[index]);
//			if(index%16==15)
//				printf("\r\n");
//	}
//}
/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
#define IMAGE_BUFFER_SIZE    512
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};
FIL file;
uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0;   

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USRH_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\r\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \r\n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\r\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \r\n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\r\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\r\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\r\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \r\n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\r\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\r\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \r\n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\r\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\r\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\r\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\r\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
u8 Explore_Folder(char * path,u8 recu_level);
//static uint8_t Image_Browser (char* path);
//static void     Show_Image(void);
//static void     Toggle_Leds(void);
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  static uint8_t startup = 0;  
  
  if(startup == 0 )
  {
    startup = 1;
		//printf((char *)MSG_HOST_INIT);
  }
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
  //LCD_UsrLog((void *)MSG_DEV_ATTACHED);
	//printf((char *)MSG_DEV_ATTACHED);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
//  LCD_ErrLog((void *)MSG_UNREC_ERROR); 
	//printf((char *)MSG_UNREC_ERROR);
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
  
//  LCD_LOG_ClearTextZone();
  
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "                                      ");
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "                                      ");  
  
  /* Set default screen color*/
//  LCD_ErrLog((void *)MSG_DEV_DISCONNECTED);
	//printf((char *)MSG_DEV_DISCONNECTED);
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
//    LCD_UsrLog((void *)MSG_DEV_HIGHSPEED);
		 // printf((char *)MSG_DEV_HIGHSPEED);
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
//    LCD_UsrLog((void *)MSG_DEV_FULLSPEED);
		// printf((char *)MSG_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
//    LCD_UsrLog((void *)MSG_DEV_LOWSPEED);
	//	 printf((char *)MSG_DEV_LOWSPEED);
  }
  else
  {
//    LCD_UsrLog((void *)MSG_DEV_ERROR);
	//	 printf((char *)MSG_DEV_ERROR);
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
 // USBH_DevDesc_TypeDef *hs;
 // hs = DeviceDesc;  
  //printf("VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
 // printf("PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
//  LCD_UsrLog("VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
//  LCD_UsrLog("PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
		 // printf((char *)MSG_MSC_CLASS);
//    LCD_UsrLog((void *)MSG_MSC_CLASS);
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
		 // printf((char *)MSG_HID_CLASS);
//    LCD_UsrLog((void *)MSG_HID_CLASS);
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	//  printf("Manufacturer : %s\n", (char *)ManufacturerString);
//  LCD_UsrLog("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
	//  printf("Product : %s\n", (char *)ProductString);
//  LCD_UsrLog("Product : %s\n", (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	 // printf( "Serial Number : %s\n", (char *)SerialNumString);
//  LCD_UsrLog( "Serial Number : %s\n", (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  
  /* Enumeration complete */
	 // printf((char *)MSG_DEV_ENUMERATED);
//  LCD_UsrLog((void *)MSG_DEV_ENUMERATED);
  
//  LCD_SetTextColor(Green);
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "To see the root content of the disk : " );
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "Press Key...                       ");
//  LCD_SetTextColor(LCD_LOG_DEFAULT_COLOR); 
  
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
	 // printf("> Device not supported.\r\n");
 // LCD_ErrLog ("> Device not supported."); 
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  
  //usbh_usr_status = USBH_USR_NO_RESP;  
  
  /*Key B3 is in polling mode to detect user action */
//  if(STM_EVAL_PBGetState(Button_KEY) == RESET) 
//  {
    
    usbh_usr_status = USBH_USR_RESP_OK;
    
//  } 
  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
	 // printf("Overcurrent detected.\r\n");
//  LCD_ErrLog ("Overcurrent detected.");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
  FRESULT res;
  uint8_t writeTextBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS   ";
  uint16_t bytesWritten, bytesToWrite;
  
  switch(USBH_USR_ApplicationState)
  {
  case USH_USR_FS_INIT: 

	  res=f_mount( &fatfs_SDCARD,"0:/",1 );
	  if(res)
		//	printf("SDCARD FATFS mount failed:  %d\r\n",res);
    /* Initialises the File System*/
		;
    res=f_mount( &fatfs_USBDISK,"1:/",1 );
    if(res)
			;
	//		printf("USB FATFS mount failed:  %d\r\n",res);
 //   LCD_UsrLog("> File System initialized.\n");
 //   LCD_UsrLog("> Disk capacity : %d Bytes\n", USBH_MSC_Param.MSCapacity * \
      USBH_MSC_Param.MSPageLength); 
    
    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
    {
		//	  printf((char *)MSG_WR_PROTECT);
  //    LCD_ErrLog((void *)MSG_WR_PROTECT);
    }
    
    USBH_USR_ApplicationState = USH_USR_FS_READLIST;
    break;
    
  case USH_USR_FS_READLIST:
    
	 // printf((char *) MSG_ROOT_CONT);
 //   LCD_UsrLog((void *)MSG_ROOT_CONT);
  //  Explore_Folder("1:/", 1);
	
	//  Explore_Folder("0:/", 1);
    line_idx = 0;   
    USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
	 //   USBH_USR_ApplicationState = USH_USR_FS_DRAW; 
    
    break;
    
  case USH_USR_FS_WRITEFILE:
    
//   LCD_SetTextColor(Green);
	 //   printf("> Write file....\r\n");
//    LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "                                              ");
//    LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "Press Key to write file");
//    LCD_SetTextColor(LCD_LOG_DEFAULT_COLOR); 
      USB_OTG_BSP_mDelay(100);
    
    /*Key B3 in polling*/
//    while((HCD_IsDeviceConnected(&USB_OTG_Core)))
// //     (STM_EVAL_PBGetState (BUTTON_KEY) == SET))          
//    {
////      Toggle_Leds();
//    }
    /* Writes a text file, STM32.TXT in the disk*/
 //   LCD_UsrLog("> Writing File to disk flash ...\n");
//    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
//    {
//      printf("> Disk flash is write protected\r\n");
// //     LCD_ErrLog ( "> Disk flash is write protected \n");
//      USBH_USR_ApplicationState = USH_USR_FS_DRAW;
//      break;
//    }
//    
//    /* Register work area for logical drives */
//    f_mount(0, &fatfs);
//    
    if(f_open(&file, "1:/CREM.TXT",FA_OPEN_ALWAYS | FA_WRITE) == FR_OK)
    { 
      /* Write buffer to file */
      bytesToWrite = sizeof(writeTextBuff); 
      res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);   
      
      if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
      {
				 // printf("> 'CREM.TXT' cannot be writen TO USB\r\n");
//        LCD_ErrLog("> STM32.TXT CANNOT be writen.\n");
      }
      else
      {
				  //printf("> 'CREM.TXT' file created IN USB\r\n");
//        LCD_UsrLog("> 'STM32.TXT' file created\n");
      }
      
      /*close file and filesystem*/
      f_close(&file);
      //f_mount(NULL,"1:/",1); 
    }
    
    else
    {
			  //printf("> 'CREM.TXT' created in the USB\r\n");
//      LCD_UsrLog ("> STM32.TXT created in the disk\n");
    }

    USBH_USR_ApplicationState = USH_USR_FS_DRAW; 
		
		if(f_open(&file, "0:/CREM.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    { 
      /* Write buffer to file */
      bytesToWrite = sizeof(writeTextBuff); 
      res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten);   
      
      if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
      {
		//		  printf("> 'CREM.TXT' cannot be writen IN SDCARD\r\n");
//        LCD_ErrLog("> STM32.TXT CANNOT be writen.\n");
      }
      else
      {
		//		  printf("> 'CREM.TXT' file created in SDCARD\r\n");
//        LCD_UsrLog("> 'STM32.TXT' file created\n");
      }
      
      /*close file and filesystem*/
      f_close(&file);
      //f_mount(NULL,"1:/",1); 
    }
    
//    LCD_SetTextColor(Green);
//    LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "                                              ");
//    LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "To start Image slide show Press Key.");
//    LCD_SetTextColor(LCD_LOG_DEFAULT_COLOR); 
  
    break;
    
  case USH_USR_FS_DRAW:
		
   // BUZZ.mode=LED_MODE_BLINK;
    /*Key B3 in polling*/
//    while((HCD_IsDeviceConnected(&USB_OTG_Core)) && \
//      (STM_EVAL_PBGetState (BUTTON_KEY) == SET))
//    {
//      Toggle_Leds();
//    }
//    SD_TEST();
	  
    USBH_USR_ApplicationState = USH_USR_FS_IDLE; 
    break;
	case USH_USR_FS_IDLE:
		break;
  default: break;
  }
  return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/

//Scan files in folder
//path: Folder path; recu_level: always set to 1 to scan current folder and subfolder
//return: 0 scan files succeed;  other: error number of FATFS 
u8 Explore_Folder(char * path,u8 recu_level)
{
	u8 res;
FILINFO fileinfo;	
	DIR dir;
	char *pathtemp;
    char *fn;   /* This function is assuming non-Unicode cfg. */
	
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
	  static char tpath[_MAX_LFN+1];
	  pathtemp=tpath;
    fileinfo.lfname = lfn;
    fileinfo.lfsize = sizeof lfn;
#endif

	  if(recu_level==1)
    printf("%s\r\n",path);	
    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	        
      if(recu_level == 1)
      {
				printf("   |__");
      }
      else if(recu_level == 2)
      {
				printf("   |   |__");
      }
      printf("%s\r\n",fn);
      if(((fileinfo.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
      {
				        strcpy(pathtemp,path);
				        //printf("pathtemp=%s\r\n",pathtemp);
			          strcat((char *)pathtemp,(char *)fn);
				        //printf("pathtemp=%s\r\n",pathtemp);
                res = Explore_Folder(pathtemp,2);
				        //printf("mf_scan_file2 path 2level res=%d\r\n",res);
                if (res != FR_OK) break;
        //mf_scan_files(fn, 2);
      }
        }				   
    }	  
	//myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}

//static uint8_t Image_Browser (char* path)
//{
//  FRESULT res;
//  uint8_t ret = 1;
//  FILINFO fno;
//  DIR dir;
//  char *fn;
//  
//  res = f_opendir(&dir, path);
//  if (res == FR_OK) {
//    
//    for (;;) {
//      res = f_readdir(&dir, &fno);
//      if (res != FR_OK || fno.fname[0] == 0) break;
//      if (fno.fname[0] == '.') continue;

//      fn = fno.fname;
// 
//      if (fno.fattrib & AM_DIR) 
//      {
//        continue;
//      } 
//      else 
//      {
//        if((strstr(fn, "bmp")) || (strstr(fn, "BMP")))
//        {
//          res = f_open(&file, fn, FA_OPEN_EXISTING | FA_READ);
//          Show_Image();
//          USB_OTG_BSP_mDelay(100);
//          ret = 0;
//          while((HCD_IsDeviceConnected(&USB_OTG_Core)) && \
//            (STM_EVAL_PBGetState (BUTTON_KEY) == SET))
//          {
//            Toggle_Leds();
//          }
//          f_close(&file);
//          
//        }
//      }
//    }  
//  }
//  
//  #ifdef USE_USB_OTG_HS 
//  LCD_LOG_SetHeader(" USB OTG HS MSC Host");
//#else
//  LCD_LOG_SetHeader(" USB OTG FS MSC Host");
//#endif
//  LCD_LOG_SetFooter ("     USB Host Library v2.1.0" );
//  LCD_UsrLog("> Disk capacity : %d Bytes\n", USBH_MSC_Param.MSCapacity * \
//      USBH_MSC_Param.MSPageLength); 
//  USBH_USR_ApplicationState = USH_USR_FS_READLIST;
//  return ret;
//}

/**
* @brief  Show_Image 
*         Displays BMP image
* @param  None
* @retval None
*/
//static void Show_Image(void)
//{
//  
//  uint16_t i = 0;
//  uint16_t numOfReadBytes = 0;
//  FRESULT res; 
//  
//  LCD_SetDisplayWindow(239, 319, 240, 320);
//  LCD_WriteReg(R3, 0x1008);
//  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
//  
//  /* Bypass Bitmap header */ 
//  f_lseek (&file, 54);
//  
//  while (HCD_IsDeviceConnected(&USB_OTG_Core))
//  {
//    res = f_read(&file, Image_Buf, IMAGE_BUFFER_SIZE, (void *)&numOfReadBytes);
//    if((numOfReadBytes == 0) || (res != FR_OK)) /*EOF or Error*/
//    {
//      break; 
//    }
//    for(i = 0 ; i < IMAGE_BUFFER_SIZE; i+= 2)
//    {
//      LCD_WriteRAM(Image_Buf[i+1] << 8 | Image_Buf[i]); 
//    } 
//  }
//  
//}

/**
* @brief  Toggle_Leds
*         Toggle leds to shows user input state
* @param  None
* @retval None
*/
/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

