
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
//void startCameraServer();
String ssid;
String pass;
String Payload;
char password[35];
char ssid1[35];
int QRkontrol = 1;
int s;
int p;

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
int setupkontrol = 0;
using namespace websockets;
WebsocketsServer socket_server;
WebsocketsClient client;


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

        int  n = Payload.indexOf("---");
        Serial.print("n:");
        Serial.println(n);

        String ssid = Payload.substring(0, n);
        Serial.print("ssid parçalanmış hali: ");
        Serial.println(ssid);

        String pass = Payload.substring(n + 3, l);
        Serial.print("pass: ");
        Serial.println(pass);

        int z = l - (n + 2);


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


        Serial.println(s);
        for (int i = 0; i < s; i++) {
          EEPROM.write(i, ssid1[i]);
          Serial.println(ssid1[i]);
          // Serial.println(s);

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


        if (deneme3 != deneme4) {
          ESP.restart();
        }


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

  EEPROM.begin(EEPROM_SIZE);
  pinMode(buttonPin, INPUT);

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

    setupkontrol = 1;

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

void handle_message(WebsocketsClient &client, WebsocketsMessage msg) {
  Serial.println(msg.data());
}
void loop()
{

  int reading = digitalRead(buttonPin);
  if (reading == LOW)
  {
    Serial.println("butona basıldı");

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
  if (setupkontrol == 1) {
    while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
      delay(500);
      Serial.print(".");
    }
    client.onMessage(handle_message);
    Serial.println("Websocket Connected cam!");
    while (client.available()) {
      client.poll();
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
        /*while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
          delay(500);
          Serial.print(".");
          }*/
      }
      camera_fb_t *fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        esp_camera_fb_return(fb);
        return;
      }
      if (fb->format != PIXFORMAT_JPEG) {
        Serial.println("Non-JPEG data not implemented");
        return;
      }
      client.sendBinary((const char*) fb->buf, fb->len);
      esp_camera_fb_return(fb);
      //Serial.println("burada1");
      delay(100);
    }
    kontrol2 = 0;

  }
}
