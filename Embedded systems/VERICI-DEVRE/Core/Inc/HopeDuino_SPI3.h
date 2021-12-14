#ifndef HopeDuino_SPI3_h
#define HopeDuino_SPI3_h

//#include <arduino.h>
//#include <util/delay.h>
//#include "stm32l0xx_hal_gpio.h"
//#include "stm32l0xx_hal.h"
#ifndef  byte
typedef unsigned char byte;
#endif

#ifndef word
typedef unsigned int  word;
#endif

#define SPI3_SPEED  1
UART_HandleTypeDef hlpuart1;
/** Hardware brief **/

//#define NC2       4
//#define NC1       5
//#define GPO3        14

//#define GPO2      7

//#define GPO1      14
//2-4-5-16-17-18-21-22

/*#define FCSB      15
#define SDIO      25//19
#define SCLK        27 // 18
#define CSB         13// 5

//#define NC0         9

#define SetCSB()  digitalWrite(CSB,HIGH)
#define ClrCSB()  digitalWrite(CSB,LOW)

#define SetFCSB() digitalWrite(FCSB,HIGH)
#define ClrFCSB() digitalWrite(FCSB,LOW)

#define SetSDCK() digitalWrite(SCLK,HIGH)
#define ClrSDCK() digitalWrite(SCLK,LOW)

#define SetSDIO() digitalWrite(SDIO,HIGH)
#define ClrSDIO() digitalWrite(SDIO,LOW)

#define OutputCSB()   pinMode(CSB,OUTPUT)
#define OutputFCSB()  pinMode(FCSB,OUTPUT)
#define OutputSDCK()  pinMode(SCLK,OUTPUT)

#define OutputSDIO()  pinMode(SDIO,OUTPUT)
#define InputSDIO()     pinMode(SDIO,INPUT)

#define SDIO_H()  (digitalRead(SDIO)==1)
#define SDIO_L()  (digitalRead(SDIO)==0)*/

#define	SetCSB()	HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, 1)
  #define	ClrCSB()	HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, 0)

  #define	SetFCSB()	HAL_GPIO_WritePin(FCSB_GPIO_Port, FCSB_Pin, 1)
  #define	ClrFCSB()	HAL_GPIO_WritePin(FCSB_GPIO_Port, FCSB_Pin, 0)

  #define	SetSDCK()	HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, 1)
  #define	ClrSDCK()	HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, 0)

  #define	SetSDIO()	HAL_GPIO_WritePin(SDIO_GPIO_Port, SDIO_Pin, 1)
  #define	ClrSDIO()	HAL_GPIO_WritePin(SDIO_GPIO_Port, SDIO_Pin, 0)

  //#define   InputSDIO()		//GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_IN_FL_NO_IT)
  //#define	OutputSDIO()	//GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_OUT_PP_LOW_FAST)

  #define	SDIO_H()	HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin) == 1
  #define	SDIO_L()	HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin) == 0
/*volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004; //address of the register

volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000; //address of the register

volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC; //address of the register

void EnableTiming(void)

{

*SCB_DEMCR = *SCB_DEMCR | 0x01000000;

*DWT_CYCCNT = 0; // reset the counter

*DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter

}



//******************************************************************************

void HAL_Delay_us(uint32_t tick)

{

unsigned int start, current;

start = *DWT_CYCCNT;

do

{

current = *DWT_CYCCNT;

} while((current - start) < tick);

}*/




void vSpi3Init(void);       /** initialize software SPI-3 **/
void InputSDIO(void);
void OutputSDIO(void);
void vSpi3Write(word dat);      /** SPI-3 send one word **/
byte bSpi3Read(byte addr);      /** SPI-3 read one byte **/

void vSpi3WriteFIFO(byte dat);    /** SPI-3 send one byte to FIFO **/
byte bSpi3ReadFIFO(void);     /** SPI-3 read one byte from FIFO **/
void vSpi3BurstWriteFIFO(byte ptr[], byte length);      /** SPI-3 burst send N byte to FIFO**/
void vSpi3BurstReadFIFO(byte ptr[], byte length);     /** SPI-3 burst read N byte to FIFO**/

void vSpi3WriteByte(byte dat);    /** SPI-3 send one byte **/
byte bSpi3ReadByte(void);     /** SPI-3 read one byte **/


int c;



void InputSDIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	    /* Check the parameters */
	GPIO_InitStruct.Pin = SDIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void OutputSDIO(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(SDIO_GPIO_Port, SDIO_Pin, GPIO_PIN_RESET);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	GPIO_InitStruct.Pin = SDIO_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void OutputSDCK(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	GPIO_InitStruct.Pin = SCLK_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void OutputFCSB(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(FCSB_GPIO_Port, FCSB_Pin, GPIO_PIN_RESET);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	GPIO_InitStruct.Pin = FCSB_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


}

void OutputCSB(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  	GPIO_InitStruct.Pin = CSB_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}






/**********************************************************
**Name:   vSpi3Init
**Func:   Init Spi-3 Config
**Note:
**********************************************************/
void vSpi3Init(void)
{
  OutputCSB();
  OutputFCSB();
  OutputSDCK();
  OutputSDIO();

  SetCSB();
  SetFCSB();
  SetSDIO();
  ClrSDCK();
}

/**********************************************************
**Name:   vSpi3WriteByte
**Func:   SPI-3 send one byte
**Input:
**Output:
**********************************************************/
void vSpi3WriteByte(byte dat)
{

  byte bitcnt;

  SetFCSB();        //FCSB = 1;
  OutputSDIO();     //SDA output mode
  SetSDIO();        //    output 1
  ClrSDCK();
  ClrCSB();
  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
	  HAL_UART_Transmit( &hlpuart1, "buradaa18\r\n",11, 100);
	  ClrSDCK();

	  if(dat&0x80){
		  SetSDIO();
		  HAL_UART_Transmit( &hlpuart1, "buradaa19\r\n",11, 100);}
	  else{
		  ClrSDIO();
		  HAL_UART_Transmit( &hlpuart1, "buradaa20\r\n",11, 100);
	  }
	  HAL_UART_Transmit( &hlpuart1, "buradaa21\r\n",11, 100);
	  SetSDCK();
	  HAL_UART_Transmit( &hlpuart1, "buradaa22\r\n",11, 100);
	  dat <<= 1;
	  HAL_UART_Transmit( &hlpuart1, "buradaa23\r\n",11, 100);
	  HAL_Delay_us(SPI3_SPEED);
	  HAL_UART_Transmit( &hlpuart1, "buradaa2\r\n",11, 100);

    }

  ClrSDCK();
  SetSDIO();
}

/**********************************************************
**Name:   bSpi3ReadByte
**Func:   SPI-3 read one byte
**Input:
**Output:
**********************************************************/
byte bSpi3ReadByte(void)
{
  byte RdPara = 0;
  byte bitcnt;

  ClrCSB();
  InputSDIO();

  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    ClrSDCK();
    RdPara <<= 1;
    HAL_Delay_us(SPI3_SPEED);
    SetSDCK();
    HAL_Delay_us(SPI3_SPEED);

    if(HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin) == 1){
      c=1;
    }else if(HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin) == 0){
      c=0;
    }
     //Serial.print("bSpi3ReadByte: ");
   // Serial.println(c);

    if(c==1)
      RdPara |= 0x01;
    else
      RdPara |= 0x00;
    }
  ClrSDCK();
  OutputSDIO();
  SetSDIO();
  SetCSB();
  return(RdPara);
}

/**********************************************************
**Name:   vSpi3Write
**Func:   SPI Write One word
**Input:  Write word
**Output: none
**********************************************************/
void vSpi3Write(word dat)
{
  //Serial.print("dat: ");   Serial.println(dat);
	HAL_UART_Transmit( &hlpuart1, "buradaa10\r\n",11, 100);
  vSpi3WriteByte((byte)(dat>>8)&0x7F);
  HAL_UART_Transmit( &hlpuart1, "buradaa11\r\n",11, 100);
  vSpi3WriteByte((byte)dat);
  HAL_UART_Transmit( &hlpuart1, "buradaa12\r\n",11, 100);
  SetCSB();
  HAL_UART_Transmit( &hlpuart1, "buradaa13\r\n",11, 100);
}

/**********************************************************
**Name:   bSpi3Read
**Func:   SPI-3 Read One byte
**Input:  readout addresss
**Output: readout byte
**********************************************************/
byte bSpi3Read(byte addr)
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
void vSpi3WriteFIFO(byte dat)
{
  byte bitcnt;

  SetCSB();
  OutputSDIO();
  ClrSDCK();
  ClrFCSB();      //FCSB = 0
  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    ClrSDCK();

    if(dat&0x80)
      SetSDIO();
    else
      ClrSDIO();
    HAL_Delay_us(SPI3_SPEED);
    SetSDCK();
    HAL_Delay_us(SPI3_SPEED);
    dat <<= 1;
    }
  ClrSDCK();
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  SetFCSB();
  SetSDIO();
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
}

/**********************************************************
**Name:   bSpi3ReadFIFO
**Func:   SPI-3 read one byte to FIFO
**Input:  none
**Output: one byte buffer
**********************************************************/
byte bSpi3ReadFIFO(void)
{
  byte RdPara;
  byte bitcnt;

  SetCSB();
  InputSDIO();
  ClrSDCK();
  ClrFCSB();

  for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
    ClrSDCK();
    RdPara <<= 1;
    HAL_Delay_us(SPI3_SPEED);
    SetSDCK();
    HAL_Delay_us(SPI3_SPEED);

    if(HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin) == 1){
      c=1;
    }else if(HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin) == 0){
      c=0;
    }


    if(c==1)
      RdPara |= 0x01;   //NRZ MSB
    else
      RdPara |= 0x00;   //NRZ MSB
    }

  ClrSDCK();
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  SetFCSB();
  OutputSDIO();
  SetSDIO();
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  HAL_Delay_us(SPI3_SPEED);    //Time-Critical
  return(RdPara);
}

/**********************************************************
**Name:   vSpi3BurstWriteFIFO
**Func:   burst wirte N byte to FIFO
**Input:  array length & head pointer
**Output: none
**********************************************************/
void vSpi3BurstWriteFIFO(byte ptr[], byte length)
{
  byte i;
  //printf("length: %x\r\n", length);
  //Serial.print(length);
  if(length!=0x00)
    {
    for(i=0;i<length;i++)
      vSpi3WriteFIFO(ptr[i]);

      //printf("i: %x\r\n", i);

      byte vSpi3WriteFIFO1;
  vSpi3WriteFIFO1 = bSpi3Read(i);
  //printf("vSpi3WriteFIFO: %x\r\n", vSpi3WriteFIFO1);
    }
  return;
}

/**********************************************************
**Name:   vSpiBurstRead
**Func:   burst wirte N byte to FIFO
**Input:  array length  & head pointer
**Output: none
**********************************************************/
void vSpi3BurstReadFIFO(byte ptr[], byte length)
{
  byte i;
  if(length!=0)
    {
    for(i=0;i<length;i++)
      ptr[i] = bSpi3ReadFIFO();
    }
  return;
}

#else
#warning "HopeDuino_SPI3.h have been defined!"
#endif
