#include <arduino.h>
#include "RFM300_Driver.h"
#include "rf_conf.h"

#define TX_NUM  21

byte tx_buf[32] = {'H','o','p','e','R','F',' ','R','F','M',' ','C','O','B','R','F','M','3','0','0','A'};
byte rx_buf[32];
byte mode = 1;   // 0: receive , 1: transmitter 

//#define KEY1  2 //arduino pin 2 for select work mode
//#define LED1  3 //arduino pin 3 for led indicate


void setup(void)
{
  //system source setting
  //pinMode(KEY1,INPUT_PULLUP);  
  //pinMode(LED1,OUTPUT);  
  //digitalWrite(LED1,LOW);   //turn off LED
  Serial.begin(115200);

  //radio configure
  FixedPktLength    = false;
  PayloadLength     = 21; 
  vInit();
  vCfgBank(CMTBank, 12);
  vCfgBank(SystemBank, 12);
  vCfgBank(FrequencyBank, 8);
  vCfgBank(DataRateBank, 24);
  vCfgBank(BasebandBank, 29);
  vCfgBank(TXBank, 11);
  vEnableAntSwitch(0);
  vGpioFuncCfg(GPIO1_DCLK+GPIO2_DCLK+GPIO3_INT2);   //GPIO Maping


 
    mode = 1;
    vIntSrcCfg(INT_FIFO_NMTY_TX, INT_TX_DONE);    //IRQ maping
    vIntSrcEnable(TX_DONE_EN);                   
    vClearFIFO();
    bGoSleep();
    Serial.println("Tx...");
 
}

void loop(void)
{
  static uint16_t tx_cnt=0;
  static uint16_t rx_cnt=0;
  byte tmp;
      bSendMessage(tx_buf, TX_NUM);
      while(GPO3_L());
      bIntSrcFlagClr();
      vClearFIFO(); 
      bGoSleep();
      Serial.print("Tx: ");
      Serial.println(++tx_cnt);
      Serial.println("...............................................................................................");
      delay(1000);  
  
  
}
