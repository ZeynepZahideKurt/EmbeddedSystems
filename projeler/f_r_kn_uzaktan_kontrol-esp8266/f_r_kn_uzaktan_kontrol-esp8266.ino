#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>

//ESP8266 İÇİN KOD

//#include <HTTPClient.h>
#include <ArduinoJson.h>
//#include <WiFi.h>
#define EEPROM_SIZE 1
/*const char* ssid = "iPhone";
const char* password = "11223344";
*/
const int led = 2;
const int role = 4;
int sonuc;
const char* ssid = "Enelsis1";
  const char* password = "1vn6egph";
void setup() {
  EEPROM.begin(EEPROM_SIZE);
  digitalWrite(led, HIGH);

  sonuc = EEPROM.read(0);
  if (sonuc == 0) {
    digitalWrite(role, LOW);
    Serial.println("EEProm'dan sonuç=0 okundu");
  } else {
    digitalWrite(role, HIGH);
    Serial.println("EEProm'dan sonuç=1 okundu");
  }

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(role, OUTPUT);
  pinMode(led, OUTPUT);
  //digitalWrite(role, LOW);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  digitalWrite(led, LOW);
  Serial.println("WiFi connected");
}
void loop() {
  if (WiFi.status() == WL_CONNECTED) {


    digitalWrite(led, LOW);
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
        boolean datam = doc["data"];
        sonuc = doc["data"]["status"];
        if (datam == NULL) {
          sonuc = 2;
          Serial.println("datanull");
        }

        Serial.println(sonuc);



        if (sonuc == 1) {

          EEPROM.write(0, sonuc);
          EEPROM.commit();
          Serial.println("EEProm'dan sonuç=1 yazıldı");
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

        else if (sonuc == 0) {
          EEPROM.write(0, sonuc);
          EEPROM.commit();
          Serial.println("EEProm'dan sonuç=0 yazıldı");
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
    digitalWrite(led, HIGH);
  }
  delay(5000);
}
