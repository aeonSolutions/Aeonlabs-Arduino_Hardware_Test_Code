#include "Wire.h"
// Onboard sensors  *******************************
#include "onboard_sensors.h"
ONBOARD_SENSORS* onBoardSensors = new ONBOARD_SENSORS();
// Interface class ******************************
#include "interface_class.h"
INTERFACE_CLASS* interface = new INTERFACE_CLASS();
// serial comm
#include <HardwareSerial.h>
HardwareSerial UARTserial(0);

#include "mserial.h"
mSerial* mserial = new mSerial(true, &UARTserial);

// Telegram
  #include "telegram.h"
  TELEGRAM_CLASS* telegram = new TELEGRAM_CLASS();



void setup() {
  Wire.begin(8,9, 400000);
  Serial.begin(115200);
      // initialize Telegram
  telegram->init();
  mserial->telegram = telegram;
  
  //init interface ___________________________
  interface->init( telegram, mserial, true); // debug EN ON

  
  // init onboard sensors ___________________________
  onBoardSensors->init(interface, mserial);
  onBoardSensors->initOnboardSensors();

  onBoardSensors->I2Cscanner();
  
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
