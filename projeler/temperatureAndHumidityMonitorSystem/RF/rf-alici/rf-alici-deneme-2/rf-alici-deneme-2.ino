#include <arduino.h>
#include "RFM300_Driver.h"
#include "rf_conf.h"


byte rx_buf[32];


void setup(void)
{
  Serial.begin(115200);

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
  vGpioFuncCfg(GPIO1_DCLK + GPIO2_DCLK + GPIO3_INT2); //GPIO Maping

  vIntSrcCfg(INT_FIFO_WBYTE_RX, INT_PKT_DONE);    //IRQ  Maping
  vIntSrcEnable(CRC_PASS_EN | PKT_DONE_EN);
  bIntSrcFlagClr();
  vClearFIFO();
  bGoSleep();
  bGoRx();
  Serial.println("Rx...");

}

void loop(void)
{
  static uint16_t tx_cnt = 0;
  static uint16_t rx_cnt = 0;
  byte tmp;
 pinMode(GPO3_PIN,INPUT);


  /*if (GPO3_H()){
    //delay (200);
   // Serial.print("RSSI: ");
   // Serial.println(bReadRssi(1) - 128);
    tmp = bGetMessage(rx_buf);
    bIntSrcFlagClr();
    vClearFIFO();
   // Serial.println(tmp);
    if (tmp) {
      Serial.print(tmp);
      Serial.print(" bytes: ");
      Serial.write(rx_buf, tmp);
     Serial.print("\r\n");
    }
   }

   */
   delay(100);
if (GPO3_L()){
    //delay (200);
   // Serial.print("RSSI: ");
   // Serial.println(bReadRssi(1) - 128);
    tmp = bGetMessage(rx_buf);
    bIntSrcFlagClr();
    vClearFIFO();
   // Serial.println(tmp);
    if (tmp) {
      Serial.print(tmp);
      Serial.print(" bytes: ");
      Serial.write(rx_buf, tmp);
     Serial.print("\r\n");
    // Serial.print(" şimdi aldı");
    }
  }
   

}
