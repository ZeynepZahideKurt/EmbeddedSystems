
/**********************************************************
3-wire spi
**********************************************************/



  #define	SetCSB()	HAL_GPIO_WritePin(csb_GPIO_Port, csb_Pin, 1)
  #define	ClrCSB()	HAL_GPIO_WritePin(csb_GPIO_Port, csb_Pin, 0)

  #define	SetFCSB()	HAL_GPIO_WritePin(fcsb_GPIO_Port, fcsb_Pin, 1)
  #define	ClrFCSB()	HAL_GPIO_WritePin(fcsb_GPIO_Port, fcsb_Pin, 0)

  #define	SetSDCK()	HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, 1)
  #define	ClrSDCK()	HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, 0)

  #define	SetSDIO()	HAL_GPIO_WritePin(SDIO_GPIO_Port, SDIO_Pin, 1)
  #define	ClrSDIO()	HAL_GPIO_WritePin(SDIO_GPIO_Port, SDIO_Pin, 0)

  //#define   InputSDIO()		//GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_IN_FL_NO_IT)
 // #define	OutputSDIO()	//GPIO_Init(GPIO_PORT_C,GPIO_PINS_SDIO,GPIO_MODE_OUT_PP_LOW_FAST)

  #define	SDIO_H()	HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin)
  #define	SDIO_L()	HAL_GPIO_ReadPin(SDIO_GPIO_Port, SDIO_Pin)

/**********************************************************
**Name: 	vSpi3Init
**Func: 	Init Spi-3 Config
**Note:
**********************************************************/
void spi3Class_vSpi3Init()
{
//	GPIO_Init(GPIO_PORT_C, (GPIO_Pin_TypeDef)GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);;	//SPI-3 for output
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
void vSpi3WriteByte(uint8_t dat)
{

	uint8_t bitcnt;

	SetFCSB();				//FCSB = 1;

 	//OutputSDIO();			//SDA output mode
 	//OutputSDIO();			//SDA output mode
 	SetSDIO();				//    output 1

 	ClrSDCK();
 	ClrCSB();

 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
		ClrSDCK();
		//HAL_Delay(SPI3_SPEED);
 		if(dat&0x80)
 			SetSDIO();
 		else
 			ClrSDIO();
		SetSDCK();
 		dat <<= 1;
 		//HAL_Delay(SPI3_SPEED);
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
uint8_t bSpi3ReadByte()
{
	uint8_t RdPara = 0;
	uint8_t bitcnt;

 	ClrCSB();
 	//InputSDIO();
  	//InputSDIO();
 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();
 		RdPara <<= 1;
 		//HAL_Delay(SPI3_SPEED);
 		SetSDCK();
 		//HAL_Delay(SPI3_SPEED);
 		if(SDIO_H())
 			RdPara |= 0x01;
 		else
 			RdPara |= 0x00;
 		}
 	ClrSDCK();
 	//OutputSDIO();
	//OutputSDIO();
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
void spi3Class_vSpi3Write(uint16_t dat)
{
 	vSpi3WriteByte((uint8_t)(dat>>8)&0x7F);
 	vSpi3WriteByte((uint8_t)dat);
 	SetCSB();
}

/**********************************************************
**Name:	 	bSpi3Read
**Func: 	SPI-3 Read One byte
**Input: 	readout addresss
**Output:	readout byte
**********************************************************/
uint8_t spi3Class_bSpi3Read(uint8_t addr)
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
void spi3Class_vSpi3WriteFIFO(uint8_t dat)
{
	uint8_t bitcnt;

 	SetCSB();
	//OutputSDIO();
	ClrSDCK();
 	ClrFCSB();			//FCSB = 0
	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();

 		if(dat&0x80)
			SetSDIO();
		else
			ClrSDIO();
		HAL_Delay(1);
		SetSDCK();
		HAL_Delay(1);
 		dat <<= 1;
 		}
 	ClrSDCK();
 	HAL_Delay(1);		//Time-Critical
 	HAL_Delay(1);		//Time-Critical
 	SetFCSB();
	SetSDIO();
 	HAL_Delay(1);		//Time-Critical
 	HAL_Delay(1);		//Time-Critical
}

/**********************************************************
**Name:	 	bSpi3ReadFIFO
**Func: 	SPI-3 read one byte to FIFO
**Input: 	none
**Output:	one byte buffer
**********************************************************/
uint8_t spi3Class_bSpi3ReadFIFO()
{
	uint8_t RdPara;
	uint8_t bitcnt;

 	SetCSB();
	//InputSDIO();
 	ClrSDCK();
	ClrFCSB();

 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		ClrSDCK();
 		RdPara <<= 1;
 		HAL_Delay(1);
		SetSDCK();
		HAL_Delay(1);
 		if(SDIO_H())
 			RdPara |= 0x01;		//NRZ MSB
 		else
 		 	RdPara |= 0x00;		//NRZ MSB
 		}

 	ClrSDCK();
 	HAL_Delay(1);		//Time-Critical
 	HAL_Delay(1);		//Time-Critical
 	SetFCSB();
	//OutputSDIO();
	SetSDIO();
 	HAL_Delay(1);		//Time-Critical

 	HAL_Delay(1);		//Time-Critical
 	return(RdPara);
}

/**********************************************************
**Name:	 	vSpi3BurstWriteFIFO
**Func: 	burst wirte N byte to FIFO
**Input: 	array length & head pointer
**Output:	none
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
**Name:	 	vSpiBurstRead
**Func: 	burst wirte N byte to FIFO
**Input: 	array length  & head pointer
**Output:	none
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







