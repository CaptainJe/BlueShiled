/***************************************************************************
 * Name:bsp_FileSystem.h 				                                                
 *											
 * Function:  SD & USB HOST File system manage
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#ifndef bsp_FileSystem_H
#define bsp_FileSystem_H
#include "stm32f4xx.h"
#include "ff.h"
#include "bsp_sdio.h"
#include "stdio.h"
#define SDCARD 0
#define USBDISK 1
#define char_ManuName_Length  4
#define char_ModelNum_Length  5
#define char_SN_Length        12
#define char_HardRev_Length   5
#define char_FirmRev_Length   5
#define char_SoftRev_Length   5
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define App_ToRun_Flag ((u32)0x0800C000) 

extern u8 char_SysID_Length;
extern u8 Key_Sound;
extern u8 Battery_Threshold;

extern FATFS fatfs_SDCARD,fatfs_USBDISK;
extern FIL file_sdif,file_read,file_write,file_info;
extern char char_ManuName[];
extern char char_ModelNum[];
extern char char_SN[];
extern char char_HardRev[];
extern char char_FirmRev[];
extern char char_SoftRev[];
extern char char_SysID[];
struct SYSINFO {
	char* Manufacturer_Name;
  char* Model_Number;
  char* Serial_Number;
  char* Hardware_Rev;     
  char* Firmware_Rev;     
  char* Software_Rev;          
	u8 Battery_Threshold;
  u8 KeySound;
	char* System_ID;  
};
extern struct SYSINFO Sysinfo;
u8 bsp_FSInit(void);
u8 bsp_T_Write(u8* writebuffer,u16 writelength);
u8* bsp_T_Read(u16 readlength);
void bsp_SysinfoSave(void);
u8* bsp_MCU_Read(u8 blockid,u16 readlength);
//u8* bsp_MCU_Read(u8 blockid);
u8* bsp_MCU_Write(u8* writebuffer,u8 blockid,u16 writelength);
void bsp_SysinfoInit(void);
u8* bsp_Firmware_Write(u8* writebuffer,u32 writeaddress,u16 writelength);
u8  bsp_Firmware_Erase(u8 APP_Select);
u8 bsp_ChangeAPPtoRun(u8 App_Select);
#endif
