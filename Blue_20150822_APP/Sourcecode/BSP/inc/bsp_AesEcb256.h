#ifndef AES_ECB256_H
#define AES_ECB256_H
#include "stm32f4xx.h"
#include "crypto.h"

extern uint8_t Key[CRL_AES256_KEY];

int32_t STM32_AES_ECB_Encrypt(uint8_t* InputMessage,
                        uint32_t InputMessageLength,
                        uint8_t  *AES256_Key,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength);

int32_t STM32_AES_ECB_Decrypt(uint8_t* InputMessage,
                        uint32_t InputMessageLength,
                        uint8_t *AES256_Key,
                        uint8_t  *OutputMessage,
                        uint32_t *OutputMessageLength);
#endif
