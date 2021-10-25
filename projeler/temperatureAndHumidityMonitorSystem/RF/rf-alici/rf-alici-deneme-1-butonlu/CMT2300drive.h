#ifndef __CMT2300drive_H
  #define __CMT2300drive_H

/*
#define GPO1_H() P30

#define GPO3In()    //GPIO_Init(GPIO_PORT_C, (GPIO_Pin_TypeDef)GPIO1, GPIO_MODE_IN_FL_NO_IT)
#define GPO3_H()   P30//(GPIO_ReadInputPin(GPIO_PORT_C,GPIO1) ? 1 : 0)
#define GPO3_L()  (!P30)  //(GPIO_ReadInputPin(GPIO_PORT_C,GPIO1) ? 0 : 1 )*/
//
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

  #define CMT23_PKT1        0x38    //data mode & Preamble unit & Preamble rx size
    #define PREAM_UNIT_SET    0x04    //0:nibble;  1:unsigned char

  #define CMT23_TX_PREAM_L    0x39
  #define CMT23_TX_PREAM_H    0x3A
  #define CMT23_PREAM_VALUE   0x3B    //

  #define CMT23_SYNC_CTL      0x3C    //SyncTol[3] & SyncSize[3] & SyncDcFree
    #define SYNC_MAN_CODE   0x01

  #define CMT23_SYNC_ADDR     0x44    //0x3D~0x44 8unsigned chars SyncValue


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

  #define CMT23_NODE_ADDR     0x4B

  #define CMT23_CRC_CTL     0x4C
    #define CRC_ENABLE      0x01

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
    #define GPIO1_DOUT      (0<<0)      //0x00
    #define GPIO1_INT1      (1<<0)      //0x01
    #define GPIO1_INT2      (2<<0)      //0x02
    #define GPIO1_DCLK      (3<<0)      //0x03

    #define GPIO2_INT1      (0<<2)      //0x00
    #define GPIO2_INT2      (1<<2)      //0x04
    #define GPIO2_Dout      (2<<2)      //0x08
    #define GPIO2_DCLK      (3<<2)      //0x0C

    #define GPIO3_CLKO      (0<<4)      //0x00
    #define GPIO3_DOUT      (1<<4)      //0x10
    #define GPIO3_INT2      (2<<4)      //0x20
    #define GPIO3_DCLK      (3<<4)      //0x30

    #define GPIO4_NRST      (0<<6)      //0x00
    #define GPIO4_INT1      (1<<6)      //0x40
    #define GPIO4_DOUT      (2<<6)      //0x80
    #define GPIO4_DCLK      (3<<6)      //0xC0

  #define CMT23_INT1_CTL      0x66
    #define RF_SWT1_EN      (1<<7)      //GPIO1 »áÊä³ö RX_ACTIVE£¬GPIO2 »áÊä³ö TX_ACTIVE
    #define RF_SWT2_EN          (1<<6)      //GPIO1 »áÊä³ö RX_ACTIVE£¬ GPIO2 »áÊä³ö RX_ACTIVE È¡·´£¬¼´ÍêÈ«²î·Ö

    #define INT_POLAR     (1<<5)
    #define INT_MASK      0xE0

  #define CMT23_INT2_CTL      0x67
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
    #define INT_FIFO_Wunsigned_char_RX  0x0E
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
    #define TX_DIN_EN     (1<<7)


    #define RX_FIFO_CLR_DIS   (1<<4)    //0: Entry Rx, auto clear fifo
    #define TX_FIFO_RD_EN   (1<<3)    //0: Tx FIFO can not be read
    #define FIFO_RX_TX_SEL    (1<<2)    //when "FIFO_SHARE_EN=1" active, 0=use for Rx; 1=use for Tx
    #define FIFO_SHARE_EN   (1<<1)    //0: not share, both 32unsigned char for Tx/Rx FIFO; 1: total 64unsigned char
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


typedef struct
{
  unsigned char FixedPktLength;
  unsigned short int PayloadLength;
  unsigned char  PktRssi;
  unsigned char RssiTrig;
}cmt2300aEasy;

extern cmt2300aEasy radio;
/**********************************************************
**Name:     bGoStandby
**Function: Entry Standby Mode
**Input:    none
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bGoStandby(void)
{
 
 unsigned char tmp, i;

 radio.RssiTrig = false;
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_STBY);
 Serial.println("6");
 
 for(i=0; i<50; i++)
  {
  delay(400);
  tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));
  Serial.print("tmp");
  Serial.println(tmp);
  Serial.println("7");
  //Serial.println(MODE_STA_STBY);
 // tmp==MODE_STA_STBY;
  if(tmp==MODE_STA_STBY){
    Serial.println("8");
    break;
    }
  }
  //i=3;
 if(i>=50){
  Serial.println("9");
  return(false);
  }
 else{
  Serial.println("10");
  return(true);
  }
}
unsigned char cmt2300aEasy_bGoTx(void)
{
unsigned char tmp, i;

INIT_TX:
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_TX);
 for(i=0; i<50; i++)
  {

  delay(200);
  tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));
  if(tmp==MODE_STA_TX)
    break;
  }
 if(i>=50)
  {

    cmt2300aEasy_bGoStandby();
    goto INIT_TX;
  }
 else
  return(true);
}

/**********************************************************
**Name:     bGoRx
**Function: Entry Rx Mode
**Input:    none
**Output:   none
**********************************************************/

unsigned char cmt2300aEasy_vReadIngFlag1(void)
{
  return(spi3Class_bSpi3Read((unsigned char)(CMT23_INT_FLG>>8)));

}

unsigned char cmt2300aEasy_vReadIngFlag2(void)
{
  return(spi3Class_bSpi3Read((unsigned char)(CMT23_INT_CLR1 >>8)));

}


unsigned char cmt2300aEasy_bGoRx(void)
{
 unsigned char tmp, i;
 radio.RssiTrig = false;

 INIT_RX:
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_RX);
 for(i=0; i<50; i++)
  {
  delay(200);
  tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));
  if(tmp==MODE_STA_RX)
    break;
  }
 if(i>=50)
  {
    cmt2300aEasy_bGoStandby();
    goto INIT_RX;
  //return(true);
  }
 else
  return(true);
}

/**********************************************************
**Name:     bGoSleep
**Function: Entry Sleep Mode
**Input:    none
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bGoSleep(void)
{
 unsigned char tmp;

 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_CTL<<8)+MODE_GO_SLEEP);
 delay(100);    //enough?
 tmp = (MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));
 if(tmp==MODE_STA_SLEEP)
  return(true);
 else
  return(false);
}



/**********************************************************
**Name:     vSoftReset
**Function: Software reset Chipset
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aEasy_vSoftReset(void)
{
 spi3Class_vSpi3Write(((unsigned short int)CMT23_SOFTRST<<8)+0xFF);
 delay(1000);       //enough?
}

/**********************************************************
**Name:     bReadStatus
**Function: read chipset status
**Input:    none
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bReadStatus(void)
{
 return(MODE_MASK_STA & spi3Class_bSpi3Read(CMT23_MODE_STA));
}

/**********************************************************
**Name:     bReadRssi
**Function: Read Rssi
**Input:    true------dBm;
            false-----Code;
**Output:   none
**********************************************************/
unsigned char cmt2300aEasy_bReadRssi(unsigned char unit_dbm)
{
 if(unit_dbm)
  return(spi3Class_bSpi3Read(CMT23_RSSI_DBM));
 else
  return(spi3Class_bSpi3Read(CMT23_RSSI_CODE));
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
void cmt2300aEasy_vGpioFuncCfg(unsigned char io_cfg)
{
 spi3Class_vSpi3Write(((unsigned short int)CMT23_IO_SEL<<8)+io_cfg);
}

/**********************************************************
**Name:     vIntSrcCfg
**Function: config interrupt source
**Input:    int_1, int_2
**Output:   none
**********************************************************/
void cmt2300aEasy_vIntSrcCfg(unsigned char int_1, unsigned char int_2)
{
 unsigned char tmp;
 tmp = INT_MASK & spi3Class_bSpi3Read(CMT23_INT1_CTL);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT1_CTL<<8)+(tmp|int_1));

 tmp = INT_MASK & spi3Class_bSpi3Read(CMT23_INT2_CTL);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vEnableAntSwitch
**Function:
**Input:
**Output:   none
**********************************************************/
void cmt2300aEasy_vEnableAntSwitch(unsigned char mode)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_INT1_CTL);
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
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT1_CTL<<8)+tmp);
}


/**********************************************************
**Name:     vIntSrcEnable
**Function: enable interrupt source
**Input:    en_int
**Output:   none
**********************************************************/
void cmt2300aEasy_vEnablePLLcheck(void)
{
  unsigned char tmp;
  tmp = spi3Class_bSpi3Read(CMT23_EN_CTL);
  tmp |= LD_STOP_EN;
  spi3Class_vSpi3Write(((unsigned short int)CMT23_EN_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vEnablePLLcheck
**Function: enable PLLcheck
**Input:    en_int
**Output:   none
**********************************************************/
void cmt2300aEasy_vIntSrcEnable(unsigned char en_int)
{
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT_EN<<8)+en_int);
}


/**********************************************************
**Name:     vIntSrcFlagClr
**Function: clear flag
**Input:    none
**Output:   equ CMT23_INT_EN
**********************************************************/
unsigned char cmt2300aEasy_bIntSrcFlagClr(void)
{
 unsigned char tmp;
 unsigned char int_clr2 = 0;
 unsigned char int_clr1 = 0;
 unsigned char flg = 0;

 tmp = spi3Class_bSpi3Read(CMT23_INT_FLG);
 if(tmp&LBD_STATUS_FLAG)    //LBD_FLG_Active
  int_clr2 |= LBD_CLR;

 if(tmp&PREAMBLE_PASS_FLAG)   //Preamble Active
  {
  int_clr2 |= PREAMBLE_PASS_CLR;
  flg |= PREAMBLE_PASS_EN;
  }
 if(tmp&SYNC_PASS_FLAG)     //Sync Active
  {
  int_clr2 |= SYNC_PASS_CLR;
  flg |= SYNC_PASS_EN;
  }
 if(tmp&NODE_PASS_FLAG)     //Node Addr Active
  {
  int_clr2 |= NODE_PASS_CLR;
  flg |= NODE_PASS_EN;
  }
 if(tmp&CRC_PASS_FLAG)      //Crc Pass Active
  {
  int_clr2 |= CRC_PASS_CLR;
  flg |= CRC_PASS_EN;
  }
 if(tmp&RX_DONE_FLAG)     //Rx Done Active
  {
  int_clr2 |= RX_DONE_CLR;
  flg |= PKT_DONE_EN;
  }

 if(tmp&COLLISION_ERR_FLAG)   //ÕâÁ½¸ö¶¼±ØĞëÍ¨¹ıRX_DONEÇå³ı
  int_clr2 |= RX_DONE_CLR;
 if(tmp&DC_ERR_FLAG)
  int_clr2 |= RX_DONE_CLR;

 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT_CLR2<<8)+int_clr2);  //Clear flag


 tmp = spi3Class_bSpi3Read(CMT23_INT_CLR1);
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
 spi3Class_vSpi3Write(((unsigned short int)CMT23_INT_CLR1<<8)+int_clr1);  //Clear flag

 return(flg);
}

/**********************************************************
**Name:     bClearFIFO
**Function: clear FIFO buffer
**Input:    none
**Output:   FIFO state
**********************************************************/
unsigned char cmt2300aEasy_vClearFIFO(void)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_FIFO_FLG);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_FIFO_CLR<<8)+FIFO_CLR_RX+FIFO_CLR_TX);
 return(tmp);
}

void cmt2300aEasy_vEnableWrFifo(void)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_FIFO_CTL);
 tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_FIFO_CTL<<8)+tmp);
}

void cmt2300aEasy_vEnableRdFifo(void)
{
 unsigned char tmp;
 tmp = spi3Class_bSpi3Read(CMT23_FIFO_CTL);
 tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
 spi3Class_vSpi3Write(((unsigned short int)CMT23_FIFO_CTL<<8)+tmp);
}

/**********************************************************
CFG
**********************************************************/
/**********************************************************
**Name:     vInit
**Function: Init. CMT2300A
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aEasy_vInit(void)
{
 //unsigned char i;
 unsigned char tmp;
 unsigned char tmp1;
 //unsigned short int len;
 spi3Class_vSpi3Init();
 //GPO3In();

 cmt2300aEasy_vSoftReset();
 delay(20);
tmp1 = cmt2300aEasy_bGoStandby();
if(tmp1 == false)
{
  printf("tmp1=%d while(1)\n",(u16)tmp1);
  //while(1);
}
 //
 tmp = spi3Class_bSpi3Read(CMT23_MODE_STA);
 tmp|= EEP_CPY_DIS;
 tmp&= (~RSTN_IN_EN);     //Disable RstPin
 spi3Class_vSpi3Write(((unsigned short int)CMT23_MODE_STA<<8)+tmp);

 cmt2300aEasy_bIntSrcFlagClr();

}

void cmt2300aEasy_vCfgBank(unsigned short int cfg[], unsigned char length)
{
 unsigned char i;

 if(length!=0)
  {
  for(i=0; i<length; i++)
    spi3Class_vSpi3Write(cfg[i]);
  }
}


/******************************************************************************
**º¯ÊıÃû³Æ£ºbGetMessage
**º¯Êı¹¦ÄÜ£º½ÓÊÕÒ»°üÊı¾İ
**ÊäÈë²ÎÊı£ºÎŞ
**Êä³ö²ÎÊı£º·Ç0¡ª¡ª½ÓÊÕ³É¹¦
**          0¡ª¡ª½ÓÊÕÊ§°Ü
******************************************************************************/
unsigned char cmt2300aEasy_bGetMessage(unsigned char msg[])
{
 unsigned char i;

 cmt2300aEasy_vEnableRdFifo();
 if(radio.FixedPktLength)
  {
    spi3Class_vSpi3BurstReadFIFO(msg, radio.PayloadLength);
  i = radio.PayloadLength;
  }
 else
  {
  i = spi3Class_bSpi3ReadFIFO();
  spi3Class_vSpi3BurstReadFIFO(msg, i);
  }
 return(i);
}

unsigned char cmt2300aEasy_bGetMessageByFlag(unsigned char msg[])
{
 unsigned char tmp;
 unsigned char tmp1;
 unsigned char rev = 0;
 tmp = spi3Class_bSpi3Read(CMT23_INT_FLG);
 if((tmp&SYNC_PASS_FLAG)&&(!radio.RssiTrig))
  {
  radio.PktRssi = cmt2300aEasy_bReadRssi(false);
  radio.RssiTrig = true;
  }

 tmp1 = spi3Class_bSpi3Read(CMT23_CRC_CTL);
 cmt2300aEasy_vEnableRdFifo();
 if(tmp1&CRC_ENABLE)    //Enable CrcCheck
  {
  if(tmp&CRC_PASS_FLAG)
    {
    if(radio.FixedPktLength)
      {
        spi3Class_vSpi3BurstReadFIFO(msg, radio.PayloadLength);
      rev = radio.PayloadLength;
      }
    else
      {
      rev = spi3Class_bSpi3ReadFIFO();
      spi3Class_vSpi3BurstReadFIFO(msg, rev);
      }
    radio.RssiTrig = false;
    }
  }
 else
  {
  if(tmp&RX_DONE_FLAG)
    {
    if(radio.FixedPktLength)
      {
        spi3Class_vSpi3BurstReadFIFO(msg, radio.PayloadLength);
      rev = radio.PayloadLength;
      }
    else
      {
      rev = spi3Class_bSpi3ReadFIFO();
      spi3Class_vSpi3BurstReadFIFO(msg, rev);
      }
    radio.RssiTrig = false;
    }
  }

 if(tmp&COLLISION_ERR_FLAG)     //´íÎó´¦Àí
  rev = 0xFF;
 return(rev);
}

/******************************************************************************
**º¯ÊıÃû³Æ£ºbSendMessage
**º¯Êı¹¦ÄÜ£º·¢ÉäÒ»°üÊı¾İ
**ÊäÈë²ÎÊı£ºÎŞ
**Êä³ö²ÎÊı£º
**
******************************************************************************/

void cmt2300aEasy_vSetTxPayloadLength(unsigned short int length)
{
 unsigned char tmp;
 unsigned char len;
 cmt2300aEasy_bGoStandby();
 tmp = spi3Class_bSpi3Read(CMT23_PKT_CTRL1);
 tmp&= 0x8F;

 if(length!=0)
  {
  if(radio.FixedPktLength)
    len = length-1;
  else
    len = length;
  }
 else
  len = 0;

 tmp|= (((unsigned char)(len>>8)&0x07)<<4);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_PKT_CTRL1<<8)+tmp);
 spi3Class_vSpi3Write(((unsigned short int)CMT23_PKT_LEN<<8)+(unsigned char)len); //Payload length
 //bGoSleep();
}

unsigned char cmt2300aEasy_bSendMessage(unsigned char msg[], unsigned char length)
{
 //mode1
 /*cmt2300aEasy_vSetTxPayloadLength(length);
 cmt2300aEasy_bGoStandby();
 cmt2300aEasy_vEnableWrFifo();
 spi3Class_vSpi3BurstWriteFIFO(msg, length);
 cmt2300aEasy_bGoTx();*/

 //mode2
 cmt2300aEasy_bIntSrcFlagClr();  //ÇåÖĞ¶Ï
 cmt2300aEasy_vSetTxPayloadLength(length);
 cmt2300aEasy_bGoTx();
 cmt2300aEasy_vEnableWrFifo();
 spi3Class_vSpi3BurstWriteFIFO(msg, length);
 return(true);
}



#else
  #warning "HopeDuino_CMT2300A.h have been defined!"

#endif
