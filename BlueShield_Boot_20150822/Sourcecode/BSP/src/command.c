/**
  ******************************************************************************
  * @file    FW_upgrade/src/command.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   This file provides all the IAP command functions.
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
#include "command.h"
/** @addtogroup STM32F429I-Discovery_FW_Upgrade
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define DOWNLOAD_FILENAME          "0:/Akey.bin"

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define BUFFER_SIZE        ((uint16_t)512*64) 
static uint8_t RAM_Buf[BUFFER_SIZE] =
  {
    0x00
  };
static uint32_t TmpProgramCounter = 0x00, TmpReadSize = 0x00 , RamAddress = 0x00;
static uint32_t LastPGAddress = APPLICATION_ADDRESS_A;

extern FATFS fatfs_SDCARD,fatfs_USBDISK;
extern unsigned int bw,br;
 FIL fileR;
 DIR dir;
 FILINFO fno;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  IAP Read all flash memory.
  * @param  None
  * @retval None
  */


uint8_t COMMAND_DOWNLOAD(void)
{
  /* Open the binary file to be downloaded */
  if (f_open(&fileR, DOWNLOAD_FILENAME, FA_READ) == FR_OK)
  {
    if (fileR.fsize > USER_FLASH_SIZE)
    {
      /* Toggle Red LED in infinite loop: No available Flash memory size for 
         the binary file */
        //  printf("Download file too large for flash size\r\n");
			  return 1; 
    }
		else
		{	
 
			//printf("Erase Sectors...\r\n");
      /* Erase FLASH sectors to download image */
      if (FLASH_If_EraseSectors(APPLICATION_ADDRESS_A) != 0x00)
      {
        /* Toggle Red LED in infinite loop: Flash erase error */
         //printf("EraseSectors failed\r\n");
				return 1;
				
      }
      else
			{
					//printf("Programming...\r\n");
					/* Program flash memory */
				FLASH_If_FlashUnlock();
				if(COMMAND_ProgramFlashMemory())
					return 1;
				else
				{
					
					/* Set Green LED ON: Download Done */

					/* Close file and filesystem */
					f_close (&fileR);
						//printf("UPDATE Finished!\r\n");
					return 0;
				}
			}
    
	}
  }
  else
  {
    /* Toggle Red LED in infinite loop: the binary file is not available */
		    return 1;
  }
}

/**
  * @brief  IAP jump to user program.
  * @param  None
  * @retval None
  */
void COMMAND_JUMP(void)
{
  /* Software reset */
  NVIC_SystemReset();
}

/**
  * @brief  Programs the internal Flash memory. 
  * @param  None
  * @retval None
  */
uint8_t COMMAND_ProgramFlashMemory(void)
{
  int32_t programcounter = 0x00;
  uint8_t readflag = TRUE;
  uint16_t BytesRead;
  
  /* RAM Address Initialization */
  RamAddress = (uint32_t) & RAM_Buf;
  
  /* Erase address init */
  LastPGAddress = APPLICATION_ADDRESS_A;
  
  /* While file still contain data */
  while (readflag == TRUE )
  {
    /* Read maximum 512 Kbyte from the selected file */
    f_read (&fileR, RAM_Buf, BUFFER_SIZE, (void *)&BytesRead);

		bsp_LED2On();
		
    /* Temp variable */
    TmpReadSize = BytesRead;
   // printf("Readsize=%d \r\n",TmpReadSize);
    /* The read data < "BUFFER_SIZE" Kbyte */
    if (TmpReadSize < BUFFER_SIZE)
    {
      readflag = FALSE;
    }
    
    /* Program flash memory */
    for (programcounter = TmpReadSize; programcounter >0; programcounter -= 4)
    {
      TmpProgramCounter = programcounter;
      /* Write word into flash memory */
      if (FLASH_If_ProgramWord((LastPGAddress - TmpProgramCounter + TmpReadSize), \
        *(__IO uint32_t *)(RamAddress - programcounter + TmpReadSize)) != FLASH_COMPLETE)
      {
        /* Toggle Red LED in infinite loop: Flash programming error */
        //printf("Write Flash Error\r\n");
				return 1;
      }
    }
    /* Update last programmed address value */
    LastPGAddress = LastPGAddress + TmpReadSize;
  }
	return 0;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
