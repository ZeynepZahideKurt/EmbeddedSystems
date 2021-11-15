/*
  #define digitalWrite(csb, 1)  digitalWrite(csb, 1)
  #define digitalWrite(csb, 0)  digitalWrite(csb, 0)
  #define digitalWrite(fcsb, 1) digitalWrite(fcsb, 1)
  #define digitalWrite(fcsb, 0) digitalWrite(fcsb, 0)
  #define digitalWrite(SCLK, 1) digitalWrite(SCLK, 1)
  #define digitalWrite(SCLK, 0) digitalWrite(SCLK, 0)
  
  #define digitalWrite(SDIO, 1) digitalWrite(SDIO, 1)
  #define digitalWrite(SDIO, 0) digitalWrite(SDIO, 0)
  //#define   InputSDIO()   //GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_IN_FL_NO_IT)
 // #define OutputSDIO()  //GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_OUT_PP_LOW_FAST)
  #define SDIO_H()  */
  
   #define SPI3_SPEED 1
/**********************************************************
**Name:   vSpi3Init
**Func:   Init Spi-3 Config
**Note:
**********************************************************/
void spi3Class_vSpi3Init()
{
//  GPIO_Init(GPIO_PORT_C, (GPIO_Pin_TypeDef)GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);;  //SPI-3 for output
  digitalWrite(csb, 1);
  digitalWrite(fcsb, 1);
  digitalWrite(SDIO, 1);
  digitalWrite(SCLK, 0);
}
/**********************************************************
**Name:   vSpi3WriteByte
**Func:   SPI-3 send one byte
**Input:
**Output:
**********************************************************/
void vSpi3WriteByte(uint8_t dat)
{
  uint8_t bitcnt;
  digitalWrite(fcsb, 1);        //FCSB = 1;
  pinMode(SDIO, OUTPUT);
  //pinMode(SDIO, OUTPUT);
  digitalWrite(SDIO, 1);        //    output 1
  digitalWrite(SCLK, 0);
  digitalWrite(csb, 0);
  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    digitalWrite(SCLK, 0);
    delayMicroseconds(SPI3_SPEED);//delay(SPI3_SPEED);
    
    if(dat&0x80){
      digitalWrite(SDIO, 1);
    }
    else
      digitalWrite(SDIO, 0);
    digitalWrite(SCLK, 1);
    dat <<= 1;
    delayMicroseconds(SPI3_SPEED); //delay(SPI3_SPEED);  
    }
  digitalWrite(SCLK, 0);
  digitalWrite(SDIO, 1);
}
/**********************************************************
**Name:   bSpi3ReadByte
**Func:   SPI-3 read one byte
**Input:
**Output:
**********************************************************/
uint8_t bSpi3ReadByte()
{
  uint8_t RdPara = 0;
  uint8_t bitcnt;
  digitalWrite(csb, 0);
  pinMode(SDIO, INPUT);
  //pinMode(SDIO, INPUT);
  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    digitalWrite(SCLK, 0);
    
    RdPara <<= 1;
    delayMicroseconds(SPI3_SPEED); //delay(SPI3_SPEED);
    digitalWrite(SCLK, 1);
    delayMicroseconds(SPI3_SPEED); //delay(SPI3_SPEED);
    if(digitalRead(SDIO)){
      RdPara |= 0x01;
    }
    else{
      RdPara |= 0x00;
      }
    
    }
  digitalWrite(SCLK, 0);
  
  pinMode(SDIO, OUTPUT);
  //pinMode(SDIO, OUTPUT);
  digitalWrite(SDIO, 1);
  digitalWrite(csb, 1);
  return(RdPara);
}
/**********************************************************
**Name:   vSpi3Write
**Func:   SPI Write One word
**Input:  Write word
**Output: none
**********************************************************/
void spi3Class_vSpi3Write(uint16_t dat)
{
  vSpi3WriteByte((uint8_t)(dat>>8)&0x7F);
  vSpi3WriteByte((uint8_t)dat);
  digitalWrite(csb, 1);
}
/**********************************************************
**Name:   bSpi3Read
**Func:   SPI-3 Read One byte
**Input:  readout addresss
**Output: readout byte
**********************************************************/
uint8_t spi3Class_bSpi3Read(uint8_t addr)
{
    vSpi3WriteByte(addr|0x80);
  return(bSpi3ReadByte());
}
/**********************************************************
**Name:   vSpi3WriteFIFO
**Func:   SPI-3 send one byte to FIFO
**Input:  one byte buffer
**Output: none
**********************************************************/
void spi3Class_vSpi3WriteFIFO(uint8_t dat)
{
  uint8_t bitcnt;
  digitalWrite(csb, 1);
  pinMode(SDIO, OUTPUT);
  digitalWrite(SCLK, 0);
  digitalWrite(fcsb, 0);      //FCSB = 0
  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    digitalWrite(SCLK, 0);
    if(dat&0x80)
      digitalWrite(SDIO, 1);
    else
      digitalWrite(SDIO, 0);
    delayMicroseconds(SPI3_SPEED); //delay(1);
    digitalWrite(SCLK, 1);
    delayMicroseconds(SPI3_SPEED); //delay(1);
    dat <<= 1;
    }
  digitalWrite(SCLK, 0);
  delayMicroseconds(SPI3_SPEED); //delay(1);   //Time-Critical
  delayMicroseconds(SPI3_SPEED); //delay(1);   //Time-Critical
  digitalWrite(fcsb, 1);
  digitalWrite(SDIO, 1);
  delayMicroseconds(SPI3_SPEED); //delay(1);   //Time-Critical
  delayMicroseconds(SPI3_SPEED); //delay(1);   //Time-Critical
}
/**********************************************************
**Name:   bSpi3ReadFIFO
**Func:   SPI-3 read one byte to FIFO
**Input:  none
**Output: one byte buffer
**********************************************************/
uint8_t spi3Class_bSpi3ReadFIFO()
{
  uint8_t RdPara;
  uint8_t bitcnt;
  digitalWrite(csb, 1);
  pinMode(SDIO, INPUT);
  digitalWrite(SCLK, 0);
  digitalWrite(fcsb, 0);
  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    digitalWrite(SCLK, 0);
    RdPara <<= 1;
    delayMicroseconds(SPI3_SPEED); //delay(1);
    digitalWrite(SCLK, 1);
    delayMicroseconds(SPI3_SPEED);
   if(digitalRead(SDIO))
      RdPara |= 0x01;   //NRZ MSB
    else
      RdPara |= 0x00;   //NRZ MSB
    }
  digitalWrite(SCLK, 0);
  delayMicroseconds(SPI3_SPEED);  //delay(1);   //Time-Critical
  delayMicroseconds(SPI3_SPEED);  //delay(1);   //Time-Critical
  digitalWrite(fcsb, 1);
  pinMode(SDIO, OUTPUT);
  digitalWrite(SDIO, 1);
  delayMicroseconds(SPI3_SPEED); //delay(1);   //Time-Critical
  delayMicroseconds(SPI3_SPEED); //delay(1);   //Time-Critical
  return(RdPara);
}
/**********************************************************
**Name:   vSpi3BurstWriteFIFO
**Func:   burst wirte N byte to FIFO
**Input:  array length & head pointer
**Output: none
**********************************************************/
void spi3Class_vSpi3BurstWriteFIFO(uint8_t ptr[], uint8_t length)
{
  uint8_t i;
  if(length!=0x00)
    {
    for(i=0;i<length;i++)
      spi3Class_vSpi3WriteFIFO(ptr[i]);
    }
  return;
}
/**********************************************************
**Name:   vSpiBurstRead
**Func:   burst wirte N byte to FIFO
**Input:  array length  & head pointer
**Output: none
**********************************************************/
void spi3Class_vSpi3BurstReadFIFO(uint8_t ptr[], uint8_t length)
{
  uint8_t i;
  if(length!=0)
    {
    for(i=0;i<length;i++)
      ptr[i] = spi3Class_bSpi3ReadFIFO();
    }
  return;
}
