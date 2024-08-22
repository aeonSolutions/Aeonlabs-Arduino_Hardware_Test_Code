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
    mySerial.begin(115200, SERIAL_8N1, 20, 21); // rxpin, txpin
 #elif defined(CONFIG_IDF_TARGET_ESP32S2)
    mySerial.begin(115200);
 #elif defined(CONFIG_IDF_TARGET_ESP32S3)
   mySerial.begin(115200, SERIAL_8N1, 18, 17);// rxpin, txpin
 #endif

}
  byte error, address;
  int nDevices = 0;
void loop() {
  nDevices = 0;
  delay(5000);

  mySerial.println("Scanning for I2C devices ...");
  for(address = 0x01; address < 0x7f; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      if (deviceName(address) != "")
        mySerial.println( deviceName(address) );

      nDevices++;
    } else if(error != 2){
      mySerial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0){
    mySerial.println("No I2C devices found");
  }
  mySerial.println("");
}

String deviceName(byte address){
  switch(address) {
    case 0x64:
      return "0x64 -> ATSHA204a";
      break;
    case 0x6B:
      return "0x6B -> LSM6DS3";
      break;
    case 0x48:
      return "0x48 -> ADS1115";
      break;
    case 0x52:
      return "0x52 -> ENS160";
      break;
    case 0x38:
      return "0x38 -> AHT20";
      break;
    case 0x69:
      return "0x69 -> RV-1805-C3";
      break;
    case 0x76:
      return "0x76 -> BMP280";
      break;
    default:
      mySerial.printf("I2C device found at address 0x%02X\n", address);
      return "";
  }
}

