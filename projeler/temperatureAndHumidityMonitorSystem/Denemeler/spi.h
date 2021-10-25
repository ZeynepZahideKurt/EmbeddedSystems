  #ifndef __SPI_H
    #define __SPI_H
   //#include <stm32l0xx_hal.h> // #include <STC15F2K60S2.H>
 #include "main.h"
	void Delay_us(unsigned long i);
	void Delay_ms(unsigned long i);

  #define SPI3_SPEED 1


  #ifndef	byte
          typedef unsigned char byte;
  #endif

  #ifndef word
          typedef unsigned int  word;
  #endif





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

          typedef struct
          	{
          		void (*vSpi3Init)(void);				/** initialize software SPI-3 **/
          	 	void (*vSpi3Write)(word dat);			/** SPI-3 send one word **/
          		byte (*bSpi3Read)(byte addr);			/** SPI-3 read one byte **/

          		void (*vSpi3WriteFIFO)(byte dat);		/** SPI-3 send one byte to FIFO **/
          		byte (*bSpi3ReadFIFO)(void);			/** SPI-3 read one byte from FIFO **/
          		void (*vSpi3BurstWriteFIFO)(byte ptr[], byte length);			/** SPI-3 burst send N byte to FIFO**/
          		void (*vSpi3BurstReadFIFO)(byte ptr[], byte length);			/** SPI-3 burst read N byte to FIFO**/

          	    void (*vSpi3WriteByte)(byte dat);		/** SPI-3 send one byte **/
          	    byte (*bSpi3ReadByte)(void);			/** SPI-3 read one byte **/

          	}spi3Class;


            #else
                    #warning "spi.h have been defined!"

            #endif


