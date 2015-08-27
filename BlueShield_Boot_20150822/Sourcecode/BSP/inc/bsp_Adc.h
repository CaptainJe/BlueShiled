/***************************************************************************
 * Name:			bsp_Adc.h 				                                                
 *											
 * Function:	Battery power level
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#ifndef bsp_Adc_H
#define bsp_Adc_H

#include "stm32f4xx.h"
#define ADC1_DR_ADDRESS   ((uint32_t)0x4001204C) 
#define ADC_ConvertTime 100

extern u8 Battery_Level;
void bsp_AdcInit(void);
u16 Get_Battery(void);
#endif

