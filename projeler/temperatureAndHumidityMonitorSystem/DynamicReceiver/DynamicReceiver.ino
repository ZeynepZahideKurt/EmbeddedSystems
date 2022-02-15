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

unsigned long simdikizaman, eskizaman, eskizaman2, eskizaman3, eskizaman4;
int circle_kontrol = 0;
int circle_kontrol2 = 5;
int circle_sira = 0;
int LED = LED_BUILTIN;

float cTemp;
float humidity;


char stemp[6], kontrol_stemp[6];
char nem[6], kontrol_nem[6];


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

int int_pil_durumu;
#define cihazsayisi 6

int eeprom_cihaz_sayisi = 0;

typedef struct cihaz_bilgileri {
  float sicaklik;
  float nem;
  int voltaj;
  int seri_numara;
  int eeprom_seri_numara;
  int eksimi;
  unsigned long timeout;
  bool timeout_kontrol;
} cihaz_t;

cihaz_t galeri[cihazsayisi];
bool cihaz_sayisi_kontrol = true;
int eksikontrol = 0;
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
  //  sprintf(arr, "%02u", DEVICEbattery1);
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
  //Serial.print("eeprom_cihaz_sayisi: "); Serial.println(eeprom_cihaz_sayisi);
  //clearPage(2);
  setPage(2);
  if (eeprom_cihaz_sayisi == 0) {
    circle_sira = 0;
    if (circle_kontrol == 0) {
      clclearPage(2);
      circle_kontrol = 1;
    }
    display(2, "            ANA CIHAZ             ",  CENTER, 0);

  } else {
    circle_kontrol = 0;
    //Serial.print("circleye girdi ");//Serial.print("c: ");Serial.println(c);
    int a = (65 - (8 * eeprom_cihaz_sayisi));
    int b = (65 + (8 * eeprom_cihaz_sayisi)) ;
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


  for (int i = 0; i < eeprom_cihaz_sayisi; i++) {


    if (circle_sira == 0) {
      displayBattery(3300); //içine şimdilik 5 yazdım yani batarya dolu gözüküyor aslında ana cihazın  pili yok
      postTempHum();
      if (HATA_KONROL0 == 1) {
        circleust();
        circlealt();
        displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
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
      eksikontrol = 0; //1. sıranın -'si veya S harfinin sürekli loop olarak dönmemesi için,
      clclkontrol = 0;
    }
    else if (circle_sira == (i + 1)) {

      if (clclkontrol == 0) {
        clclearPage(2); // Cihazdaki ana ekran yazısından yuvarlaklara geçtiğinde ana ekran yazısının değişmesi için
      }
      clclkontrol = 1;

      if (eksikontrol == 0) {
        if (galeri[i].eksimi == 0) {
          circleust();
          circlealt();
          circleyuzdeust();
          circleyuzdealt();
          displayBig(5, "N", LEFT, 0);
          displayBig(3, "S", LEFT, 0);

        } else if (galeri[i].eksimi == 1) { //1'se eksi geliyor demektir
          circleust_eksi();
          circlealt_eksi();
          circleyuzdeust();
          circleyuzdealt();
          displayBig(5, "N", LEFT, 0);
          displayBig(3, "S", LEFT, 0);
        }
        eksikontrol = 1;
      }
      String mytemp = String(galeri[i].sicaklik); //float to string,
      char sicaklik_cihaz[6], nem_cihaz[6];
      mytemp.toCharArray(sicaklik_cihaz, 6); //string to char

      String myhum = String(galeri[i].nem);
      myhum.toCharArray(nem_cihaz, 6);

      displayBattery(galeri[i].voltaj); // Eğer cihaz i'den bir veri geldiyse pil kısmını receive() kısmında galeri[i].voltaj'e atadım circle_sıra=i iken i. cihazın batarya durumunu gösterecek
      
    if (galeri[i].timeout==-1 || galeri[i].timeout==0) {
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);//60 saniye boyunca veri geldi mi kontrol eder gelmediyse hata yazdırır
    }else {
            displayBig(3, sicaklik_cihaz, CENTER, 0); displayBig(5, nem_cihaz, CENTER, 0);//60 saniye boyunca veri geldi mi kontrol eder geldiyse sıcaklığı ve nemi yazdırır

     }
    
      
      if (circle_sira == eeprom_cihaz_sayisi + 1) //LCD Ekranda son cihazdan ana ekrana geçerken yuvarlaklar silinsin diye
      clclkontrol = 0;  
     


    }
  }




}
void handle_message(WebsocketsClient &client, WebsocketsMessage msg) {
  Serial.println(msg.data());
  StaticJsonDocument<1024> doc1;
  DeserializationError error1 = deserializeJson(doc1, msg.data());
  int cihaz_sayisi, periyot_suresi;

  /* if (error1) {
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
    //Serial.print("cihaz1: "); Serial.println(cihaz1);
    // Serial.print("cihaz2: "); Serial.println(cihaz2);
    // Serial.print("cihaz3: "); Serial.println(cihaz3);
    // Serial.print("cihaz4: "); Serial.println(cihaz4);
    // Serial.print("cihaz5: "); Serial.println(cihaz5);
    //  Serial.print("cihaz6: "); Serial.println(cihaz6);

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

    }//else{
    //  Serial.println("farklı değil");
    // }Serial.print("cihaz1_eeprom: "); Serial.println(cihaz1_eeprom);

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
    }//else{
    // Serial.println("farklı değil");
    //}Serial.print("cihaz2_eeprom: "); Serial.println(cihaz2_eeprom);

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
    }//else{
    // Serial.println("farklı değil");
    //}Serial.print("cihaz3_eeprom: "); Serial.println(cihaz3_eeprom);

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
    }//else{
    // Serial.println("farklı değil");
    // } Serial.print("cihaz4_eeprom: "); Serial.println(cihaz4_eeprom);

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
    }//else{
     //Serial.println("farklı değil");
    //}Serial.print("cihaz5_eeprom: "); Serial.println(cihaz5_eeprom);

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
    }//else{
    // Serial.println("farklı değil");
    //} Serial.print("cihaz6_eeprom: "); Serial.println(cihaz6_eeprom);


    EEPROM.write(24, cihaz_sayisi);
    EEPROM.commit();
  */


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
  /* //client.onMessage(handle_message); // server dinleniyor
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
    }*/

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
      memcpy(buf, rx_buf, tmp + 1); //unsigned char to char
      String mystring = String(buf) ; //char to string
      // String mystring_kontrol = mystring.substring(14, 24); //string parçalama
      //Serial.print("mystring: "); Serial.println(mystring);
      bool exist = false;
      if (rx_buf[13] == 'A' && rx_buf[24] == 'B') {

        int mystring_int = mystring.substring(14, 24).toInt(); //string parçalama, string to int
        for (int i = 0; i < cihazsayisi; i++) {
          if (galeri[i].eeprom_seri_numara == 0) { // Eğer galerideki eeprom boşsa buraya girer
            for (int j = i - 1; j >= -1 ; j--) { // örneğin 2. galeri boş olarak geldi 1. ve 0. galeriye bakar onlardan farklıysa galeri 2.'ye kaydeder
              if (mystring_int == galeri[j].eeprom_seri_numara ) exist = true; //|| (galeri[0].eeprom_seri_numara == 0)
              else exist = false;

              if (exist == true) break;
            }

            if (exist == false) {
              galeri[i].eeprom_seri_numara = mystring_int;
              Serial.println("buraya giriyor mu");
              EEPROM.write((4 * i), mystring_int >> 24); //cihaz biz 32 bitlik integer sayısının ilk 8 biti örneğin 100 0010 1110 0101 0111 0110 1000 0110 sayısının 24 bit sağa kaydırılmış hali yani 100 0010 66
              EEPROM.commit();
              EEPROM.write((4 * i) + 1, mystring_int >> 16); // 100 0010 1110 0101  17125
              EEPROM.commit();
              EEPROM.write((4 * i) + 2, mystring_int >> 8); // 4.384.118 100 0010 1110 0101 0111 0110
              EEPROM.commit();
              EEPROM.write((4 * i) + 3, mystring_int); //100 0010 1110 0101 0111 0110 1000 0110
              EEPROM.commit();
              cihaz_sayisi_kontrol = true;
            }
          }
          printf(" galeri[%d].eeprom_seri_numara: %.d \r\n", i, galeri[i].eeprom_seri_numara); //Serial.println(galeri[i].nem);

          if (galeri[i].eeprom_seri_numara == mystring_int ) {

            if (rx_buf[11] != 'e') { //2555

              galeri[i].sicaklik = mystring.substring(0, 4).toFloat() / 100 ; // string parçalama, string to float
              galeri[i].nem = mystring.substring(4, 8).toFloat() / 100;
              galeri[i].voltaj = mystring.substring(8, 12).toInt(); //string to int

              if (rx_buf[12] == '+') galeri[i].eksimi = 0;
              else if (rx_buf[12] == '-' ) galeri[i].eksimi = 1;

            } if (rx_buf[11] == 'e') {
              galeri[i].sicaklik = mystring.substring(0, 3).toFloat() / 100 ; // string parçalama, string to float
              galeri[i].nem = mystring.substring(3, 7).toFloat() / 100;
              galeri[i].voltaj = mystring.substring(7, 11).toInt(); //string to int

              if (rx_buf[12] == '+') galeri[i].eksimi = 0;
              else if (rx_buf[12] == '-' ) galeri[i].eksimi = 1;
            }
            printf(" galeri[%d].nem: %.2f \r\n", i, galeri[i].nem); //Serial.println(galeri[i].nem);
            printf(" galeri[%d].sicaklik: %.2f\r\n", i, galeri[i].sicaklik); //Serial.println(galeri[i].sicaklik);
            printf(" galeri[%d].voltaj: %d \r\n", i, galeri[i].voltaj); // Serial.println(galeri[i].voltaj);
            galeri[i].timeout=simdikizaman;        
            
          }


        }
      }
    }
  }
  if (cihaz_sayisi_kontrol == true) {
    eeprom_cihaz_sayisi = 0;
    for (int i = 0; i < cihazsayisi; i++) {
      if (galeri[i].eeprom_seri_numara != 0) {
        eeprom_cihaz_sayisi++;
      }
    }
    cihaz_sayisi_kontrol = false;
    printf(" eeprom_cihaz_sayisi: %d \r\n", eeprom_cihaz_sayisi); //Serial.println(galeri[i].nem);
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
  for (int i = 0; i < cihazsayisi; i++) {

    galeri[i].eeprom_seri_numara = 0;

  }
  for (int i = 0; i < cihazsayisi; i++) {
    galeri[i].timeout=0;
    galeri[i].eeprom_seri_numara = (EEPROM.read(4 * i) << 24 ) | (EEPROM.read((4 * i) + 1) << 16) | (EEPROM.read((4 * i) + 2) << 8) | (EEPROM.read((4 * i) + 3));
    /* if ( temp == 0) {
       break;
      } else {
       galeri[i].eeprom_seri_numara = temp;
      }*/
    printf("galeri[%d].eeprom_seri_numara, %d \r\n", i, galeri[i].eeprom_seri_numara); //Serial.println(galeri[i].eeprom_seri_numara);

  }
}


void loop() {
  //eeprom_cihaz_sayisi = EEPROM.read(24);
  //Serial.print("eeprom_cihaz_sayisi: "); Serial.println(eeprom_cihaz_sayisi);

  if (kontrol1 == 0) { // burası olmazsa lcd ekranı çok uzun süre sonra gelir
    //postTempHum();

    printLocalTime();
    printWifiStatus();
    kontrol1 = 1;
    circle_location();
    receive_();
    //displayBattery();
    //    cihaz_sayisi();
  }

  if (!client.available()) {

    simdikizaman = millis();
    receive_();
    

    //    cihaz_sayisi();
    //postTempHum();

    for(int i = 0; i < eeprom_cihaz_sayisi; i++){
      if (simdikizaman - galeri[i].timeout >= 30000 && galeri[i].timeout!=-1) //60 saniyede bir wifii çekimi güncelenir
    {
      galeri[i].timeout=-1;
      galeri[i].timeout_kontrol=true;
      printf("buyuktur hatalaıdır, %d\r\n", i);//galeri[i].timeout=
    }else{
     galeri[i].timeout_kontrol=false;
    }
   }


   
    if (simdikizaman - eskizaman >= 60000) //60 saniyede bir wifii çekimi güncelenir
    {
      Serial.print("displaye girdi ");
      // postTempHum();
      //displayBattery();
      printWifiStatus();
      eskizaman = simdikizaman;
    }
    if (simdikizaman - eskizaman2 >= 20000) { //Tarih ve zaman her 20 saniyede bir değişir
      printLocalTime();
      eskizaman2 = simdikizaman;

    }
    if (simdikizaman - eskizaman3 >= 6000) { //Circle'ların LCD ekranda gözükme süresi x saniye
      kontrolcircle = kontrolcircle + 1;
      eskizaman3 = simdikizaman; //Serial.print("kontrolcircle: "); Serial.println(kontrolcircle);
      if (eeprom_cihaz_sayisi != 0)
        kontrolsayi = kontrolcircle % (eeprom_cihaz_sayisi + 1); //Serial.print("kontrolsayi: "); Serial.println(kontrolsayi);

      
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
      if (simdikizaman - eskizaman2 >= 20000) {
        eskizaman2 = simdikizaman;
        //Serial.print("print locale girdi ");
        printLocalTime();
        eskizaman2 = simdikizaman;

      }
      if (simdikizaman - eskizaman3 >= 2000) {
        kontrolcircle = kontrolcircle + 1;
        eskizaman3 = simdikizaman;

        if (eeprom_cihaz_sayisi != 0)
          kontrolsayi = kontrolcircle % eeprom_cihaz_sayisi;


      }
      circle_location();

    }

    Serial.println("while dan çıktı");
  }


}
