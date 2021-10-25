#define RTC_FCSB 34 //RTC 12 NUMARA FCSB
#define RTC_CSB 35 //RTC 11 NUMARA CSB
#define RTC_SDA 15 //rtc 10 numara SDIO
#define RTC_SCL 6 //rtc 9 numara SCLK


#include "dk_hal.h"

/*#define RTC_RFIP 15 //RTC 1 NUMARA RF signal input P
#define RTC_RFIN 6 // RTC 2 NUMARA rtc 9 numara SCLK

#define RTC_RFIP 15 //RTC 1 NUMARA RF signal input P
#define RTC_RFIN 6 // RTC 2 NUMARA rtc 9 numara SCLK */

void TxLedOff(void)
{
 LATD &= 0x81;    
}

void setup() {
  // put your setup code here, to run once:
for(SysTime=0;SysTime<32;);  //ÑÓÊ±
 
 RFM219_Config();

 RxLed = 1;
}

void loop() {
  // put your main code here, to run repeatedly:
if(RFM219_RxPacket())  //does receive one packet message
    {
    RxLimtTime = 0;

    }
  if(RxLimtTime>=15)    //over time stop Buzzer   
 
}
