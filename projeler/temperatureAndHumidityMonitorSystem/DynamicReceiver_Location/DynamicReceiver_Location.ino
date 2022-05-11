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
#define EEPROM_SIZE 25
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

int kontrolcircle = 0;
int kontrolsayi;
int kontrol1 = 0;

uint8_t counter, page, column;
uint16_t screenTempI, screenTempI2;


typedef enum {
  LEFT = 0, CENTER = 1, RIGHT = 2
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


char stemp[6], nem[6], kontrol_stemp[6], kontrol_nem[6]; //Ana cihazın sıcaklık ve nemi
float nemm,sicaklikk;

int HATA_KONROL0 = 0;
int clclkontrol = 0;

byte rf_kontrol;
byte rx_buf[32];


#define cihazsayisi 6

int eeprom_cihaz_sayisi = 0;

typedef struct cihaz_bilgileri {
  float sicaklik;
  float nem;
  int voltaj;
  int eeprom_seri_numara;
  int eksimi;
  unsigned long timeout;
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
void displaycl() {
  for (screenTempI = 0; screenTempI < 8; screenTempI++) {
    setPage(screenTempI);
    setColumn(0);
    for (screenTempI2 = 0; screenTempI2 < 128; screenTempI2++) {
      trans(DATA, cl[(screenTempI * 128) + screenTempI2]);
    }
  }
}
void circle_arti_derece_yuzde() {


  setPage(5); //yüzde işaretinin üst kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_yuzde_ust[i]);
  }


  setPage(6); //yüzde işaretinin alt kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_yuzde_alt[i]);
  }


  setPage(3);//derece üst kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_ust[i]);
  }


  setPage(4); //derece alt kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_alt[i]);
  }
  displayBig(5, "N", LEFT, 0);
  displayBig(3, "S", LEFT, 0);
}
void circle_eksi_derece_yuzde() {

  setPage(5); //yüzde işaretinin üst kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_yuzde_ust[i]);
  }

  setPage(6); //yüzde işaretinin alt kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_yuzde_alt[i]);
  }

  setPage(3); //- ve derecenin üst kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_ust_eksi[i]);
  }
  setPage(4);//- ve derecenin alt kısmı görünür
  setColumn(1);
  for (int i = 0; i < 128; i++) {
    trans(DATA, circle_derece_alt_eksi[i]);
  }

  displayBig(5, "N", LEFT, 0);
  displayBig(3, "S", LEFT, 0);
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
void displayBattery(int int_pil_durumu) {
  //DEVICEbattery1 = '80';
  //Pil durumu hesaplanır
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
  //int_pil_durumu = a;
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
 //Wifii sinyal gücü hesaplanır ve LCD ekranda gösterilir
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
  setPage(2);
  if (eeprom_cihaz_sayisi == 0) { // Eğer eepromda bir cihaz kayıtlı değilse LCD ekranda sadece ANA EKRAN yazısı görünür
    circle_sira = 0;
    if (circle_kontrol == 0) { //Yuvarlaklardan ana ekran yazısına geçerken yuvarlakların silinmesi için
      clclearPage(2); 
      circle_kontrol = 1;
    }
    display(2, "            ANA CIHAZ             ",  CENTER, 0);

  } else { //Eğer eepromda cihaz kayıtlıysa buya girer. Kaç cihaz varsa ona göre yuvarlakların otomatik yeri hesaplanır
    circle_kontrol = 0;
    int a = (65 - (8 * eeprom_cihaz_sayisi)); //LCD Ekranda yuvarlakların başlayacağı yer
    int b = (65 + (8 * eeprom_cihaz_sayisi)) ; //LCD Ekranda yuvarlakların biteceği yer
    int d = (b - a) / 16; //d=eeprom cihaz sayısı sonda sıfırlanacağı için farklı bir yere atadım
    setColumn(a); 
    for (int j = 0; j <=cihazsayisi; j++) { //Toplamda maksimum 6 cihaz olabileceği için 6 
      if (kontrolsayi == 0  ) { //Sıra Ana Cihazdayken bu if'e girer
        display(2, "             ANA CIHAZ             ",  CENTER, 0);
        circle_sira = 0;
        circle_kontrol2 = 0;

      } else if(kontrolsayi==j) { //Sıra vericideki cihazlardayken bu satıra girer     
        circle_sira = kontrolsayi;
        if (circle_kontrol2 == 0) clclearPage(2); //ekrandaki ana ekran yazısından yuvarlaklara geçerken ana ekranın yazısının bir defalığına silinmmesi için               
        circle_kontrol2 = 1;
        for (int i = d; i > 0 ; i--) { //kaç tane cihaz varsa o kadar yuvarlak gözükecek LCD Ekranda
          for (int l = a + ((i - 1) * 16); l < a + (i * 16); l++) {//Yuvarlaklar kaç taneyse ona göre LCD'de ortalıyor ve yerini buluyor
            //mesela 1. circle sırası 1. yuvarlağın içi açık yanıyor diğerlerinin ki koyu yani kapalı oluyor
            if (i == circle_sira) trans(DATA, acik_dizi[ i - 1 ][ l - a - ((i - 1) * 16)]); //acik_dizi[ i - 1 ] dedim çünkü array'de 1. cihazın yeri acik_dizi[ 0 ][], 2. cihazın yeri acik_dizi[ 1 ][]
            else trans(DATA, koyu_dizi[ i - 1 ][ l - a - ((i - 1) * 16)]);
          }
        } //for (int i = d; i > 0 ; i--)
      }

    }
    a = 0;
    b = 0;
    d = 0;

  }


  if (circle_sira == 0) {
    displayBattery(3300); //içine şimdilik 3300 yazdım yani batarya dolu gözüküyor aslında ana cihazın  pili yok
    AnaCihazTempHum(); //Ana cihazsaki sensörden sıcaklık ve nem belgesini alır
    if (HATA_KONROL0 == 1) {
      circle_arti_derece_yuzde(); //LCD ekrana S:, N:, C, % işaretlerini  koyar
      displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);
    } else if (HATA_KONROL0 == 0) {
      if (cTemp < 0) { //-'li değer gelirse lcd ekrana - gönderecek
        cTemp = (-1) * cTemp;
        circle_eksi_derece_yuzde();
      } else {
        circle_arti_derece_yuzde();
      }
      displayBig(3, kontrol_stemp, CENTER, 0); displayBig(5, kontrol_nem, CENTER, 0);
    }
    clclkontrol = 0;
  }

  for (int i = 0; i < eeprom_cihaz_sayisi; i++) {
    if (circle_sira == (i + 1)) {
      if (clclkontrol == 0) clclearPage(2); // Cihazdaki ana ekran yazısından yuvarlaklara geçtiğinde ana ekran yazısının değişmesi için
      clclkontrol = 1;

      if (eksikontrol == 0) { //gelen sıcaklığı kontrol eder ve negatif ise LCD ekrana - işareti koyar
        if (galeri[i].eksimi == 0) circle_arti_derece_yuzde();
        else if (galeri[i].eksimi == 1) circle_eksi_derece_yuzde(); //1'se eksi geliyor demektir
        eksikontrol = 1;
      }
      char sicaklik_cihaz[6], nem_cihaz[6];

      String mytemp = String(galeri[i].sicaklik); //float to string,
      mytemp.toCharArray(sicaklik_cihaz, 6); //string to char

      String myhum = String(galeri[i].nem);
      myhum.toCharArray(nem_cihaz, 6);

      if (galeri[i].timeout == -1 || galeri[i].timeout == 0) {
        displayBattery(1);
        displayBig(3, "  HATA  ", CENTER, 0); displayBig(5, "  HATA  ", CENTER, 0);//60 saniye boyunca veri geldi mi kontrol eder gelmediyse hata yazdırır
      } else {
        displayBig(3, sicaklik_cihaz, CENTER, 0); displayBig(5, nem_cihaz, CENTER, 0);//60 saniye boyunca veri geldi mi kontrol eder geldiyse sıcaklığı ve nemi yazdırır
        displayBattery(galeri[i].voltaj);// Eğer cihaz i'den bir veri geldiyse pil kısmını receive() kısmında galeri[i].voltaj'e atadım circle_sıra=i iken i. cihazın batarya durumunu gösterecek
      }
      if (circle_sira == eeprom_cihaz_sayisi + 1) clclkontrol = 0;//LCD Ekranda son cihazdan ana ekrana geçerken yuvarlaklar silinmesi için
    }
  }




}
void handle_message(WebsocketsClient &client, WebsocketsMessage msg) {
  Serial.println(msg.data());
  StaticJsonDocument<1024> doc1;
  DeserializationError error1 = deserializeJson(doc1, msg.data());
  int cihaz_sayisi, periyot_suresi;
//mac adres-String / int periyot süresi / int cihaz sayısı / string cihaz adları 
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
    EEPROM.write(24, cihaz_sayisi);
    EEPROM.commit();
  */


}
void AnaCihazTempHum() {
  //Ana cihazdaki sıcaklık ve nem ölçülür
  unsigned int data[6];
  Wire.beginTransmission(Addr);
  // Send 16-bit command byte
  Wire.write(0x2C);
  Wire.write(0x06);
  // Stop I2C transmission
  Wire.endTransmission();
  //delay(300);
  Wire.beginTransmission(Addr);
  Wire.endTransmission();
  Wire.requestFrom(Addr, 6);
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
  cTemp = -45.0 + (175.0 * temp / 65535.0);  //float to int
  
  String mytemp = String(cTemp);
  mytemp.toCharArray(stemp, 6); //string to char
  
    if ((kontrol_stemp[0] != stemp[0]) || (kontrol_stemp[1] != stemp[1]) || (kontrol_stemp[3] != stemp[3])){//ana cihazdaki gelen sensör değerinin ilk 2 rakamı değişikse alır mesela 29.80 iken 29.98 geldi almaz fakat 30.80 geldi alır
      if( ( (sicaklikk + 0.5) < cTemp ) || (cTemp < (sicaklikk - 0.5) ) ){ 
      sicaklikk=cTemp;
      strcpy(kontrol_stemp, stemp); // char to char 
      }
    }
      
  
  humidity = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;  
  String myhum = String(humidity);
  myhum.toCharArray(nem, 6);
  
  if ((kontrol_nem[0] != nem[0]) || (kontrol_nem[1] != nem[1]) || (kontrol_nem[3] != nem[3])){
      if( ( (nemm + 0.5) < humidity ) || (humidity < (nemm - 0.5) ) ){ 
      nemm=humidity;
      strcpy(kontrol_nem, nem);
    }
  }
      
//Eğer ana cihazdan gelen sıcaklık -40 ve 100 arasındaysa ve nem 0 ile 100 arasındaysa hata yok demektir.
  if (-40 <= cTemp && cTemp <= 100 && 0 <= humidity && humidity <= 100) {
    HATA_KONROL0 = 0;
  } else { //hatalıysa
    HATA_KONROL0 = 1;
  }
  /* 
    int inem = (int) humidity;//float int
    char bufnem[3];
    itoa (inem, nem, 10);// int to char
    const char *firstnem = "NEM:";
    strcpy(bufnem, firstnem);
    */

}
void postData() {
  //mac adres- sıcaklık - nem - pil durumu- seri numarası
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
void cihaz_sil() {
  for (int i = 0; i < 6; i++) {
    int mystring_int = 0000000000;
    EEPROM.write((4 * i), mystring_int >> 24); //cihaz biz 32 bitlik integer sayısının ilk 8 biti örneğin 100 0010 1110 0101 0111 0110 1000 0110 sayısının 24 bit sağa kaydırılmış hali yani 100 0010 66
    EEPROM.commit();
    EEPROM.write((4 * i) + 1, mystring_int >> 16); // 100 0010 1110 0101  17125
    EEPROM.commit();
    EEPROM.write((4 * i) + 2, mystring_int >> 8); // 4.384.118 100 0010 1110 0101 0111 0110
    EEPROM.commit();
    EEPROM.write((4 * i) + 3, mystring_int); //100 0010 1110 0101 0111 0110 1000 0110
    EEPROM.commit();

    galeri[i].eeprom_seri_numara = EEPROM.read(4 * i) << 24 | EEPROM.read((4 * i) + 1) << 16 | EEPROM.read((4 * i) + 2) << 8 | EEPROM.read((4 * i) + 3);
    Serial.println(galeri[i].eeprom_seri_numara);
    printf(" burada galeri[%d].eeprom_seri_numara: %.d \r\n", i, galeri[i].eeprom_seri_numara); //Serial.println(galeri[i].nem);
  }
}

void receive_() {

  /*Bu kodda alıcıya 25 bit String olarak alıyoruz. Ama sayarken ilk biti 0. diye sayıyoruz
     örneğin: +247244023294A0000000001B
     ilk 5 biti sıcaklık yani, şu anda +24.72 derece
     sonraki 4 biti nem yani şu andan %44.02'lik bir nem var
     sonraki 4 biti vericiye gelen voltaj mili volt cinsinden 3294 milivolt

     örneğin: -094744023294A0000000001B
     ilk 5 biti sıcaklık yani, -09.47 derece
     sonraki 4 biti nem yani şu andan %44.02'lik bir nem var
     sonraki 4 biti vericiye gelen voltaj mili volt cinsinden 3294 milivolt
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
        String reset_eeprom = mystring.substring(12, 13);

        if (reset_eeprom == "s") { // Burası serverden silme işlemi yapıldıktan sonra silinecek
          Serial.println("reset_eeprom");
          cihaz_sil();
          eeprom_cihaz_sayisi = 0;
        } else {
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

            if (galeri[i].eeprom_seri_numara == mystring_int ) {// vericiden gelen seri numarası galerideki seri numarasıyla uyuşursa bu ifin içine girer
              //sıcaklık,nem,voltaj parçalanır 
              if (rx_buf[0] == '+') galeri[i].eksimi = 0;
              else if (rx_buf[0] == '-' ) galeri[i].eksimi = 1;
              
              galeri[i].sicaklik = mystring.substring(1, 5).toFloat() / 100 ; // string parçalama, string to float
              galeri[i].nem = mystring.substring(5, 9).toFloat() / 100;
              galeri[i].voltaj = mystring.substring(9, 13).toInt(); //string to int

              printf(" galeri[%d].nem: %.2f \r\n", i, galeri[i].nem); //Serial.println(galeri[i].nem);
              printf(" galeri[%d].sicaklik: %.2f\r\n", i, galeri[i].sicaklik); //Serial.println(galeri[i].sicaklik);
              printf(" galeri[%d].voltaj: %d \r\n", i, galeri[i].voltaj); // Serial.println(galeri[i].voltaj);
              printf(" galeri[%d].eeprom_seri_numara: %.d \r\n", i, galeri[i].eeprom_seri_numara); //Serial.println(galeri[i].nem);

              galeri[i].timeout = simdikizaman; // Galeri[i]'den gelen datanın zaman bilgi tutulur ve loop fonksiyonunda ne kadar süre arayla gelmiş kontrol edilir.
            }

          }//for(i++) döngüsü kapanış
        }
      } // A ve B bitiş



    }
  }
  if (cihaz_sayisi_kontrol == true) { //Eğer yeni bir cihaz gelirse cihaz sayısı arttırılır
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

  Serial.println("connected...");
  ticker.detach();

  digitalWrite(LED, LOW);
  eskizaman = millis();
  eskizaman2 = millis();
  eskizaman3 = millis();
  eskizaman4 = millis();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //Tarih ve zaman alınabilmesi için gerekli ayarlar yapılır


  FixedPktLength    = false; // RF ayarları yapılır.
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
    galeri[i].timeout = 0;
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

  if (kontrol1 == 0) { // burası olmazsa lcd ekranı çok uzun süre sonra gelir
    printLocalTime(); // Tarih ve zamanın LCD ekranda gösterilmesini sağlar
    printWifiStatus(); // wifii çekim gücünün LCD ekranda gösterilmesini sağlar
    circle_location(); // Eeprom'da kaç tane cihaz varsa LCD ekranda gösterilmesi sağlanır
    receive_();// Etraftaki verici cihazlar dinlenir
    kontrol1 = 1;
  }

  if (!client.available()) {
    //Eğer internet koparsa bu fonksiyonun içine girer
    simdikizaman = millis();
    receive_(); // Etraftaki verici cihazlar dinlenir


    for (int i = 0; i < eeprom_cihaz_sayisi; i++) {
      if (simdikizaman - galeri[i].timeout >= 60000 && galeri[i].timeout != -1) //cihazlardan veri x saniye boyunca gelmezse cihazda hata yazar
        galeri[i].timeout = -1;
    }



    if (simdikizaman - eskizaman >= 60000) //60 saniyede bir wifii çekimi güncelenir
    {
      Serial.print("displaye girdi ");
      printWifiStatus();
      eskizaman = simdikizaman;
    }
    if (simdikizaman - eskizaman2 >= 20000) { //Tarih ve zaman her 20 saniyede bir değişir
      printLocalTime();
      eskizaman2 = simdikizaman;

    }
    if (simdikizaman - eskizaman3 >= 3000) { //Circle'ların LCD ekranda gözükme süresi x saniye
      kontrolcircle = kontrolcircle + 1;
      eskizaman3 = simdikizaman; 
      if (eeprom_cihaz_sayisi != 0) //Eğer verici bir cihaz eeproma kayıtlıysa aşağıdaki işlemi yapar
        kontrolsayi = kontrolcircle % (eeprom_cihaz_sayisi + 1); //Örneğin 2 tane kayıtlı cihaz var kontrol sayı sırayla 0,1,2 olur. Kontrol sayıya göre LCD Ekrandaki yuvarlağın sırası belirtilir
      eksikontrol = 0; //Eğer bu satır olmazsa LCD Ekrandaki S:, N:, %.. Sürekli yanıp söner
      

    }

    circle_location(); //LCD Ekranda yuvarlaklar gösterilir
    /* kontrolsayi 0 iken 1.cihazdaki veriler
         kontrolsayi 1 iken 2.cihazdaki veriler
         kontrolsayi 2 iken 3.cihazdaki veriler
         kontrolsayi 3 iken 4.cihazdaki veriler
         kontrolsayi 4 iken 5.cihazdaki veriler
         kontrolsayi 5 iken 6.cihazdaki veriler gönderilecek
    */



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

        //AnaCihazTempHum();

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
        //AnaCihazTempHum();
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
