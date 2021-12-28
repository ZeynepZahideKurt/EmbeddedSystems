#include "HopeDuino_SPI3.h"
int c;
/**********************************************************
**Name: 	vSpi3Init
**Func: 	Init Spi-3 Config
**Note:
**********************************************************/
void InputSDIO(void)
{


	GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin : GPO3_Pin */
	  GPIO_InitStruct.Pin = SDIO_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(SDIO_GPIO_Port, &GPIO_InitStruct);
}
void OutputSDIO(){
	 GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(SDIO_GPIO_Port, SDIO_Pin, GPIO_PIN_RESET);

	  /*Configure GPIO pins : CSB_Pin FCSB_Pin SCLK_Pin SDIO_Pin */
	  GPIO_InitStruct.Pin = SDIO_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


}

void OutputSDCK(void){

	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  	  /* GPIO Ports Clock Enable */
	  	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  	  /*Configure GPIO pin Output Level */
	  	  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);

	  	  /*Configure GPIO pins : CSB_Pin FCSB_Pin SCLK_Pin SDIO_Pin */
	  	  GPIO_InitStruct.Pin = SCLK_Pin;
	  	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void OutputFCSB(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

		  	  /* GPIO Ports Clock Enable */
		  	  __HAL_RCC_GPIOH_CLK_ENABLE();
		  	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(FCSB_GPIO_Port, FCSB_Pin, GPIO_PIN_RESET);
  	GPIO_InitStruct.Pin = FCSB_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


}

void OutputCSB(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);
  	GPIO_InitStruct.Pin = CSB_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void _delay_us(void){
	// for (int i = 0; i <= 1; i++){
	//HAL_UART_Transmit( &hlpuart1, (uint8_t *)"geliyor\r\n",9, 100);
	//int i=i;
 	// HAL_UART_Transmit( &hlpuart1, (uint8_t *)"giriyor\r\n",9, 100);

	//
	  	//  }
	 //HAL_Delay(1);
}
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
**Name: 	vSpi3WriteByte
**Func: 	SPI-3 send one byte
**Input:
**Output:  
**********************************************************/
void vSpi3WriteByte(byte dat)
{

 	byte bitcnt;
 
	SetFCSB();				//FCSB = 1;
 
 	OutputSDIO();			//SDA output mode
 	SetSDIO();				//    output 1
 
 	ClrSDCK();
 	ClrCSB();

 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
		ClrSDCK();
		_delay_us(); //_delay_us(1);
 		if(dat&0x80)
 			SetSDIO();
 		else
 			ClrSDIO();
		SetSDCK();
 		dat <<= 1;
 		_delay_us(); //_delay_us(1);
 		}
 	ClrSDCK();
 	SetSDIO();
}

/**********************************************************
**Name: 	bSpi3ReadByte
**Func: 	SPI-3 read one byte
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
 		_delay_us(); //_delay_us(1);
 		SetSDCK();
 		_delay_us(); //_delay_us(1);

 		if(SDIO_H())
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
**Name:	 	vSpi3Write
**Func: 	SPI Write One word
**Input: 	Write word
**Output:	none
**********************************************************/
void vSpi3Write(word dat)
{
 	vSpi3WriteByte((byte)(dat>>8)&0x7F);
 	vSpi3WriteByte((byte)dat);
 	SetCSB();
}

/**********************************************************
**Name:	 	bSpi3Read
**Func: 	SPI-3 Read One byte
**Input: 	readout addresss
**Output:	readout byte
**********************************************************/
byte bSpi3Read(byte addr)
{
  	vSpi3WriteByte(addr|0x80);
 	return(bSpi3ReadByte());
}

/**********************************************************
**Name:	 	vSpi3WriteFIFO
**Func: 	SPI-3 send one byte to FIFO
**Input: 	one byte buffer
**Output:	none
**********************************************************/
void vSpi3WriteFIFO(byte dat)
{
 	byte bitcnt;
 
 	SetCSB();
	OutputSDIO();
	ClrSDCK();
 	ClrFCSB();			//FCSB = 0
	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();

 		if(dat&0x80)
			SetSDIO();
		else
			ClrSDIO();
		_delay_us(); //_delay_us(1);
		SetSDCK();
		_delay_us(); //_delay_us(1);
 		dat <<= 1;
 		}
 	ClrSDCK();
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	SetFCSB();
	SetSDIO();
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	_delay_us(); //_delay_us(1);		//Time-Critical
}

/**********************************************************
**Name:	 	bSpi3ReadFIFO
**Func: 	SPI-3 read one byte to FIFO
**Input: 	none
**Output:	one byte buffer
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
 		_delay_us(); //_delay_us(1);
		SetSDCK();
		_delay_us(); //_delay_us(1);
 		if(SDIO_H())
 			RdPara |= 0x01;		//NRZ MSB
 		else
 		 	RdPara |= 0x00;		//NRZ MSB
 		}

 	ClrSDCK();
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	SetFCSB();
	OutputSDIO();
	SetSDIO();
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	_delay_us(); //_delay_us(1);		//Time-Critical
 	return(RdPara);
}

/**********************************************************
**Name:	 	vSpi3BurstWriteFIFO
**Func: 	burst wirte N byte to FIFO
**Input: 	array length & head pointer
**Output:	none
**********************************************************/
void vSpi3BurstWriteFIFO(byte ptr[], byte length)
{
 	byte i;
 	if(length!=0x00)
	 	{
 		for(i=0;i<length;i++)
 			vSpi3WriteFIFO(ptr[i]);
 		}
 	return;
}

/**********************************************************
**Name:	 	vSpiBurstRead
**Func: 	burst wirte N byte to FIFO
**Input: 	array length  & head pointer
**Output:	none
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
