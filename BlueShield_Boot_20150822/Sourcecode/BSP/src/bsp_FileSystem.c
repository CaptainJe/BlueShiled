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
FIL file_sdif,file_read,file_write,file_info;
unsigned int bw,br;


u8 bsp_FSInit(void)
{
	u8 res=0;
	SD_Init();
	res=f_mount(&fatfs_SDCARD,"0:/",1 );
	if(res) return 1;
	
	res=f_open(&file_sdif, "0:/Akey.bin",FA_OPEN_EXISTING|FA_WRITE|FA_READ);
	if(res) return 2;
	f_close(&file_sdif);
	return 0;	
}




