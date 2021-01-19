#include <Arduino.h>
#include "ESP32QRCodeReader.h"
String ssid;
String pass;
#include "esp_camera.h"
#include <WiFi.h>

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
String Payload;
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
        Serial.print("ssid: ");
        Serial.println(ssid);

        String pass = Payload.substring(n + 3, l);
        Serial.print("pass: ");
        Serial.println(pass);

        int z=l-(n+3);
        char ssid1[n];
        ssid.toCharArray(ssid1, n+1);
        
        char password[z];
        pass.toCharArray(password, z+1);
        Serial.print("....ssid1: ");
        Serial.println(ssid1);
        Serial.print("....password: ");
        Serial.println(password);
        
        WiFi.begin(ssid1, password);

        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");

      //  startCameraServer();

        Serial.print("Camera Ready! Use 'http://");
        Serial.print(WiFi.localIP());
        Serial.println("' to connect");




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
  delay(100);
}
