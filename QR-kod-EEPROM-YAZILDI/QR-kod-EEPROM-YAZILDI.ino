#include <Arduino.h>
#include "ESP32QRCodeReader.h"
#include "esp_camera.h"
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
#include <WiFi.h>
#include <EEPROM.h>
#define EEPROM_SIZE 42


#include "camera_pins.h"
void startCameraServer();
char hepsi[50];
String ssid;
String pass;
String Payload;
char password[20];
char ssid1[20];
int QRkontrol = 0;
int s;
int p;





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

        /* hepsi.toCharArray(Payload, Payload.length() + 1);
          for (int i = 0; i < Payload.length(); i++) {
           EEPROM.write(i, hepsi[i]);
           Serial.println(hepsi[i]);
           // Serial.println(s);

          }*/
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

        WiFi.begin(ssid1, password);


        Serial.println(s);
        for (int i = 0; i < s; i++) {
          EEPROM.write(i, ssid1[i]);
          Serial.println(ssid1[i]);
          // Serial.println(s);

        }
        for (int i = s; i < 20; i++) {
          EEPROM.write(i, '-');

        }
        int o = 3;
        EEPROM.write(20, o);
        EEPROM.commit();



        for (int a = 21; a < p + 21; a++) {
          EEPROM.write(a, password[a - 21]);
          Serial.println(password[a - 21]);
        }
        for (int i = 21 + p; i < 41; i++) {
          EEPROM.write(i, '-');

        }


        Serial.print("eeproma yazıldıııııı:");

        Serial.println(ssid1);
        Serial.println(password);


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
  int kontrol = 0;
  EEPROM.begin(EEPROM_SIZE);
  /*for (int j = 0; j < 42; j++) {
    if(EEPROM.read(j)=="-"&&kontrol!=0){
      if(EEPROM.read(j+1)=="-"){
      if(EEPROM.read(j+2)=="-"){
      kontrol=1;

      }
      else{
         ssid1[j] = EEPROM.read(j);
      }
      }
      else{
         ssid1[j] = EEPROM.read(j);
      }
      }
    else if(kontrol==0){
      ssid1[j] = EEPROM.read(j);
    }
    else{
    if()

    }


    }*/
  for (int j = 0; j < 19; j++) {
    ssid1[j] = EEPROM.read(j);
  }
  int o = EEPROM.read(20);

  for (int i = 21; i < 41; i++) {
    password[i - 21] = EEPROM.read(i);
  }
  String ad = String( ssid1);
  String sifre = String( password);
  Serial.print("eeproma yazılan:");
  Serial.println(ad);
  Serial.println(sifre);
  int s2 = ad.length();
  int p2 = sifre.length();
  Serial.print("s2");
  Serial.print(s2);
  Serial.print("p2");
  Serial.print(p2);

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

  char ssid4[n+1];
  char password4[m+1];
  ssid3.toCharArray(ssid4, n+1);
  password3.toCharArray(password4, m+1);

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
  
  if (QRkontrol == 1) {
    reader.setup();
    Serial.println("Setup QRCode Reader");
    reader.beginOnCore(1);
    Serial.println("Begin on Core 1");
    xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);

  } else {


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
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    //init with high specs to pre-allocate larger buffers
    if (psramFound()) {
      config.frame_size = FRAMESIZE_UXGA;
      config.jpeg_quality = 10;
      config.fb_count = 2;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.jpeg_quality = 12;
      config.fb_count = 1;
    }

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }

    sensor_t * s = esp_camera_sensor_get();
    //initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1);//flip it back
      s->set_brightness(s, 1);//up the blightness just a bit
      s->set_saturation(s, -2);//lower the saturation
    }
    //drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif
    WiFi.begin(ssid4, password4);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    startCameraServer();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");


  }



}

void loop()
{

  delay(10000);
}
