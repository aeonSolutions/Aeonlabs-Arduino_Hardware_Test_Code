/*
 Copyright (c) 2023 Miguel Tomas, http://www.aeonlabs.science; https://github.com/aeonSolutions

License Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
You are free to:
   Share — copy and redistribute the material in any medium or format
   Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms. Under the following terms:
Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.

NonCommercial — You may not use the material for commercial purposes.

ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under
 the same license as the original.

No additional restrictions — You may not apply legal terms or technological measures that legally restrict others
 from doing anything the license permits.

Notices:
You do not have to comply with the license for elements of the material in the public domain or where your use
 is permitted by an applicable exception or limitation.
No warranties are given. The license may not give you all of the permissions necessary for your intended use. 
For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.


Before proceeding to download any of AeonLabs software solutions for open-source development
 and/or PCB hardware electronics development make sure you are choosing the right license for your project. See 
https://github.com/aeonSolutions/PCB-Prototyping-Catalogue/wiki/AeonLabs-Solutions-for-Open-Hardware-&-Source-Development
 for Open Hardware & Source Development for more information.

*/

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
