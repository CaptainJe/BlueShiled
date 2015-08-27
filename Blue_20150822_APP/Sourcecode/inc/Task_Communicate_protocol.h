/***************************************************************************
 * Name:Task_Communicate_protocol.h 				                                                
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
#ifndef Communicate_protocol_H
#define Communicate_protocol_H

#include "stm32f4xx.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "task.h"
#include "list.h"
#include "string.h"
#include "bsp_AesEcb256.h"
#include "stdlib.h"
#include "Task_HardwareControl.h"
#include "bsp_FileSystem.h"
#include "bsp_Adc.h"
/******************* Macro defination *************************************/
#define L1_HEADER_MAGIC  (0xAB)     /*header magic number */
#define L1_HEADER_VERSION (0x00)     /*protocol version */
#define L1_HEADER_SIZE   (8)      /*L1 header length*/
#define L2_KEY_HEADER_RESERVE (0x00) /*L2 key header reserve*/
/**************************************************************************
* define L1 header byte order
***************************************************************************/
#define L1_HEADER_MAGIC_POS             (0)
#define L1_HEADER_PROTOCOL_VERSION_POS  (1)
#define L1_PAYLOAD_LENGTH_HIGH_BYTE_POS (2)         /* L1 payload lengh high byte */
#define L1_PAYLOAD_LENGTH_LOW_BYTE_POS  (3)
#define L1_HEADER_CRC16_HIGH_BYTE_POS   (4)
#define L1_HEADER_CRC16_LOW_BYTE_POS    (5)
#define L1_HEADER_SEQ_ID_HIGH_BYTE_POS  (6)
#define L1_HEADER_SEQ_ID_LOW_BYTE_POS   (7)
#define L1_FIRST_PAYLOAD_POS            (8)

#define L1_ACCESS_RESPONSE_PAYLOAD_LENGTH 37
#define L2_ACCESS_RESPONSE_KEYVALUE_LENGTH 32
/********************************************************************************
* define version response
*********************************************************************************/
typedef enum {
    DATA_PACKAGE = 0,
    RESPONSE_PACKAGE =  1,
}L1_PACKAGE_TYPE;

/********************************************************************************
* define ack or nak
*********************************************************************************/
typedef enum {
    ACK = 0,
    NAK = 1,
}L1_ERROR_FLAG;

/*******************************************************************************
* debvice loss alert level
********************************************************************************/
typedef enum {
    NO_ALERT = 0,
    MIDDLE_ALERT = 1,
    HIGH_ALERT = 2
} DEV_LOSS_ALERT_LEVEL;

#define L2_HEADER_SIZE   (2)      /*L2 header length*/
#define L2_HEADER_VERSION (0x00)     /*L2 header version*/
#define L2_KEY_SIZE         (1)
#define L2_PAYLOAD_HEADER_SIZE (3)        /*L2 payload header*/

#define L2_HEADER_CMDID_POS (0)
#define L2_HEADER_VERSION_POS (1)
#define L2_FIRST_KEY_POS (2)
#define L2_FISRT_KEYHEADER_HIGH_BYTE_POS	(3)
#define L2_FISRT_KEYHEADER_LOW_BYTE_POS	(4)
#define L2_FIRST_VALUE_POS (L2_HEADER_SIZE + L2_PAYLOAD_HEADER_SIZE)

/******************* Enum & Struct defination ******************************/
/* L1 header struct */
/*L1 version defination */
typedef struct
{
uint8_t version  :
    4;
uint8_t ack_flag  :
    1;
uint8_t err_flag  :
    1;
uint8_t reserve  :
    2;
}
L1_version_def_t;

typedef union{
    L1_version_def_t version_def;
    uint8_t value;
}L1_version_value_u;

typedef struct
{
    uint8_t magic;
    L1_version_value_u version;
    uint16_t payload_len;
    uint16_t crc16;
    uint16_t sequence_id;
}
L1Header_t;


enum crc_check_result
{
    CRC_SUCCESS = 0,
    CRC_FAIL = 1
};


typedef struct
{

  uint16_t v_length  :
    12;
	uint16_t reserve  :
    4;
}
Key_Header_t;
typedef union
{
    uint16_t data;
    Key_Header_t key_header_bit_field;
} Key_Header_u;

typedef struct
{
    uint8_t cmd_ID;
    uint8_t version;
    uint8_t key;
    Key_Header_u key_header;
}
L2DataHeader_t;

typedef enum  {
		COMMAND_ACCESS_REQ = 0x00,
    COMMAND_DEVICE_INFO = 0x10,
    COMMAND_DEVICE_SETTING  = 0x20,
    COMMAND_DEVICE_CONTROL  = 0x30,
    COMMAND_DEVICE_KEYBACK  = 0x31,
    COMMAND_DEVICE_LOG      = 0x40,
    COMMAND_FIRMWARE_UPDATE = 0x50,
    COMMAND_T_TRANS= 0x60,
	  COMMAND_TEST=0X75,
	  COMMAND_MCU_ROM=0x61,
	  COMMAND_NOTIFY=0x62,
	  COMMAND_PING=0X63,
	  
}COMMAND_ID_TYPE;

typedef struct  {
	u8 L2_KEY;
	Key_Header_u L2_KEY_Header;
	u8 *L2_KEY_Value;
}L2_KEY;

typedef enum  {
		L1_IDLE = 0x01,
    L1_RCVING  = 0x02,
		L1_UNPACKING = 0x03,
}L1_RCV_STATUS;

typedef enum  {
		L1_TX_IDLE = 0x01,
    L1_TX_START = 0x02,
		L1_TX_SENDING = 0x03,
}L1_TX_STATUS;


typedef enum  {
		KEY_ACCESS_REQ = 0x01,
    KEY_ACCESS_RESPONSE  = 0x02,
}ACCESS_KEY;

typedef enum  {
    KEY_INFO_Manufacturer_Name = 0x10,
    KEY_INFO_Model_Number = 0x20,
    KEY_INFO_Serial_Number = 0x21,
    KEY_INFO_Hardware_Rev =  0x22,   
    KEY_INFO_Firmware_Rev =  0x30,   
    KEY_INFO_Software_Rev =  0x31,        
	  KEY_INFO_Battery_Level =      0x32,
	  KEY_INFO_Req = 0x41,
    KEY_INFO_System_ID =   0x42,
	  KEY_INFO_Battery_Test = 0x43,
	  KEY_IST_IN=0x50,
}INFO_KEY;

typedef enum  {
		KEY_ACCESS_TOKEN = 0x00,
    KEY_SETTING_QUERY  = 0x10,
    KEY_DEVICE_NAME_SET  = 0x20,
    KEY_DEVICE_BATTERY_THRESHOLD  = 0x21,
    KEY_KEY_SOUND_SET    = 0x22,
	  KEY_DELAY_SET = 0X01,
}SETTING_KEY;

typedef enum  {
    KEY_STOP_CONTROL  = 0x10,
    KEY_REDLED_MODE  = 0x20,
    KEY_GREENLED_MODE = 0x21,
    KEY_BLUELED_MODE  = 0x22,
		KEY_BUZZER_MODE = 0x30,
	  KEY_START_CONTROL = 0X31,
}CONTROL_KEY;

typedef enum  {
    KEY_KEYBACK  = 0x10,

}KEYBACK_KEY;

typedef enum  {
    KEY_START_LOG = 0x10,
    KEY_STOP_LOG  = 0x11,
    KEY_UPLOAD_LOG = 0x12,
}LOG_KEY;

typedef enum  {
	  KEY_T_COMMAND = 0x10,
	  KEY_T_RESPOND = 0X20,
    KEY_T_Connect = 0x00,
		KEY_T_Discon  = 0x21,
	  KEY_T_GetSN   = 0x20,
	  KEY_T_Write   = 0x30,
	  KEY_T_Read    = 0x40,
}T_TRANS_KEY;

typedef enum  {
	  KEY_MCU_COMMAND = 0x10,
	  KEY_MCU_RESPOND = 0X20,
    KEY_MCU_Read = 0x00,
		KEY_MCU_Write  = 0x21,
}T_MCU_KEY;


typedef enum {
    KEY_FIRMWARE_INFO  = 0x01,
    KEY_FIRMWARE_WRITEBLOCK         = 0x02,
    KEY_FIRMWARE_WRITEFINISHED      = 0x10,
    KEY_RET_FIRMWARE_CHECK   = 0x11,
	  KEY_START_UPDATE         = 0x20,
}FIRMWARE_UPDATE_KEY;

typedef enum{
    PRIVATE_NOT_BOND = 0,
    PRIVATE_BOND_SUCCESS = 1,
} BLUETOOTH_BOND_STATE;


typedef enum  {
    SEND_SUCCESS = 1,
    SEND_FAIL   = 0,
}SEND_STATUS;

typedef enum  {
    RCV_SUCCESS = 1,
    RCV_FAIL   = 0,
}RCV_STATUS;

typedef enum {
    WAIT_START = 0,
    WAIT_MESSAGE,
    MESSAGE_RESOLVE
}RECEIVE_STATE;

//async send operation callback
typedef void (*send_status_callback_t)(SEND_STATUS status );


/****************************************************************
* sending Buffer use state
*****************************************************************/
typedef struct
{
uint8_t isUsed      :
    1;
uint8_t TxComplete  :
    1;
uint8_t GetResPack  :
    1;
uint8_t reserved    :
    5;
}
SendingBufferUseState_t;

typedef union {
    uint8_t data;
    SendingBufferUseState_t usingState;
}SendingBufferUseState;

/*Used by L1, used to send whole L1 package throw bluetooth */
typedef struct
{
    uint8_t *         content;   /* content to be send */
    send_status_callback_t                   callback;   /* send status callback */
    uint16_t          length;    /* content length */
    uint16_t             contentLeft;     /* content left for send*/
    uint16_t         sequence_id;        /* sequence id for this package*/
    uint8_t                         isUsed;    /* is the current struct used*/
    uint8_t             resendCount;     /* whole L1 package resend count */
}
L1_Send_Content;

/*used by L2, To describe the L2 content to be send */
typedef struct
{
    uint8_t *         content;   /* content to be send */
    send_status_callback_t   callback;   /* send status callback */
    uint16_t          length;    /* content length */
}
L2_Send_Content;

typedef enum {
    CONTENT_NONE = 0,
    CONTENT_HEADER = 1,
    CONTENT_DATA = 2,
    CONTENT_ACK = 3
}SEND_CONTENT_TYPE_T;

/***********************************************************************
* This enum describe the current task type
************************************************************************/
typedef enum {
    TASK_NONE = 0,
    TASK_DATA = 1,
    TASK_ACK = 2
}SEND_TASK_TYPE_T;

typedef struct
{
    L1_Send_Content * content;
    uint16_t        isUsed;
}
L1_Header_Schedule_type_t;

/* time bit field */
typedef struct
{
uint32_t seconds  :
    6;
uint32_t minute  :
    6;
uint32_t hours  :
    5;
uint32_t day   :
    5;
uint32_t month  :
    4;
uint32_t year   :
    6;
}
time_bit_field_type_t;

typedef union
{
    uint32_t data;
    time_bit_field_type_t time;
} time_union_t;
/* time bit field */

/* Definend for send callback */
typedef void (*send_complete_callback_t)(void *context,SEND_TASK_TYPE_T type);

typedef  struct
{
    send_complete_callback_t callback;
    void * context;
    SEND_TASK_TYPE_T task_type;
}
SendCompletePara;


/* log command and key */
typedef enum {
    KEY_ENABLE_BLUETOOTH_LOG    = 0x01,
    KEY_DISABLE_BLUETOOTH_LOG   = 0x02,
    KEY_BLUETOOTH_LOG_CONTENT   = 0x03,
}log_command_key_t;

/******************* Function defination **********************************/
void USART_L1_TXTask(void * pvParameters);
void USART_L1_RXTask(void * pvParameters);
void USART_L0_TXTask(void * pvParameters);
void USART_L0_RXTask(void * pvParameters);
#endif
