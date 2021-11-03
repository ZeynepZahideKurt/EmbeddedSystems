#ifndef __TXCONFIG_H
#define __TXCONFIG_H
/*
;---------------------------------------
;  CMT2119B Configuration File
;  Generated by CMOSTEK RFPDK 1.48
;  2020.03.11 12:30
;---------------------------------------
; Mode                      = Advanced
; Part Number               = CMT2119B
; Frequency                 = 433.920 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = FSK
; AGC                       = On
; Data Rate                 = 2.4 kbps
; Deviation                 = 5.0 kHz
; TRx Matching Network Type = 13 dBm
; Tx Power                  = +13 dBm
; Gaussian BT               = NA
; Bandwidth                 = Auto-Select kHz
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Tx Exit State             = STBY
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Variable Length
; Node-Length Position      = First Node, then Length
; Payload Bit Order         = Start from msb
; Preamble Tx Size          = 8
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 2-byte
; Sync Value                = 11732
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 32
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; LBD Threshold             = 2.4 V
*/
unsigned short int CMTBank[12] = {
0x0040,
0x0166,
0x02EC,
0x031C,
0x04F0,
0x0580,
0x0614,
0x0708,
0x0811,
0x0902,
0x0A02,
0x0B00
           };

unsigned short int SystemBank[12] = {
0x0CAE,
0x0DE0,
0x0E75,
0x0F00,
0x1000,
0x11F4,
0x1210,
0x13E2,
0x1442,
0x1520,
0x1600,
0x1781
          };

unsigned short int FrequencyBank[8] = {
0x1842,
0x19CF, //değişti 0x19A8
0x1AA7, //değişti 0x1A31
0x1B8C, //değişti 0x1B8A
0x1C42,
0x1DC4, //değişti  0x1D9D
0x1E4E, //değişti  0x1ED8
0x1F1C //değişti0x1F19  
             };

unsigned short int DataRateBank[24] = {
0x2032,
0x2118,
0x2210,
0x2399,
0x24D2,
0x25BD,
0x260C, 
0x270A,
0x289F,
0x2926,
0x2A29,
0x2B29,
0x2CC0,
0x2D51,
0x2E2A,
0x2F53,
0x3000,
0x3100,
0x32B4,
0x3300,
0x3400,
0x3501,
0x3600,
0x3700
            };

unsigned short int BasebandBank[29] = {
0x3812, //DEĞİŞTİ 0x3810 
0x3908,
0x3A00,
0x3BAA,
0x3C02,
0x3D00,
0x3E00,
0x3F00,
0x4000,
0x4100,
0x4200,
0x43D4,
0x442D,
0x4500,
0x461F,
0x4700,
0x4800,
0x4900,
0x4A00,
0x4B00,
0x4C00,
0x4D00,
0x4E00,
0x4F60,
0x50FF,
0x5100,
0x5200,
0x531F,
0x5410
            };
unsigned short int TXBank[11] = {
0x5550, //DEĞİŞTİ 0x5554
0x564D,
0x5706,
0x5800,
0x5942,
0x5AB0,
0x5B00,
0x5C42,
0x5D0C,
0x5E3F,
0x5F7F 
              };

//;---------------------------------------
//;  The following is the CRC result for
//;  the above Register contents
//;---------------------------------------
//0x4CFA

#else
  #error Multiple definitions are not allowed.
#endif
