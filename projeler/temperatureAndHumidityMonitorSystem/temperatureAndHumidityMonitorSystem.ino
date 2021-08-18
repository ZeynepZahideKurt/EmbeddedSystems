// LED will blink when in config mode
/*
  GPIO 0 12k-13k direnç ve butonla ve EN( Resetleme butonu-12k 13k direnç tak )programlama moduna geç. önce I0 butonuna basıp sonra EN butonuna basarsan programlama moduna geçer
  GPIO1 (TX) / GPIO3 (RX) ile program at
  GPIO2 bi tane led tak wifii ye bağlandığınında ışığı söner
  GPIO15 bir buton tak wifiiye bağlanma oradaki butonla eepromdaki kullanıcı adı şifreyi değiştir
  GPIO21 SDA / GPIO22 SCL 3.3 volt

*/

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Ticker.h>
Ticker ticker;

#include <HTTPClient.h>
#include <ArduinoJson.h>


#include <Wire.h>
#define Addr 0x44

#define I2C_SDA 21
#define I2C_SCL 22

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

unsigned long simdikizaman, eskizaman;

int LED = LED_BUILTIN;

float cTemp;
float humidity;

int kontrol1=0;

void tick()
{
  //toggle state
  digitalWrite(LED, !digitalRead(LED));     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
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

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(LED, LOW);
  eskizaman= millis();


}

void loop() {
  
  if (kontrol1==0){
    postDataToServer();
    kontrol1=1;
  }
  
  simdikizaman = millis();
  if (simdikizaman - eskizaman >= 600000)
  {
    postDataToServer();
    eskizaman=simdikizaman;
  }
  

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
    doc["mac"] = "123b";

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
}
