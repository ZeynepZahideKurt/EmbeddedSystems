#include <Wire.h>
#define I2C_SDA 14
#define I2C_SCL 15

#define Addr 0x44

int temp;
float cTemp,humidity;
void SHT30() {
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

  temp = (data[0] * 256) + data[1];
  cTemp = -45.0 + (175.0 * temp / 65535.0);
  humidity = (100.0 * ((data[3] * 256.0) + data[4])) / 65535.0;
  Serial.print("Temp:"); Serial.println(cTemp);
  Serial.print("humidity:"); Serial.println(humidity);
}
void setup() {
  // put your setup code here, to run once:
Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  SHT30();
}
