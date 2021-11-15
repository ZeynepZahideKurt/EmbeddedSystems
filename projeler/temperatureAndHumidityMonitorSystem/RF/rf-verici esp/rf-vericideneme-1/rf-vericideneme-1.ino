 

/*
  #define fcsb  32//RTC 12 NUMARA FCSB
  #define csb 13 //RTC 11 NUMARA CSB
  #define SDIO  15//rtc 10 numara SDIO,
  #define SCLK 14 //rtc 9 numara SCLK
*/
/*
#define fcsb 34 //RTC 12 NUMARA FCSB
#define csb 35 //RTC 11 NUMARA CSB
#define SDIO 15 //rtc 10 numara SDIO
#define SCLK 14 //rtc 9 numara SCLK yeni*/

#define fcsb  15//RTC 12 NUMARA FCSB
#define csb 5 //RTC 11 NUMARA CSB
#define SDIO 19 //rtc 10 numara SDIO
#define SCLK 18 //rtc 9 numara SCLK yeni
#define GPIO3  14 // Transmitter için buton


#define GPIO1  27 // Receiver için buton burada kullanılmıyor

/* RF entegre                                                                                     işlemci uçları
  sclk  spi  clock  (burada input işlemcide output)                                                 9      14  e   6

  sdio   spi data input and output  (I/O)                                                           10     13  e   15,

  csb   spi chip selection bar for register access active low  (burada input  işlemcide output)     11     11 ye   35

  fcsb  spi chip selection bar for fifo access active low  (burada  input işlemcide output)         12     12 e    34
*/

#include "type.h"
#include "spi.h"
#include "CMT2300drive.h"
#include "stdio.h"
#include "TxConfig.h"
#include "RxConfig.h"


static unsigned char statetx = true;  //  false为RX  true为TX
#define LEN 6

unsigned char str[LEN] =  {" TEST"};
unsigned char getstr[LEN];
cmt2300aEasy radio;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(fcsb, OUTPUT);
  pinMode(csb, OUTPUT);
  pinMode(SDIO, OUTPUT);
  pinMode(SCLK, OUTPUT);
  
  pinMode(GPIO3, OUTPUT);
  digitalWrite(GPIO3, 0);
  pinMode(GPIO3, INPUT);
 // pinMode(GPIO1, INPUT);
  /*P3M1&=~(1<1);
       P3M0|=(1<<1);
       P3M1&=~(1<5);
       P3M0|=(1<<5);
       UART_INIT();
       EA = 1;*/
       printf("start0!\r\n");
  if (false == statetx )
  {
    CMT2219B_RxInit();
    setup_Rx();
    while (1)
    {
      loop_Rx();
    }
  }
  else
  {
    CMT2119B_TxInit();
    setup_Tx();
    while (1)
    {
      delay(1000);
      loop_Tx();
    }
  }

  Serial.println("4");
}

void CMT2119B_TxInit()
{

  radio.FixedPktLength    = false;
  radio.PayloadLength     = LEN;
  cmt2300aEasy_vInit();

  cmt2300aEasy_vCfgBank(CMTBank, 12);
  cmt2300aEasy_vCfgBank(SystemBank, 12);
  cmt2300aEasy_vCfgBank(FrequencyBank, 8);
  cmt2300aEasy_vCfgBank(DataRateBank, 24);
  cmt2300aEasy_vCfgBank(BasebandBank, 29);
  cmt2300aEasy_vCfgBank(TXBank, 11);

  cmt2300aEasy_vEnablePLLcheck();
  cmt2300aEasy_bGoSleep();          //Yapılandırmanın etkili olmasına izin verin

}



void CMT2219B_RxInit()
{

  radio.FixedPktLength    = false;
  radio.PayloadLength     = LEN;
  cmt2300aEasy_vInit();

  cmt2300aEasy_vCfgBank(CMT_Bank, sizeof(CMT_Bank) / sizeof(unsigned short int));
  cmt2300aEasy_vCfgBank(System_Bank, sizeof(System_Bank) / sizeof(unsigned short int));
  cmt2300aEasy_vCfgBank(Frequency_Bank, sizeof(Frequency_Bank) / sizeof(unsigned short int));
  cmt2300aEasy_vCfgBank(Data_Rate_Bank, sizeof(Data_Rate_Bank) / sizeof(unsigned short int));
  cmt2300aEasy_vCfgBank(Baseband_Bank, sizeof(Baseband_Bank) / sizeof(unsigned short int));
  cmt2300aEasy_vCfgBank(Reserve_Bank, sizeof(Reserve_Bank) / sizeof(unsigned short int));
  cmt2300aEasy_vCfgBank(LBD_Bank, sizeof(LBD_Bank) / sizeof(unsigned short int));

  cmt2300aEasy_vEnablePLLcheck();
  cmt2300aEasy_bGoSleep();          //Yapılandırmanın etkili olmasına izin verin

}


void setup_Tx(void)
{
cmt2300aEasy_bGoStandby();
  if (cmt2300aEasy_bGoStandby()) //;   //Yapılandırma moduna girin
  {
    printf("bGoStandby ok!\r\n");
  }
  cmt2300aEasy_vEnableAntSwitch(0);  //Anten switch_IO bağlantı noktası anahtarını ayarla
  cmt2300aEasy_vGpioFuncCfg(GPIO1_INT2 + GPIO2_INT2 + GPIO3_INT2); //
  
  cmt2300aEasy_vIntSrcCfg(INT_FIFO_NMTY_TX, INT_TX_DONE);    //IO bağlantı noktası eşlemesi
  cmt2300aEasy_vIntSrcEnable(TX_DONE_EN);           //Kesinti etkinleştir

  cmt2300aEasy_vClearFIFO();  //FIFO'yu temizle
  if (cmt2300aEasy_bGoSleep()) //;    //Uyku moduna geçin ve yapılandırmanın etkili olmasına izin verin
  {
    printf("bGoSleep ok!\r\n");;
  }

}

void setup_Rx(void)
{
  cmt2300aEasy_bGoStandby();   //Yapılandırma moduna girin
  cmt2300aEasy_vEnableAntSwitch(0); //1 olduğunda, GPIO1 ve GPIO2 kullanılamaz
  cmt2300aEasy_vGpioFuncCfg(GPIO1_INT1+GPIO2_Dout+GPIO3_DCLK);  //IOİşlev eşleme

  //cmt2300aEasy_vIntSrcCfg(INT_RSSI_VALID, INT_CRC_PASS);   //GPO3, CRC_pass kesmesiyle eşlenmiştir, bu kesmeyi burada kullanmak istiyorsanız, RFPDK'nin CRC'yi yapılandırması gerekir
  cmt2300aEasy_vIntSrcCfg(INT_PKT_DONE, INT_PKT_DONE);  //GPO3, PKT_DONE kesmesiyle eşleştirildi //IO bağlantı noktası kesintisinin eşlenmesi
  cmt2300aEasy_vIntSrcEnable(PKT_DONE_EN );          //(PKT_DONE_EN + CRC_PASS_EN);

  cmt2300aEasy_vClearFIFO();
  cmt2300aEasy_bGoSleep();           //Yapılandırmanın etkili olmasına izin verin
  cmt2300aEasy_bGoRx();              //for Rx
}



void loop_Tx()
{
  printf("send!\r\n");
  cmt2300aEasy_bSendMessage(str, LEN);
  int c= digitalRead(GPIO3);
   pinMode(GPIO3, INPUT);
  while(digitalRead(GPIO3)== 0);//while(GPO3_L());   // GPIO kesintisinin düşük olup olmadığını belirleyin ve yüksek için aşağıdaki kodu çalıştırın.
  cmt2300aEasy_bIntSrcFlagClr();
  cmt2300aEasy_vClearFIFO();
  delay(20);
}

unsigned char tmp;
void loop_Rx()
{
  //pinMode(GPIO1, INPUT);
//  int c=digitalRead(GPIO1);
  //Serial.print("c:");
 //Serial.println(c);
  
  //if(digitalRead(GPIO1)== 1) //if(GPO1_H())
 // {
  cmt2300aEasy_bGoStandby();
  tmp = cmt2300aEasy_bGetMessage(getstr); //Simülasyonun bu noktasında getstr tarafından alınan veri paketlerini görebilirsiniz.
  //Serial.println("getstr");
  printf("recv=%s\r\n", getstr);
  cmt2300aEasy_bIntSrcFlagClr();
  cmt2300aEasy_vClearFIFO();
  cmt2300aEasy_bGoRx();
  //}else
  //{
  printf("nothing!\r\n");
  delay(1000);

  //}
}
void loop() {

  // put your main code here, to run repeatedly:
//loop_Tx();

}
