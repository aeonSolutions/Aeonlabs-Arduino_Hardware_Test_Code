#include "Wire.h"
#include <HardwareSerial.h>
#if defined(CONFIG_IDF_TARGET_ESP32) 
    #define mySerial Serial2
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    #define mySerial Serial1
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
   #define mySerial Serial2
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
   #define mySerial Serial
#endif

void setup() {
//  pinMode(38, OUTPUT);
//  digitalWrite(38, HIGH); // Enable sensors, inc I2C
  
    Wire.begin(5, 6); // SDA, SCL

  #if defined(CONFIG_IDF_TARGET_ESP32) 
    mySerial.begin(115200);
 #elif defined(CONFIG_IDF_TARGET_ESP32C3)
    mySerial.begin(115200);
 #elif defined(CONFIG_IDF_TARGET_ESP32S2)
    mySerial.begin(115200);
 #elif defined(CONFIG_IDF_TARGET_ESP32S3)
   mySerial.begin(115200, SERIAL_8N1, 18, 17);// rxpin, txpin
 #endif

}

void loop() {
  byte error, address;
  int nDevices = 0;

  delay(5000);

  Serial.println("Scanning for I2C devices ...");
  for(address = 0x01; address < 0x7f; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.printf("I2C device found at address 0x%02X\n", address);
      nDevices++;
    } else if(error != 2){
      Serial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0){
    Serial.println("No I2C devices found");
  }
}
