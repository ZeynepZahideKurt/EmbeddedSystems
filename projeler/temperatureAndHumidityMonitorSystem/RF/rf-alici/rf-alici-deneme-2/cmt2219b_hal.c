#include "stdint.h"

#define CMT2219B_MASK_CFG_RETAIN 0x10
#define CMT2219B_CUS_MODE_STA 0x61
#define CMT2219B_MASK_RSTN_IN_EN 0x20
#define CMT2219B_CUS_EN_CTL 0x62
#define CMT2219B_MASK_LOCKING_EN 1



/*! ********************************************************
* @name CMT2219B_InitGpio
* @desc Initializes the CMT2219B interface GPIOs.
* *********************************************************/
void CMT2219B_InitGpio(void);
/*! ********************************************************
* @name CMT2219B_ReadReg
* @desc Read the CMT2219B register at the specified address.
* @paramaddr: register address
* @return Register value
* *********************************************************/
uint8_t CMT2219B_ReadReg(uint8_t addr);
/*! ********************************************************
* @name CMT2219B_WriteReg
* @desc Write the CMT2219B register at the specified address.
* @paramaddr: register address
* dat: register value
* *********************************************************/
void CMT2219B_WriteReg(uint8_t addr, uint8_t dat);
/*! ********************************************************
* @name CMT2219B_ReadFifo
* @desc Reads the contents of the CMT2219B FIFO.
* @parambuf: buffer where to copy the FIFO read data
* len: number of bytes to be read from the FIFO
* *********************************************************/
void CMT2219B_ReadFifo(uint8_t buf[], uint16_t len);
/*! ********************************************************
* @name CMT2219B_WriteFifo
* @desc Writes the buffer contents to the CMT2219B FIFO.
* @parambuf: buffer containing data to be put on the FIFO
* len: number of bytes to be written to the FIFO
* *********************************************************/
void CMT2219B_WriteFifo(const uint8_t buf[], uint16_t len);
/*! ********************************************************
* @name CMT2219B_Init
* @desc Initialize chip status.
* *********************************************************/
void CMT2219B_Init(void)
{
 uint8_t tmp;
 CMT2219B_SoftReset();
 CMT2219B_DelayMs(20);
 CMT2219B_GoStby();
tmp = CMT2219B_ReadReg(CMT2219B_CUS_MODE_STA);
tmp |= CMT2219B_MASK_CFG_RETAIN; /* Enable CFG_RETAIN */
tmp&= ~CMT2219B_MASK_RSTN_IN_EN; /* Disable RSTN_IN */
 CMT2219B_WriteReg(CMT2219B_CUS_MODE_STA, tmp);
tmp = CMT2219B_ReadReg(CMT2219B_CUS_EN_CTL);
tmp |= CMT2219B_MASK_LOCKING_EN; /* Enable LOCKING_EN */
 CMT2219B_WriteReg(CMT2219B_CUS_EN_CTL, tmp);
 CMT2219B_EnableLfosc(0); /* Diable LFOSC */
 CMT2219B_ClearInterruptFlags();
}
