#include "dk_hal.h"

/**********************************************************
函数名称：Interrupt ISR
函数功能：初始化Sfr
**********************************************************/
void interrupt ISR(void)
{
 //byte i;	
 if(TMR0IF)						//TMR0中断
 	{
 	TMR0IF = 0;
	SysTime++;
 	RxLimtTime++;
 	}
 
 if(TMR1IF)						//TMR1中断
 	{
 	TMR1IF = 0;
 	TMR1L  = (byte)RTC_INTV;
 	TMR1H  = (byte)(RTC_INTV>>8);	
	SecTime++;
 	}
 
 if(RCIF)
 	{
 	RCIF = 0;
	TXREG = RCREG;
 	}	
}