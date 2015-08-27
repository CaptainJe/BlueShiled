/***************************************************************************
 * Name:bsp_Adc.c 				                                                
 *											
 * Function:	Battery power level
 *                                                                                      																		
 * Prepared: 	Jerry Yang
 *																				 											
 * Rev	Date	 Author
 * 0.1  150305  JY	   										
 ***************************************************************************/
#include "bsp_Adc.h"

//Initialize ADC ports,config ADC and DMA
//float VoltagetoPercent[]={
//3.90, 3.80, 3.76, 3.73, 3.72, //200,194,188,182,176,
//3.71, 3.69, 3.68, 3.67, 3.64, //170,164,158,152,146,
//3.63, 3.62, 3.60, 3.59, 3.58, //140,134,128,122,116,
//3.57, 3.56, 3.55, 3.54, 3.53, //110,104, 98, 92, 86,
//3.52, 3.51, 3.50, 3.49, 3.48, //80,  74, 68, 62, 56
//3.47, 3.46, 3.45, 3.44, 3.43, //50,  44, 38, 32, 26
//3.42, 3.41, 3.40, 3.38	            //20,  14, 8,  0
//};

float VoltagetoCapacity[]={
4.146, 4.123, 4.110, 4.099,	4.091, //200,199,198,197,196
4.083, 4.076, 4.069, 4.063, 4.057, //195,194,193,192,191
4.051, 4.046, 4.040, 4.034, 4.029, //190,189,188,187,186
4.024, 4.018, 4.014, 4.009, 4.004, //185,184,183,182,181
3.999, 3.994, 3.989, 3.984, 3.979, //180,179,178,177,176
3.975, 3.970, 3.965, 3.961, 3.956, //175,174,173,172,171
3.952, 3.947, 3.943, 3.938, 3.934, //170------------175
3.930, 3.925, 3.921, 3.917, 3.913, //
3.909, 3.904, 3.900, 3.896, 3.892,
3.889, 3.884, 3.881, 3.877, 3.873,
3.869, 3.865, 3.862, 3.858, 3.855,
3.851, 3.847, 3.844, 3.841, 3.837,
3.833, 3.830, 3.827, 3.823, 3.820,
3.817, 3.814, 3.810, 3.807, 3.804,
3.801, 3.798, 3.795, 3.792, 3.788,
3.786, 3.783, 3.780, 3.777, 3.774,
3.771, 3.768, 3.765, 3.762, 3.760,
3.757, 3.754, 3.752, 3.749, 3.746,
3.743, 3.741, 3.738, 3.736, 3.733,
3.730, 3.728, 3.726, 3.723, 3.721,
3.718, 3.716, 3.713, 3.711, 3.709,  //100,99,98,97,96
3.706, 3.704, 3.702, 3.699, 3.698,
3.695, 3.693, 3.690, 3.689, 3.686,
3.685, 3.682, 3.681, 3.679, 3.676,
3.674, 3.672, 3.671, 3.669, 3.667,
3.665, 3.664, 3.662, 3.660, 3.659,
3.656, 3.655, 3.653, 3.651, 3.650,
3.648, 3.646, 3.645, 3.643, 3.641,
3.640, 3.638, 3.637, 3.635, 3.633,
3.632, 3.630, 3.628, 3.627, 3.626,
3.624, 3.622, 3.621, 3.619, 3.617,
3.615, 3.613, 3.611, 3.609, 3.607,
3.605, 3.602, 3.600, 3.597, 3.595, //40
3.593, 3.590, 3.587, 3.584, 3.581,
3.578, 3.575, 3.571, 3.568, 3.564, //30,29,28,27,26,
3.560, 3.556, 3.552, 3.547, 3.542, //25,24,23,22,21,
3.536                              //20
};

u8 Battery_Level=100;
double Battery_Voltage=0;
u16 FilteredADC=0;
u16 Battery_Voltage_Test=0;
float VotperCount=((float)3.3/4095);
u16 Get_Adc(void)  
{
	u32 Adc_Sum=0;
	u8 	Con_index;
	u16 ADC_Value[ADC_ConvertTime]={0};
	u16 i,j=0;
	u16 temp=0;
	u16 FilteredValue=0;
	for(Con_index=0;Con_index<ADC_ConvertTime;Con_index++)
	{
		ADC_SoftwareStartConv(ADC1);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		ADC_Value[Con_index]=ADC_GetConversionValue(ADC1);
	}
	
	for(i=0;i<ADC_ConvertTime-1;i++)
	{
		for(j=0;j<ADC_ConvertTime-1-i;j++)
		{
			if(ADC_Value[j]>ADC_Value[j+1])
			{
				temp=ADC_Value[j];
				ADC_Value[j]=ADC_Value[j+1];
				ADC_Value[j+1]=temp;
			}
		}
	}
	
	for(i=20;i<ADC_ConvertTime-20;i++)
	{
		Adc_Sum+=ADC_Value[i];
	}
	FilteredValue=(u16)(Adc_Sum/(ADC_ConvertTime-40));
	return FilteredValue;
}

u8 Get_Percent(float Bat_Vot)
{
	u8 index=0;
	float Bate=0;
	for(index=0;index<181;index++)
	{
		if(Bat_Vot>VoltagetoCapacity[index])
		{
			break;
		}
	}
	if(index==181)
	{
		Bate=0;
	}
	else if(index==0)
	{
		Bate=100;
	}
	else
	{	
		//Bate=(200-index*6)+(((float)Bat_Vot-VoltagetoPercent[index])/(VoltagetoPercent[index-1]-VoltagetoPercent[index]))*(6);
		Bate=(float)((float)((200-index)-20)/180.0);
		Bate=Bate*100;
	}
  return Bate;
}

u8 Get_Battery(void)
{
	u16 Battery_Percent=0;
	FilteredADC=Get_Adc();
	Battery_Voltage=(double)FilteredADC*VotperCount;
	Battery_Voltage*=1.300752;
	
	
	if(Battery_Voltage>=4.0)
	{
		Battery_Voltage+=0.037;
	}
	
	else if(Battery_Voltage>=3.9)
	{
		Battery_Voltage+=0.036;
	}
	
	else if(Battery_Voltage>=3.87)
	{
		Battery_Voltage+=0.035;
	}
	
	else if(Battery_Voltage>=3.84)
	{
		Battery_Voltage+=0.034;
	}
	else
	{
		Battery_Voltage+=0.032;
	}
	Battery_Voltage_Test=(u16)(Battery_Voltage*1000);
	Battery_Percent=Get_Percent(Battery_Voltage);
	return Battery_Percent;
}
void bsp_AdcInit(void)
{
	/*  Pinout & Signal
			BAT_ADC--------------PA4--------------ADC1_CH4
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
  ADC_DeInit();
  /* ADC2 configuration ------------------------------------------------------*/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
/* ADCCLK = PCLK2/2 */
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
/* Available only for multi ADC mode */
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
/* Delay between 2 sampling phases */
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles; 
  ADC_CommonInit(&ADC_CommonInitStructure);

  //ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
	
  /* ADC1 regular channel configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_480Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  Battery_Level=Get_Battery();
}



