#include "bsp_Delay.h"
#include "stm32f4xx.h"
//168mhz
u16 ms_index=0;	

void ForDelay_1us(void)//1us(1.05us)
{
	u32 Delayi;
	for(Delayi=0;Delayi<25;Delayi++)
	{
		;
	}
}

void ForDelay_1ms(void) //1ms(0.9994ms)
{       
	u32 Delayj;
	for(Delayj=0;Delayj<935;Delayj++)
	{
		ForDelay_1us();
	}
}

void ForDelay_nus(unsigned int us)//1000us(916us)
{
	unsigned int i;
	for(i=0;i<us;i++)
	{
		ForDelay_1us();
	}
}

void ForDelay_nms(unsigned int ms) //1000ms(999.3ms)
{
	unsigned int i;
	for(i=0;i<ms;i++)
	{
		ForDelay_1ms(); 
	}
}
