#include <Arduino.h>
#include "ESP32QRCodeReader.h"
int kontrol=0;
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
#include <WiFi.h>

String ssid;
String pass;
String Payload;
char password[8];
char ssid1[13];

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
        while(1==1){
          vTaskDelay(100 / portTICK_PERIOD_MS);
          if(kontrol==0){
            
          
          String Payload = (const char *)qrCodeData.payload;
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


        Serial.println("15");
        WiFi.begin(ssid1, password);
        Serial.println("7");
        while (WiFi.status() != WL_CONNECTED) {
          Serial.println("8");
          delay(500);
          Serial.print("!");
        }
        Serial.println("9");
        Serial.print("WİFİİ BAŞLADI ");
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.print("Camera Ready! Use 'http://");
        Serial.print(WiFi.localIP());
        Serial.println("' to connect");
        kontrol=1;
        }
        else{
          Serial.print("şimdi ne yapacağuk");
          }
        }
      }
      else
      {
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
  Serial.println();

  reader.setup();

  Serial.println("Setup QRCode Reader");

  reader.beginOnCore(1);

  Serial.println("Begin on Core 1");

  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
}

void loop()
{
  Serial.println("43");
  delay(100);
}
