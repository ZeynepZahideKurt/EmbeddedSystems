// LED will blink when in config mode
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

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Ticker.h>
#include "time.h"
#include "lcd.h"
Ticker ticker;

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include <string.h>

#include <Wire.h>
#define Addr 0x44

#define I2C_SDA 21
#define I2C_SCL 22

#define SCREEN_CS_Pin 18
#define SCREEN_RST_Pin 5
#define SCREEN_A0_Pin 17
#define SCREEN_SCL_Pin 16
#define  SCREEN_SDA_Pin 4
#define NOP __asm__ __volatile__ ("nop\n\t")

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
int c=7;
int kontrolcircle = 0;
int kontrolsayi;
int kontrol1 = 0;

uint8_t counter, page, column;
uint16_t screenTempI, screenTempI2;
char arr[20];
char displayArr[40];

typedef enum {
  LEFT = 0, CENTER = 1, RIGHT = 2, RIGHT_UP = 3, CENTER_UP = 4, CENTER_LEFT = 5
} LOCATION;
typedef enum {
  INSTRUCTION = 0, DATA = 1
} TRANSMODE;


#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

unsigned long simdikizaman, eskizaman, simdikizaman2, eskizaman2, simdikizaman3, eskizaman3;

int LED = LED_BUILTIN;

float cTemp;
float humidity;

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
  Krish_hour = timeinfo.tm_hour + 1;
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
  Serial.println("giriyor");
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
  display(4, "ENELSIS CIHAZINA BAGLANINIZ!", CENTER, 0);
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
void circle1() {
  //clearPage(2);
  setPage(2);

  //Serial.print("circleye girdi ");Serial.print("c: ");Serial.println(c);
  int a = (65 - (8 * c));
  int b = (65 + (8 * c)) ;
  int d = (b - a) / 16;
  setColumn(a);
  //Serial.print("a: "); Serial.println(a);Serial.print("b: ");Serial.println(b);Serial.print("d: ");Serial.println(d);
    
  if (kontrolsayi == 0  ) {
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
  } else if (kontrolsayi == 1) {
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
  } else if (kontrolsayi == 2) {
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
  } else if (kontrolsayi == 3) {
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
  } else if (kontrolsayi == 4) {
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
  } else if (kontrolsayi == 5) {
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
  //displayBattery();
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
void getBatteryValues(){
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
void displayBattery() {
 // getBatteryValues();
char DEVICEbattery1;
  DEVICEbattery1= '80';
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
  if ( DEVICEbattery1 >= 83) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if ( DEVICEbattery1 >= 67) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if ( DEVICEbattery1 >= 50) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if ( DEVICEbattery1 >= 33) {
    trans(DATA, 0b01011101);
  } else {
    trans(DATA, 0b01000001);
  }
  if ( DEVICEbattery1 >= 17) {
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
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
 // long rssi = WiFi.RSSI();
  long rssi= -40;
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  setPage(1);
  setColumn(110);

  
  if (rssi > -50 &  rssi < 0 ){ //çok iyi
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
    
  }else if (rssi > -90 &  rssi < -80) { //çok kötü
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


void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);

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
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

void loop() {
  if (Serial.available() > 0) {
    clclearPage(2);
    inByte = Serial.read();
    Serial.print("I received: ");  Serial.println(inByte);
    
    if (inByte == '1') {
      c = 1;
      //Serial.print("c: "); Serial.println(c);
    } if (inByte == '2') {
      c = 2;
      Serial.print("c: ");
      Serial.println(c);
    } if (inByte == '3') {
      c = 3;
      Serial.print("c: ");
      Serial.println(c);
    } if (inByte == '4') {
      c = 4;
      Serial.print("c: ");
      Serial.println(c);
    } if (inByte == '5') {
      c = 5;
      Serial.print("c: ");
      Serial.println(c);
    } if (inByte == '6') {
      c = 6;
      Serial.print("c: ");
      Serial.println(c);
    }
    if (inByte == '7') { //verici cihaz yoksa bir tek alıcı cihaz varsa
      c = 7;
      Serial.print("c: ");
      Serial.println(c);
      //Serial.print("c: "); Serial.println(c);
    }

  }
  if (kontrol1 == 0) { // burası olmazsa lcd ekranı çok uzun süre sonra gelir
    postDataToServer();
    displayBattery();
    printLocalTime();
    printWifiStatus();
    kontrol1 = 1;
  }

  simdikizaman = millis();
  simdikizaman2 = millis();
  simdikizaman3 = millis();
  if (simdikizaman - eskizaman >= 60000)
  {
    Serial.print("displaye girdi ");
    postDataToServer();
    displayBattery();
    printWifiStatus();
    eskizaman = simdikizaman;
  }
  if (simdikizaman2 - eskizaman2 >= 20000)
  {
    eskizaman2 = simdikizaman2;
    Serial.print("print locale girdi ");
    printLocalTime();
    eskizaman2 = simdikizaman2;
    
  }
  if (simdikizaman3 - eskizaman3 >= 2000)
  {
    if(c!=7){
    kontrolcircle = kontrolcircle + 1;
    eskizaman3 = simdikizaman3;
    
    //Serial.println(kontrolcircle);
    kontrolsayi = kontrolcircle % c;
    Serial.println(kontrolsayi);

    } else{
      display(3, "Ana Cihaz", CENTER, 1);
    }
  }



  /*if (simdikizaman - eskizaman >= 2000)
  {
    kontrolcircle = kontrolcircle + 1;
    eskizaman = simdikizaman;
    
   // Serial.println(kontrolcircle);
    kontrolsayi = kontrolcircle % c;
    //Serial.println(kontrolsayi);


  }*/
  
circle1();

    
  
  
  
  /* kontrolsayi 0 iken 1.cihazdaki veriler
     kontrolsayi 1 iken 2.cihazdaki veriler
     kontrolsayi 2 iken 3.cihazdaki veriler
     kontrolsayi 3 iken 4.cihazdaki veriler
     kontrolsayi 4 iken 5.cihazdaki veriler
     kontrolsayi 5 iken 6.cihazdaki veriler gönderilecek
  */


}

void postDataToServer() {
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
  cTemp = -45.0 + (175.0 * temp / 65535.0);
  humidity = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;
  Serial.println("cTemp: ");
  Serial.println(cTemp);
  Serial.println("humidity: ");
  Serial.println(humidity);


  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin("https://www.enelsis.com:3000/newdata");
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    // Add values in the document
    //
    doc["temperature"] = cTemp;
    doc["humidity"] = humidity;
    doc["mac"] = "123kk";

    // Add an array.
    //
    JsonArray data = doc.createNestedArray("data");
    data.add(48.756080);
    data.add(2.302038);

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {

      String response = http.getString();

      Serial.println(httpResponseCode);
      Serial.println(response);

    }
    else {
      Serial.println("Error occurred while sending HTTP POST: %s\n");
      //Serial.println("Error occurred while sending HTTP POST: %s\n", httpClient.errorToString(statusCode).c_str());

    }

  }
  int icTemp = (int) cTemp;
  char stemp[3];
  //dtostrf(icTemp, 2, 2, stemp);
  itoa (icTemp, stemp, 10);
  // char buf[2];
  // const char *first = "TEMP:";
  // strcpy(buf, first);
  // strcat(buf, stemp);
  int inem = (int) humidity;
  char nem[3];
  //  char cnem[5];
  itoa (inem, nem, 10);
  // dtostrf(humidity, 5, 2, cnem);
  char bufnem[3];
  const char *firstnem = "NEM:";
  strcpy(bufnem, firstnem);
  //strcat(bufnem, nem);
  //display(1, "TEMP:", LEFT, 1);
  //  display(1, cnem, CENTER, 1);
  //display(1, buf, LEFT, 0);
  //display(1, bufnem, CENTER_UP, 0);
  //display(6, "ENELSIS", RIGHT, 0);
  displaycl();




  circleust();
  circlealt();
  circleyuzdeust();
  circleyuzdealt();


  //displayBig(3,"0123456789", CENTER,0);
  displayBig(3, "S", LEFT, 0);
  displayBig(3, stemp, CENTER, 0);
  displayBig(5, "N", LEFT, 0);
  displayBig(5, nem, CENTER_LEFT, 0);


}
