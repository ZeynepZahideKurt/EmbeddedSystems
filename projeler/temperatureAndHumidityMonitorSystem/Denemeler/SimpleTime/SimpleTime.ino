#include <WiFi.h>
#include "time.h"

const char* ssid     = "Enelsis3";
const char* password = "1vn6egph";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int Krish_day;
int Krish_month;
int Krish_year;
int Krish_hour;
int Krish_min;
int Krish_sec;



void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%m %d %Y / %H:%M:%S"); //scanf(&timeinfo, "%m %d %Y / %H:%M:%S")
  Krish_hour = timeinfo.tm_hour+1; Krish_min = timeinfo.tm_min; Krish_sec = timeinfo.tm_sec;

  Krish_day = timeinfo.tm_mday; Krish_month = timeinfo.tm_mon + 1; Krish_year = timeinfo.tm_year + 1900;
}

void setup()
{
  Serial.begin(115200);

  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);

  printLocalTime();
  Serial.println(Krish_day);
  Serial.println(Krish_month);
Serial.println(Krish_year);
Serial.println(Krish_hour);
Serial.println(Krish_min);
Serial.println(Krish_sec);
}
