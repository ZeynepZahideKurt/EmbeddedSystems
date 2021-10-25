#include "stdio.h"
#include "stdint.h"

#define fcsb  15//RTC 12 NUMARA FCSB
#define csb 5 //RTC 11 NUMARA CSB
#define SDIO 19 //rtc 10 numara SDIO
#define SCLK 18 //rtc 9 numara SCLK yeni

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  pinMode(fcsb, OUTPUT);
  pinMode(csb, OUTPUT);
  pinMode(SDIO, OUTPUT);
  pinMode(SCLK, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
void cmt_spi3_write(uint8_t addr, uint8_t dat)
{
  digitalWrite(SDIO, 1);
  digitalWrite(SCLK, 0);
  digitalWrite(SCLK, 0);
  digitalWrite(fcsb, 1);
  digitalWrite(fcsb, 1);
  digitalWrite(csb, 0);
  /* > 0.5 SCLK cycle */
  delay(0.5);
  /* r/w = 0 */
  cmt_spi3_send(addr & 0x7F);
  cmt_spi3_send(dat);
  digitalWrite(SCLK, 0);
  /* > 0.5 SCLK cycle */
  cmt_spi3_delay();
  cmt_spi3_delay();
  digitalWrite(csb, 1);
  digitalWrite(SDIO, 1);

  digitalWrite(fcsb, 1);
}
void cmt_spi3_read(uint8_t addr, uint8_t* p_dat)
{
  digitalWrite(SDIO, 1);

  digitalWrite(SCLK, 0);

  digitalWrite(SCLK, 0);
  digitalWrite(fcsb, 1);

  digitalWrite(fcsb, 1);
  digitalWrite(csb, 0);
  /* > 0.5 SCLK cycle */
  delay(0.5);
  /* r/w = 1 */
  cmt_spi3_send(addr | 0x80);
  /* Must set SDIO to input before the falling edge of SCLK */

  *p_dat = cmt_spi3_recv();
  digitalWrite(SCLK, 0);
  /* > 0.5 SCLK cycle */
  delay(0.5);
  digitalWrite(csb, 1);
  digitalWrite(SDIO, 1);

  digitalWrite(fcsb, 1);
}
void cmt_spi3_read_fifo(uint8_t* p_buf, uint16_t len)
{
  uint16_t i;
  digitalWrite(fcsb, 1);

  digitalWrite(fcsb, 1);
  digitalWrite(csb, 1);

  digitalWrite(csb, 1);
  digitalWrite(SCLK, 0);

  digitalWrite(SCLK, 0);

  for (i = 0; i < len; i++)
  {
    digitalWrite(fcsb, 0);
    /* > 1 SCLK cycle */
    delay(1);
    p_buf[i] = cmt_spi3_recv();
    digitalWrite(SCLK, 0);
    /* > 2 us */
    delay(2);
    digitalWrite(fcsb, 1);
    /* > 4 us */
    delay(4);
  }

  digitalWrite(fcsb, 1);
}
