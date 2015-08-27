/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
#include "bsp_FileSystem.h"
#include "stdio.h"
#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "usb_bsp.h"
#include "bsp_Sdio.h"
/*--------------------------------------------------------------------------

Module Private Functions and Variables

---------------------------------------------------------------------------*/

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */
extern u8 SD_IN;

/* Definitions of physical drive number for each media */


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
//	DSTATUS stat;
	u8 res;

	switch (pdrv) {
	case SDCARD :
		//result = Nand_nATA_disk_initialize();

		// translate the reslut code here
    res=SD_Init();
	  SD_IN=res;
	  //res=0;
		return res;
	
 }
		return RES_PARERR;
}


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
//	DSTATUS stat;
//	int result;

	switch (pdrv) {
	case SDCARD :
     return 0;
	}
		return RES_PARERR;

}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	  u8 res;
//	int result;
//	int i;
	BYTE status = USBH_MSC_OK;
    if (!count)return RES_PARERR;//count????0,????????	
	switch (pdrv) {
		
	case SDCARD :
      res=SD_ReadDisk(buff,sector,count);	 
			while(res)//???
			{
				SD_Init();	//?????SD?
				ForDelay_nms(1);
				res=SD_ReadDisk(buff,sector,count);	
				//bsp_LED1On();
//				printf("sd rd error:%d\r\n",res);
			}
		//	break;
		// translate the reslut code here
		res=RES_OK;
		
		return (DRESULT)res;
	
	}
		return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{  
	u8 res;
//	uint32_t result;
			  BYTE status = USBH_MSC_OK;
	if (!count)return RES_PARERR;//count????0,????????		 	
//	int i;
	switch (pdrv) {
	case SDCARD :
   res=SD_WriteDisk((u8*)buff,sector,count);
			while(res)//???
			{
				SD_Init();	//?????SD?
				ForDelay_nms(1);
				res=SD_WriteDisk((u8*)buff,sector,count);	
				//bsp_LED2On();
			//	printf("sd wr error:%d\r\n",res);
			}
    return RES_OK;
  
	}
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
//	int result;

	switch (pdrv) {
	case SDCARD:
	
					// pre-process here
					switch (cmd) 
			{
			case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;

     } 
	

		// post-process here

		return res;

	
	}
	return RES_PARERR;
}

#endif

DWORD get_fattime (void)
{				 
	return 0;
}			 


