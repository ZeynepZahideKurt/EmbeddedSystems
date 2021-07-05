//https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
<<<<<<< Updated upstream
//ESP Dev MODUL-1 MB APP-3MB SPIFFS 
=======
>>>>>>> Stashed changes

#include <ArduinoWebsockets.h>
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "camera_index.h"
#include "Arduino.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "fr_flash.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <Adafruit_MLX90614.h>

#include <EEPROM.h>
#define EEPROM_SIZE 16

float IRPWMValue1 = 0;
float IRPWMValue2 = 0;
float IRPWMValue3 = 0;
float IRPWMValue4 = 0;
float LEDPWMValue = 0;

int int_IRPWMValue1 = 0;
int int_IRPWMValue2 = 0;
int int_IRPWMValue3 = 0;
int int_IRPWMValue4 = 0;
int int_LEDPWMValue = 0;

boolean ilk_acilma = false;
int IRPWMValue1_durum = 0;
int IRPWMValue2_durum = 0;
int IRPWMValue3_durum = 0;
int IRPWMValue4_durum = 0;
int LEDPWMValue_durum = 0;
int o_periotsure_durum = 0;
int o_calismasure_durum = 0;
int istenilen_nem_durum = 0;

float l_periotsure_durum;
float l_calismasure_durum;
float istenilen_sicaklik_durum;
float girilen_min_sicaklik_durum;
float girilen_max_sicaklik_durum;

int girilen_min_sicaklik_durum1;
int girilen_min_sicaklik_durum2;

int girilen_max_sicaklik_durum1;
int girilen_max_sicaklik_durum2;

int istenilen_sicaklik_durum1;
int istenilen_sicaklik_durum2;

boolean kontrol = false;
int l_calismasure;
int l_calismabitis;
boolean l_calisma = false;
boolean l_calisma_ilk = false;
int l_periotsure;
int l_periotbitis;
boolean l_periot = false;
boolean l_periot_ilk = false;
unsigned long l_yenizaman;
unsigned long l_simdi;
boolean l_tiklandi = false;

int sensor_sure = 0;

int o_calismasure;
int o_calismabitis;
boolean o_calisma = false;
boolean o_calisma_ilk = false;
int o_periotsure;
int o_periotbitis;
boolean o_periot;
boolean o_periot_ilk;
unsigned long o_yenizaman;
unsigned long o_simdi;
boolean o_tiklandi = false;



int ac;
const int LEDPin = 4; //1.pin
const int IRPin1 = 2; //2.pin
const int IRPin2 = 13; //5.pin
const int IRPin3 = 12; //6.pin
const int IRPin4 = 3; //11.pin //rx-yeşil kablo
const int motor = 16; //15.pin
const int h2o = 1; //10.pin //tx-mavi kablo

int istenilen_nem;
float ort_humidity = 1000;
float istenilen_sicaklik;
float ort_cTemp = 1000;
float girilen_min_sicaklik;
float girilen_max_sicaklik;

// setting PWM properties
const int PWMFrequency = 4000;//40000000;
const int PWMResolation = 8; //0-65535   //0.0, 100.0
const int LEDChannel = 14;
const int IRChannel1 = 15;
const int IRChannel2 = 13;
const int IRChannel3 = 12;
const int IRChannel4 = 11;
// SHT31 I2C address is 0x44(68)
#define Addr 0x44

#define I2C_SDA 14
#define I2C_SCL 15
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


const char* ssid = "ESP";
const char* password = "123456789";

/*   const char* ssid = "Enelsis1_EXT";
  const char* password = "1vn6egph";
  const char* ssid = "Zehra";
  const char* password = "11223344";
  const char* ssid = "iPhone";
  const char* password = "11223344";
  const char* ssid = "Enelsis1_EXT";
  const char* password = "1vn6egph";*/
// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
using namespace websockets;
WebsocketsServer socket_server;
camera_fb_t * fb = NULL;

void app_httpserver_init();
typedef struct
{
  uint8_t *image;
  box_array_t *net_boxes;
  dl_matrix3d_t *face_id;
} http_img_process_result;
static inline mtmn_config_t app_mtmn_config()
{
  mtmn_config_t mtmn_config = {0};
  mtmn_config.type = FAST;
  mtmn_config.min_face = 80;
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;
  return mtmn_config;
}
mtmn_config_t mtmn_config = app_mtmn_config();
httpd_handle_t camera_httpd = NULL;
typedef enum
{
  START_STREAM,
  DELETE_ALL,
} en_fsm_state;
en_fsm_state g_state;

void setup() {

  Wire.begin(I2C_SDA, I2C_SCL);
  mlx.begin();

  //Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  //Wire.setClock(100000);
  ledcSetup(IRChannel1, PWMFrequency, PWMResolation);
  ledcAttachPin(IRPin1, IRChannel1);
  ledcSetup(IRChannel2, PWMFrequency, PWMResolation);
  ledcAttachPin(IRPin2, IRChannel2);
  ledcSetup(IRChannel3, PWMFrequency, PWMResolation);
  ledcAttachPin(IRPin3, IRChannel3);
  ledcSetup(IRChannel4, PWMFrequency, PWMResolation);
  ledcAttachPin(IRPin4, IRChannel4);
  ledcSetup(LEDChannel, PWMFrequency, PWMResolation);
  ledcAttachPin(LEDPin, LEDChannel);

  pinMode(motor, OUTPUT);
  pinMode(h2o, OUTPUT);

  LEDPWMValue_durum = EEPROM.read(0);
  ////Serial.printl("LEDPWMValue_durum:");
  //Serial.println(LEDPWMValue_durum);
  if (LEDPWMValue_durum <= 100 && LEDPWMValue_durum != 0) {
    LEDPWMValue = 2.55 * LEDPWMValue_durum;
    int_LEDPWMValue = LEDPWMValue_durum;
    ledcWrite(LEDChannel, LEDPWMValue);
  }
  else {
    LEDPWMValue = 0;
    int_LEDPWMValue = 0;
    LEDPWMValue_durum = 0;
    ledcWrite(LEDChannel, LEDPWMValue);
  }

  IRPWMValue1_durum = EEPROM.read(1);
  //Serial.println("IRPWMValue1_durum:");
  //Serial.println(IRPWMValue1_durum);
  if (IRPWMValue1_durum <= 100 && IRPWMValue1_durum != 0) {
    IRPWMValue1 = 2.55 * IRPWMValue1_durum;
    int_IRPWMValue1 = IRPWMValue1_durum;

    ledcWrite(15, IRPWMValue1);
  }
  else {
    IRPWMValue1 = 0;
    int_IRPWMValue1 = 0;
    IRPWMValue1_durum = 0;
    ledcWrite(15, IRPWMValue1);
  }

  IRPWMValue2_durum = EEPROM.read(2);
  //Serial.println("IRPWMValue2_durum:");
  //Serial.println(IRPWMValue2_durum);
  if (IRPWMValue2_durum <= 100 && IRPWMValue2_durum != 0) {
    int_IRPWMValue2 = IRPWMValue2_durum;
    IRPWMValue2 = IRPWMValue2_durum;
    ledcWrite(13, IRPWMValue2);
  }
  else {
    IRPWMValue2 = 0;
    int_IRPWMValue2 = 0;
    IRPWMValue2_durum = 0;
    ledcWrite(13, IRPWMValue2);
  }
  IRPWMValue3_durum = EEPROM.read(3);
  //Serial.println("IRPWMValue3_durum:");
  //Serial.println(IRPWMValue3_durum);
  if (IRPWMValue3_durum <= 100 && IRPWMValue3_durum != 0) {
    IRPWMValue3 = 2.55 * IRPWMValue3_durum;
    int_IRPWMValue3 = IRPWMValue3_durum;

    ledcWrite(12, IRPWMValue3);
  }
  else {
    IRPWMValue3 = 0;
    int_IRPWMValue3 = 0;
    IRPWMValue3_durum = 0;
    ledcWrite(12, IRPWMValue3);
  }
  IRPWMValue4_durum = EEPROM.read(4);

  //Serial.println("IRPWMValue4_durum:");
  //Serial.println(IRPWMValue4_durum);
  if (IRPWMValue4_durum <= 100 && IRPWMValue4_durum != 0) {
    IRPWMValue4 = 2.55 * IRPWMValue4_durum;
    int_IRPWMValue4 = IRPWMValue4_durum;
    ledcWrite(11, IRPWMValue4);
  }
  else {
    IRPWMValue4 = 0;
    int_IRPWMValue4 = 0;
    IRPWMValue4_durum = 0;
    ledcWrite(11, IRPWMValue4);
  }
  istenilen_nem_durum = EEPROM.read(5);
  //Serial.println("ilk an istenilen_nem_durum:");
  //Serial.println(istenilen_nem_durum);
  if (istenilen_nem_durum <= 100 && istenilen_nem_durum != 0) {
    istenilen_nem = istenilen_nem_durum;
  }
  else {
    istenilen_nem = 0;
  }

  l_periotsure_durum = EEPROM.read(6);
  //Serial.println("l_periotsure_durum:");
  //Serial.println(l_periotsure_durum);

  l_calismasure_durum = EEPROM.read(7);
  //Serial.println("l_calismasure_durum:");
  //Serial.println(l_calismasure_durum);
  if (l_calismasure_durum != 0 && l_periotsure_durum != 0 && l_calismasure_durum <= 100 && l_periotsure_durum <= 100) {
    l_tiklandi = true;
    l_calisma = true;
    l_calismasure = l_calismasure_durum;
    l_periotsure = l_periotsure_durum;
  }
  else {
    l_calismasure = 0;
    l_periotsure = 0;
  }


  istenilen_sicaklik_durum1 = EEPROM.read(8);//..............................................................................
  istenilen_sicaklik_durum1 = istenilen_sicaklik_durum1 << 8;

  istenilen_sicaklik_durum2 = EEPROM.read(9);
  istenilen_sicaklik_durum = istenilen_sicaklik_durum1 | istenilen_sicaklik_durum2;
  istenilen_sicaklik_durum = istenilen_sicaklik_durum / 100;






  //Serial.println("istenilen_sicaklik_durum:");
  //Serial.println(istenilen_sicaklik_durum);
  if (istenilen_sicaklik_durum <= 100 && istenilen_sicaklik_durum != 0) {
    istenilen_sicaklik = istenilen_sicaklik_durum;
  }
  else {
    istenilen_sicaklik = 0;
  }



  girilen_min_sicaklik_durum1 = EEPROM.read(10);//..............................................................................
  girilen_min_sicaklik_durum1 = girilen_min_sicaklik_durum1 << 8;

  girilen_min_sicaklik_durum2 = EEPROM.read(11);
  girilen_min_sicaklik_durum = girilen_min_sicaklik_durum1 | girilen_min_sicaklik_durum2;
  girilen_min_sicaklik_durum = girilen_min_sicaklik_durum / 100;

  //Serial.println("girilen_min_sicaklik_durum:");
  //Serial.println(girilen_min_sicaklik_durum);

  if (girilen_min_sicaklik_durum <= 100 && girilen_min_sicaklik_durum != 0) {
    girilen_min_sicaklik = girilen_min_sicaklik_durum;
  }
  else {
    girilen_min_sicaklik = 0;
  }

  girilen_max_sicaklik_durum1 = EEPROM.read(12);//..............................................................................
  girilen_max_sicaklik_durum1 = girilen_max_sicaklik_durum1 << 8;

  girilen_max_sicaklik_durum2 = EEPROM.read(13);
  girilen_max_sicaklik_durum = girilen_max_sicaklik_durum1 | girilen_max_sicaklik_durum2;
  girilen_max_sicaklik_durum = girilen_max_sicaklik_durum / 100;


  //Serial.println("girilen_max_sicaklik_durum:");
  //Serial.println(girilen_max_sicaklik_durum);
  if (girilen_max_sicaklik_durum <= 100 && girilen_max_sicaklik_durum != 0) {
    girilen_max_sicaklik = girilen_max_sicaklik_durum;
  }
  else {
    girilen_max_sicaklik = 0;
  }

  o_periotsure_durum = EEPROM.read(14);
  //Serial.println("o_periotsure_durum:");
  //Serial.println(o_periotsure_durum);

  o_calismasure_durum = EEPROM.read(15);

  //Serial.println("o_calismasure_durum:");
  //Serial.println(o_calismasure_durum);
  //Serial.println("o_periotsure_durum:");
  //Serial.println(o_periotsure_durum);
  if (o_calismasure_durum != 0 && o_periotsure_durum != 0 && o_calismasure_durum <= 100 && o_periotsure_durum <= 100) {
    o_tiklandi = true;
    o_calisma = true;
    o_calismasure = o_calismasure_durum;
    o_periotsure = o_periotsure_durum;
  }
  else {

    o_calismasure = 0;
    o_periotsure = 0;


  }


  //Serial.setDebugOutput(true);
  //Serial.println();
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
    // //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  ////Serial.print("AP IP address: ");

  app_httpserver_init();
  socket_server.listen(82);
  //Serial.print(WiFi.localIP());



}
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)index_ov2640_html_gz, index_ov2640_html_gz_len);
}
httpd_uri_t index_uri = {
  .uri       = "/",
  .method    = HTTP_GET,
  .handler   = index_handler,
  .user_ctx  = NULL
};
void app_httpserver_init ()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  if (httpd_start(&camera_httpd, &config) == ESP_OK)
    // //Serial.println("httpd_start");
  {
    httpd_register_uri_handler(camera_httpd, &index_uri);
  }
}
static esp_err_t LED_gelen(WebsocketsClient &client)
{
}

void o_calismabitisbelirlef() {
  o_simdi = millis();
  o_calismabitis = o_simdi + (1000 * o_calismasure);
}

void o_calismasuref() {

  digitalWrite(h2o, HIGH);

  o_calisma = true;
  if (o_calisma_ilk == false) {
    o_calismabitisbelirlef();
    o_calisma_ilk = true;
  }
  o_yenizaman = millis();
  if (o_yenizaman >= o_calismabitis) {
    o_periotsuref();
    o_calisma = false;
    o_calisma_ilk = false;
  }
}

void o_periotbitisbelirlef() {
  o_simdi = millis();
  o_periotbitis = o_simdi + (1000 * o_periotsure);
}


void o_periotsuref() {
  digitalWrite(h2o, LOW);

  o_periot = true;
  if (o_periot_ilk == false) {
    o_periotbitisbelirlef();
    o_periot_ilk = true;
  }
  o_yenizaman = millis();
  if (o_yenizaman >= o_periotbitis) {
    o_calismasuref();
    o_periot = false;
    o_periot_ilk = false;
  }
}


void l_calismabitisbelirlef() {
  l_simdi = millis();
  l_calismabitis = l_simdi + (1000 * l_calismasure);
}

void l_calismasuref() {

  ledcWrite(IRChannel1, IRPWMValue1);
  ledcWrite(IRChannel2, IRPWMValue2);
  ledcWrite(IRChannel3, IRPWMValue3);
  ledcWrite(IRChannel4, IRPWMValue4);
  l_calisma = true;
  if (l_calisma_ilk == false) {
    l_calismabitisbelirlef();
    l_calisma_ilk = true;
  }
  l_yenizaman = millis();
  if (l_yenizaman >= l_calismabitis) {
    l_periotsuref();
    l_calisma = false;
    l_calisma_ilk = false;
  }

}

void l_periotbitisbelirlef() {
  l_simdi = millis();
  l_periotbitis = l_simdi + (1000 * l_periotsure);
}


void l_periotsuref() {
  ledcWrite(IRChannel1, 0);
  ledcWrite(IRChannel2, 0);
  ledcWrite(IRChannel3, 0);
  ledcWrite(IRChannel4, 0);
  l_periot = true;
  if (l_periot_ilk == false) {
    l_periotbitisbelirlef();
    l_periot_ilk = true;
  }
  l_yenizaman = millis();
  if (l_yenizaman >= l_periotbitis) {
    l_calismasuref();
    l_periot = false;
    l_periot_ilk = false;
  }
}



void handle_message(WebsocketsClient &client, WebsocketsMessage msg)
{

  //Serial.println(msg.data());

  if (msg.data().substring(0, 2) == "K1") {
    StaticJsonDocument<200> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, msg.data().substring(2));

    if (error) {
      // //Serial.print(F("deserializeJson() failed: "));
      //Serial.println(error.f_str());
      return;
    }
    else {
      girilen_min_sicaklik = doc["Kmin"];
      //Serial.println("ara yüzden gelen girilen_min_sicaklik");
      //Serial.println(girilen_min_sicaklik);

      girilen_max_sicaklik = doc["Kmax"];
      //Serial.println(girilen_max_sicaklik);
      if (girilen_min_sicaklik != girilen_min_sicaklik_durum  || girilen_max_sicaklik != girilen_max_sicaklik_durum)
      {
        girilen_min_sicaklik_durum1 = 100 * girilen_min_sicaklik;
        girilen_min_sicaklik_durum1 = girilen_min_sicaklik_durum1 >> 8;
        //Serial.println("girilen_min_sicaklik_durum1" );
        //Serial.println(girilen_min_sicaklik_durum1 );
        //girilen_min_sicaklik=girilen_min_sicaklik_durum1;
        girilen_min_sicaklik_durum2 = 100 * girilen_min_sicaklik;
        girilen_min_sicaklik_durum2 = 255 & girilen_min_sicaklik_durum2 ; //.................................................................
        //Serial.println("girilen_min_sicaklik_durum2");
        //Serial.println(girilen_min_sicaklik_durum2);
        girilen_min_sicaklik_durum = girilen_min_sicaklik;
        EEPROM.write(10, girilen_min_sicaklik_durum1);
        EEPROM.commit();
        EEPROM.write(11, girilen_min_sicaklik_durum2);
        //girilen_min_sicaklik=girilen_min_sicaklik_durum1 |girilen_min_sicaklik_durum2;
        EEPROM.commit();
        //Serial.println("kaydedilen girilen_min_sicaklik: ");
        //Serial.println(girilen_min_sicaklik);



        girilen_max_sicaklik_durum1 = 100 * girilen_max_sicaklik;
        girilen_max_sicaklik_durum1 = girilen_max_sicaklik_durum1 >> 8;
        //Serial.println("girilen_max_sicaklik_durum1" );
        //Serial.println(girilen_max_sicaklik_durum1 );
        //girilen_min_sicaklik=girilen_min_sicaklik_durum1;
        girilen_max_sicaklik_durum2 = 100 * girilen_max_sicaklik;
        girilen_max_sicaklik_durum2 = 255 & girilen_max_sicaklik_durum2 ; //.................................................................
        //Serial.println("girilen_max_sicaklik_durum2");
        //Serial.println(girilen_max_sicaklik_durum2);
        girilen_max_sicaklik_durum = girilen_max_sicaklik;
        EEPROM.write(12, girilen_max_sicaklik_durum1);
        EEPROM.commit();
        EEPROM.write(13, girilen_max_sicaklik_durum2);
        EEPROM.commit();

        //Serial.println("girilen_max_sicaklik saved in flash memory");
      }
      istenilen_sicaklik = doc["Ksicaklik"];
      //Serial.println(istenilen_sicaklik);
      if (istenilen_sicaklik != istenilen_sicaklik_durum)
      {

        istenilen_sicaklik_durum = istenilen_sicaklik;
        istenilen_sicaklik_durum1 = 100 * istenilen_sicaklik_durum;
        istenilen_sicaklik_durum1 = istenilen_sicaklik_durum1 >> 8;
        //Serial.println("istenilen_sicaklik_durum1" );
        //Serial.println(istenilen_sicaklik_durum1 );
        //girilen_min_sicaklik=girilen_min_sicaklik_durum1;
        istenilen_sicaklik_durum2 = 100 * istenilen_sicaklik_durum;
        istenilen_sicaklik_durum2 = 255 & istenilen_sicaklik_durum2 ; //.................................................................
        //Serial.println("istenilen_sicaklik_durum2");
        //Serial.println(istenilen_sicaklik_durum2);

        EEPROM.write(8, istenilen_sicaklik_durum1);
        EEPROM.commit();
        EEPROM.write(9, istenilen_sicaklik_durum2);
        EEPROM.commit();
        //Serial.println("istenilen_sicaklik saved in flash memory");

      }
      l_calismasure = doc["KLcalis"];
      //Serial.println(l_calismasure);
      l_periotsure = doc["KLdur"];
      //Serial.println(l_periotsure);
      if (l_periotsure != l_periotsure_durum  || l_calismasure != l_calismasure_durum)
      {
        l_periotsure_durum = l_periotsure;
        EEPROM.write(6, l_periotsure);
        EEPROM.commit();
        //Serial.println(l_periotsure_durum);

        l_calismasure_durum = l_calismasure;
        EEPROM.write(7, l_calismasure);
        EEPROM.commit();
        //Serial.println("IRPWMValue3 saved in flash memory");

      }
      int_IRPWMValue1 = doc["Kled1"];
      //Serial.print("eeproma yazılmadan önce int_IRPWMValue1: ");

      //Serial.println(int_IRPWMValue1);
      if (int_IRPWMValue1 != IRPWMValue1_durum)
      {
        ledcWrite(15, IRPWMValue1);
        IRPWMValue1_durum = int_IRPWMValue1;
        EEPROM.write(1, int_IRPWMValue1);
        EEPROM.commit();
        //Serial.print("eeproma yazılan IRPWMValue1:");

        //Serial.println(IRPWMValue1);
      }
      //Serial.print("eeproma yazıldıktan sonraki IRPWMValue1: ");
      //Serial.println(IRPWMValue1);

      int_IRPWMValue2 = doc["Kled2"];
      //Serial.print("eeproma yazılmadan önce IRPWMValue2_durum: ");
      //Serial.println(int_IRPWMValue2);

      if (int_IRPWMValue2 != IRPWMValue2_durum)
      {
        ledcWrite(13, IRPWMValue2);
        IRPWMValue2_durum = int_IRPWMValue2;
        EEPROM.write(2, int_IRPWMValue2);
        EEPROM.commit();
        //Serial.print("eeproma yazılan IRPWMValue2:");
        //Serial.println(IRPWMValue2);

      }
      //Serial.print("eeproma yazıldıktan sonraki IRPWMValue2: ");
      //Serial.println(IRPWMValue2);

    }
  }
  if (msg.data().substring(0, 2) == "K2") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(2));

    if (error) {
      ////Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      int_IRPWMValue3 = doc["Kled3"];
      //Serial.print("eeproma yazılmadan önce int_IRPWMValue3: ");
      //Serial.println(int_IRPWMValue3);
      if (int_IRPWMValue3 != IRPWMValue3_durum)
      {
        ledcWrite(12, int_IRPWMValue3);
        IRPWMValue3_durum = int_IRPWMValue3;
        EEPROM.write(3, int_IRPWMValue3);
        EEPROM.commit();
        //Serial.print("eeproma yazılan IRPWMValue3:");
        //Serial.println(IRPWMValue3);
      }
      //Serial.print("eeproma yazıldıktan sonraki int_IRPWMValue3: ");
      //Serial.println(int_IRPWMValue3);



      int_IRPWMValue4 = doc["Kled4"];
      //Serial.print("eeproma yazılmadan önce int_IRPWMValue4: ");
      //Serial.println(int_IRPWMValue4);
      if (int_IRPWMValue4 != IRPWMValue4_durum)
      {
        ledcWrite(11, IRPWMValue4);
        IRPWMValue4_durum = int_IRPWMValue4;
        EEPROM.write(4, int_IRPWMValue4);
        EEPROM.commit();
        //Serial.print("eeproma yazılan IRPWMValue4:");
        //Serial.println(IRPWMValue4);
      }
      //Serial.print("eeproma yazıldıktan sonraki int_IRPWMValue4: ");
      //Serial.println(int_IRPWMValue4);



      int_LEDPWMValue = doc["Kled5"];
      //Serial.println(int_LEDPWMValue);
      if (int_LEDPWMValue != LEDPWMValue_durum)
      {
        ledcWrite(14, LEDPWMValue);
        LEDPWMValue_durum = int_LEDPWMValue;
        EEPROM.write(0, int_LEDPWMValue);
        EEPROM.commit();
        //  //Serial.print("LEDPWMValue:");
        //Serial.println(LEDPWMValue);
      }
      istenilen_nem = doc["Kmotor"];
      //Serial.println(istenilen_nem);
      if (istenilen_nem != istenilen_nem_durum)
      {
        istenilen_nem_durum = istenilen_nem;
        EEPROM.write(5, istenilen_nem);
        EEPROM.commit();
        ////Serial.print("istenilen_nem:");
        //Serial.println(istenilen_nem);
      }
      o_periotsure = doc["Kodur"];
      //Serial.println(o_periotsure);
      //Serial.println(o_periotsure_durum);
      o_calismasure = doc["Kocalis"];
      //Serial.println(o_calismasure_durum);
      //Serial.println(o_calismasure);
      if (o_periotsure != o_periotsure_durum || o_calismasure != o_calismasure_durum)
      {
        //Serial.println("yolluyom");
        o_periotsure_durum = o_periotsure;
        EEPROM.write(14, o_periotsure);
        EEPROM.commit();
        //Serial.println("o_periotsure saved in flash memory");
        o_calismasure_durum = o_calismasure;
        EEPROM.write(15, o_calismasure);
        EEPROM.commit();
        //Serial.println("o_calismasure saved in flash memory");
      }

      client.send("kaydet");
    }
  }

  if (msg.data().substring(0, 5) == "led1:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(5));

    if (error) {
      // //Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      int_IRPWMValue1 = doc["led1"];
    }
    IRPWMValue1 = int_IRPWMValue1 * 2.55;
    //Serial.println("gelen mesaj int_IRPWMValue1");
    //Serial.println(int_IRPWMValue1);
    //Serial.println(IRPWMValue1);

    ledcWrite(15, IRPWMValue1);

  }
  if (msg.data().substring(0, 5) == "led2:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(5));

    if (error) {
      ////Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      int_IRPWMValue2 = doc["led2"];
    }
    IRPWMValue2 = int_IRPWMValue2 * 2.55;
    //Serial.println(int_IRPWMValue2);
    //Serial.println(IRPWMValue2);


    ledcWrite(13, IRPWMValue2);

  }
  if (msg.data().substring(0, 5) == "led3:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(5));

    if (error) {
      //  //Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      int_IRPWMValue3 = doc["led3"];
    }
    IRPWMValue3 = int_IRPWMValue3 * 2.55;
    //Serial.println(int_IRPWMValue3);
    //Serial.println(IRPWMValue3);


    ledcWrite(12, IRPWMValue3);


  }
  if (msg.data().substring(0, 5) == "led4:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(5));

    if (error) {
      ////Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      int_IRPWMValue4 = doc["led4"];
    }
    IRPWMValue4 = int_IRPWMValue4 * 2.55;
    //Serial.print(int_IRPWMValue4);



    //Serial.print("IRLED4:");
    //Serial.println(IRPWMValue4);

    ledcWrite(11, IRPWMValue4);


  }


  if (msg.data().substring(0, 5) == "led5:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(5));
    if (error) {
      //  //Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      int_LEDPWMValue = doc["led5"];

    }
    LEDPWMValue = int_LEDPWMValue * 2.55;
    //Serial.print(int_LEDPWMValue);
    //Serial.println(LEDPWMValue);

    ledcWrite(14, LEDPWMValue);


  }



  if (msg.data().substring(0, 6) == "motor:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(6));

    if (error) {
      ////Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      istenilen_nem = doc["motor"];
    }

    //Serial.print(istenilen_nem);


  }

  if (msg.data().substring(0, 4) == "Odur") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(4));

    if (error) {
      // //Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      o_periotsure = doc["o_periotsure"];
      o_calismasure = doc["o_calismasure"];
      if (o_periotsure == 0 || o_calismasure == 0) {
        if (o_periotsure == 0 && o_calismasure != 0) {

          o_tiklandi = false;
          o_calisma = false;
           digitalWrite(h2o, HIGH);
        }
        else{
          o_tiklandi = false;
          o_calisma = false;
           digitalWrite(h2o, LOW);
        }

      }

      else {
        o_tiklandi = true;
        o_calisma = true;
      }
    }
    //Serial.println(o_periotsure);
    //Serial.println(o_calismasure);



  }

  if (msg.data().substring(0, 4) == "Ldur") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(4));

    if (error) {
      ////Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      //Serial.println("led setup ayarlaya basıldı");
      //Serial.print("l_calismasure: ");
      //Serial.println(l_calismasure);

      //Serial.print("l_periotsure: ");
      //Serial.println(l_periotsure);
      l_periotsure = doc["l_periotsure"];
      l_calismasure = doc["l_calismasure"];
      if (l_periotsure == 0 || l_calismasure == 0) {
        if (l_periotsure == 0 && l_calismasure != 0) {
          l_tiklandi = false;
          l_calisma = false;

        }
        else {

          //Serial.println("olacağımm");
          l_tiklandi = false;
          l_calisma = false;
          int int_IRPWMValue1 = 0;
          int int_IRPWMValue2 = 0;
          int int_IRPWMValue3 = 0;
          int int_IRPWMValue4 = 0;
          ledcWrite(IRChannel1, 0);
          ledcWrite(IRChannel2, 0);
          ledcWrite(IRChannel3, 0);
          ledcWrite(IRChannel4, 0);
        }

      }
      else {
        l_tiklandi = true;
        l_calisma = true;
      }
    }

    //Serial.print("burada l_periotsure: ");
    //Serial.println(l_periotsure);

    //Serial.print("burada l_calismasure: ");
    //Serial.println(l_calismasure);



  }

  if (msg.data().substring(0, 9) == "sicaklik:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(9));

    if (error) {
      //   //Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      istenilen_sicaklik = doc["sicaklik"];

    }
    //Serial.println(istenilen_sicaklik);



  }
  if (msg.data().substring(0, 6) == "setup:") {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, msg.data().substring(6));

    if (error) {
      ////Serial.print(F("deserializeJson() failed: "));
      ////Serial.println(error.f_str());
      return;
    }
    else {
      girilen_min_sicaklik = doc["min"];
      girilen_max_sicaklik = doc["max"];
    }
    //Serial.println("arayüzden gelen girilen_min_sicaklik");
    //Serial.println(girilen_min_sicaklik);

  }


}



void loop() {
  WebsocketsClient client;
  if (socket_server.poll()) {
    client = socket_server.accept();

    client.onMessage(handle_message);
  }

  //if(client.available()) {
  while (client.available()) {


    client.poll();
    fb = esp_camera_fb_get();
    client.sendBinary((const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    fb = NULL;
    unsigned int data[6];
    // ledcWrite(ledChannel, 50);
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Send 16-bit command byte
    Wire.write(0x2C);
    Wire.write(0x06);
    // Stop I2C transmission
    Wire.endTransmission();
    //delay(300);
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Stop I2C Transmission
    Wire.endTransmission();
    // Request 6 bytes of data
    Wire.requestFrom(Addr, 6);
    // Read 6 bytes of data
    // temp msb, temp lsb, temp crc, hum msb, hum lsb, hum crc

    /* unsigned long simdiki_sensor_sure = millis();
      //Serial.println(sensor_sure);
      //Serial.println(simdiki_sensor_sure);*/
    //if(sensor_sure <= simdiki_sensor_sure){

    if (Wire.available() == 6)
    {
      /*if (sensor_sure == 0) {
        sensor_sure = millis();
        }*/
      // sensor_sure = millis() + 5000;
      data[0] = Wire.read();
      data[1] = Wire.read();
      data[2] = Wire.read();
      data[3] = Wire.read();
      data[4] = Wire.read();
      data[5] = Wire.read();

    }
    //}else{

    // }
    float cTemp = mlx.readObjectTempC();


    ////Serial.print("Ambient = "); //Serial.print(mlx.readAmbientTempC());
    ////Serial.print("*C\tObject = "); //Serial.print(mlx.readObjectTempC());//Serial.println("*C");


    // Convert the data
    int temp2 = (data[0] * 256) + data[1];
    float cTemp2 = -45.0 + (175.0 * temp2 / 65535.0); //ortam sıcaklığı
    //float fTemp = (cTemp * 1.8) + 32.0;
    float humidity2 = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;



    String sicaklik1; //YANİ SOL SICAKLIK ORTAM SICAKLIĞI
    String nem1;

    String sicaklik2; //YANİ SAĞ SICAKLIK VÜCUT SICAKLIĞI

    sicaklik1 = String(cTemp2);
    nem1 = String(humidity2);

    sicaklik2 = String(cTemp);


    //ORTAM SICAKLIĞI SENSÖRÜNDEN GELEN
    if (cTemp2 > 100 || cTemp2 < 0) {

      client.send("sol_sicaklik:Hata");
    }
    else {

      client.send("sol_sicaklik:" + sicaklik1);
    }

    if (humidity2 > 100 || humidity2 < 0) {
      client.send("sol_nem:Hata");
    }
    else {
      ort_humidity = humidity2;
      client.send("sol_nem:" + nem1);
    }
    //VÜCUT SICAKLIĞI SENSÖRÜNDEN GELEN
    if (cTemp > 100 || cTemp < 0) {
      client.send("sag_sicaklik:Hata");

    }
    else {
      ort_cTemp = cTemp;
      client.send("sag_sicaklik:" + sicaklik2);
    }


    if (ilk_acilma == false) {

      String json = "{" ;
      json = json + "\"girilen_min_sicaklik\":" + girilen_min_sicaklik +
             ",\"girilen_max_sicaklik\":" + girilen_max_sicaklik +
             ",\"istenilen_sicaklik\":" + istenilen_sicaklik +
             ",\"l_periotsure\":" + l_periotsure +
             ",\"l_calismasure\":" + l_calismasure +
             ",\"IRPWMValue1\":" + int_IRPWMValue1 +
             ",\"IRPWMValue2\":" + int_IRPWMValue2 +
             ",\"IRPWMValue3\":" + int_IRPWMValue3 +
             ",\"IRPWMValue4\":" + int_IRPWMValue4 +
             ",\"LEDPWMValue\":" + int_LEDPWMValue +
             ",\"istenilen_nem\":" + istenilen_nem +
             ",\"o_periotsure\":" + o_periotsure +
             ",\"o_calismasure\":" + o_calismasure +
             "}";
      client.send("veriler" + json);
      ilk_acilma = true;
    }




    //ledcWrite(IRChannel1,IRPWMValue1);
    //ledcWrite(IRChannel2,IRPWMValue2);
    // ledcWrite(IRChannel3,IRPWMValue3);
    //ledcWrite(IRChannel4,IRPWMValue4);
    // ledcWrite(LEDChannel,LEDPWMValue);
    if (o_tiklandi == true) {

      if (o_calisma == true) {

        o_calismasuref();

      } else {
        o_periotsuref();

      }
    }

    if (ort_humidity < istenilen_nem) {

      digitalWrite(motor, HIGH);
    } else {
      digitalWrite(motor, LOW);
    }


    if (ort_cTemp < istenilen_sicaklik) {
      if (kontrol == false) {
        if (l_tiklandi == true) {
          if (l_calisma == true) {

            l_calismasuref();

          } else {
            l_periotsuref();

          }
        }
      }
      else if (kontrol == true) {
        l_calisma = true;
        l_calisma_ilk = false;
        l_periot_ilk = false;
        kontrol = false;
        l_calismasuref();
      }
    }
    else {
      kontrol == true;
      ledcWrite(IRChannel1, 0);
      ledcWrite(IRChannel2, 0);
      ledcWrite(IRChannel3, 0);
      ledcWrite(IRChannel4, 0);
    }



    // client.close();
  }
  ilk_acilma = false;
  //return;
  unsigned int data[6];
  // ledcWrite(ledChannel, 50);
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Sesend 16-bit command byte
  Wire.write(0x2C);
  Wire.write(0x06);
  // Stop I2C transmission
  Wire.endTransmission();
  //delay(300);
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
  //second write and read
  // Convert the data
  int temp2 = (data[0] * 256) + data[1];
  float cTemp2 = -45.0 + (175.0 * temp2 / 65535.0); //ortam sıcaklığı

  float humidity2 = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;

  float cTemp = mlx.readObjectTempC();

  String sicaklik1; //YANİ SOL SICAKLIK ORTAM SICAKLIĞI
  String nem1;

  String sicaklik2; //YANİ SAĞ SICAKLIK VÜCUT SICAKLIĞI

  sicaklik1 = String(cTemp2);
  nem1 = String(humidity2);

  sicaklik2 = String(cTemp);

  //ORTAM SICAKLIĞI SENSÖRÜNDEN GELEN
  if (cTemp2 > 100 || cTemp2 < 0) {
    client.send("sol_sicaklik:Hata");
  }
  else {
    client.send("sol_sicaklik:" + sicaklik1);
  }

  if (humidity2 > 100 || humidity2 < 0) {

    client.send("sol_nem:Hata");
  }
  else {
    ort_humidity = humidity2;
    client.send("sol_nem:" + nem1);
  }
  //VÜCUT SICAKLIĞI SENSÖRÜNDEN GELEN
  if (cTemp > 100 || cTemp < 0) {
    client.send("sag_sicaklik:Hata");
  }
  else {
    ort_cTemp = cTemp;
    client.send("sag_sicaklik:" + sicaklik2);
  }

  //ledcWrite(IRChannel1,IRPWMValue1);
  //ledcWrite(IRChannel2,IRPWMValue2);
  // ledcWrite(IRChannel3,IRPWMValue3);
  //ledcWrite(IRChannel4,IRPWMValue4);
  // ledcWrite(LEDChannel,LEDPWMValue);
  if (o_tiklandi == true) {

    if (o_calisma == true) {

      o_calismasuref();

    } else {
      o_periotsuref();

    }
  }

  if (ort_humidity < istenilen_nem) {
    /*  //Serial.println(istenilen_nem);
      //Serial.println(ort_humidity);
       //Serial.println("nemçalıştım");*/
    digitalWrite(motor, HIGH);
  } else {
    /*//Serial.println(istenilen_nem);
      //Serial.println(ort_humidity);
      //Serial.print+ln("nemdurdum");*/
    digitalWrite(motor, LOW);
  }


  if (ort_cTemp < istenilen_sicaklik) {
    if (kontrol == false) {
      if (l_tiklandi == true) {
        if (l_calisma == true) {

          l_calismasuref();

        } else {
          l_periotsuref();

        }
      }
    }
    else if (kontrol == true) {
      l_calisma = true;
      l_calisma_ilk = false;
      l_periot_ilk = false;
      kontrol = false;
      l_calismasuref();
    }
  }
  else {
    kontrol == true;
    ledcWrite(IRChannel1, 0);
    ledcWrite(IRChannel2, 0);
    ledcWrite(IRChannel3, 0);
    ledcWrite(IRChannel4, 0);
  }


}
