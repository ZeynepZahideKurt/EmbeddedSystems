/*Bu çalışmada kart: ESP32 WROVER MODULE
               Partition Scheme: HUGE APP:3 MB NO OTA/1 MB SPIFFS KULLANILMIŞTIR.

*/
#include <Arduino.h>
#include "ESP32QRCodeReader.h"
#include "esp_camera.h"
#include <ArduinoWebsockets.h>
#include <HTTPClient.h>
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
#include <WiFi.h>
#include <EEPROM.h>
#define EEPROM_SIZE 72

#include "camera_pins.h"
//unsigned long LastMeasureTime = 0;
//unsigned long Interval = 5000; //Normally set from config stored in EEPROM

//void startCameraServer();
String ssid;
String pass;
String Payload;
char password[35];
char ssid1[35];
int QRkontrol = 1;
int s;
int p;
String mail;

const int gpioPIR = 13;
const int buttonPin = 2;
char deneme[7];
char deneme2[7];
const char* websocket_server_host = "192.168.1.239";
//const char* websocket_server_host = "89.19.23.98";
const uint16_t websocket_server_port = 8888;
const uint16_t websocket_server_port1 = 8887;
const uint16_t websocket_server_port2 = 8886;

int kontrol = 0;
int kontrol2 = 0;
int kontrol3 = 0;
int kontrol4 = 1;
int kontrol5 = 1;
int kontrol6;
int kontrol7 = 1;
int loopkontrol = 0;
int serverkontrol;
int PIRkontrol = 0;
int registerkontrol = 0;

using namespace websockets;
WebsocketsServer socket_server;
WebsocketsClient client;


void handle_message(WebsocketsClient &client, WebsocketsMessage msg) {
  Serial.println(msg.data());
  if (msg.data() == "bağlan") {
    serverkontrol = 1;
    Serial.println("bağlan komutu geldi");
  } else if (msg.data() == "kop") {
    serverkontrol = 2;
    Serial.println("kop komutu geldi");
  } else if (msg.data() == "registerTrue") {
    Serial.println("registerTrue girdi");
    registerkontrol = 1;
  } else if (msg.data() == "registerFalse") {
    Serial.println("registerFalse girdi");
    registerkontrol = 1;
  }


}

void onQrCodeTask(void *pvParameters)
{
  struct QRCodeData qrCodeData;
  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        Serial.print("Payload: ");
        Serial.println((const char *)qrCodeData.payload);
        String Payload = (const char *)qrCodeData.payload;

        Serial.println(Payload);

        int l = Payload.length();
        Serial.print("l uzunluk :");
        Serial.println(l);

        int  n = Payload.indexOf("-_-");
        Serial.print("n:");
        Serial.println(n);

        int  c = Payload.indexOf("_-_");
        Serial.print("c:");
        Serial.println(c);

        String ssid = Payload.substring(0, n);
        Serial.print("ssid parçalanmış hali: ");
        Serial.println(ssid);

        String pass = Payload.substring(n + 3, c);
        Serial.print("pass: ");
        Serial.println(pass);

        String mail = Payload.substring(c + 3, l);
        Serial.print("mail: ");
        Serial.println(mail);

        int z = c - (n + 2);
        Serial.print("z:");
        Serial.println(z);

        int e = l - (n + 2);
        Serial.print("z:");
        Serial.println(z);

        ssid.toCharArray(ssid1, n + 1);

        pass.toCharArray(password, z);
        int s = strlen(ssid1);
        int p = strlen(password);
        Serial.print("ssid1 uzunluk: ");
        Serial.println(s);
        Serial.println(ssid1);

        Serial.print("password uzunluk: ");
        Serial.println(p);
        Serial.println(password);

        //WiFi.begin(ssid1, password);


        
        WiFi.begin(ssid1, password);

        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
          int reading = digitalRead(buttonPin);
          if (reading == LOW)
          {
            break;
          }
        }


        Serial.print("QR KODDAKİ Camera Ready! Use 'http://");
        Serial.print(WiFi.localIP());
        Serial.println("' to connect");
        String mac = WiFi.macAddress();
        mac.replace(":", "-");
        //Serial.println(mac);

        while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
          delay(500);
          Serial.print("?");
        }
        Serial.println("Websocket Connected cam!");
        client.onMessage(handle_message);
        unsigned long simdiki_zaman = millis();
        unsigned long gelecek_zaman = simdiki_zaman + 1000;
        while (client.available()) {
          simdiki_zaman = millis();
          client.poll();
          String json = "{" ;
          json = json + "\"type\":" + "\"register\"" +
                 ",\"sifre\":" + "\"1234\"" +
                 ",\"mac\":" + "\"" + mac + "\"" + ",\"mail\":" + "\"" + mail + "\"" +
                 "}";
          if (simdiki_zaman >= gelecek_zaman) {
            client.send(json);
            gelecek_zaman = simdiki_zaman + 1000;
          }


          if (registerkontrol == 1) {
             Serial.println(s);
          for (int i = 0; i < s; i++) {
           EEPROM.write(i, ssid1[i]);
           Serial.println(ssid1[i]);
          }
          for (int i = s; i < 35; i++) {
           EEPROM.write(i, '-');
           EEPROM.commit();
          }
          int o = 3;
          EEPROM.write(35, o);
          EEPROM.commit();

          for (int a = 36; a < p + 36; a++) {
           EEPROM.write(a, password[a - 36]);
           Serial.println(password[a - 36]);
          }
          for (int i = 36 + p; i < 71; i++) {
           EEPROM.write(i, '-');
           EEPROM.commit();
          }
          Serial.print("eeproma yazıldıııııı:");
          Serial.println(ssid1);
          Serial.println(password);

          for (int i = 0; i < 6; i++) {
           deneme[i] = EEPROM.read(i);
          }
          for (int i = 0; i < 6; i++) {
           deneme2[i] = 'a';
          }
          String deneme3 = String(deneme);
          String deneme4 = String(deneme2);
            Serial.println("break girdi");
            
        if (deneme3 != deneme4) {
          ESP.restart();
          }
            
            break;          
          }


        }
        /* while (registerkontrol==0) {
          Serial.println("SDFGDdi");
          }*/

      }
      else {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(gpioPIR, INPUT);
  EEPROM.begin(EEPROM_SIZE);
  pinMode(buttonPin, INPUT);
  //pinMode(33, OUTPUT);

  for (int i = 0; i < 6; i++) {
    deneme[i] = EEPROM.read(i);
  }
  for (int i = 0; i < 6; i++) {
    deneme2[i] = 'a';
  }
  String deneme3 = String(deneme);
  String deneme4 = String(deneme2);


  if (deneme3 != deneme4) {
    for (int j = 0; j < 34; j++) {

      Serial.print(j);
      Serial.print(": ");
      ssid1[j] = EEPROM.read(j);
      Serial.println(ssid1[j]);
    }
    int o = EEPROM.read(35);

    for (int i = 36; i < 71; i++) {

      Serial.print(i);
      Serial.print(":  ");
      password[i - 36] = EEPROM.read(i);
      Serial.println(  password[i - 36]);

    }
    String ad = String( ssid1);
    String sifre = String( password);
    Serial.print("eeproma yazılan:");
    Serial.println(ad);
    Serial.println(sifre);
    int s2 = ad.length();
    int p2 = sifre.length();
    Serial.print("s2: ");
    Serial.println(s2);
    Serial.print("p2: ");
    Serial.println(p2);

    int  n = ad.indexOf("---");
    Serial.print("n:");
    Serial.println(n);

    String ssid3 = ad.substring(0, n);
    Serial.print("ssid3 parçalanmış hali: ");
    Serial.println(ssid3);

    int  m = sifre.indexOf("---");
    Serial.print("m:");
    Serial.println(m);

    String password3 = sifre.substring(0, m);
    Serial.print("password3: ");
    Serial.println(password3);

    char ssid4[n + 1];
    char password4[m + 1];
    ssid3.toCharArray(ssid4, n + 1);
    password3.toCharArray(password4, m + 1);

    int nn = strlen(ssid4);
    int mm = strlen(password4);

    Serial.print("nn: ");
    Serial.println(nn);
    Serial.print("mm: ");
    Serial.println(mm);
    Serial.print("password4: ");
    Serial.println(password4);

    Serial.print("ssid4: ");
    Serial.println(ssid4);


    Serial.setDebugOutput(true);
    Serial.println();
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 10000000;
    config.pixel_format = PIXFORMAT_JPEG;
    //init with high specs to pre-allocate larger buffers
    if (psramFound()) {
      config.frame_size = FRAMESIZE_VGA;
      config.jpeg_quality = 40;
      config.fb_count = 2;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.jpeg_quality = 12;
      config.fb_count = 1;
    }
    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }
    WiFi.begin(ssid4, password4);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      int reading = digitalRead(buttonPin);
      if (reading == LOW)
      {
        break;
      }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");
    loopkontrol = 1;

  }
  else {

    reader.setup();
    Serial.println("Setup QRCode Reader");
    reader.beginOnCore(1);
    Serial.println("Begin on Core 1");
    xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
    delay(500);

  }


}


void loop()
{

  int reading = digitalRead(buttonPin);


  if (reading == LOW)
  {
    Serial.println(" QR butona basıldı");

    delay(1500);
    for (int i = 0; i < 6; i++) {
      Serial.println(i);
      Serial.println("a harfini yazıyor:");
      EEPROM.write(i, 'a');
      EEPROM.commit();

    }
    Serial.print("eeproma yazıldıııııı:");
    ESP.restart();
  }
  if (loopkontrol == 1) {


    while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
      delay(500);
      Serial.print("!");
    }
    client.onMessage(handle_message);
    Serial.println("Websocket Connected cam!");

    while (client.available()) {
      
      client.poll();
      int PIR = digitalRead(gpioPIR);
      if (PIR == LOW)
      {
        Serial.println("butona basıldı");
        PIRkontrol = 1;

      } if (PIR == HIGH) {
        Serial.println("butona basılmadı");
        PIRkontrol = 0;

      }

      if (kontrol2 == 0) {
        String mac = WiFi.macAddress();
        mac.replace(":", "-");
        //Serial.println(mac);
        String json = "{" ;
        json = json + "\"type\":" + "\"getList\"" +
               ",\"sifre\":" + "\"1234\""  +
               ",\"mac\":" + "\"" + mac + "\"" +
               "}";
        client.send(json);
        kontrol2 = 1;
        delay(1000);
      }


      if (serverkontrol == 1) { //.........................................................................
        Serial.print("SERVER BAĞLANDI");


        if (PIRkontrol == 1)
        {

          if (kontrol6 == 0) {
            String mac = WiFi.macAddress();
            mac.replace(":", "-");
            //Serial.println(mac);
            String json = "{" ;
            json = json + "\"type\":" + "\"kaydet\"" +
                   ",\"sifre\":" + "\"1234\""  +
                   ",\"mac\":" + "\"" + mac + "\"" +
                   "}";
            client.send(json);

            Serial.println("json gonderildi");
            kontrol6 = 1;
            kontrol5 = 0;
            delay(1000);
          }

        }
        else if (PIRkontrol == 0) //sensör hareket alıgılıyoken (yani yayını kaydediyorken) canlı yayın izleye basılmıştı, hareket kesilince server kaydetmeyi durdurdu canlı yayın izlenmeye devam edildi
        {


          if (kontrol5 == 0) {
            String mac = WiFi.macAddress();
            mac.replace(":", "-");
            //Serial.println(mac);
            String json = "{" ;
            json = json + "\"type\":" + "\"kaydettamamlandi\"" +
                   ",\"sifre\":" + "\"1234\""  +
                   ",\"mac\":" + "\"" + mac + "\"" +
                   "}";
            client.send(json);

            Serial.println("sensör hareket alıgılıyoken (yani yayını kaydediyorken)  canlı yayın izleye basılmıştı, hareket kesilince server kaydetmeyi durdurdu");
            kontrol5 = 1;
            delay(1000);
          }
          kontrol6 = 0;
        }
        /*  else{

          if (kontrol7 == 0) {
             String mac = WiFi.macAddress();
             mac.replace(":", "-");
             //Serial.println(mac);
             String json = "{" ;
             json = json + "\"type\":" + "\"kaydettamamlandi\"" +
                    ",\"sifre\":" + "\"1234\""  +
                    ",\"mac\":" + "\"" + mac + "\"" +
                    "}";
             client.send(json);

             Serial.println("json gonderildi");
             kontrol7 = 1;
             delay(1000);
           }
           kontrol6 = 0;
          }*/

        do {
          Serial.println("buraya giriyor");
          camera_fb_t *fb = esp_camera_fb_get();
          int x = fb->len;
          int foundffd9 = 0;
          for (int j = 1; j <= 1025; j++) {

            if (fb->buf[x - j] == 0xD9) {
              if (fb->buf[x - j - 1] == 0xFF ) {
                //Serial.print("Found the FFD9, junk is "); Serial.println(j);
                if (j == 1) {
                } else {
                  //Serial.print(", Len = "); Serial.print(x);
                  //Serial.print(", Corrent Len = "); Serial.print(x - j + 1);
                  //Serial.print(", Extra Bytes = "); Serial.println( j - 1);
                }
                foundffd9 = 1;
                break;               // break out of for() - we found a ffd9
              }
            }
          }
          if (!foundffd9) {
            //Serial.print("Bad jpeg, Len = "); Serial.println(x);
            esp_camera_fb_return(fb);
          } else
          {
          client.sendBinary((const char*) fb->buf, fb->len);
            esp_camera_fb_return(fb);
            break;                 // break out of while(1) - we have a good jpeg
          }

        } while (1);
        delay(100);
      } //.................................................................................................


      else if (PIRkontrol == 1 ) {

        if (kontrol3 == 0) {
          String mac = WiFi.macAddress();
          mac.replace(":", "-");
          //Serial.println(mac);
          String json = "{" ;
          json = json + "\"type\":" + "\"kaydet\"" +
                 ",\"sifre\":" + "\"1234\""  +
                 ",\"mac\":" + "\"" + mac + "\"" +
                 "}";
          client.send(json);
          kontrol3 = 1;
          delay(1000);
        }

        Serial.print("BUTONA BASILDI");
        do {
          camera_fb_t *fb = esp_camera_fb_get();
          int x = fb->len;
          int foundffd9 = 0;
          for (int j = 1; j <= 1025; j++) {

            if (fb->buf[x - j] == 0xD9) {
              if (fb->buf[x - j - 1] == 0xFF ) {
                //Serial.print("Found the FFD9, junk is "); Serial.println(j);
                if (j == 1) {
                } else {
                  //Serial.print(", Len = "); Serial.print(x);
                  //Serial.print(", Corrent Len = "); Serial.print(x - j + 1);
                  //Serial.print(", Extra Bytes = "); Serial.println( j - 1);
                }
                foundffd9 = 1;
                break;               // break out of for() - we found a ffd9
              }
            }
          }
          if (!foundffd9) {
            //Serial.print("Bad jpeg, Len = "); Serial.println(x);
            esp_camera_fb_return(fb);
          } else
          {
            client.sendBinary((const char*) fb->buf, fb->len);
            esp_camera_fb_return(fb);
            break;                 // break out of while(1) - we have a good jpeg
          }

        } while (1);
        delay(100);
        kontrol4 = 0;

      }





      else if (PIRkontrol == 0 ) {
        if (kontrol4 == 0) {
          String mac = WiFi.macAddress();
          mac.replace(":", "-");
          //Serial.println(mac);
          String json = "{" ;
          json = json + "\"type\":" + "\"kaydettamamlandi\"" +
                 ",\"sifre\":" + "\"1234\""  +
                 ",\"mac\":" + "\"" + mac + "\"" +
                 "}";
          client.send(json);

          Serial.println("json gonderildi");
          kontrol4 = 1;
          delay(1000);
        }
        Serial.println("hareket yok");
        delay(2000);
        kontrol3 = 0;
        kontrol6 = 0;

      }
      else if (serverkontrol == 2 ) {
        Serial.println("Server kamerayı başlatmadı");
        delay(2000);
      }
      //kontrol3=0;
    }
    kontrol2 = 0;

  }


}
