
#ifndef __RXCONFIG_H
#define __RXCONFIG_H
/*;---------------------------------------
;  CMT2219B Configuration File
;  Generated by CMOSTEK RFPDK 1.48
;  2020.03.10 09:51
;---------------------------------------
; Mode                    = Advanced
; Part Number             = CMT2219B
; Frequency               = 433.920 MHz
; Xtal Frequency          = 26.0000 MHz
; Demodulation            = FSK
; AGC                     = On
; Data Rate               = 2.4 kbps
; Deviation               = 5.0 kHz
; Rx Xtal Tol.            = 40 ppm
; Bandwidth               = Auto-Select kHz
; CDR Type                = Counting
; CDR DR Range            = NA
; AFC                     = On
; AFC Method              = Auto-Select
; Data Representation     = 0:F-low 1:F-high
; Rx Duty-Cycle           = Off
; Sleep Timer             = Off
; Sleep Time              = NA
; Rx Timer                = Off
; Rx Time T1              = NA
; Rx Time T2              = NA
; Rx Exit State           = SLEEP
; SLP Mode                = Disable
; RSSI Valid Source       = PJD
; PJD Window              = 8 Jumps
; LFOSC Calibration       = On
; Xtal Stable Time        = 2480 us
; RSSI Compare TH         = NA
; Data Mode               = Packet
; Whitening               = Disable
; Whiten Type             = NA
; Whiten Seed Type        = NA
; Whiten Seed             = NA
; Manchester              = Disable
; Manchester Type         = NA
; FEC                     = Disable
; FEC Type                = NA
; Packet Type             = Fixed Length
; Node-Length Position    = NA
; Payload Bit Order       = Start from msb
; Preamble Rx Size        = 2
; Preamble Value          = 170
; Preamble Unit           = 8-bit
; Sync Size               = 2-byte
; Sync Value              = 11732
; Sync Tolerance          = None
; Sync Manchester         = Disable
; Node ID Size            = NA
; Node ID Value           = NA
; Node ID Mode            = None
; Node ID Err Mask        = Disable
; Node ID Free            = Disable
; Payload Length          = 32
; CRC Options             = None
; CRC Seed                = NA
; CRC Range               = NA
; CRC Swap                = NA
; CRC Bit Invert          = NA
; CRC Bit Order           = NA
; Dout Mute               = Off
; Dout Adjust Mode        = Disable
; Dout Adjust Percentage  = NA
; Collision Detect        = Off
; Collision Detect Offset = NA
; RSSI Detect Mode        = Always
; RSSI Filter Setting     = No Filtering
; RF Performance          = High
; LBD Threshold           = 2.4 V
; RSSI Offset             = 26
; RSSI Offset Sign        = 1

;---------------------------------------
;  The following are the Register contents
;---------------------------------------*/
unsigned short int CMT_Bank[]={
0x0002,
0x0166,
0x02EC,
0x031C,
0x0430,//4f0
0x0580,
0x0614,
0x0708,
0x0891,
0x0902,
0x0A02,
0x0BD0
};
unsigned short int System_Bank[]={
0x0CAE,
0x0DE0,
0x0E70,
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

unsigned short int Frequency_Bank[]={
0x1842,
0x1971,
0x1ACE,
0x1B1C,
0x1C42,
0x1D5B,
0x1E1C,
0x1F1C
};

unsigned short int Data_Rate_Bank[]={
0x201F, //2032
0x21F8, //2118
0x2271, //200
0x2310, //2399
0x24EB, //24c1
0x2524, // 259b
0x261F, //2606
0x270A, 
0x289F,
0x294B, //2939
0x2A29,
0x2B29,
0x2CC0,
0x2D08, //2d51
0x2E02, //2e2a
0x2F53,
0x3018, //3000
0x3100,
0x32B4,
0x3300,
0x3400,
0x3501,
0x3600,
0x3700
};

unsigned short int Baseband_Bank[]={
0x3812,
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
0x460F,//461f
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
0x5101,
0x5200,
0x531F,
0x5410
};

unsigned short int Reserve_Bank[]={
0x5550,
0x5603,
0x573F,
0x5800,
0x5902,
0x5A30,
0x5B00,
0x5C01,
0x5D01,
0x5E3F
};

unsigned short int LBD_Bank[]={
0x5F7F
};

//;---------------------------------------
//;  The following is the CRC result for
//;  the above Register contents
//;---------------------------------------
//0x0E6A

#else
  #error Multiple definitions are not allowed.
#endif
