#include <arduino.h>
#include "RFM300_Driver.h"
#include "rf_conf.h"

#define TX_NUM	21

byte tx_buf[32] = {'H','o','p','e','R','F',' ','R','F','M',' ','C','O','B','R','F','M','3','0','0','A'};
byte rx_buf[32];
byte mode = 0;	 // 0: receive , 1: transmitter	

#define KEY1	2	//arduino pin 2 for select work mode
#define LED1	3	//arduino pin 3 for led indicate


void setup(void)
{
	//system source setting
	pinMode(KEY1,INPUT_PULLUP);  
	pinMode(LED1,OUTPUT);	 
	digitalWrite(LED1,LOW); 	//turn off LED
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

	//determine work mode 
	if(!digitalRead(KEY1)){
		mode = 1;
		vIntSrcCfg(INT_FIFO_NMTY_TX, INT_TX_DONE);    //IRQ maping
		vIntSrcEnable(TX_DONE_EN);                   
		vClearFIFO();
		bGoSleep();
		Serial.println("Tx...");
	}
	else{
		mode = 0;
		vIntSrcCfg(INT_FIFO_WBYTE_RX, INT_PKT_DONE);    //IRQ  Maping
		vIntSrcEnable(CRC_PASS_EN|PKT_DONE_EN); 
		bIntSrcFlagClr();
		vClearFIFO();
		bGoSleep();
		bGoRx(); 
		Serial.println("Rx...");
	}
}

void loop(void)
{
	static uint16_t tx_cnt=0;
	static uint16_t rx_cnt=0;
	byte tmp;

	if(mode){
			bSendMessage(tx_buf, TX_NUM);
			while(GPO3_L());
			bIntSrcFlagClr();
			vClearFIFO(); 
			bGoSleep();
			Serial.print("Tx: ");
			Serial.println(++tx_cnt);
			delay(1000);  
	}
	else{
		if(GPO3_H())
		{
			Serial.print("RSSI: ");
			Serial.println(bReadRssi(1)-128);
			tmp =bGetMessage(rx_buf);
			bIntSrcFlagClr();
			vClearFIFO(); 
			if(tmp){
				Serial.print(tmp);
				Serial.print(" bytes: ");
				Serial.write(rx_buf,tmp);
				Serial.print("\r\n");
			}
		}			
	}
}
