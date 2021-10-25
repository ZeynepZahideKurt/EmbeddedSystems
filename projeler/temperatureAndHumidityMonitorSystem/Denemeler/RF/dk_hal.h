/*#include <pic.h>
#include <htc.h>
#include <stdio.h>
#include <pic16f1519.h>*/

/**********************************************************
//
**********************************************************/
#define RTC_INTV		0x8000		//RTC interval time    2s==0x0000
									//				       1s==0x8000
									//                  500ms==0xC000					
									//                  250ms==0xE000

#define	DIDA_8ms		0x06		//OPTION_REG 8ms��ʱ��λ
#define	DIDA_2ms		0x04		//OPTION_REG 2ms��ʱ��λ

#define	S1_KEY		0x01
#define	S2_KEY		0x02
#define	S3_KEY		0x04
#define	S4_KEY		0x08

#define	DOWN_KEY	0x01
#define	LEFT_KEY	0x02
#define	UP_KEY		0x04
#define	RIGHT_KEY	0x08	

/**********************************************************
�궨��
**********************************************************/
#define BitSet(var, bitno) ((var)|=(1<<(bitno)))	
#define BitClr(var, bitno) ((var)&= (~(1<<(bitno))))

typedef unsigned char byte;
typedef unsigned int  word;

/**********************************************************
�ṹ��
**********************************************************/
typedef union               //struct	
{
 struct
	{
	byte Bit0: 1;
	byte Bit1: 1;
	byte Bit2: 1;
	byte Bit3: 1;
	byte Bit4: 1;
	byte Bit5: 1;
	byte Bit6: 1;
	byte Bit7: 1;
	}BBits;
 byte BByte;
}FlagSTR;	



#define RTC_FCSB 34 //RTC 12 NUMARA FCSB
//#define RTC_CSB 35 //RTC 11 NUMARA CSB
#define RTC_SDA 15 //rtc 10 numara SDIO
#define RTC_SCL 6 //rtc 9 numara SCLK

/*#define		GPO4In		6  		//	1(0)	   0	    0	     0       -	//RF -GPO4 DATA	 
#define		GPO3In		15		//	1(0)     0		  0      0       -	//RF -GPO3 DCLK*/


#define		FCSB		34	//	0		 1      1      0       0 	//RF -FCSB


#define		SCK			6	//	0        0      0      0       -    //SPI-SCK
#define		SDI			15	//	0		 1      1      0	   -  	//SPI-SDI


//PortD							// TRISD   PORTD   LATD  ANSELD   WPD
#define		POROut		LATD0	//	1(0)	 0		0      0  	   - 	//RF -POR	
#define		TxLed4		LATD1	//	0        0      0      0 	   -  	//HAL-D4
#define		TxLed5		LATD2	//	0        0      0      0 	   -  	//HAL-D5
#define		TxLed6		LATD3	//	0        0      0      0 	   -  	//HAL-D6
#define		TxLed7		LATD4	//	0        0      0      0 	   -  	//HAL-D7
#define		TxLed8		LATD5	//	0        0      0      0 	   -  	//HAL-D8
#define		TxLed9		LATD6	//	0        0      0      0 	   -  	//HAL-D9
#define		RxLed		LATD7	//	0        0      0      0 	   -  	//HAL-D10




#define		SDI_IN()		(TRISC |= 0x10)
#define		SDI_OUT()		(TRISC &= 0xEF)

/**********************************************************
��������
**********************************************************/
extern byte RxData[32];
extern byte	RxLimtTime;

extern FlagSTR _SysTime;
	#define	SysTime		_SysTime.BByte
	#define	SysTime0	_SysTime.BBits.Bit0	
	#define	SysTime1	_SysTime.BBits.Bit1
	#define	SysTime2	_SysTime.BBits.Bit2
	#define	SysTime3	_SysTime.BBits.Bit3
	#define	SysTime4	_SysTime.BBits.Bit4
	#define	SysTime5	_SysTime.BBits.Bit5
	#define	SysTime6	_SysTime.BBits.Bit6
	#define	SysTime7	_SysTime.BBits.Bit7
	
extern FlagSTR _SecTime;
	#define	SecTime		_SecTime.BByte
	#define	SecTime0	_SecTime.BBits.Bit0
	#define	SecTime1	_SecTime.BBits.Bit1
	#define	SecTime2	_SecTime.BBits.Bit2
	#define	SecTime3	_SecTime.BBits.Bit3
	#define	SecTime4	_SecTime.BBits.Bit4
	#define	SecTime5	_SecTime.BBits.Bit5
	#define	SecTime6	_SecTime.BBits.Bit6
	#define	SecTime7	_SecTime.BBits.Bit7






/**********************************************************
��������
**********************************************************/
//main.c
extern void InitSfr(void);
extern void InitPort(void);
extern void TxLedOff(void);

//interrupt.c

//buzzer.c
extern void BeepOn(void);
extern void BeepOff(void);
extern void BeepInd(byte cnt);

//spi.c
extern void SPIWrite(word WrPara);
extern byte SPIRead(byte adr);
extern void SPICmd8bit(byte WrPara);
extern byte SPIRead8bit(void);
extern byte FIFORead(void);

//radio.c
extern byte RFM219_RxPacket(void);
extern void RFM219_Config(void);
extern void GPIO_FuncCfg(void);
extern void EntryStandby(void);
extern void EntrySleep(void);
extern void EntryRx(void);
extern void RFSoftReset(void);
extern void ClearIRQ(void);
extern void ClearFIFO(void);
extern void ReadStatus(void);
