#ifndef RFM300_Driver_h
#define RFM300_Driver_h

#ifndef  byte
  typedef unsigned char byte;
#endif

#ifndef word
  typedef unsigned int  word;
#endif

#ifndef lword
  typedef unsigned long int lword;
#endif

#include "RFM300_Driver.h"
#include "HopeDuino_SPI3.h"
#include "main.h"
  UART_HandleTypeDef hlpuart1;
  typedef unsigned char u8;
  typedef unsigned int u16;
  typedef unsigned long u32;
  #define true 1
  #define false 0
/** Hardware brief **/
//#define GPO1_PIN  8
//#define GPO2_PIN  7
//#define GPO3_PIN  14

/*//#define TX_ANT      5
#define RX_ANT      4

#define GPO1In()  pinMode(GPO1_PIN,INPUT)
#define GPO1_H()  (digitalRead(GPO1_PIN)==1)
#define GPO1_L()  (digitalRead(GPO1_PIN)==0)

#define GPO2In()  pinMode(GPO2_PIN,INPUT)
#define GPO2_H()  (digitalRead(GPO2_PIN)==1)
#define GPO2_L()  (digitalRead(GPO2_PIN)==0)*/

//#define GPO3In()   pinMode(GPO3_PIN,INPUT)
#define GPO3_H()	HAL_GPIO_ReadPin(GPO3_GPIO_Port, GPO3_Pin) == 1
#define GPO3_L()  	HAL_GPIO_ReadPin(GPO3_GPIO_Port, GPO3_Pin) == 0
/*
#define TX_ANTOut()  pinMode(TX_ANT,OUTPUT)
#define SetTX_ANT()  digitalWrite(TX_ANT,1)
#define ClrTX_ANT()  digitalWrite(TX_ANT,0)

#define RX_ANTOut()  pinMode(RX_ANT,OUTPUT)
#define SetRX_ANT()  digitalWrite(RX_ANT,1)
#define ClrRX_ANT()  digitalWrite(RX_ANT,0)

#define SET_SW_AS_TX()  do{ClrTX_ANT();SetRX_ANT();}while(0)
#define SET_SW_AS_RX()  do{ClrRX_ANT();SetTX_ANT();}while(0)*/

//*********************************************************
//System bank
//*********************************************************
#define CMT23_LNA_CTL     0x0C

#define CMT23_DUTY_CTL      0x0D

  #define DUTY_MASK     0xE0

  #define TIMER_RX_EN     (1<<4)
  #define TIMER_SLEEP_EN    (1<<3)
  #define TX_DUTY_CYC_EN    (1<<2)
  #define RX_DUTY_CYC_EN    (1<<1)
  #define DUTY_CYC_PAUSE    (1<<0)

#define CMT23_EXIT_STATE    0x0E

  #define EXIT_MASK     0xF0

  #define EXIT_TO_SLEEP   0
  #define EXIT_TO_STBY    1
  #define EXIT_TO_FS      2

  #define TX_EXIT_TO_SLEEP  (0<<2)
  #define TX_EXIT_TO_STBY   (1<<2)
  #define TX_EXIT_TO_FS     (2<<2)

#define CMT23_SL_TIMER_CTL1   0x0F

#define CMT23_SL_TIMER_CTL2   0x10

#define CMT23_RX_TIMER_CTL1   0x11

#define CMT23_RX_TIMER_CTL2   0x12

#define CMT23_EXT_TIMER_CTL1  0x13

#define CMT23_EXT_TIMER_CTL2  0x14

#define CMT23_EXT_METHOD    0x15

#define CMT23_RSSI_CTL      0x16

#define CMT23_CLK_CTL     0x17

//*********************************************************
//Data rate bank
//*********************************************************
#define CMT23_CDR_CTL1      0x2B

  #define CDR_AVG_MASK    0x8F
  #define CDR_AVG_SHIFT   4

  #define CDR_MODE_MASK   0xFC
  #define CDR_MODE_SHIFT    0

  #define CDR_RANGE_MASK    0xF3
  #define CDR_RANGE_SHIFT   2

#define CMT23_CDR_CTL2      0x2C

  #define CDR_3RD_EN      (1<<6)
  #define CDR_4TH_EN      (1<<5)

//*********************************************************
//Baseband bank
//*********************************************************
#define CMT23_PKT1        0x38    //data mode & Preamble unit & Preamble rx size
  #define PREAM_UNIT_SET    0x04    //0:nibble;  1:byte

#define CMT23_TX_PREAM_L    0x39
#define CMT23_TX_PREAM_H    0x3A
#define CMT23_PREAM_VALUE   0x3B    //

#define CMT23_SYNC_CTL      0x3C    //SyncTol[3] & SyncSize[3] & SyncDcFree
  #define SYNC_MAN_CODE   0x01

#define CMT23_SYNC_ADDR_0   0x3D    //sync word base address
#define CMT23_SYNC_ADDR_7   0x44    //0x3D~0x44 8Bytes SyncValue

#define CMT23_PKT_CTRL1     0x45    //
  #define PKT_FORMAT      0x01
  #define PKT_LSB_PRIO    0x02    //Payload Bigend or Smallend
  #define NODE_POSITION   0x04    //1:after length;   0:before length
  #define AUTO_ACK      0x08

#define CMT23_PKT_LEN     0x46    //Low Payload length

#define CMT23_NODE_CTL      0x47
  #define NODE_DISABLE    0x00
  #define NODE_ONLY     0x01    //only
  #define NODE_AND0     0x02    //with broadcast 0
  #define NODE_ALL      0x03    //with broadcast 0 and 1

#define CMT23_NODE_ADDR_0   0x48    //base Node address
#define CMT23_NODE_ADDR_1   0x49    //second Node address
#define CMT23_NODE_ADDR_2   0x4A    //the third node address
#define CMT23_NODE_ADDR_3   0x4B    //the forth node address

#define CMT23_CRC_CTL     0x4C
  #define CRC_ENABLE      (1<<0)
  #define CRC_TYPE_CCITT    (0<<1)
  #define CRC_TYPE_IBM    (1<<1)
  #define CRC_TYPE_ITU    (2<<1)
  #define CRC_DATA_ONLY   (1<<3)
  #define CRC_BIT_INV     (1<<4)    //resutlt inverse
  #define CRC_SWAP      (1<<5)
  #define FEC_ENABLE      (1<<6)
  #define FEC_TYPE      (1<<7)


#define CMT23_CRC_SEEDL     0x4D
#define CMT23_CRC_SEEDH     0x4E

#define CMT23_PKT_CTRL2     0x4F
  #define MAN_ENABLE      (1<<0)
  #define MAN_TYPE      (1<<1)
  #define WHT_ENABLE      (1<<2)
  #define WHT_TYPE_CCITT    (0<<3)
  #define WHT_TYPE_IBM    (1<<3)
  #define WHT_TYPE_PN7    (2<<3)
  #define WHT_PN7_SEED    (1<<5)

#define CMT23_WHT_SEED      0x50

#define CMT23_PREFIX      0x51

#define CMT23_TX_PKT_NUM    0x52

#define CMT23_TX_PKT_GAP      0x53

#define CMT23_FIFO_TH     0x54
  #define FIFO_AUTO_RESTORE_EN (1<<7)

#define CMT23_MODE_CTL      0x60
  #define MODE_GO_EEPROM    (1<<0)      //0x01
  #define MODE_GO_STBY    (1<<1)      //0x02
  #define MODE_GO_RFS     (1<<2)      //0x04
  #define MODE_GO_RX      (1<<3)      //0x08
  #define MODE_GO_SLEEP   (1<<4)      //0x10
  #define MODE_GO_TFS     (1<<5)      //0x20
  #define MODE_GO_TX      (1<<6)      //0x40
  #define MODE_GO_SWITCH    (1<<7)      //0x80    //only use for "Tx to Rx" or "Rx to Tx"


#define CMT23_MODE_STA      0x61
  #define RSTN_IN_EN      (1<<5)      //0x20
  #define EEP_CPY_DIS     (1<<4)      //0x10    //import
  #define MODE_MASK_STA   0x0F
  #define MODE_STA_IDLE   0x00
  #define MODE_STA_SLEEP    0x01
  #define MODE_STA_STBY   0x02
  #define MODE_STA_RFS    0x03
  #define MODE_STA_TFS    0x04
  #define MODE_STA_RX     0x05
  #define MODE_STA_TX     0x06
  #define MODE_STA_EEPROM   0x07
  #define MODE_STA_TUNE   0x08


#define CMT23_EN_CTL      0x62
  #define LD_STOP_EN      (1<<5)      //0x20
  #define LBD_STOP_EN     (1<<4)      //0x10
  #define EEP_STOP      (1<<3)      //0x08
  #define EEP_START     (1<<2)      //0x04
  #define EEP_PWRON     (1<<1)      //0x02
  #define EEP_NPROTECT    (1<<0)      //0x01

#define CMT23_FREQ_CHNL     0x63      //Channel Number

#define CMT23_FREQ_OFS      0x64      //Channel OFFSET

#define CMT23_IO_SEL      0x65
  #define GPIO1_DIO     (0<<0)      //0x00
  #define GPIO1_INT1      (1<<0)      //0x01
  #define GPIO1_INT2      (2<<0)      //0x02
  #define GPIO1_DCLK      (3<<0)      //0x03

  #define GPIO2_INT1      (0<<2)      //0x00
  #define GPIO2_INT2      (1<<2)      //0x04
  #define GPIO2_DIO     (2<<2)      //0x08
  #define GPIO2_DCLK      (3<<2)      //0x0C

  #define GPIO3_CLKO      (0<<4)      //0x00
  #define GPIO3_DIO     (1<<4)      //0x10
  #define GPIO3_INT2      (2<<4)      //0x20
  #define GPIO3_DCLK      (3<<4)      //0x30

  #define GPIO4_NRST      (0<<6)      //0x00
  #define GPIO4_INT1      (1<<6)      //0x40
  #define GPIO4_DIO     (2<<6)      //0x80
  #define GPIO4_DCLK      (3<<6)      //0xC0

#define CMT23_INT1_CTL      0x66
  #define RF_SWT1_EN      (1<<7)      //GPIO1 浼氳緭鍑? RX_ACTIVE锛孏PIO2 浼氳緭鍑? TX_ACTIVE
  #define RF_SWT2_EN          (1<<6)      //GPIO1 浼氳緭鍑? RX_ACTIVE锛? GPIO2 浼氳緭鍑? RX_ACTIVE 鍙栧弽锛屽嵆瀹屽叏宸垎
  #define INT_POLAR     (1<<5)
  #define INT_MASK      0xE0

#define CMT23_INT2_CTL      0x67
  #define LOSC_OUT_EN     0x40
  #define TX_DATA_INV     0x20
  #define INT_RX_ACTIVE   0x00
  #define INT_TX_ACTIVE   0x01
  #define INT_RSSI_VALID    0x02
  #define INT_PREAM_PASS    0x03
  #define INT_SYNC_PASS   0x04
  #define INT_NODE_PASS   0x05
  #define INT_CRC_PASS    0x06
  #define INT_PKT_DONE    0x07
  #define INT_SLEEP_TMO   0x08
  #define INT_RX_TMO      0x09
  #define INT_TX_DONE     0x0A
  #define INT_FIFO_NMTY_RX  0x0B
  #define INT_FIFO_TH_RX    0x0C
  #define INT_FIFO_FULL_RX  0x0D
  #define INT_FIFO_WBYTE_RX 0x0E
  #define INT_FIFO_OVF_RX   0x0F
  #define INT_FIFO_NMTY_TX  0x10
  #define INT_FIFO_TH_TX    0x11
  #define INT_FIFO_FULL_TX  0x12
  #define INT_STBY_STATE    0x13
  #define INT_FS_STATE    0x14
  #define INT_RX_STATE    0x15
  #define INT_TX_STATE    0x16
  #define INT_LBD_STATE   0x17
  #define INT_TRX_ACTIVE    0x18    //?
  #define INT_PKT_ERR_COL   0x19

#define CMT23_INT_EN      0x68
  #define SLEEP_TMO_EN    (1<<7)    //0x80
  #define RX_TMO_EN     (1<<6)    //0x40
  #define TX_DONE_EN      (1<<5)    //0x20
  #define PREAMBLE_PASS_EN  (1<<4)    //0x10
  #define SYNC_PASS_EN    (1<<3)    //0x08
  #define NODE_PASS_EN    (1<<2)    //0x04
  #define CRC_PASS_EN     (1<<1)    //0x02
  #define PKT_DONE_EN     (1<<0)    //0x01

#define CMT23_FIFO_CTL      0x69
  #define DIN_EN        (1<<7)
  #define DIN_DIS       (0<<7)
  #define GPIO1_DIN     (0<<5)
  #define GPIO2_DIN     (1<<5)
  #define GPIO3_DIN     (2<<5)
  #define RX_FIFO_CLR_DIS   (1<<4)    //0: Entry Rx, auto clear fifo
  #define TX_FIFO_RD_EN   (1<<3)    //0: Tx FIFO can not be read
  #define FIFO_RX_TX_SEL    (1<<2)    //when "FIFO_SHARE_EN=1" active, 0=use for Rx; 1=use for Tx
  #define FIFO_SHARE_EN   (1<<1)    //0: not share, both 32Byte for Tx/Rx FIFO; 1: total 64Byte
  #define SPI_FIFO_RD_WR_SEL  (1<<0)    //0: SPI use to read FIFO;  1: SPI use to write FIFO;

#define CMT23_INT_CLR1      0x6A
  #define SLEEP_TIMEOUT_FLAG  (1<<5)    //0x20
  #define RX_TIMEOUT_FLAG   (1<<4)    //0x10
  #define TX_DONE_FLAG    (1<<3)    //0x08
  #define TX_DONE_CLR     (1<<2)    //0x04
  #define SLEEP_TIMEOUT_CLR (1<<1)    //0x02
  #define RX_TIMEOUT_CLR    (1<<0)    //0x01

#define CMT23_INT_CLR2      0x6B
  #define LBD_CLR       (1<<5)    //0x20
  #define PREAMBLE_PASS_CLR (1<<4)    //0x10
  #define SYNC_PASS_CLR   (1<<3)    //0x08
  #define NODE_PASS_CLR   (1<<2)    //0x04
  #define CRC_PASS_CLR    (1<<1)    //0x02
  #define RX_DONE_CLR     (1<<0)    //0x01

#define CMT23_FIFO_CLR      0x6C
  #define FIFO_RESTORE    (1<<2)    //0x04
  #define FIFO_CLR_RX     (1<<1)    //0x02
  #define FIFO_CLR_TX     (1<<0)    //0x01

#define CMT23_INT_FLG     0x6D
  #define LBD_STATUS_FLAG   (1<<7)    //0x80
  #define COLLISION_ERR_FLAG  (1<<6)    //0x40
  #define DC_ERR_FLAG     (1<<5)    //0x20
  #define PREAMBLE_PASS_FLAG  (1<<4)    //0x10
  #define SYNC_PASS_FLAG    (1<<3)    //0x08
  #define NODE_PASS_FLAG    (1<<2)    //0x04
  #define CRC_PASS_FLAG   (1<<1)    //0x02
  #define RX_DONE_FLAG    (1<<0)    //0x01

#define CMT23_FIFO_FLG      0x6E
  #define RX_FIFO_FULL_FLAG (1<<6)    //0x40
  #define RX_FIFO_NMTY_FLAG (1<<5)    //0x20
  #define RX_FIFO_THRESH_FLAG (1<<4)    //0x10
  #define RX_FIFO_OV_FLAG   (1<<3)    //0x08
  #define TX_FIFO_FULL_FLAG (1<<2)    //0x04
  #define TX_FIFO_NMTY_FLAG (1<<1)    //0x02
  #define TX_FIFO_THRESH_FLAG (1<<0)    //0x01

#define CMT23_RSSI_CODE     0x6F
#define CMT23_RSSI_DBM      0x70
#define CMT23_LBD_RESULT    0x71

#define CMT23_EE_DAT_LOW    0x76
#define CMT23_EE_DAT_HIG    0x77
#define CMT23_EE_ADD_USER   0x78
#define CMT23_EE_CTL      0x79
#define CMT23_EE_ADD_CONF   0x7A
#define CMT23_EE_EXT_CTL    0x7B
#define CMT23_SOFTRST     0x7F


extern  byte  FixedPktLength;           //false: for contain packet length in Tx message, the same mean with variable lenth
                                                //true : for doesn't include packet length in Tx message, the same mean with fixed length
extern  word  PayloadLength;            //unit: byte  range: 1-2047

extern  byte  PktRssi;

extern  byte  RssiTrig;

extern byte  node_id_size;      //0: none,disable,  max:4
extern word  node_id;        //16bit,


//State Ctrl
byte bGoTx(void);
byte bGoRx(void);
byte bGoSleep(void);
byte bGoStandby(void);
void vSoftReset(void);
byte bReadStatus(void);
byte bReadRssi(byte unit_dbm);

//GPIO & Interrupt CFG
void vGpioFuncCfg(byte io_cfg);
void vIntSrcCfg(byte int_1, byte int_2);
void vEnableAntSwitch(byte mode);         //
void vIntSrcEnable(byte en_int);
byte bIntSrcFlagClr(void);
byte vClearFIFO(void);
void vEnableRdFifo(void);
void vEnableWrFifo(void);
void vSetTxPayloadLength(word length);
void vEnableDIN(byte pin);      //add by vison
void vDisableDIN(void);

//CFG
void vInit(void);
void vCfgBank(word cfg[], byte length);

byte bGetMessage(byte msg[]);
byte bGetMessageByFlag(byte msg[]);
byte bSendMessage(byte msg[], byte length);

//control
void vSetClockDiv(byte div, byte enable);
void set_node_id(word node);
word get_node_id(void);
byte get_node_setting(void);



byte  FixedPktLength;            //false: for contain packet length in Tx message, the same mean with variable lenth
                      //true : for doesn't include packet length in Tx message, the same mean with fixed length
word  PayloadLength;            //unit: byte  range: 1-2047
byte  PktRssi;
byte  RssiTrig;
byte  node_id_size=0;     //0: none,disable,  max:4
word node_id=0;        //16bit, for testing

/**********************************************************
State Ctrl
**********************************************************/
/**********************************************************
**Name:     bGoTx
**Function: Entry Tx Mode
**Input:    none
**Output:   none
**********************************************************/
byte bGoTx(void)
{
  byte tmp, i;

  #if 0
  vSpi3Write(((word)CMT23_MODE_CTL<<8)+MODE_GO_TFS);
  for(i=0; i<100; i++){
    HAL_Delay_us(100);
    tmp = (MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));
    if(tmp==MODE_STA_TFS)
      break;
  }
  if(i>=100)
    return(false);
  #endif

  vSpi3Write(((word)CMT23_MODE_CTL<<8)+MODE_GO_TX);
  for(i=0; i<100; i++){
    HAL_Delay_us(100);
    tmp = (MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));
    if(tmp==MODE_STA_TX)
      break;
  }
  if(i>=100)
    return(false);
  else
    return(true);
}

/**********************************************************
**Name:     bGoRx
**Function: Entry Rx Mode
**Input:    none
**Output:   none
**********************************************************/
byte bGoRx(void)
{
  byte tmp, i;
  #if 0
  RssiTrig = false;
  vSpi3Write(((word)CMT23_MODE_CTL<<8)+MODE_GO_RFS);
  for(i=0; i<100; i++){
    HAL_Delay_us(100);
    tmp = (MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));
    if(tmp==MODE_STA_RFS)
      break;
  }
  if(i>=100)
    return(false);
  #endif

  vSpi3Write(((word)CMT23_MODE_CTL<<8)+MODE_GO_RX);
  for(i=0; i<100; i++){
    HAL_Delay_us(100);
    tmp = (MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));
    if(tmp==MODE_STA_RX)
      break;
  }
  if(i>=100)
    return(false);
  else
    return(true);
}

/**********************************************************
**Name:     bGoSleep
**Function: Entry Sleep Mode
**Input:    none
**Output:   none
**********************************************************/
byte bGoSleep(void)
{
  byte tmp;

  vSpi3Write(((word)CMT23_MODE_CTL<<8)+MODE_GO_SLEEP);
  HAL_Delay_us(100);   //enough?
  tmp = (MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));
  if(tmp==MODE_GO_SLEEP)
    return(true);
  else
    return(false);
}

/**********************************************************
**Name:     bGoStandby
**Function: Entry Standby Mode
**Input:    none
**Output:   none
**********************************************************/
byte bGoStandby(void)
{
  byte tmp, i;

  RssiTrig = false;
  vSpi3Write(((word)CMT23_MODE_CTL<<8)+MODE_GO_STBY);
  for(i=0; i<100; i++){
    HAL_Delay_us(100);
    tmp = (MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));

    if(tmp==MODE_STA_STBY)
      break;
  }
  if(i>=100)
    return(false);
  else
    return(true);
}

/**********************************************************
**Name:     vSoftReset
**Function: Software reset Chipset
**Input:    none
**Output:   none
**********************************************************/
void vSoftReset(void)
{
	HAL_UART_Transmit( &hlpuart1, "buradaa7\r\n",10, 100);
  vSpi3Write(((word)CMT23_SOFTRST<<8)+0xFF);
  HAL_UART_Transmit( &hlpuart1, "buradaa8\r\n",10, 100);
  HAL_Delay(1);       //enough?
  HAL_UART_Transmit( &hlpuart1, "buradaa9\r\n",10, 100);
}

/**********************************************************
**Name:     bReadStatus
**Function: read chipset status
**Input:    none
**Output:   none
**********************************************************/
byte bReadStatus(void)
{
  return(MODE_MASK_STA & bSpi3Read(CMT23_MODE_STA));
}

/**********************************************************
**Name:     bReadRssi
**Function: Read Rssi
**Input:    true------dBm;
            false-----Code;
**Output:   none
**********************************************************/
byte bReadRssi(byte unit_dbm)
{
  if(unit_dbm)
    return(bSpi3Read(CMT23_RSSI_DBM));
  else
    return(bSpi3Read(CMT23_RSSI_CODE));
}

void set_node_id(word node)
{
  vSpi3Write((word)((CMT23_NODE_ADDR_3<<8)| (node>>8))&0x00FF);
  vSpi3Write((word)(CMT23_NODE_ADDR_2<<8)| (node&0x00FF));

  //vSpi3Write((word)(CMT23_NODE_ADDR_3<<8)|node&0x00FF);
}

word get_node_id(void)
{
  word node;
  node = bSpi3Read(CMT23_NODE_ADDR_3);     //msb first
  node <<= 8;
  node |= bSpi3Read(CMT23_NODE_ADDR_2);    //second byte
  return node;
}

byte get_node_setting(void)
{
   return bSpi3Read(0x47);  //node condigure register
}

/**********************************************************
GPIO & Interrupt CFG
**********************************************************/
/**********************************************************
**Name:     vGpioFuncCfg
**Function: GPIO Function config
**Input:    none
**Output:   none
**********************************************************/
void vGpioFuncCfg(byte io_cfg)
{
  vSpi3Write(((word)CMT23_IO_SEL<<8)+io_cfg);
}

/**********************************************************
**Name:     vIntSrcCfg
**Function: config interrupt source
**Input:    int_1, int_2
**Output:   none
**********************************************************/
void vIntSrcCfg(byte int_1, byte int_2)
{
  byte tmp;
  tmp = INT_MASK & bSpi3Read(CMT23_INT1_CTL);
  vSpi3Write(((word)CMT23_INT1_CTL<<8)+(tmp|int_1));

  tmp = INT_MASK & bSpi3Read(CMT23_INT2_CTL);
  vSpi3Write(((word)CMT23_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vEnableDIN
**Function: for direct mode, add by vison
**Input:    none
**Output:
**********************************************************/
void vEnableDIN(byte pin)
{
  byte tmp;
  tmp = 0x1F & bSpi3Read(CMT23_FIFO_CTL);
  vSpi3Write(((word)CMT23_FIFO_CTL<<8)+(tmp|DIN_EN|pin));
}

/**********************************************************
**Name:     vEnableDIN
**Function: for direct mode, add by vison
**Input:    none
**Output:
**********************************************************/
void vDisableDIN(void)
{
  byte tmp;
  tmp = 0x1F & bSpi3Read(CMT23_FIFO_CTL);
  vSpi3Write(((word)CMT23_FIFO_CTL<<8)+(tmp|DIN_DIS));
}
/**********************************************************
**Name:     vEnableAntSwitch
**Function:
**Input:
**Output:   none
**********************************************************/
void vEnableAntSwitch(byte mode)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_INT1_CTL);
  tmp&= 0x3F;
  switch(mode)
  {
    case 1:
      tmp |= RF_SWT1_EN; break;   //GPO1=RxActive; GPO2=TxActive
    case 2:
      tmp |= RF_SWT2_EN; break;   //GPO1=RxActive; GPO2=!RxActive
    case 0:
    default:
      break;              //Disable
  }
  vSpi3Write(((word)CMT23_INT1_CTL<<8)+tmp);
}


/**********************************************************
**Name:     vIntSrcEnable
**Function: enable interrupt source
**Input:    en_int
**Output:   none
**********************************************************/
void vIntSrcEnable(byte en_int)
{
  vSpi3Write(((word)CMT23_INT_EN<<8)+en_int);
}

/********* ***************/
void vSetClockDiv(byte div, byte enable)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_CLK_CTL);
 // Serial.println(tmp,HEX);  //print old value
  vSpi3Write(((word)CMT23_CLK_CTL<<8)+((tmp&0xC0)|(enable<<5)|div));
  //Serial.println(((word)CMT23_CLK_CTL<<8)+(tmp&0xC0|enable<<5|div),HEX);
}


/**********************************************************
**Name:     vIntSrcFlagClr
**Function: clear flag
**Input:    none
**Output:   equ CMT23_INT_EN
**********************************************************/
byte bIntSrcFlagClr(void)
{
  //byte tmp;
  //byte int_clr2 = 0;
  //byte int_clr1 = 0;
  byte flg = 0;
#if 0
  tmp = bSpi3Read(CMT23_INT_FLG);
  if(tmp&LBD_STATUS_FLAG)   //LBD_FLG_Active
    int_clr2 |= LBD_CLR;

  if(tmp&PREAMBLE_PASS_FLAG)    //Preamble Active
  {
    int_clr2 |= PREAMBLE_PASS_CLR;
    flg |= PREAMBLE_PASS_EN;
  }
  if(tmp&SYNC_PASS_FLAG)      //Sync Active
  {
    int_clr2 |= SYNC_PASS_CLR;
    flg |= SYNC_PASS_EN;
  }
  if(tmp&NODE_PASS_FLAG)      //Node Addr Active
  {
    int_clr2 |= NODE_PASS_CLR;
    flg |= NODE_PASS_EN;
  }
  if(tmp&CRC_PASS_FLAG)     //Crc Pass Active
  {
    int_clr2 |= CRC_PASS_CLR;
    flg |= CRC_PASS_EN;
  }
  if(tmp&RX_DONE_FLAG)      //Rx Done Active
  {
    int_clr2 |= RX_DONE_CLR;
    flg |= PKT_DONE_EN;
  }

  if(tmp&COLLISION_ERR_FLAG)    //杩欎袱涓兘蹇呴』閫氳繃RX_DONE娓呴�?
    int_clr2 |= RX_DONE_CLR;
  if(tmp&DC_ERR_FLAG)
    int_clr2 |= RX_DONE_CLR;

  vSpi3Write(((word)CMT23_INT_CLR2<<8)+int_clr2); //Clear flag

  tmp = bSpi3Read(CMT23_INT_CLR1);
  if(tmp&TX_DONE_FLAG)
  {
    int_clr1 |= TX_DONE_CLR;
    flg |= TX_DONE_EN;
  }
  if(tmp&SLEEP_TIMEOUT_FLAG)
  {
    int_clr1 |= SLEEP_TIMEOUT_CLR;
    flg |= SLEEP_TMO_EN;
  }
  if(tmp&RX_TIMEOUT_FLAG)
  {
    int_clr1 |= RX_TIMEOUT_CLR;
    flg |= RX_TMO_EN;
  }
  vSpi3Write(((word)CMT23_INT_CLR1<<8)+int_clr1); //Clear flag
#endif
  vSpi3Write(((word)CMT23_INT_CLR1<<8)+0x07); //Clear flag
  vSpi3Write(((word)CMT23_INT_CLR2<<8)+0xFF); //Clear flag

  return(flg);
}

/**********************************************************
**Name:     bClearFIFO
**Function: clear FIFO buffer
**Input:    none
**Output:   FIFO state
**********************************************************/
byte vClearFIFO(void)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_FIFO_FLG);
  vSpi3Write(((word)CMT23_FIFO_CLR<<8)+FIFO_CLR_RX+FIFO_CLR_TX);
  return(tmp);
}

#if 1   //splite fifo, tx:32bytes, rx:32bytes;
void vEnableWrFifo(void)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_FIFO_CTL);
  //Serial.print("vEnableWrFifo tmp: "); Serial.println(tmp);

  tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);
 // printf("tmp: %x\r\n", tmp);

  vSpi3Write(((word)CMT23_FIFO_CTL<<8)+tmp);
  //printf("CMT23_FIFO_CTL: %x\r\n", (CMT23_FIFO_CTL<<8));

  /*byte CMT23_FIFO_CTL1;
  CMT23_FIFO_CTL1 = bSpi3Read(CMT23_FIFO_CTL);
  printf("CMT23_FIFO_CTL'de yazan: %x\r\n", CMT23_FIFO_CTL1);*/

}

void vEnableRdFifo(void)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_FIFO_CTL);
  tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
  vSpi3Write(((word)CMT23_FIFO_CTL<<8)+tmp);
}
#endif

#if 0   //modify by vison
void vEnableWrFifo(void)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_FIFO_CTL);

  tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);

  vSpi3Write(((word)CMT23_FIFO_CTL<<8)+tmp|FIFO_SHARE_EN);  //enbale merge fifo
}

void vEnableRdFifo(void)
{
  byte tmp;
  tmp = bSpi3Read(CMT23_FIFO_CTL);
  tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
  vSpi3Write(((word)CMT23_FIFO_CTL<<8)+tmp|FIFO_SHARE_EN);  //enable merge fifo
}
#endif

/**********************************************************
CFG
**********************************************************/
/**********************************************************
**Name:     vInit
**Function: Init. CMT2300A
**Input:    none
**Output:   none
**********************************************************/
void vInit(void)
{
  byte tmp;
  HAL_UART_Transmit( &hlpuart1, "buradaa3\r\n",10, 100);

  vSpi3Init();
  HAL_UART_Transmit( &hlpuart1, "buradaa4\r\n",10, 100);
//  GPO1In();
  //GPO2In();
  //GPO3In();

//  TX_ANTOut();
  //RX_ANTOut();

  vSoftReset();
  HAL_UART_Transmit( &hlpuart1, "buradaa5\r\n",10, 100);
  HAL_Delay(20);
  bGoStandby();
  HAL_UART_Transmit( &hlpuart1, "buradaa6\r\n",10, 100);

  //
  tmp = bSpi3Read(CMT23_MODE_STA);
  tmp|= EEP_CPY_DIS;
  tmp&= (~RSTN_IN_EN);      //Disable RstPin
  vSpi3Write(((word)CMT23_MODE_STA<<8)+tmp);

  bIntSrcFlagClr();
}

void vCfgBank(word cfg[], byte length)
{
  byte i;

  if(length!=0){
    for(i=0; i<length; i++)
      vSpi3Write(cfg[i]);
  }
}

byte bGetMessage(byte msg[])
{
  byte i;

  vEnableRdFifo();
  if(FixedPktLength){
    vSpi3BurstReadFIFO(msg, PayloadLength);
    i = PayloadLength;
  }else{
    i = bSpi3ReadFIFO();
    vSpi3BurstReadFIFO(msg, i);
  }
  return(i);
}

byte bGetMessageByFlag(byte msg[])
{
  byte tmp;
  byte tmp1;
  byte rev = 0;
  tmp = bSpi3Read(CMT23_INT_FLG);
  if((tmp&SYNC_PASS_FLAG)&&(!RssiTrig)){
    PktRssi = bReadRssi(false);
    RssiTrig = true;
  }

  tmp1 = bSpi3Read(CMT23_CRC_CTL);
  vEnableRdFifo();
  if(tmp1&CRC_ENABLE)   //Enable CrcCheck
  {
    if(tmp&CRC_PASS_FLAG){
      if(FixedPktLength){
        vSpi3BurstReadFIFO(msg, PayloadLength);
        rev = PayloadLength;
      }else{
        rev = bSpi3ReadFIFO();
        vSpi3BurstReadFIFO(msg, rev);
      }
      RssiTrig = false;
    }
  }else{
    if(tmp&RX_DONE_FLAG){
      if(FixedPktLength){
        vSpi3BurstReadFIFO(msg, PayloadLength);
        rev = PayloadLength;
      }else{
        rev = bSpi3ReadFIFO();
        vSpi3BurstReadFIFO(msg, rev);
      }
      RssiTrig = false;
    }
  }

  if(tmp&COLLISION_ERR_FLAG)      //閿欒澶勭悊
  rev = 0xFF;
  return(rev);
}

byte bSendMessage(byte msg[], byte length)
{
  bIntSrcFlagClr();
#if 1
  //mode1
  vSetTxPayloadLength(length);

  bGoStandby();

  vEnableWrFifo();
  vSpi3BurstWriteFIFO(msg, length);
  bGoTx();
#else
  //mode2
  vSetTxPayloadLength(length);
  bGoTx();
  vEnableWrFifo();
  vSpi3BurstWriteFIFO(msg, length);
#endif
  return(true);
}

void vSetTxPayloadLength(word length)
{
  byte tmp;
  byte len;
  bGoStandby();
  tmp = bSpi3Read(CMT23_PKT_CTRL1);
  tmp&= 0x8F;

  if(length!=0){
    if(FixedPktLength){
      tmp &= ~(1<<0);   //clear bit0
      len = length - 1;
    }
    else{
      tmp |= (1<<0);     //set bit0
      len = length;
    }
  }else
    len = 0;

  tmp|= (((byte)(len>>8)&0x07)<<4);
  vSpi3Write(((word)CMT23_PKT_CTRL1<<8)+tmp);
  vSpi3Write(((word)CMT23_PKT_LEN<<8)+(byte)len); //Payload length
  //bGoSleep();
}



#else
  #warning "HopeDuino_CMT2300A.h have been defined!"

#endif

