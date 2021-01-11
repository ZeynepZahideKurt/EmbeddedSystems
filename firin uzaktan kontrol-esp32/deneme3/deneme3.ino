#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

const char* ssid = "iPhone";
const char* password = "11223344";

#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

const int role = 2;
/*
  const char* ssid = "Enelsis1";
  const char* password = "1vn6egph";*/
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(role, OUTPUT);
  //digitalWrite(role, LOW);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
}
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://enelsis.com/portal/server/main.php");
    http.addHeader("Content-Type", "application/json");
    //  int httpResponseCode = http.GET();
    int httpResponseCode = http.POST("{\"type\":\"getList\",\"type2\":\"query\",\"username\":\"esp\",\"password\":\"thWUFzNEN5wWRW9\",\"relayNo\":\"1\"}");
    if (httpResponseCode == 200) {

   

      String payload = http.getString();
      Serial.println(httpResponseCode);
      Serial.println("sorgu cevabı");
      Serial.println(payload);
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
       boolean datam=doc["data"];
       int sonuc = doc["data"]["status"];
        if(datam==NULL){
          sonuc=2;
          Serial.println("datanull");
          }
        
        Serial.println(sonuc);
        if (sonuc == 1) {
         // if (role == LOW) {
            digitalWrite(role, HIGH);
            HTTPClient http2;
            http2.begin("http://enelsis.com/portal/server/main.php");
            http2.addHeader("Content-Type", "application/json");
            int httpResponseCode2 = http2.POST("{\"type\":\"getList\",\"type2\":\"result\",\"username\":\"esp\",\"password\":\"thWUFzNEN5wWRW9\",\"relayNo\":\"1\"}");
            if (httpResponseCode2 == 200) {
              Serial.println(httpResponseCode2);
              String payload2 = http2.getString();          //ISSO É PARA GET REQUEST
              Serial.println(httpResponseCode2);
              Serial.println("Status 1 cevabı");
              Serial.println(payload2);
              StaticJsonDocument<200> doc2;
              DeserializationError error = deserializeJson(doc2, payload2);
              if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
              }
              else {
                Serial.println("Status 1");
                String mesaj = doc2["message"];
                Serial.println(mesaj);
              }
            }
         // }
          //else{
           //Serial.println("ROLE ZATEN AÇIK ");
          //}
        }
        
        else if(sonuc==0) {
          //if (role == HIGH) {
            //role kapandı
            digitalWrite(role, LOW);
            HTTPClient http3;
            http3.begin("http://enelsis.com/portal/server/main.php");
            http3.addHeader("Content-Type", "application/json");
            int httpResponseCode3 = http3.POST("{\"type\":\"getList\",\"type2\":\"result\",\"username\":\"esp\",\"password\":\"thWUFzNEN5wWRW9\",\"relayNo\":\"1\"}");
            if (httpResponseCode3 == 200) {
              Serial.println(httpResponseCode3);
              String payload3 = http3.getString();
              Serial.println(httpResponseCode3);
              Serial.println("Status 0 cevabı");
              Serial.println(payload3);
              StaticJsonDocument<200> doc3;
              DeserializationError error = deserializeJson(doc3, payload3);
              if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
              }
              else {
                String mesaj = doc3["message"];
                Serial.println("Status 0");
                Serial.println(mesaj);
              }
            }
          //}//
          //else{
            //Serial.println("ROLE ZATEN KAPALI ");
          //}
        }
      }
    }
    else {
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println(" WiFi BAĞLANTISI KURULAMADI");
  }
  delay(5000);
}
