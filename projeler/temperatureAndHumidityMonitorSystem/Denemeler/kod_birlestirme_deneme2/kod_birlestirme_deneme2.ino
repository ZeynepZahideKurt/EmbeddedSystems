/*
  GPIO 0 12k-13k direnç ve butonla ve EN( Resetleme butonu-12k 13k direnç tak )programlama moduna geç. önce I0 butonuna basıp sonra EN butonuna basarsan programlama moduna geçer
  GPIO1 (TX) / GPIO3 (RX) ile program at
  GPIO2 bi tane led tak wifii ye bağlandığınında ışığı söner
  GPIO15 (pcbde 12) bir buton tak wifiiye bağlanma oradaki butonla eepromdaki kullanıcı adı şifreyi değiştir
  GPIO21 SDA / GPIO22 SCL 3.3 volt

  SPI   MOSI    MISO    CLK     CS
  VSPI  GPIO 23 GPIO 19 GPIO 18 GPIO 5
  HSPI  GPIO 13 GPIO 12 GPIO 14 GPIO 15
*/

/*
  receive_() fonksiyonunda ve verileri alıyor ver yine aynı fonksiyonda sıcaklık nem sensörünün bilgilerini lcd ekrana gönderiyor


*/

#include <ArduinoWebsockets.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Ticker.h>
#include "time.h"
#include "lcd.h"
Ticker ticker;

#include <EEPROM.h>
#define EEPROM_SIZE 50
#include <stdio.h>
#include <string.h>

#include <Wire.h>
#define Addr 0x44
#define I2C_SDA 21
#define I2C_SCL 22

#include <arduino.h>
#include "RFM300_Driver.h"
#include "rf_conf.h"

#define SCREEN_CS_Pin 18
#define SCREEN_RST_Pin 5
#define SCREEN_A0_Pin 17
#define SCREEN_SCL_Pin 16
#define  SCREEN_SDA_Pin 4
#define NOP __asm__ __volatile__ ("nop\n\t")

using namespace websockets;
WebsocketsServer socket_server;
WebsocketsClient client;

const char* websocket_server_host = "192.168.1.239";
//const char* websocket_server_host = "89.19.23.98";
const uint16_t websocket_server_port = 8080;
const uint16_t websocket_server_port1 = 8887;
const uint16_t websocket_server_port2 = 8886;




const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int Krish_day;
int Krish_month;
int Krish_year;
int Krish_hour;
int Krish_min;
int Krish_sec;
char inByte;
int kontrolcircle = 0;
int kontrolsayi;
int kontrol1 = 0;

uint8_t counter, page, column;
uint16_t screenTempI, screenTempI2;
char arr[20];
char displayArr[40];

typedef enum {
  LEFT = 0, CENTER = 1, RIGHT = 2, RIGHT_UP = 3, CENTER_UP = 4, CENTER_LEFT = 5, EKSILEFT = 6
} LOCATION;
typedef enum {
  INSTRUCTION = 0, DATA = 1
} TRANSMODE;


#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

unsigned long simdikizaman, eskizaman, simdikizaman2, eskizaman2, simdikizaman3, eskizaman3, simdikizaman4, eskizaman4;
int circle_kontrol = 0;
int circle_kontrol2 = 5;
int circle_sira = 0;
int LED = LED_BUILTIN;

float cTemp;
float humidity;

String cihaz1, cihaz2, cihaz3, cihaz4, cihaz5, cihaz6;
int cihaz1_eeprom, cihaz1_eeprom1, cihaz1_eeprom2, cihaz1_eeprom3, cihaz1_eeprom4;
int cihaz2_eeprom, cihaz2_eeprom1, cihaz2_eeprom2, cihaz2_eeprom3, cihaz2_eeprom4;
int cihaz3_eeprom, cihaz3_eeprom1, cihaz3_eeprom2, cihaz3_eeprom3, cihaz3_eeprom4;
int cihaz4_eeprom, cihaz4_eeprom1, cihaz4_eeprom2, cihaz4_eeprom3, cihaz4_eeprom4;
int cihaz5_eeprom, cihaz5_eeprom1, cihaz5_eeprom2, cihaz5_eeprom3, cihaz5_eeprom4;
int cihaz6_eeprom, cihaz6_eeprom1, cihaz6_eeprom2, cihaz6_eeprom3, cihaz6_eeprom4;
int cihaz_sayisi_eeprom = 0;
int mac_kontrol = 0;

String string_parcala;
String string1_kontrol, string2_kontrol, string3_kontrol, string4_kontrol, string5_kontrol, string6_kontrol;
int string1_eeprom, string1_eeprom1, string1_eeprom2, string1_eeprom3, string1_eeprom4;
int string2_eeprom, string2_eeprom1, string2_eeprom2, string2_eeprom3, string2_eeprom4;
int string3_eeprom, string3_eeprom1, string3_eeprom2, string3_eeprom3, string3_eeprom4;
int string4_eeprom, string4_eeprom1, string4_eeprom2, string4_eeprom3, string4_eeprom4;
int string5_eeprom, string5_eeprom1, string5_eeprom2, string5_eeprom3, string5_eeprom4;
int string6_eeprom, string6_eeprom1, string6_eeprom2, string6_eeprom3, string6_eeprom4;

float cihaz1ft, cihaz2ft, cihaz3ft, cihaz4ft, cihaz5ft, cihaz6ft;
float cihaz1fh, cihaz2fh, cihaz3fh, cihaz4fh, cihaz5fh, cihaz6fh;
char sicaklik_cihaz1[6], sicaklik_cihaz2[6], sicaklik_cihaz3[6], sicaklik_cihaz4[6], sicaklik_cihaz5[6], sicaklik_cihaz6[6];
char nem_cihaz1[6], nem_cihaz2[6], nem_cihaz3[6], nem_cihaz4[6], nem_cihaz5[6], nem_cihaz6[6];
char gonder_sicaklik_cihaz1[6], gonder_sicaklik_cihaz2[6], gonder_sicaklik_cihaz3[6], gonder_sicaklik_cihaz4[6], gonder_sicaklik_cihaz5[6], gonder_sicaklik_cihaz6[6];
char gonder_nem_cihaz1[6], gonder_nem_cihaz2[6], gonder_nem_cihaz3[6], gonder_nem_cihaz4[6], gonder_nem_cihaz5[6], gonder_nem_cihaz6[6];
char stemp[6], kontrol_stemp[6];
char nem[6], kontrol_nem[6];
char kontrol_sicaklik_cihaz1[6], kontrol_sicaklik_cihaz2[6], kontrol_sicaklik_cihaz3[6], kontrol_sicaklik_cihaz4[6], kontrol_sicaklik_cihaz5[6], kontrol_sicaklik_cihaz6[6];

String pil_durumu;
char DEVICEbattery1;
int int_pil_durumu, int_pil_durumu1, int_pil_durumu2, int_pil_durumu3, int_pil_durumu4, int_pil_durumu5, int_pil_durumu6;

int eksimi1 = 0;
int eksimi2 = 0;
int eksimi3 = 0;
int eksimi4 = 0;
int eksimi5 = 0;
int eksimi6 = 0;

int eksikontrol1 = 0;
int eksikontrol2 = 0;
int eksikontrol3 = 0;
int eksikontrol4 = 0;
int eksikontrol5 = 0;
int eksikontrol6 = 0;

int HATA_KONROL0 = 0;
int HATA_KONROL1 = 0;
int HATA_KONROL2 = 0;
int HATA_KONROL3 = 0;
int HATA_KONROL4 = 0;
int HATA_KONROL5 = 0;
int HATA_KONROL6 = 0;

int clclkontrol = 0;

byte rf_kontrol;

String rf_seri_numb;
byte rx_buf[32];
String rx;
void tick() {
  //toggle state
  digitalWrite(LED, !digitalRead(LED));     // set pin to the opposite state
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Krish_hour = timeinfo.tm_hour + 2;
  Krish_min = timeinfo.tm_min;
  Krish_sec = timeinfo.tm_sec;
  Krish_day = timeinfo.tm_mday;
  Krish_month = timeinfo.tm_mon + 1;
  Krish_year = timeinfo.tm_year + 1900;
  setPage(0);
  setColumn(98);
  char dateTime[20];
  sprintf(dateTime, "%02u-%02u-%02u %02u:%02u", Krish_day  ,
          Krish_month, Krish_year, Krish_hour,
          Krish_min);
  display(1, dateTime, RIGHT, false);
}
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
  // display(3, "FAILED TO CONNECT", CENTER, 1);
  display(3, "ENELSIS", CENTER, 0);
  display(4, "CIHAZINA BAGLANINIZ!", CENTER, 0);
  //display(5, "IF NOT CONNECTED, ENTER USER NAME PASSWORD", CENTER, 1);
  delay(3000);
}
void trans(TRANSMODE type, uint8_t command) {
  digitalWrite(SCREEN_CS_Pin, 0);

  digitalWrite(SCREEN_A0_Pin, type != INSTRUCTION);
  for (counter = 0; counter < 8; counter++) {

    if (((command & 0x80) == 0x80) == 1) {
      digitalWrite(SCREEN_SDA_Pin, 1);
    } else {
      digitalWrite(SCREEN_SDA_Pin, 0);
    }

    digitalWrite(SCREEN_SCL_Pin, 0);
    NOP;
    digitalWrite(SCREEN_SCL_Pin, 1);
    NOP;
    digitalWrite(SCREEN_SCL_Pin, 0);
    NOP;
    command <<= 1;
  }

  digitalWrite(SCREEN_CS_Pin, HIGH);
}
void initial() {
  for (screenTempI = 0; screenTempI < 13; screenTempI++) {
    trans(INSTRUCTION, initialArray[screenTempI]);
  }
}
void setPage(uint8_t page) {
  trans(INSTRUCTION, 0xb0 + page);
}
void setColumn(uint8_t column) {
  trans(INSTRUCTION, 0x10 | (column >> 4));
  trans(INSTRUCTION, 0x00 | (column & 0x0f));
}
void clearScreen() { //uint8_t removeFirstPage
  /*for (page = !removeFirstPage; page < 8; page++) {
    setPage(page);
    setColumn(0);
    for (column = 0; column < 1024; column++) {
      trans(DATA, 0x00);
    }

    clearPage(1);

    } */
  clearPage(0);
  clearPage(1);
  clearPage(2);
  clearPage(3);
  clearPage(4);
  clearPage(5);
  clearPage(6);
  clearPage(7);
  //clearPage(2);

  /*
    for (int i = 0; i < 1024; i++) {
      trans(i, 0x00);
    }
    clearPage(page);

    for (screenTempI = 0; screenTempI < 8; screenTempI++) {
      setPage(screenTempI);
      setColumn(0);
      for (screenTempI2 = 0; screenTempI2 < 128; screenTempI2++) {
        trans(DATA, 0x00);
      }
    } */
}
void displaycl() {
  for (screenTempI = 0; screenTempI < 8; screenTempI++) {
    setPage(screenTempI);
    setColumn(0);
    for (screenTempI2 = 0; screenTempI2 < 128; screenTempI2++) {
      trans(DATA, cl[(screenTempI * 128) + screenTempI2]);
    }
  }
}
void circleust() {


  setPage(3);
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_ust[i]);
  }
}
void circlealt() {


  setPage(4);
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_alt[i]);
  }
}
void circleust_eksi() {


  setPage(3);
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_ust_eksi[i]);
  }
}
void circlealt_eksi() {


  setPage(4);
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_alt_eksi[i]);
  }
}
void circleyuzdeust() {


  setPage(5);
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_yuzde_ust[i]);
  }
}
void satirbir() {


  setPage(1);
  setColumn(0);
  for (int i = 0; i < 128; i++) {
    trans(DATA, SATIRBIR[i]);
  }
}
void circleyuzdealt() {


  setPage(6);
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_yuzde_alt[i]);
  }
}
void setScreenAndBackgroundLED(bool isOpen) {

  digitalWrite(SCREEN_A0_Pin, 0);
  digitalWrite(SCREEN_CS_Pin, 0);
  digitalWrite(SCREEN_RST_Pin, 0);
  digitalWrite(SCREEN_SCL_Pin, 0);
  digitalWrite(SCREEN_SDA_Pin, 0);

}
void initScreen() {
  setScreenAndBackgroundLED(true);
  digitalWrite(SCREEN_RST_Pin, 0);
  delay(500);
  digitalWrite(SCREEN_RST_Pin, 1);
  initial();

  displaycl();
}
void clearPage(uint8_t page) {
  setPage(page);
  setColumn(0);
  for (column = 0; column < 128; column++) {
    trans(DATA, 0x00);
  }
}
void clclearPage(uint8_t page) {
  setPage(page);
  setColumn(0);
  for (column = 0; column < 128; column++) {
    trans(DATA,  clcl[column]);
  }
}
void displayChar(char array[]) {
  uint8_t size = array[0];
  for (screenTempI = 1; screenTempI < size + 1; screenTempI++) {
    trans(DATA, array[screenTempI]);
  }
  trans(DATA, 0x00);
}
void displayBigChar(char array[]) {
  for (screenTempI = 0; screenTempI < 10; screenTempI++) {
    trans(DATA, array[screenTempI]);
  }
  trans(DATA, 0x00);
}
void display(uint8_t page, char sentence[], LOCATION location, bool isClearRow) {
  if (isClearRow)
    clearPage(page);
  int16_t i;
  char temp2[20];
  uint16_t sentence_size = strlen(sentence);
  uint8_t total_column = 0;
  char letter;
  for (i = 0; i < sentence_size; i++) {
    letter = sentence[i];
    total_column += charArray[letter - 32][0];
  }
  total_column += sentence_size - 1;
  setPage(page);
  switch (location) {
    case LEFT:
      setColumn(123 - total_column); //gün çift haneliyse
      break;
    case EKSILEFT:
      setColumn(101); //gün çift haneliyse
      break;
    case CENTER:
      setColumn(64 - (total_column / 2));
      break;
    case CENTER_UP:
      setColumn(78 - total_column);
      break;
    case RIGHT:
      setColumn(9);
      break;



    default:
      setColumn(location);
      break;

  }
  /*if (location == LEFT) {
    setColumn(123 - total_column); //gün çift haneliyse
    //  kontrol=0;

    }
    if (location == CENTER) {
    setColumn(64 - (total_column / 2));
    }
    if (location == CENTER_UP) {
    setColumn(78 - total_column);
    }
    if (location == RIGHT) {
    setColumn(9);
    }
    if (location == RIGHT_UP) {
    setColumn(32 - (total_column / 2));
    }



    if (location == BIR_PIKSEL) {
    setColumn(107 - total_column); //ay çift haneliyse
    //  kontrol=0;

    }
    if (location == IKI_PIKSEL) {
    setColumn(89 - total_column);
    }

    if (location == UC_PIKSEL) {
    setColumn(117 - total_column); // gün tek haneliyse
    }
    if (location == DORT_PIKSEL) {
    setColumn(101 - total_column);  // ay tek haneliyse
    }

    if (location == SAAT) {
    setColumn(25);
    }*/


  for (i = sentence_size - 1; i >= 0; i--) {
    letter = sentence[i];
    memcpy(temp2, charArray[letter - 32], charArray[letter - 32][0] + 1);
    displayChar(temp2);
  }
}
void circle() {


  setPage(2);
  //  setColumn(65 - (8*i));
  for (int i = 5; i < 123; i++) {
    trans(DATA, circle_yuzde_ust[i]);
  }
}
void displayBig(uint8_t page, char sentence[], LOCATION location, bool isClearRow) { //page: 0, 2, 4, 6//MAX 11 KARAKTER
  if (isClearRow) {
    clearPage(page);
    clearPage(page + 1);
  }
  int8_t i;
  char temp2[10];
  char letter;
  uint8_t sentence_size = strlen(sentence); //KART YOK
  uint8_t total_column = 0;
  for (i = 0; i < sentence_size; i++) {
    if (sentence[i] == ' ')
      total_column += 3;
    else
      total_column += 11;
  }
  switch (location) {
    case LEFT:
      total_column = 118 - (total_column / 2);
      break;
    case CENTER:
      total_column = 64 - (total_column / 2);
      break;
    case RIGHT:
      total_column = 8;
      break;
    case CENTER_LEFT:
      total_column = 45 - (total_column / 2);
      break;
  }
  //clearPage(page);
  setPage(page);
  setColumn(total_column);
  for (i = sentence_size - 1; i >= 0; i--) {
    letter = sentence[i];
    if (letter == ' ') {
      trans(DATA, 0x00);
      trans(DATA, 0x00);
      trans(DATA, 0x00);
      trans(DATA, 0x00);
    } else {
      memcpy(temp2, big_char_array_page_top[letter - 32], 10);

      displayBigChar(temp2);
    }
  }
  //clearPage(page + 1);
  setPage(page + 1);
  setColumn(total_column);
  for (i = sentence_size - 1; i >= 0; i--) {
    letter = sentence[i];
    if (letter == ' ') {
      trans(DATA, 0x00);
      trans(DATA, 0x00);
      trans(DATA, 0x00);
      trans(DATA, 0x00);
    } else {
      memcpy(temp2, big_char_array_page_bottom[letter - 32], 10);
      displayBigChar(temp2);
    }
  }
}
void displayLogo() {
  for (screenTempI = 0; screenTempI < 8; screenTempI++) {
    setPage(screenTempI);
    setColumn(0);
    for (screenTempI2 = 0; screenTempI2 < 128; screenTempI2++) {
      trans(DATA, logoArray[(screenTempI * 128) + screenTempI2]);
    }
  }
}
void displayMainScreen() {
  ////displayBattery();
  //displayDateTime();
  //displayAlarms();
  displayBig(3, "TOPLAM", CENTER, 1);
  sprintf(displayArr, "DENEME");
  displayBig(6, displayArr, CENTER, 1);
}
void displayMainScreen2() {
  displayBig(3, "KALAN", CENTER, 1);
  sprintf(displayArr, "DENEME2");
  displayBig(6, displayArr, CENTER, 1);
}
void getBatteryValues() {
  //BATARYAYI DENERKEN : https://forum.arduino.cc/t/measuring-the-battery-voltage-using-the-adc-on-mini-3v3-8mhz/422944   /   https://forum.arduino.cc/t/battery-level-check-using-arduino/424054  /  https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/

  /* const long InternalReferenceVoltage = 1062;  // Adjust this value to your board's specific internal BG voltage

    // Code courtesy of "Coding Badly" and "Retrolefty" from the Arduino forum
    // results are Vcc * 100
    // So for example, 5V would be 500.
    int getBandgap ()
    {
    // REFS0 : Selects AVcc external reference
    // MUX3 MUX2 MUX1 : Selects 1.1V (VBG)
    ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
    ADCSRA |= bit( ADSC );  // start conversion
    while (ADCSRA & bit (ADSC))
      { }  // wait for conversion to complete (toss this measurement)
    ADCSRA |= bit( ADSC );  // start conversion
    while (ADCSRA & bit (ADSC))
      { }  // wait for conversion to complete
    int results = (((InternalReferenceVoltage * 1024) / ADC) + 5) / 10;
    return results;
    } // end of getBandgap */

  //OR
  /*
    int value = 0;
    float voltage;
    float perc;

    void setup(){
    Serial.begin(9600);
    }

    void loop(){
    value = analogRead(A0);
    voltage = value * 5.0/1023;
    perc = map(voltage, 3.6, 4.2, 0, 100);
    Serial.print("Voltage= ");
    Serial.println(voltage);
    Serial.print("Battery level= ");
    Serial.print(perc);
    Serial.println(" %");
    delay(500);
    }
  */




}
void displayBattery(int a) {
  //DEVICEbattery1 = '80';

  setPage(1);
  setColumn(95);
  sprintf(arr, "%02u", DEVICEbattery1);
  // displayChar(charArray[arr[1] - 32]);
  // displayChar(charArray[arr[0] - 32]);
  //displayChar(charArray['%' - 32]);
  trans(DATA, 0b00011100);
  trans(DATA, 0b00011100);
  trans(DATA, 0b01111111);
  trans(DATA, 0b01000001);
  int_pil_durumu = a;
  if (int_pil_durumu >= 3240) { //if ( DEVICEbattery1 >= 83) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if (int_pil_durumu >= 2880) { //if ( DEVICEbattery1 >= 67) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if (int_pil_durumu >= 2520) { //if ( DEVICEbattery1 >= 50) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if (int_pil_durumu >= 2160) { //if ( DEVICEbattery1 >= 33) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if (int_pil_durumu >= 1800) { //if ( DEVICEbattery1 >= 17) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  trans(DATA, 0b01011101);
  trans(DATA, 0b01000001);
  trans(DATA, 0b01111111);
}
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  /*Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);*/

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  //long rssi = -40;
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  setPage(1);
  setColumn(110);


  if (rssi > -50 &  rssi < 0 ) { //çok iyi
    trans(DATA, 0b00001000);
    trans(DATA, 0b00010100);
    trans(DATA, 0b00101010);
    trans(DATA, 0b00010101);

    trans(DATA, 0b11010101);
    trans(DATA, 0b11010101);

    trans(DATA, 0b00010101);
    trans(DATA, 0b00101010);
    trans(DATA, 0b00010100);
    trans(DATA, 0b00001000);



  } else if (rssi > -70 &  rssi < -50) {

    trans(DATA, 0b00000000);
    trans(DATA, 0b00010000);
    trans(DATA, 0b00101000);
    trans(DATA, 0b00010100);

    trans(DATA, 0b11010100);
    trans(DATA, 0b11010100);

    trans(DATA, 0b00010100);
    trans(DATA, 0b00101000);
    trans(DATA, 0b00010000);
    trans(DATA, 0b00000000);
  } else if (rssi > -80 &  rssi < -70) {
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);
    trans(DATA, 0b00100000);
    trans(DATA, 0b00010000);

    trans(DATA, 0b11010000);
    trans(DATA, 0b11010000);

    trans(DATA, 0b00010000);
    trans(DATA, 0b00100000);
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);

  } else if (rssi > -90 &  rssi < -80) { //çok kötü
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);

    trans(DATA, 0b11000000);
    trans(DATA, 0b11000000);

    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);
    trans(DATA, 0b00000000);
  }


}
void circle_location() {
  //Serial.print("cihaz_sayisi_eeprom: "); Serial.println(cihaz_sayisi_eeprom);
  //clearPage(2);
  setPage(2);
  if (cihaz_sayisi_eeprom == 0) {
    circle_sira = 0;
    if (circle_kontrol == 0) {
      clclearPage(2);
      circle_kontrol = 1;
    }
    display(2, "             ANA CIHAZ             ",  CENTER, 0);

  } else {
    circle_kontrol = 0;
    //Serial.print("circleye girdi ");//Serial.print("c: ");Serial.println(c);
    int a = (65 - (8 * cihaz_sayisi_eeprom));
    int b = (65 + (8 * cihaz_sayisi_eeprom)) ;
    int d = (b - a) / 16;
    setColumn(a);
    //Serial.print("a: "); Serial.println(a);Serial.print("b: ");Serial.println(b);Serial.print("d: ");Serial.println(d);
    if (kontrolsayi == 0  ) {
      display(2, "             ANA CIHAZ             ",  CENTER, 0);
      circle_sira = 0;
      circle_kontrol2 = 0;


    } else if (kontrolsayi == 1  ) {
      circle_sira = 1;
      if (circle_kontrol2 == 0) {
        clclearPage(2);
      }
      circle_kontrol2 = 1;
      for (int i = d; i > 0 ; i--) {
        // Serial.print("i: "); Serial.println(i);
        if (i == 1) {
          for (int l = a; l < a + 16; l++) {
            //   Serial.print("l: "); Serial.println(l);
            trans(DATA, KOYUBIR[l - a]);
          }
        } else if (i == 2) {
          for (int l = a + 16; l < a + 32; l++) {
            trans(DATA, ACIKIKI[l - a - 16]);
          }
        } else if (i == 3) {
          for (int l = a + 32; l < a + 48; l++) {
            trans(DATA, ACIKUC[l - a - 32]);
          }
        } else if (i == 4) {
          for (int l = a + 48; l < a + 64; l++) {
            trans(DATA, ACIKDORT[l - a - 48]);
          }
        } else if (i == 5) {
          for (int l = a + 64; l < a + 80; l++) {
            trans(DATA, ACIKBES[l - a - 64]);
          }
        } else if (i == 6) {
          for (int l = a + 80; l < a + 96; l++) {
            trans(DATA, ACIKALTI[l - a - 80]);
          }
        }

      }
    } else if (kontrolsayi == 2) {
      circle_sira = 2;
      for (int i = d; i > 0 ; i--) {
        // Serial.print("i: "); Serial.println(i);
        if (i == 1) {
          for (int l = a; l < a + 16; l++) {
            //   Serial.print("l: "); Serial.println(l);
            trans(DATA, ACIKBIR[l - a]);
          }
        } else if (i == 2) {
          for (int l = a + 16; l < a + 32; l++) {
            trans(DATA, KOYUIKI[l - a - 16]);
          }
        } else if (i == 3) {
          for (int l = a + 32; l < a + 48; l++) {
            trans(DATA, ACIKUC[l - a - 32]);
          }
        } else if (i == 4) {
          for (int l = a + 48; l < a + 64; l++) {
            trans(DATA, ACIKDORT[l - a - 48]);
          }
        } else if (i == 5) {
          for (int l = a + 64; l < a + 80; l++) {
            trans(DATA, ACIKBES[l - a - 64]);
          }
        } else if (i == 6) {
          for (int l = a + 80; l < a + 96; l++) {
            trans(DATA, ACIKALTI[l - a - 80]);
          }
        }

      }
    } else if (kontrolsayi == 3) {
      circle_sira = 3;
      for (int i = d; i > 0 ; i--) {
        // Serial.print("i: "); Serial.println(i);
        if (i == 1) {
          for (int l = a; l < a + 16; l++) {
            //   Serial.print("l: "); Serial.println(l);
            trans(DATA, ACIKBIR[l - a]);
          }
        } else if (i == 2) {
          for (int l = a + 16; l < a + 32; l++) {
            trans(DATA, ACIKIKI[l - a - 16]);
          }
        } else if (i == 3) {
          for (int l = a + 32; l < a + 48; l++) {
            trans(DATA, KOYUUC[l - a - 32]);
          }
        } else if (i == 4) {
          for (int l = a + 48; l < a + 64; l++) {
            trans(DATA, ACIKDORT[l - a - 48]);
          }
        } else if (i == 5) {
          for (int l = a + 64; l < a + 80; l++) {
            trans(DATA, ACIKBES[l - a - 64]);
          }
        } else if (i == 6) {
          for (int l = a + 80; l < a + 96; l++) {
            trans(DATA, ACIKALTI[l - a - 80]);
          }
        }

      }
    } else if (kontrolsayi == 4) {
      circle_sira = 4;
      for (int i = d; i > 0 ; i--) {
        // Serial.print("i: "); Serial.println(i);
        if (i == 1) {
          for (int l = a; l < a + 16; l++) {
            //   Serial.print("l: "); Serial.println(l);
            trans(DATA, ACIKBIR[l - a]);
          }
        } else if (i == 2) {
          for (int l = a + 16; l < a + 32; l++) {
            trans(DATA, ACIKIKI[l - a - 16]);
          }
        } else if (i == 3) {
          for (int l = a + 32; l < a + 48; l++) {
            trans(DATA, ACIKUC[l - a - 32]);
          }
        } else if (i == 4) {
          for (int l = a + 48; l < a + 64; l++) {
            trans(DATA, KOYUDORT[l - a - 48]);
          }
        } else if (i == 5) {
          for (int l = a + 64; l < a + 80; l++) {
            trans(DATA, ACIKBES[l - a - 64]);
          }
        } else if (i == 6) {
          for (int l = a + 80; l < a + 96; l++) {
            trans(DATA, ACIKALTI[l - a - 80]);
          }
        }

      }
    } else if (kontrolsayi == 5) {
      circle_sira = 5;
      for (int i = d; i > 0 ; i--) {
        // Serial.print("i: "); Serial.println(i);
        if (i == 1) {
          for (int l = a; l < a + 16; l++) {
            //   Serial.print("l: "); Serial.println(l);
            trans(DATA, ACIKBIR[l - a]);
          }
        } else if (i == 2) {
          for (int l = a + 16; l < a + 32; l++) {
            trans(DATA, ACIKIKI[l - a - 16]);
          }
        } else if (i == 3) {
          for (int l = a + 32; l < a + 48; l++) {
            trans(DATA, ACIKUC[l - a - 32]);
          }
        } else if (i == 4) {
          for (int l = a + 48; l < a + 64; l++) {
            trans(DATA, ACIKDORT[l - a - 48]);
          }
        } else if (i == 5) {
          for (int l = a + 64; l < a + 80; l++) {
            trans(DATA, KOYUBES[l - a - 64]);
          }
        } else if (i == 6) {
          for (int l = a + 80; l < a + 96; l++) {
            trans(DATA, ACIKALTI[l - a - 80]);
          }
        }

      }
    } else if (kontrolsayi == 6) {
      circle_sira = 6;
      for (int i = d; i > 0 ; i--) {
        if (i == 1) {
          for (int l = a; l < a + 16; l++) {
            trans(DATA, ACIKBIR[l - a]);
          }
        } else if (i == 2) {
          for (int l = a + 16; l < a + 32; l++) {
            trans(DATA, ACIKIKI[l - a - 16]);
          }
        } else if (i == 3) {
          for (int l = a + 32; l < a + 48; l++) {
            trans(DATA, ACIKUC[l - a - 32]);
          }
        } else if (i == 4) {
          for (int l = a + 48; l < a + 64; l++) {
            trans(DATA, ACIKDORT[l - a - 48]);
          }
        } else if (i == 5) {
          for (int l = a + 64; l < a + 80; l++) {
            trans(DATA, ACIKBES[l - a - 64]);
          }
        } else if (i == 6) {
          for (int l = a + 80; l < a + 96; l++) {
            trans(DATA, KOYUALTI[l - a - 80]);
          }
        }

      }
    }


    a = 0;
    b = 0;
    d = 0;

  }

  if (circle_sira == 0) {
    displayBattery(3300); //içine şimdilik 5 yazdım yani batarya dolu gözüküyor aslında ana cihazın  pili yok
    postTempHum();
    //LCD ekranda nem ve sıcaklık sensörlürünün sürekli görünmesini istiyorsak if (circle_sira == 0) içini tamamen silip sadaece aşağı satırdaki kodu yazağız
    //displayBig(3, stemp, CENTER, 0); displayBig(5, nem, CENTER, 0);
    //kontrol_stemp[2] aradaki nokta 33.45 örneğin

    if ((kontrol_stemp[0] != stemp[0]) || (kontrol_stemp[1] != stemp[1]) || (kontrol_stemp[3] != stemp[3])) //ana cihazdaki gelen sensör değerinin ilk 2 rakamı değişikse alır mesela 29.80 iken 29.98 geldi almaz fakat 30.80 geldi alır
      strcpy(kontrol_stemp, stemp);
    if ((kontrol_nem[0] != nem[0]) || (kontrol_nem[1] != nem[1]) || (kontrol_nem[3] != nem[3]))
      strcpy(kontrol_nem, nem);


    if (HATA_KONROL0 == 1) {
      circleust();
      circlealt();
      displayBig(3, "HATA", CENTER, 0); displayBig(5, "HATA", CENTER, 0);
    } else if (HATA_KONROL0 == 0) {
      if (cTemp < 0) { //-'li değer gelirse lcd ekrana - gönderecek
        cTemp = (-1) * cTemp;
        Serial.println("küçük ");
        circleust_eksi();
        circlealt_eksi();
      } else {
        circleust();
        circlealt();
      }
      displayBig(3, kontrol_stemp, CENTER, 0); displayBig(5, kontrol_nem, CENTER, 0);
    }
    displayBig(3, "S", LEFT, 0);
    eksikontrol1 = 0; //1. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için,
    clclkontrol = 0;
  }
  if (circle_sira == 1) {

    if (clclkontrol == 0) {
      clclearPage(2); // Cihazdaki ana ekran yazısından yuvarlaklara geçtiğinde ana ekran yazısının değişmesi için
    }
    clclkontrol = 1;
    if (eksikontrol1 == 0) {
      if (eksimi1 == 0) {
        circleust();
        circlealt();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);

      } else if (eksimi1 == 1) { //1'se eksi geliyor demektir
        circleust_eksi();
        circlealt_eksi();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      }
      eksikontrol1 = 1;
    }

    displayBattery(int_pil_durumu1); // Eğer cihaz 1'den bir veri geldiyse pil kısmını receive() kısmında int_pil_durumu1'e atadım circle_sıra=1 iken 1. cihazın batarya durumunu gösterecek
    if (HATA_KONROL1 == 1) { //vericideki sensörden veriler hatalı gelirse buraya giriyor
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL1 == 0) { //vericideki sensörden veriler doğru gelirse buraya giriyor
      displayBig(3, gonder_sicaklik_cihaz1, CENTER, 0); displayBig(5, gonder_nem_cihaz1, CENTER, 0);
    }
    //Serial.print("gonder_sicaklik_cihaz1:"); Serial.println(gonder_sicaklik_cihaz1);

    eksikontrol2 = 0; //2. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için,
  } if (circle_sira == 2) {
    // displayBig(3, "2.TEMP", CENTER, 0); displayBig(5, "2.NEM", CENTER, 0);
    clclkontrol = 0;
    if (eksikontrol2 == 0) {
      if (eksimi2 == 0) {
        circleust();
        circlealt();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      } else if (eksimi2 == 1) { //1'se eksi geliyor demektir
        circleust_eksi();
        circlealt_eksi();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      }
      eksikontrol2 = 1;
    }
    displayBattery(int_pil_durumu2); // Eğer cihaz 2'den bir veri geldiyse pil kısmını receive() kısmında int_pil_durumu2'e atadım circle_sıra=2 iken 2. cihazın batarya durumunu gösterecek
    if (HATA_KONROL2 == 1) {
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL2 == 0) {
      displayBig(3, gonder_sicaklik_cihaz2, CENTER, 0); displayBig(5, gonder_nem_cihaz2, CENTER, 0);
    }

    eksikontrol3 = 0; //3. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için,
  } if (circle_sira == 3) {
    clclkontrol = 0;
    if (eksikontrol3 == 0) {
      if (eksimi3 == 0) {
        circleust();
        circlealt();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      } else if (eksimi3 == 1) { //1'se eksi geliyor demektir
        circleust_eksi();
        circlealt_eksi();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      }
      eksikontrol3 = 1;
    }
    displayBattery(int_pil_durumu3);// Eğer cihaz 3'den bir veri geldiyse pil kısmını receive() kısmında int_pil_durumu3'e atadım circle_sıra=3 iken 3. cihazın batarya durumunu gösterecek
    // displayBig(3, "3.TEMP", CENTER, 0); displayBig(5, "3.NEM", CENTER, 0);
    if (HATA_KONROL3 == 1) {
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL3 == 0) {
      displayBig(3, gonder_sicaklik_cihaz3, CENTER, 0); displayBig(5, gonder_nem_cihaz3, CENTER, 0);
    }

    eksikontrol4 = 0; //4. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için,
  } if (circle_sira == 4) {
    clclkontrol = 0;
    if (eksikontrol4 == 0) {
      if (eksimi4 == 0) {
        circleust();
        circlealt();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      } else if (eksimi4 == 1) { //1'se eksi geliyor demektir
        circleust_eksi();
        circlealt_eksi();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      }
      eksikontrol4 = 1;
    }
    // displayBig(3, "4.TEMP", CENTER, 0); displayBig(5, "4.NEM", CENTER, 0);
    eksikontrol5 = 0; //5. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için,
    if (HATA_KONROL4 == 1) {
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL4 == 0) {
      displayBig(3, gonder_sicaklik_cihaz4, CENTER, 0); displayBig(5, gonder_nem_cihaz4, CENTER, 0);
    }
    displayBattery(int_pil_durumu4); // Eğer cihaz 4'den bir veri geldiyse pil kısmını receive() kısmında int_pil_durumu4'e atadım circle_sıra=4 iken 4. cihazın batarya durumunu gösterecek
  } if (circle_sira == 5) {
    clclkontrol = 0;
    if (eksikontrol5 == 0) {
      if (eksimi5 == 0) {
        circleust();
        circlealt();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      } else if (eksimi5 == 1) { //1'se eksi geliyor demektir
        circleust_eksi();
        circlealt_eksi();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      }
      eksikontrol5 = 1;
    }
    displayBattery(int_pil_durumu5);// Eğer cihaz 5'den bir veri geldiyse pil kısmını receive() kısmında int_pil_durumu5'e atadım circle_sıra=5 iken 5. cihazın batarya durumunu gösterecek
    // displayBig(3, "5.TEMP", CENTER, 0); displayBig(5, "5.NEM", CENTER, 0);
    if (HATA_KONROL5 == 1) {
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL5 == 0) {
      displayBig(3, gonder_sicaklik_cihaz5, CENTER, 0); displayBig(5, gonder_nem_cihaz5, CENTER, 0);

    }
    eksikontrol6 = 0; //6. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için
  } if (circle_sira == 6) {
    // displayBig(3, "6.TEMP", CENTER, 0); displayBig(5, "6.NEM", CENTER, 0);
    if (eksikontrol6 == 0) {
      if (eksimi6 == 0) {
        circleust();
        circlealt();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      } else if (eksimi6 == 1) { //1'se eksi geliyor demektir
        circleust_eksi();
        circlealt_eksi();
        circleyuzdeust();
        circleyuzdealt();
        displayBig(5, "N", LEFT, 0);
        displayBig(3, "S", LEFT, 0);
      }
      eksikontrol6 = 1;
    }
    displayBattery(int_pil_durumu6);// Eğer cihaz 6'den bir veri geldiyse pil kısmını receive() kısmında int_pil_durumu6'e atadım circle_sıra=6 iken 6. cihazın batarya durumunu gösterecek
    if (HATA_KONROL6 == 1) {
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL6 == 0) {
      displayBig(3, gonder_sicaklik_cihaz6, CENTER, 0); displayBig(5, gonder_nem_cihaz6, CENTER, 0);
    }
    clclkontrol = 0;
  }


}
void handle_message(WebsocketsClient &client, WebsocketsMessage msg) {
  Serial.println(msg.data());
  StaticJsonDocument<1024> doc1;
  DeserializationError error1 = deserializeJson(doc1, msg.data());
  int cihaz_sayisi, periyot_suresi;

  if (error1) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error1.f_str());
    return;
  } else {
    cihaz_sayisi = doc1["cihaz_sayisi"];
    Serial.print("cihaz_sayisi:"); Serial.println(cihaz_sayisi);
    periyot_suresi = doc1["periyot_suresi"];
    //Serial.print("periyot_suresi:"); Serial.println(periyot_suresi);
  }

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, msg.data());
  JsonObject obj = doc.as<JsonObject>();

  String cihaz_adlari = obj["cihaz_adlari"];
  String mac_alinan = obj["mac"];
  //String mac_alinan = "94:B9:7E:F9:68:84";

  String mac = WiFi.macAddress();
  mac_alinan.replace("-", ":");

  Serial.print("mac_alinan: "); Serial.println(mac_alinan);
  Serial.print("mac: "); Serial.println(mac);


  if (mac_alinan == mac) { //server'dan gelen mac ile esp nin mac ini kontrol ediyor  aynı değilse göndermiyor

    mac_kontrol = 1;
    Serial.println("aynı");

  } else {
    mac_kontrol = 0;
    Serial.println("aynı değil");
  }
  Serial.print("gelen mesajdaki mac_kontrol: "); Serial.println(mac_kontrol);

  int j = cihaz_sayisi; //kaç cihaz olduğu j=(k-1)/3
  //cihaz_adlari.toCharArray(dizi, k + 1);

  for (int k = 1; k <= j; k++) {
    if (k == 1)
      cihaz1 = cihaz_adlari.substring(13 * k - 11, 13 * k - 1);
    if (k == 2)
      cihaz2 = cihaz_adlari.substring(13 * k - 11, 13 * k - 1);
    if (k == 3)
      cihaz3 = cihaz_adlari.substring(13 * k - 11, 13 * k - 1);
    if (k == 4)
      cihaz4 = cihaz_adlari.substring(13 * k - 11, 13 * k - 1);
    if (k == 5)
      cihaz5 = cihaz_adlari.substring(13 * k - 11, 13 * k - 1);
    if (k == 6)
      cihaz6 = cihaz_adlari.substring(13 * k - 11, 13 * k - 1);
  }
  /* Serial.print("cihaz1: "); Serial.println(cihaz1);
    Serial.print("cihaz2: "); Serial.println(cihaz2);
    Serial.print("cihaz3: "); Serial.println(cihaz3);
    Serial.print("cihaz4: "); Serial.println(cihaz4);
    Serial.print("cihaz5: "); Serial.println(cihaz5);
    Serial.print("cihaz6: "); Serial.println(cihaz6);*/

  //1.CİHAZ

  int cihaz1_int = cihaz1.toInt();
  int cihaz1_int1 = cihaz1_int >> 24; //cihaz biz 32 bitlik integer sayısının ilk 8 biti örneğin 100 0010 1110 0101 0111 0110 1000 0110 sayısının 24 bit sağa kaydırılmış hali yani 100 0010 66
  int cihaz1_int2 = cihaz1_int >> 16; // 100 0010 1110 0101  17125
  int cihaz1_int3 = cihaz1_int >> 8; // 4.384.118 100 0010 1110 0101 0111 0110
  int cihaz1_int4 = cihaz1_int ; //100 0010 1110 0101 0111 0110 1000 0110

  cihaz1_eeprom1 = EEPROM.read(0) << 24;
  cihaz1_eeprom2 = EEPROM.read(1) << 16;
  cihaz1_eeprom3 = EEPROM.read(2) << 8;
  cihaz1_eeprom4 = EEPROM.read(3);
  cihaz1_eeprom = cihaz1_eeprom1 | cihaz1_eeprom2 | cihaz1_eeprom3 | cihaz1_eeprom4;

  if (cihaz1_eeprom != cihaz1_int) {
    EEPROM.write(0, cihaz1_int1);
    EEPROM.commit();
    EEPROM.write(1, cihaz1_int2);
    EEPROM.commit();
    EEPROM.write(2, cihaz1_int3);
    EEPROM.commit();
    EEPROM.write(3, cihaz1_int4);
    EEPROM.commit();

    cihaz1_eeprom1 = EEPROM.read(0) << 24;
    cihaz1_eeprom2 = EEPROM.read(1) << 16;
    cihaz1_eeprom3 = EEPROM.read(2) << 8;
    cihaz1_eeprom4 = EEPROM.read(3);
    cihaz1_eeprom = cihaz1_eeprom1 | cihaz1_eeprom2 | cihaz1_eeprom3 | cihaz1_eeprom4;

  }/*else{
    Serial.println("farklı değil");
  }Serial.print("cihaz1_eeprom: "); Serial.println(cihaz1_eeprom);
*/
  //2.CİHAZ
  int cihaz2_int = cihaz2.toInt();
  int cihaz2_int1 = cihaz2_int >> 24;
  int cihaz2_int2 = cihaz2_int >> 16;
  int cihaz2_int3 = cihaz2_int >> 8;
  int cihaz2_int4 = cihaz2_int ;

  cihaz2_eeprom1 = EEPROM.read(4) << 24;
  cihaz2_eeprom2 = EEPROM.read(5) << 16;
  cihaz2_eeprom3 = EEPROM.read(6) << 8;
  cihaz2_eeprom4 = EEPROM.read(7);
  cihaz2_eeprom = cihaz2_eeprom1 | cihaz2_eeprom2 | cihaz2_eeprom3 | cihaz2_eeprom4;


  if (cihaz2_eeprom != cihaz2_int) {
    EEPROM.write(4, cihaz2_int1);
    EEPROM.commit();
    EEPROM.write(5, cihaz2_int2);
    EEPROM.commit();
    EEPROM.write(6, cihaz2_int3);
    EEPROM.commit();
    EEPROM.write(7, cihaz2_int4);
    EEPROM.commit();
    cihaz2_eeprom1 = EEPROM.read(4) << 24;
    cihaz2_eeprom2 = EEPROM.read(5) << 16;
    cihaz2_eeprom3 = EEPROM.read(6) << 8;
    cihaz2_eeprom4 = EEPROM.read(7);
    cihaz2_eeprom = cihaz2_eeprom1 | cihaz2_eeprom2 | cihaz2_eeprom3 | cihaz2_eeprom4;
  }/*else{
    Serial.println("farklı değil");
  }Serial.print("cihaz2_eeprom: "); Serial.println(cihaz2_eeprom);
*/
  //3. CİHAZ
  int cihaz3_int = cihaz3.toInt();
  int cihaz3_int1 = cihaz3_int >> 24;
  int cihaz3_int2 = cihaz3_int >> 16;
  int cihaz3_int3 = cihaz3_int >> 8;
  int cihaz3_int4 = cihaz3_int ;

  cihaz3_eeprom1 = EEPROM.read(8) << 24;
  cihaz3_eeprom2 = EEPROM.read(9) << 16;
  cihaz3_eeprom3 = EEPROM.read(10) << 8;
  cihaz3_eeprom4 = EEPROM.read(11);
  cihaz3_eeprom = cihaz3_eeprom1 | cihaz3_eeprom2 | cihaz3_eeprom3 | cihaz3_eeprom4;


  if (cihaz3_eeprom != cihaz3_int) {
    EEPROM.write(8, cihaz3_int1);
    EEPROM.commit();
    EEPROM.write(9, cihaz3_int2);
    EEPROM.commit();
    EEPROM.write(10, cihaz3_int3);
    EEPROM.commit();
    EEPROM.write(11, cihaz3_int4);
    EEPROM.commit();
    cihaz3_eeprom1 = EEPROM.read(8) << 24;
    cihaz3_eeprom2 = EEPROM.read(9) << 16;
    cihaz3_eeprom3 = EEPROM.read(10) << 8;
    cihaz3_eeprom4 = EEPROM.read(11);
    cihaz3_eeprom = cihaz3_eeprom1 | cihaz3_eeprom2 | cihaz3_eeprom3 | cihaz3_eeprom4;
  }/*else{
    Serial.println("farklı değil");
  }Serial.print("cihaz3_eeprom: "); Serial.println(cihaz3_eeprom);
*/
  //4.CİHAZ
  int cihaz4_int = cihaz4.toInt();
  int cihaz4_int1 = cihaz4_int >> 24;
  int cihaz4_int2 = cihaz4_int >> 16;
  int cihaz4_int3 = cihaz4_int >> 8;
  int cihaz4_int4 = cihaz4_int ;

  cihaz4_eeprom1 = EEPROM.read(12) << 24;
  cihaz4_eeprom2 = EEPROM.read(13) << 16;
  cihaz4_eeprom3 = EEPROM.read(14) << 8;
  cihaz4_eeprom4 = EEPROM.read(15);
  cihaz4_eeprom = cihaz4_eeprom1 | cihaz4_eeprom2 | cihaz4_eeprom3 | cihaz4_eeprom4;


  if (cihaz4_eeprom != cihaz4_int) {
    EEPROM.write(12, cihaz4_int1);
    EEPROM.commit();
    EEPROM.write(13, cihaz4_int2);
    EEPROM.commit();
    EEPROM.write(14, cihaz4_int3);
    EEPROM.commit();
    EEPROM.write(15, cihaz4_int4);
    EEPROM.commit();

    cihaz4_eeprom1 = EEPROM.read(12) << 24;
    cihaz4_eeprom2 = EEPROM.read(13) << 16;
    cihaz4_eeprom3 = EEPROM.read(14) << 8;
    cihaz4_eeprom4 = EEPROM.read(15);
    cihaz4_eeprom = cihaz4_eeprom1 | cihaz4_eeprom2 | cihaz4_eeprom3 | cihaz4_eeprom4;
  }/*else{
    Serial.println("farklı değil");
  } Serial.print("cihaz4_eeprom: "); Serial.println(cihaz4_eeprom);
*/
  //5.CİHAZ
  int cihaz5_int = cihaz5.toInt();
  int cihaz5_int1 = cihaz5_int >> 24;
  int cihaz5_int2 = cihaz5_int >> 16;
  int cihaz5_int3 = cihaz5_int >> 8;
  int cihaz5_int4 = cihaz5_int ;

  cihaz5_eeprom1 = EEPROM.read(16) << 24;
  cihaz5_eeprom2 = EEPROM.read(17) << 16;
  cihaz5_eeprom3 = EEPROM.read(18) << 8;
  cihaz5_eeprom4 = EEPROM.read(19);
  cihaz5_eeprom = cihaz5_eeprom1 | cihaz5_eeprom2 | cihaz5_eeprom3 | cihaz5_eeprom4;


  if (cihaz5_eeprom != cihaz5_int) {
    EEPROM.write(16, cihaz5_int1);
    EEPROM.commit();
    EEPROM.write(17, cihaz5_int2);
    EEPROM.commit();
    EEPROM.write(18, cihaz5_int3);
    EEPROM.commit();
    EEPROM.write(19, cihaz5_int4);
    EEPROM.commit();

    cihaz5_eeprom1 = EEPROM.read(16) << 24;
    cihaz5_eeprom2 = EEPROM.read(17) << 16;
    cihaz5_eeprom3 = EEPROM.read(18) << 8;
    cihaz5_eeprom4 = EEPROM.read(19);
    cihaz5_eeprom = cihaz5_eeprom1 | cihaz5_eeprom2 | cihaz5_eeprom3 | cihaz5_eeprom4;
  }/*else{
    Serial.println("farklı değil");
  }Serial.print("cihaz5_eeprom: "); Serial.println(cihaz5_eeprom);
*/
  //6.CİHAZ
  int cihaz6_int = cihaz6.toInt();
  int cihaz6_int1 = cihaz6_int >> 24; //cihaz biz 32 bitlik integer sayısının ilk 8 biti örneğin 100 0010 1110 0101 0111 0110 1000 0110 sayısının 24 bit sağa kaydırılmış hali yani 100 0010 66
  int cihaz6_int2 = cihaz6_int >> 16; // 100 0010 1110 0101  17125
  int cihaz6_int3 = cihaz6_int >> 8; // 4.384.118 100 0010 1110 0101 0111 0110
  int cihaz6_int4 = cihaz6_int ; //100 0010 1110 0101 0111 0110 1000 0110

  cihaz6_eeprom1 = EEPROM.read(20) << 24;
  cihaz6_eeprom2 = EEPROM.read(21) << 16;
  cihaz6_eeprom3 = EEPROM.read(22) << 8;
  cihaz6_eeprom4 = EEPROM.read(23);
  cihaz6_eeprom = cihaz6_eeprom1 | cihaz6_eeprom2 | cihaz6_eeprom3 | cihaz6_eeprom4;


  if (cihaz6_eeprom != cihaz6_int) {
    EEPROM.write(20, cihaz6_int1);
    EEPROM.commit();
    EEPROM.write(21, cihaz6_int2);
    EEPROM.commit();
    EEPROM.write(22, cihaz6_int3);
    EEPROM.commit();
    EEPROM.write(23, cihaz6_int4);
    EEPROM.commit();

    cihaz6_eeprom1 = EEPROM.read(20) << 24;
    cihaz6_eeprom2 = EEPROM.read(21) << 16;
    cihaz6_eeprom3 = EEPROM.read(22) << 8;
    cihaz6_eeprom4 = EEPROM.read(23);
    cihaz6_eeprom = cihaz6_eeprom1 | cihaz6_eeprom2 | cihaz6_eeprom3 | cihaz6_eeprom4;
  }/*else{
    Serial.println("farklı değil");
  } Serial.print("cihaz6_eeprom: "); Serial.println(cihaz6_eeprom);
*/

  EEPROM.write(24, cihaz_sayisi);
  EEPROM.commit();



}
void postTempHum() {
  unsigned int data[6];
  Wire.beginTransmission(Addr);
  // Send 16-bit command byte
  Wire.write(0x2C);
  Wire.write(0x06);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(300);
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Stop I2C Transmission
  Wire.endTransmission();
  // Request 6 bytes of data
  Wire.requestFrom(Addr, 6);
  // Read 6 bytes of data
  // temp msb, temp lsb, temp crc, hum msb, hum lsb, hum crc
  if (Wire.available() == 6)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  }
  int temp = (data[0] * 256) + data[1];
  cTemp = -45.0 + (175.0 * temp / 65535.0); //65535.0 değil 6553500.0 a böldüm ki birgülden sonra 2 noktayı da alsın

  String mytemp = String(cTemp);
  //Serial.print("mytemp: ");Serial.println(mytemp);
  mytemp.toCharArray(stemp, 6);

  humidity = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;
  //Serial.print("humidity:"); Serial.println(humidity);
  String myhum = String(humidity);
  myhum.toCharArray(nem, 6);



  if (-40 <= cTemp && cTemp <= 100 && 0 <= humidity && humidity <= 100) {
    HATA_KONROL0 = 0;
  } else { //hatalıysa
    HATA_KONROL0 = 1;
  }
  /* Serial.print("cTemp: ");
    Serial.println(cTemp);
    Serial.print("humidity: ");
    Serial.println(humidity);
    int icTemp = (int) cTemp;
    itoa (icTemp, stemp, 10);
    int inem = (int) humidity;
    char bufnem[3];
    itoa (inem, nem, 10);
    const char *firstnem = "NEM:";
    strcpy(bufnem, firstnem);
    float fTemp = -45.0 + (175.0 * temp / 65535.0);
    displaycl();*/
  circleyuzdeust();
  circleyuzdealt();
  displayBig(3, "S", LEFT, 0);
  displayBig(5, "N", LEFT, 0);

}
void postData() {
  //client.onMessage(handle_message); // server dinleniyor
  String mac = WiFi.macAddress();
  Serial.print("mac: "); Serial.println(mac);
  mac.replace(":", "-");
  //Serial.println(mac);
  String json = "{" ;
  json = json + "\"temperature\":" + "\"" + cTemp + "\"" +
         ",\"humidity\":" + "\"" + humidity + "\""  +
         ",\"mac\":" + "\"" + mac + "\"" +
         "}";
  mac.replace("-", ":");


  Serial.print("mac_kontrol post data: "); Serial.println(mac_kontrol);
  if (mac_kontrol == 1) { //server'dan gelen mac ile esp nin mac ini kontrol ediyor  aynı değilse göndermiyor
    client.send(json);
    Serial.println("gönderdi");
  } else {
    Serial.println("göndermedi");
  }

}

int read_cihaz1() {
  string1_eeprom1 = EEPROM.read(25) << 24;
  string1_eeprom2 = EEPROM.read(26) << 16;
  string1_eeprom3 = EEPROM.read(27) << 8;
  string1_eeprom4 = EEPROM.read(28);
  string1_eeprom = string1_eeprom1 | string1_eeprom2 | string1_eeprom3 | string1_eeprom4;
  return string1_eeprom;
}
int read_cihaz2() {
  string2_eeprom1 = EEPROM.read(29) << 24;
  string2_eeprom2 = EEPROM.read(30) << 16;
  string2_eeprom3 = EEPROM.read(31) << 8;
  string2_eeprom4 = EEPROM.read(32);
  string2_eeprom = string2_eeprom1 | string2_eeprom2 | string2_eeprom3 | string2_eeprom4;
  return string2_eeprom;
}
int read_cihaz3() {

  string3_eeprom1 = EEPROM.read(33) << 24;
  string3_eeprom2 = EEPROM.read(34) << 16;
  string3_eeprom3 = EEPROM.read(35) << 8;
  string3_eeprom4 = EEPROM.read(36);
  string3_eeprom = string3_eeprom1 | string3_eeprom2 | string3_eeprom3 | string3_eeprom4;
  return string3_eeprom;
}
int read_cihaz4() {
  string4_eeprom1 = EEPROM.read(37) << 24;
  string4_eeprom2 = EEPROM.read(38) << 16;
  string4_eeprom3 = EEPROM.read(39) << 8;
  string4_eeprom4 = EEPROM.read(40);
  string4_eeprom = string4_eeprom1 | string4_eeprom2 | string4_eeprom3 | string4_eeprom4;
  return string4_eeprom;
}
int read_cihaz5() {

  string5_eeprom1 = EEPROM.read(41) << 24;
  string5_eeprom2 = EEPROM.read(42) << 16;
  string5_eeprom3 = EEPROM.read(43) << 8;
  string5_eeprom4 = EEPROM.read(44);
  string5_eeprom = string5_eeprom1 | string5_eeprom2 | string5_eeprom3 | string5_eeprom4;
  return string5_eeprom;
}
int read_cihaz6() {
  string6_eeprom1 = EEPROM.read(45) << 24;
  string6_eeprom2 = EEPROM.read(46) << 16;
  string6_eeprom3 = EEPROM.read(47) << 8;
  string6_eeprom4 = EEPROM.read(48);
  string6_eeprom = string6_eeprom1 | string6_eeprom2 | string6_eeprom3 | string6_eeprom4;
  return string6_eeprom;
}
void cihaz_sayisi() {
  int c1 = read_cihaz1();
  if (c1 != 0)
    cihaz_sayisi_eeprom = 1;
  int c2 = read_cihaz2();
  if (c2 != 0)
    cihaz_sayisi_eeprom = 2;
  int c3 = read_cihaz3();
  if (c3 != 0)
    cihaz_sayisi_eeprom = 3;
  int c4 = read_cihaz4();
  if (c4 != 0)
    cihaz_sayisi_eeprom = 4;
  int c5 = read_cihaz5();
  if (c5 != 0)
    cihaz_sayisi_eeprom = 5;
  int c6 = read_cihaz6();
  if (c6 != 0)
    cihaz_sayisi_eeprom = 6;

  //cihaz_sayisi_eeprom
  if (cihaz_sayisi_eeprom != 1 && cihaz_sayisi_eeprom != 2 && cihaz_sayisi_eeprom != 3 && cihaz_sayisi_eeprom != 4 && cihaz_sayisi_eeprom != 5 && cihaz_sayisi_eeprom != 6 ) {
    cihaz_sayisi_eeprom = 0;
  }

  EEPROM.write(24, cihaz_sayisi_eeprom);
  EEPROM.commit();


  //  Serial.print(" cihaz_sayisi_eepromdaki: "); Serial.println(cihaz_sayisi_eeprom);
  /*Serial.print(" c6: ");
    Serial.println(c6);*/

}
void write_cihaz1(String string1) {

  int string1_int = string1.toInt();
  //Serial.print("string1_int: "); Serial.println(string1_int);
  int string1_int1 = string1_int >> 24; //cihaz biz 32 bitlik integer sayısının ilk 8 biti örneğin 100 0010 1110 0101 0111 0110 1000 0110 sayısının 24 bit sağa kaydırılmış hali yani 100 0010 66
  int string1_int2 = string1_int >> 16; // 100 0010 1110 0101  17125
  int string1_int3 = string1_int >> 8; // 4.384.118 100 0010 1110 0101 0111 0110
  int string1_int4 = string1_int ; //100 0010 1110 0101 0111 0110 1000 0110

  read_cihaz1();
  if (string1_eeprom != string1_int) {
    EEPROM.write(25, string1_int1);
    EEPROM.commit();
    EEPROM.write(26, string1_int2);
    EEPROM.commit();
    EEPROM.write(27, string1_int3);
    EEPROM.commit();
    EEPROM.write(28, string1_int4);
    EEPROM.commit();

    read_cihaz1();
    /*string1_eeprom1 = EEPROM.read(25) << 24;
      string1_eeprom2 = EEPROM.read(26) << 16;
      string1_eeprom3 = EEPROM.read(27) << 8;
      string1_eeprom4 = EEPROM.read(28);
      string1_eeprom = string1_eeprom1 | string1_eeprom2 | string1_eeprom3 | string1_eeprom4;*/

  }
  //Serial.print("string1_eeprom"); Serial.println(string1_eeprom);
}
void write_cihaz2(String string2) {
  int string2_int = string2.toInt();
  //Serial.print("string2_int: "); Serial.println(string2_int);
  int string2_int1 = string2_int >> 24;
  int string2_int2 = string2_int >> 16;
  int string2_int3 = string2_int >> 8;
  int string2_int4 = string2_int ;

  read_cihaz2();

  if (string2_eeprom != string2_int) {
    EEPROM.write(29, string2_int1);
    EEPROM.commit();
    EEPROM.write(30, string2_int2);
    EEPROM.commit();
    EEPROM.write(31, string2_int3);
    EEPROM.commit();
    EEPROM.write(32, string2_int4);
    EEPROM.commit();
    read_cihaz2();
  }

}
void write_cihaz3(String string3) {
  int string3_int = string3.toInt();
  //Serial.print("string3_int: "); Serial.println(string3_int);
  int string3_int1 = string3_int >> 24;
  int string3_int2 = string3_int >> 16;
  int string3_int3 = string3_int >> 8;
  int string3_int4 = string3_int ;

  read_cihaz3();

  if (string3_eeprom != string3_int) {
    EEPROM.write(33, string3_int1);
    EEPROM.commit();
    EEPROM.write(34, string3_int2);
    EEPROM.commit();
    EEPROM.write(35, string3_int3);
    EEPROM.commit();
    EEPROM.write(36, string3_int4);
    EEPROM.commit();
    read_cihaz3();
  }
}
void write_cihaz4(String string4) {
  int string4_int = string4.toInt();
  //Serial.print("string4_int: "); Serial.println(string4_int);
  int string4_int1 = string4_int >> 24;
  int string4_int2 = string4_int >> 16;
  int string4_int3 = string4_int >> 8;
  int string4_int4 = string4_int ;

  read_cihaz4();

  if (string4_eeprom != string4_int) {
    EEPROM.write(37, string4_int1);
    EEPROM.commit();
    EEPROM.write(38, string4_int2);
    EEPROM.commit();
    EEPROM.write(39, string4_int3);
    EEPROM.commit();
    EEPROM.write(40, string4_int4);
    EEPROM.commit();
    read_cihaz4();
  }
}
void write_cihaz5(String string5) {
  int string5_int = string5.toInt();
  //Serial.print("string5_int: "); Serial.println(string5_int);
  int string5_int1 = string5_int >> 24;
  int string5_int2 = string5_int >> 16;
  int string5_int3 = string5_int >> 8;
  int string5_int4 = string5_int ;

  read_cihaz5();

  if (string5_eeprom != string5_int) {
    EEPROM.write(41, string5_int1);
    EEPROM.commit();
    EEPROM.write(42, string5_int2);
    EEPROM.commit();
    EEPROM.write(43, string5_int3);
    EEPROM.commit();
    EEPROM.write(44, string5_int4);
    EEPROM.commit();
    read_cihaz5();
  }
}
void write_cihaz6(String string6) {
  int string6_int = string6.toInt();
  //Serial.print("string6_int: "); Serial.println(string6_int);
  int string6_int1 = string6_int >> 24;
  int string6_int2 = string6_int >> 16;
  int string6_int3 = string6_int >> 8;
  int string6_int4 = string6_int ;

  read_cihaz6();

  if (string6_eeprom != string6_int) {
    EEPROM.write(45, string6_int1);
    EEPROM.commit();
    EEPROM.write(46, string6_int2);
    EEPROM.commit();
    EEPROM.write(47, string6_int3);
    EEPROM.commit();
    EEPROM.write(48, string6_int4);
    EEPROM.commit();
    read_cihaz6();
  }

}
void receive_() {

  /*Bu kodda alıcıya 25 bit String olarak alıyoruz. Ama sayarken ilk biti 0. diye sayıyoruz
     örneğin: 247244023294+A0000000001B
     ilk 4 biti sıcaklık yani, şu anda +24.72 derece
     sonraki 4 biti nem yani şu andan %44.02'lik bir nem var
     sonraki 4 biti vericiye gelen voltaj mili volt cinsinden 3294 milivolt
     sonraki kod gelen sıcaklık + mı - mi

     örneğin: 94744023294e-A0000000001B
     11. bitinde e olması demek sıcaklığın tek haneli olması demek yani sadece 3bit dikkate alınır
     ilk 3 biti sıcaklık yani, -9.47 derece
     sonraki 4 biti nem yani şu andan %44.02'lik bir nem var
     sonraki 4 biti vericiye gelen voltaj mili volt cinsinden 3294 milivolt
     sonraki kod gelen sıcaklık + mı - mi
  */
  byte tmp;
  rf_kontrol = bSpi3Read(CMT23_FIFO_FLG);
  if (rf_kontrol == 48) {
    //delayMicroseconds(15565); //15535 us'de 21 bit alıyor
    delay(30); //16 ms'de 21 bit alıyor, 20 ms'de 22 bit alıyor,  22 ms'de 23 bit alıyor
    //Serial.print("RSSI: ");
    //Serial.println(bReadRssi(1) - 128);
    tmp = bGetMessage(rx_buf);
    bIntSrcFlagClr();
    vClearFIFO();
    if (tmp) {
      Serial.print(tmp);
      Serial.print(" bytes: ");
      Serial.write(rx_buf, tmp);
      Serial.print("\r\n");
      // Serial.print(" şimdi aldı");
      char buf[tmp + 1];
      memcpy(buf, rx_buf, tmp + 1);

      String mystring = String(buf) ;
      String reset_eeprom = mystring.substring(11, 15);
      String mystring_reset = "0000000000";
      if (reset_eeprom == "sill") {
        write_cihaz1(mystring_reset);
        write_cihaz2(mystring_reset);
        write_cihaz3(mystring_reset);
        write_cihaz4(mystring_reset);
        write_cihaz5(mystring_reset);
        write_cihaz6(mystring_reset);
        clclkontrol = 0;
        cihaz_sayisi_eeprom = 0;
        EEPROM.write(24, cihaz_sayisi_eeprom);
        EEPROM.commit();
      } if (reset_eeprom == "sil1") {
        write_cihaz1(mystring_reset);
      } if (reset_eeprom == "sil2") {
        write_cihaz2(mystring_reset);
      } if (reset_eeprom == "sil3") {
        write_cihaz3(mystring_reset);
      } if (reset_eeprom == "sil4") {
        write_cihaz4(mystring_reset);
      } if (reset_eeprom == "sil5") {
        write_cihaz5(mystring_reset);
      } if (reset_eeprom == "sil6") {
        write_cihaz6(mystring_reset);
        Serial.println("buraya girdi");
      }





      if (rx_buf[13] == 'A' && rx_buf[24] == 'B') {



        String mystring_kontrol = mystring.substring(14, 24);

        int mystring_int = mystring_kontrol.toInt();

        if (string1_eeprom == 0 && mystring_int != string1_eeprom ) {
          write_cihaz1(mystring_kontrol);
          string1_kontrol = read_cihaz1(); //cihaz1'in seri numarası server'dan gelen ile karşılaştıracaksın
        } else if (string2_eeprom == 0 && mystring_int != string2_eeprom && mystring_int != string1_eeprom) {
          write_cihaz2(mystring_kontrol);
          string2_kontrol = read_cihaz2(); //cihaz1'in seri numarası server'dan gelen ile
        } else if (string3_eeprom == 0 && mystring_int != string3_eeprom && mystring_int != string1_eeprom && mystring_int != string2_eeprom) {

          write_cihaz3(mystring_kontrol);
          string3_kontrol = read_cihaz3(); //cihaz1'in seri numarası "string3_kontrol" server'dan gelen ile karşılaştırabilirsin
        } else if (string4_eeprom == 0 && mystring_int != string4_eeprom && mystring_int != string1_eeprom && mystring_int != string2_eeprom && mystring_int != string3_eeprom) {
          write_cihaz4(mystring_kontrol);
          string4_kontrol = read_cihaz4(); //cihaz1'in seri numarası "string3_kontrol" server'dan gelen ile karşılaştırabilirsin
        } else if (string5_eeprom == 0 && mystring_int != string5_eeprom && mystring_int != string1_eeprom && mystring_int != string2_eeprom && mystring_int != string3_eeprom && mystring_int != string4_eeprom) {
          write_cihaz5(mystring_kontrol);
          string5_kontrol = read_cihaz5(); //cihaz1'in seri numarası "string3_kontrol" server'dan gelen ile karşılaştırabilirsin
        } else if (string6_eeprom == 0 && mystring_int != string6_eeprom && mystring_int != string1_eeprom && mystring_int != string2_eeprom && mystring_int != string3_eeprom && mystring_int != string4_eeprom && mystring_int != string5_eeprom) {
          write_cihaz6(mystring_kontrol);
          string6_kontrol = read_cihaz6(); //cihaz1'in seri numarası "string3_kontrol" server'dan gelen ile karşılaştırabilirsin
        }
        /*   Serial.print("mystring_int: ");
            Serial.println(mystring_int);
             Serial.print("string1_eeprom: ");
            Serial.println(string1_eeprom);
            Serial.print("string2_eeprom: ");
            Serial.println(string2_eeprom);*/

        if (mystring_int == string1_eeprom) { //if (mystring_int == string1_eeprom ||)
          if (rx_buf[12] == '+' && rx_buf[11] != 'e') { //2555
            //circleust(); circlealt();
            eksimi1 = 0;
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz1ft = sicaklik_cihaz.toFloat();
            cihaz1ft = cihaz1ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);
            Serial.println("+ geldi ");

            String nem_cihaz = mystring.substring(4, 8);
            cihaz1fh = nem_cihaz.toFloat();
            cihaz1fh = cihaz1fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '+' && rx_buf[11] == 'e') { //255
            //circleust(); circlealt();
            eksimi1 = 0;
            Serial.println("e+ geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz1ft = sicaklik_cihaz.toFloat();
            cihaz1ft = cihaz1ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz1fh = nem_cihaz.toFloat();
            cihaz1fh = cihaz1fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          } if (rx_buf[12] == '-' && rx_buf[11] != 'e' ) { //-2555
            eksimi1 = 1;
            Serial.println("- geldi");
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz1ft = sicaklik_cihaz.toFloat();
            cihaz1ft = cihaz1ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(4, 8);
            cihaz1fh = nem_cihaz.toFloat();
            cihaz1fh = cihaz1fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '-' && rx_buf[11] == 'e') { //-255
            //circleust_eksi(); circlealt_eksi();
            eksimi1 = 1;
            Serial.println("e- geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz1ft = sicaklik_cihaz.toFloat();
            cihaz1ft = cihaz1ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz1fh = nem_cihaz.toFloat();
            cihaz1fh = cihaz1fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          }


          int_pil_durumu1 = pil_durumu.toInt(); //string to int
          Serial.print("int_pil_durumu1: "); Serial.println(int_pil_durumu1);
          // Eğer cihaz 1'den bir veri geldiyse pil kısmını circle_location() kısmında int_pil_durumu1'e atadım. receive() kısmında circle_sıra=1 iken displayBattery(int_pil_durumu1); ile 1.cihazın batarya durumunu gösterecek



          String mytemp1 = String(cihaz1ft);
          mytemp1.toCharArray(sicaklik_cihaz1, 6);
          Serial.print("sicaklik_cihaz1: "); Serial.println(sicaklik_cihaz1);

          String myhum1 = String(cihaz1fh);
          myhum1.toCharArray(nem_cihaz1, 6);
          Serial.print("nem_cihaz1: "); Serial.println(nem_cihaz1);
          if (rx_buf[0] == 'H' && rx_buf[1] == 'A' && rx_buf[2] == 'T' && rx_buf[3] == 'A')
            HATA_KONROL1 = 1;
          else
            HATA_KONROL1 = 0;

          // string_parcala = mystring; //string1i parçalayıp servera ver lcd ekrana göndereceksin,  string1_eeprom seri numarasını server'dan gelen ile karşılaştıracaksın
          Serial.println("gönder gitsin 1");

        } else if (mystring_int == string2_eeprom) {

          if (rx_buf[12] == '+' && rx_buf[11] != 'e') { //2555
            //circleust(); circlealt();
            eksimi2 = 0;
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz2ft = sicaklik_cihaz.toFloat();
            cihaz2ft = cihaz2ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);
            Serial.println("+ geldi ");

            String nem_cihaz = mystring.substring(4, 8);
            cihaz2fh = nem_cihaz.toFloat();
            cihaz2fh = cihaz2fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '+' && rx_buf[11] == 'e') { //255
            //circleust(); circlealt();
            eksimi2 = 0;
            Serial.println("e+ geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz2ft = sicaklik_cihaz.toFloat();
            cihaz2ft = cihaz2ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz2fh = nem_cihaz.toFloat();
            cihaz2fh = cihaz2fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          } if (rx_buf[12] == '-' && rx_buf[11] != 'e' ) { //-2555
            eksimi2 = 1;
            Serial.println("- geldi");
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz2ft = sicaklik_cihaz.toFloat();
            cihaz2ft = cihaz2ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(4, 8);
            cihaz2fh = nem_cihaz.toFloat();
            cihaz2fh = cihaz2fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '-' && rx_buf[11] == 'e') { //-255
            eksimi2 = 1;
            Serial.println("e- geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz2ft = sicaklik_cihaz.toFloat();
            cihaz2ft = cihaz2ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz2fh = nem_cihaz.toFloat();
            cihaz2fh = cihaz2fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          }


          int_pil_durumu2 = pil_durumu.toInt(); //string to int
          Serial.print("int_pil_durumu2: "); Serial.println(int_pil_durumu2);
          // Eğer cihaz 2'den bir veri geldiyse pil kısmını circle_location() kısmında int_pil_durumu2'e atadım. receive() kısmında circle_sıra=2 iken  displayBattery(int_pil_durumu2); ile 2. cihazın batarya durumunu gösterecek


          String mytemp2 = String(cihaz2ft);
          mytemp2.toCharArray(sicaklik_cihaz2, 6);
          Serial.print("sicaklik_cihaz2: "); Serial.println(sicaklik_cihaz2);

          String myhum2 = String(cihaz2fh);
          myhum2.toCharArray(nem_cihaz2, 6);
          Serial.print("nem_cihaz2: "); Serial.println(nem_cihaz2);
          if (rx_buf[0] == 'H' && rx_buf[1] == 'A' && rx_buf[2] == 'T' && rx_buf[3] == 'A')
            HATA_KONROL2 = 1;
          else
            HATA_KONROL2 = 0;

          // string_parcala = mystring; //string1i parçalayıp servera ver lcd ekrana göndereceksin,  string1_eeprom seri numarasını server'dan gelen ile karşılaştıracaksın
          Serial.println("gönder gitsin 2");
        } else if (mystring_int == string3_eeprom) {

          if (rx_buf[12] == '+' && rx_buf[11] != 'e') { //2555
            //circleust(); circlealt();
            eksimi3 = 0;
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz3ft = sicaklik_cihaz.toFloat();
            cihaz3ft = cihaz3ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);
            Serial.println("+ geldi ");

            String nem_cihaz = mystring.substring(4, 8);
            cihaz3fh = nem_cihaz.toFloat();
            cihaz3fh = cihaz3fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '+' && rx_buf[11] == 'e') { //255
            //circleust(); circlealt();
            eksimi3 = 0;
            Serial.println("e+ geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz3ft = sicaklik_cihaz.toFloat();
            cihaz3ft = cihaz3ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz3fh = nem_cihaz.toFloat();
            cihaz3fh = cihaz3fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          } if (rx_buf[12] == '-' && rx_buf[11] != 'e' ) { //-2555
            eksimi3 = 1;
            Serial.println("- geldi");
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz3ft = sicaklik_cihaz.toFloat();
            cihaz3ft = cihaz3ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(4, 8);
            cihaz3fh = nem_cihaz.toFloat();
            cihaz3fh = cihaz3fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '-' && rx_buf[11] == 'e') { //-255
            eksimi3 = 1;
            Serial.println("e- geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz3ft = sicaklik_cihaz.toFloat();
            cihaz3ft = cihaz3ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz3fh = nem_cihaz.toFloat();
            cihaz3fh = cihaz3fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          }

          int_pil_durumu3 = pil_durumu.toInt(); //string to int
          Serial.print("int_pil_durumu3: "); Serial.println(int_pil_durumu3);
          // Eğer cihaz 3'den bir veri geldiyse pil kısmını circle_location() kısmında int_pil_durumu3'e atadım. receive() kısmında circle_sıra=3 iken  displayBattery(int_pil_durumu3); ile 3. cihazın batarya durumunu gösterecek

          String mytemp3 = String(cihaz3ft);
          mytemp3.toCharArray(sicaklik_cihaz3, 6);
          Serial.print("sicaklik_cihaz3: "); Serial.println(sicaklik_cihaz3);

          String myhum3 = String(cihaz3fh);
          myhum3.toCharArray(nem_cihaz3, 6);
          Serial.print("nem_cihaz3: "); Serial.println(nem_cihaz3);
          //string_parcala = mystring; //string1i parçalayıp servera ver lcd ekrana göndereceksin,  string1_eeprom seri numarasını server'dan gelen ile karşılaştıracaksın

          if (rx_buf[0] == 'H' && rx_buf[1] == 'A' && rx_buf[2] == 'T' && rx_buf[3] == 'A')
            HATA_KONROL3 = 1;
          else
            HATA_KONROL3 = 0;
          Serial.println("gönder gitsin 3");

        } else if (mystring_int == string4_eeprom) {

          if (rx_buf[12] == '+' && rx_buf[11] != 'e') { //2555
            //circleust(); circlealt();
            eksimi4 = 0;
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz4ft = sicaklik_cihaz.toFloat();
            cihaz4ft = cihaz4ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);
            Serial.println("+ geldi ");

            String nem_cihaz = mystring.substring(4, 8);
            cihaz4fh = nem_cihaz.toFloat();
            cihaz4fh = cihaz4fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '+' && rx_buf[11] == 'e') { //255
            //circleust(); circlealt();
            eksimi4 = 0;
            Serial.println("e+ geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz4ft = sicaklik_cihaz.toFloat();
            cihaz4ft = cihaz4ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz4fh = nem_cihaz.toFloat();
            cihaz4fh = cihaz4fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          } if (rx_buf[12] == '-' && rx_buf[11] != 'e' ) { //-2555
            eksimi4 = 1;
            Serial.println("- geldi");
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz4ft = sicaklik_cihaz.toFloat();
            cihaz4ft = cihaz4ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(4, 8);
            cihaz4fh = nem_cihaz.toFloat();
            cihaz4fh = cihaz4fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '-' && rx_buf[11] == 'e') { //-255
            eksimi4 = 1;
            Serial.println("e- geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz4ft = sicaklik_cihaz.toFloat();
            cihaz4ft = cihaz4ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz4fh = nem_cihaz.toFloat();
            cihaz4fh = cihaz4fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          }

          int_pil_durumu4 = pil_durumu.toInt(); //string to int
          Serial.print("int_pil_durumu4: "); Serial.println(int_pil_durumu4);

          // Eğer cihaz 4'den bir veri geldiyse pil kısmını circle_location() kısmında int_pil_durumu4'e atadım. receive() kısmında circle_sıra=4 iken  displayBattery(int_pil_durumu4); ile 4. cihazın batarya durumunu gösterecek


          String mytemp4 = String(cihaz4ft);
          mytemp4.toCharArray(sicaklik_cihaz4, 6);
          Serial.print("sicaklik_cihaz4: "); Serial.println(sicaklik_cihaz4);

          String myhum4 = String(cihaz4fh);
          myhum4.toCharArray(nem_cihaz4, 6);
          Serial.print("nem_cihaz4: "); Serial.println(nem_cihaz4);
          if (rx_buf[0] == 'H' && rx_buf[1] == 'A' && rx_buf[2] == 'T' && rx_buf[3] == 'A')
            HATA_KONROL4 = 1;
          else
            HATA_KONROL4 = 0;

          //string_parcala = mystring; //string1i parçalayıp servera ver lcd ekrana göndereceksin,  string1_eeprom seri numarasını server'dan gelen ile karşılaştıracaksın
          Serial.println("gönder gitsin 4");
        } else if (mystring_int == string5_eeprom) {
          if (rx_buf[12] == '+' && rx_buf[11] != 'e') { //2555
            //circleust(); circlealt();
            eksimi5 = 0;
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz5ft = sicaklik_cihaz.toFloat();
            cihaz5ft = cihaz5ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);
            Serial.println("+ geldi ");

            String nem_cihaz = mystring.substring(4, 8);
            cihaz5fh = nem_cihaz.toFloat();
            cihaz5fh = cihaz5fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '+' && rx_buf[11] == 'e') { //255
            //circleust(); circlealt();
            eksimi5 = 0;
            Serial.println("e+ geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz5ft = sicaklik_cihaz.toFloat();
            cihaz5ft = cihaz5ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz5fh = nem_cihaz.toFloat();
            cihaz5fh = cihaz5fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          } if (rx_buf[12] == '-' && rx_buf[11] != 'e' ) { //-2555
            eksimi5 = 1;
            Serial.println("- geldi");
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz5ft = sicaklik_cihaz.toFloat();
            cihaz5ft = cihaz5ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(4, 8);
            cihaz5fh = nem_cihaz.toFloat();
            cihaz5fh = cihaz5fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '-' && rx_buf[11] == 'e') { //-255
            eksimi5 = 1;
            Serial.println("e- geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz5ft = sicaklik_cihaz.toFloat();
            cihaz5ft = cihaz5ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz5fh = nem_cihaz.toFloat();
            cihaz5fh = cihaz5fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          }


          int_pil_durumu5 = pil_durumu.toInt(); //string to int
          Serial.print("int_pil_durumu5: "); Serial.println(int_pil_durumu5);
          // Eğer cihaz 5'den bir veri geldiyse pil kısmını circle_location() kısmında int_pil_durumu5'e atadım. receive() kısmında circle_sıra=5 iken  displayBattery(int_pil_durumu5); ile 5. cihazın batarya durumunu gösterecek


          String mytemp5 = String(cihaz5ft);
          mytemp5.toCharArray(sicaklik_cihaz5, 6);
          Serial.print("sicaklik_cihaz5: "); Serial.println(sicaklik_cihaz5);

          String myhum5 = String(cihaz5fh);
          myhum5.toCharArray(nem_cihaz5, 6);
          Serial.print("nem_cihaz5: "); Serial.println(nem_cihaz5);
          if (rx_buf[0] == 'H' && rx_buf[1] == 'A' && rx_buf[2] == 'T' && rx_buf[3] == 'A')
            HATA_KONROL5 = 1;
          else
            HATA_KONROL5 = 0;
          //string_parcala = mystring; //string1i parçalayıp servera ver lcd ekrana göndereceksin,  string1_eeprom seri numarasını server'dan gelen ile karşılaştıracaksın
          Serial.println("gönder gitsin 5");
        } else if (mystring_int == string6_eeprom) {

          if (rx_buf[12] == '+' && rx_buf[11] != 'e') { //2555
            //circleust(); circlealt();
            eksimi6 = 0;
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz6ft = sicaklik_cihaz.toFloat();
            cihaz6ft = cihaz6ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);
            Serial.println("+ geldi ");

            String nem_cihaz = mystring.substring(4, 8);
            cihaz6fh = nem_cihaz.toFloat();
            cihaz6fh = cihaz6fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '+' && rx_buf[11] == 'e') { //255
            //circleust(); circlealt();
            eksimi6 = 0;
            Serial.println("e+ geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz6ft = sicaklik_cihaz.toFloat();
            cihaz6ft = cihaz6ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz6fh = nem_cihaz.toFloat();
            cihaz6fh = cihaz6fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          } if (rx_buf[12] == '-' && rx_buf[11] != 'e' ) { //-2555
            eksimi6 = 1;
            Serial.println("- geldi");
            String sicaklik_cihaz = mystring.substring(0, 4);
            cihaz6ft = sicaklik_cihaz.toFloat();
            cihaz6ft = cihaz6ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(4, 8);
            cihaz6fh = nem_cihaz.toFloat();
            cihaz6fh = cihaz6fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(8, 12);
          } if (rx_buf[12] == '-' && rx_buf[11] == 'e') { //-255
            eksimi6 = 1;
            Serial.println("e- geldi");
            String sicaklik_cihaz = mystring.substring(0, 3);
            cihaz6ft = sicaklik_cihaz.toFloat();
            cihaz6ft = cihaz6ft / 100;
            //Serial.print(" cihaz1ft: "); Serial.println(cihaz1ft);

            String nem_cihaz = mystring.substring(3, 7);
            cihaz6fh = nem_cihaz.toFloat();
            cihaz6fh = cihaz6fh / 100;
            //Serial.print(" cihaz1fh: "); Serial.println(cihaz1fh);
            pil_durumu = mystring.substring(7, 11);
          }

          int_pil_durumu6 = pil_durumu.toInt(); //string to int
          Serial.print("int_pil_durumu6: "); Serial.println(int_pil_durumu6);
          // Eğer cihaz 6'den bir veri geldiyse pil kısmını circle_location() kısmında int_pil_durumu6'e atadım. receive() kısmında circle_sıra=6 iken  displayBattery(int_pil_durumu6); ile 6. cihazın batarya durumunu gösterecek



          String mytemp6 = String(cihaz6ft);
          mytemp6.toCharArray(sicaklik_cihaz6, 6);
          Serial.print("sicaklik_cihaz6: "); Serial.println(sicaklik_cihaz6);

          String myhum6 = String(cihaz6fh);
          myhum6.toCharArray(nem_cihaz6, 6);
          Serial.print("nem_cihaz6: "); Serial.println(nem_cihaz6);

          if (rx_buf[0] == 'H' && rx_buf[1] == 'A' && rx_buf[2] == 'T' && rx_buf[3] == 'A')
            HATA_KONROL6 = 1;
          else
            HATA_KONROL6 = 0;


          //string_parcala = mystring; //string1i parçalayıp servera ver lcd ekrana göndereceksin,  string1_eeprom seri numarasını server'dan gelen ile karşılaştıracaksın
          Serial.println("gönder gitsin 6");
        }

      }


      if (nem_cihaz1 != 0) {
        for (int i = 0; i < 6; i++) //char to char
        {
          gonder_nem_cihaz1[i] = nem_cihaz1[i];
        }
      }
      if (nem_cihaz2 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_nem_cihaz2[i] = nem_cihaz2[i];
        }
      } if (nem_cihaz3 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_nem_cihaz3[i] = nem_cihaz3[i];
        }
      } if (nem_cihaz4 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_nem_cihaz4[i] = nem_cihaz4[i];
        }
      } if (nem_cihaz5 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_nem_cihaz5[i] = nem_cihaz5[i];
        }
      } if (nem_cihaz6 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_nem_cihaz6[i] = nem_cihaz6[i];
        }
      }


      if (sicaklik_cihaz1 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_sicaklik_cihaz1[i] = sicaklik_cihaz1[i];
        }
      } if (sicaklik_cihaz2 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_sicaklik_cihaz2[i] = sicaklik_cihaz2[i];
        }
      } if (sicaklik_cihaz3 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_sicaklik_cihaz3[i] = sicaklik_cihaz3[i];
        }
      } if (sicaklik_cihaz4 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_sicaklik_cihaz4[i] = sicaklik_cihaz4[i];
        }
      } if (sicaklik_cihaz5 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_sicaklik_cihaz5[i] = sicaklik_cihaz5[i];
        }
      } if (sicaklik_cihaz6 != 0) {
        for (int i = 0; i < 6; i++)
        {
          gonder_sicaklik_cihaz6[i] = sicaklik_cihaz6[i];
        }
      }

    }
  }
}
void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Wire.begin(I2C_SDA, I2C_SCL);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  //set led pin as output
  pinMode(LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);
  pinMode(SCREEN_CS_Pin, OUTPUT);
  pinMode(SCREEN_RST_Pin, OUTPUT);
  pinMode(SCREEN_A0_Pin, OUTPUT);
  pinMode(SCREEN_SCL_Pin, OUTPUT);
  pinMode(SCREEN_SDA_Pin, OUTPUT);
  initScreen();


  WiFiManager wm;
  wm.setAPCallback(configModeCallback);

  if (!wm.autoConnect()) {

    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }


  display(4, "WIFII BAGLANDI", CENTER, 0);
  delay(3000);
  displaycl();

  Serial.println("connected...yeey :)");
  ticker.detach();

  digitalWrite(LED, LOW);
  eskizaman = millis();
  eskizaman2 = millis();
  eskizaman3 = millis();
  eskizaman4 = millis();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  FixedPktLength    = false;
  PayloadLength     = 21;
  vInit();
  vCfgBank(CMTBank, 12);
  vCfgBank(SystemBank, 12);
  vCfgBank(FrequencyBank, 8);
  vCfgBank(DataRateBank, 24);
  vCfgBank(BasebandBank, 29);
  vCfgBank(TXBank, 11);



  vEnableAntSwitch(0);
  vGpioFuncCfg(GPIO1_DCLK + GPIO2_DCLK + GPIO3_INT2); //GPIO Maping

  vIntSrcCfg(INT_FIFO_WBYTE_RX, INT_PKT_DONE);    //IRQ  Maping
  vIntSrcEnable(CRC_PASS_EN | PKT_DONE_EN);
  bIntSrcFlagClr();
  vClearFIFO();
  bGoSleep();
  bGoRx();
  Serial.println("Rx...");
}


void loop() {
  //cihaz_sayisi_eeprom = EEPROM.read(24);
  //Serial.print("cihaz_sayisi_eeprom: "); Serial.println(cihaz_sayisi_eeprom);

  if (kontrol1 == 0) { // burası olmazsa lcd ekranı çok uzun süre sonra gelir
    //postTempHum();

    printLocalTime();
    printWifiStatus();
    kontrol1 = 1;
    circle_location();
    receive_();
    //displayBattery();
    cihaz_sayisi();
  }

  if (!client.available()) {

    simdikizaman = millis();
    simdikizaman2 = millis();
    simdikizaman3 = millis();
    simdikizaman4 = millis();
    receive_();
    if (simdikizaman4 - eskizaman4 >= 60000) { //Eğer vericilerden veri x saniye boyunca gelmezse lcd' de hata yazar
      eskizaman4 = simdikizaman4;
      Serial.print("kontrol_sicaklik_cihaz1: "); Serial.println(kontrol_sicaklik_cihaz1);
      if ((kontrol_sicaklik_cihaz1[0] == sicaklik_cihaz1[0]) && (kontrol_sicaklik_cihaz1[1] == sicaklik_cihaz1[1]) && (kontrol_sicaklik_cihaz1[3] == sicaklik_cihaz1[3]))//
        HATA_KONROL1 = 1;
      else
        HATA_KONROL1 = 0;
      if ((kontrol_sicaklik_cihaz2[0] == sicaklik_cihaz2[0]) && (kontrol_sicaklik_cihaz2[1] == sicaklik_cihaz2[1]) && (kontrol_sicaklik_cihaz2[3] == sicaklik_cihaz2[3]))//
        HATA_KONROL2 = 1;
      else
        HATA_KONROL2 = 0;
      if ((kontrol_sicaklik_cihaz3[0] == sicaklik_cihaz3[0]) && (kontrol_sicaklik_cihaz3[1] == sicaklik_cihaz3[1]) && (kontrol_sicaklik_cihaz3[3] == sicaklik_cihaz3[3]))//
        HATA_KONROL3 = 1;
      else
        HATA_KONROL3 = 0;
      if ((kontrol_sicaklik_cihaz4[0] == sicaklik_cihaz4[0]) && (kontrol_sicaklik_cihaz4[1] == sicaklik_cihaz4[1]) && (kontrol_sicaklik_cihaz4[3] == sicaklik_cihaz4[3]))//
        HATA_KONROL4 = 1;
      else
        HATA_KONROL4 = 0;
      if ((kontrol_sicaklik_cihaz5[0] == sicaklik_cihaz5[0]) && (kontrol_sicaklik_cihaz5[1] == sicaklik_cihaz5[1]) && (kontrol_sicaklik_cihaz5[3] == sicaklik_cihaz5[3]))//
        HATA_KONROL5 = 1;
      else
        HATA_KONROL5 = 0;
      if ((kontrol_sicaklik_cihaz6[0] == sicaklik_cihaz6[0]) && (kontrol_sicaklik_cihaz6[1] == sicaklik_cihaz6[1]) && (kontrol_sicaklik_cihaz6[3] == sicaklik_cihaz6[3]))//
        HATA_KONROL6 = 1;
      else
        HATA_KONROL6 = 0;

      for (int i = 0; i < 6; i++)
      {
        kontrol_sicaklik_cihaz1[i] = sicaklik_cihaz1[i];
        kontrol_sicaklik_cihaz2[i] = sicaklik_cihaz2[i];
        kontrol_sicaklik_cihaz3[i] = sicaklik_cihaz3[i];
        kontrol_sicaklik_cihaz4[i] = sicaklik_cihaz4[i];
        kontrol_sicaklik_cihaz5[i] = sicaklik_cihaz5[i];
        kontrol_sicaklik_cihaz6[i] = sicaklik_cihaz6[i];
      }
    }

    cihaz_sayisi();
    //postTempHum();


    if (simdikizaman - eskizaman >= 60000) //60 saniyede bir wifii çekimi güncelenir
    {
      Serial.print("displaye girdi ");
      // postTempHum();
      //displayBattery();
      printWifiStatus();
      eskizaman = simdikizaman;
    }
    if (simdikizaman2 - eskizaman2 >= 20000) { //Tarih ve zaman her 20 saniyede bir değişir
      printLocalTime();
      eskizaman2 = simdikizaman2;

    }
    if (simdikizaman3 - eskizaman3 >= 6000) { //Circle'ların LCD ekranda gözükme süresi x saniye
      kontrolcircle = kontrolcircle + 1;
      eskizaman3 = simdikizaman3; //Serial.print("kontrolcircle: "); Serial.println(kontrolcircle);
      if (cihaz_sayisi_eeprom != 0)
        kontrolsayi = kontrolcircle % (cihaz_sayisi_eeprom + 1); //Serial.print("kontrolsayi: "); Serial.println(kontrolsayi);


    }


    /* kontrolsayi 0 iken 1.cihazdaki veriler
         kontrolsayi 1 iken 2.cihazdaki veriler
         kontrolsayi 2 iken 3.cihazdaki veriler
         kontrolsayi 3 iken 4.cihazdaki veriler
         kontrolsayi 4 iken 5.cihazdaki veriler
         kontrolsayi 5 iken 6.cihazdaki veriler gönderilecek
    */
    circle_location();


  } else {

    /*  if (client.connect(websocket_server_host, websocket_server_port, "/")) {
      client.onMessage(handle_message); // server dinleniyor
      String mac = WiFi.macAddress();
      mac.replace(":", "-");
      //Serial.println(mac);
      String json = "{" ;
      json = json  +
            "\"sifre\":" + "\"1234\"" +
            ",\"mac\":" + "\"" + mac + "\"" +
            "}";
        client.send(json);

      postData();

      } */
    Serial.println("bağlı");
    receive_();
    //displayBattery();
    while (client.available()) {
      client.poll();


      client.onMessage(handle_message);

      //rf alıcı burada dinleyecek
      //sensör verilerini gönder
      String mac = WiFi.macAddress();
      //Serial.println(mac);
      mac.replace(":", "-");

      String json = "{" ;
      json = json + "\"type\":" + "\"getList\"" +
             ",\"sifre\":" + "\"1234\""  +
             ",\"mac\":" + "\"" + mac + "\"" +
             "}";
      // client.send(json);
      // Serial.println("gönderdi");

      if (kontrol1 == 0) { // burası olmazsa lcd ekranı çok uzun süre sonra gelir

        //postTempHum();

        //displayBattery();
        printLocalTime();
        printWifiStatus();
        kontrol1 = 1;
      }
      simdikizaman = millis();
      simdikizaman2 = millis();
      simdikizaman3 = millis();

      if (simdikizaman - eskizaman >= 60000)
      {
        //60 saniyede bir veya 10 dakikada bir cihaz verilerini burada gönderceeksin
        Serial.print("displaye girdi ");
        postData();
        //postTempHum();
        //displayBattery();
        printWifiStatus();
        eskizaman = simdikizaman;
      }
      if (simdikizaman2 - eskizaman2 >= 20000) {
        eskizaman2 = simdikizaman2;
        //Serial.print("print locale girdi ");
        printLocalTime();
        eskizaman2 = simdikizaman2;

      }
      if (simdikizaman3 - eskizaman3 >= 2000) {
        kontrolcircle = kontrolcircle + 1;
        eskizaman3 = simdikizaman3;

        if (cihaz_sayisi_eeprom != 0)
          kontrolsayi = kontrolcircle % cihaz_sayisi_eeprom;


      }
      circle_location();

    }

    Serial.println("while dan çıktı");
  }


}
