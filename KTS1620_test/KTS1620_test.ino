//
//    FILE: KTS1620_test.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: https://github.com/RobTillaart/KTS1620

#include "KTS1620.h"
#include <HardwareSerial.h>

//  adjust addresses if needed
KTS1620 KTS_22(0x22);  //  add LEDs to lines      (used as output)


void send(uint8_t reg, uint8_t val){
  Wire.beginTransmission(0x22);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
  delay(1);  

}
void setup()
{
  Wire.begin(13,14);
  Serial2.begin(115200, SERIAL_8N1, 44, 43); // rxpin, txpin

  Serial2.println(__FILE__);
  Serial2.print("KTS1620_LIB_VERSION:\t");
  Serial2.println(KTS1620_LIB_VERSION);

 
//send(0x0D, 0xF3);
 //send(0x42, 0xF0);
 //delay(15000);

/////////////////////////////////////////////////


  KTS_22.begin(0x22);

// The recommended command sequence to program the output pin is to program ODENx (5Ch), the IOCRx, and 
// finally the Configuration Register (0Ch, 0Dh, 0Eh) to set the pins as outputs.

  bool pushPull[3] = {false, false , false};

  KTS_22.outputPortConfig( pushPull );
  KTS_22.configInputOutput(1,"11110011");
  delay(2000);

  KTS_22.outputDriveStrength(0x42, "00000000");
}

void loop(){
  KTS_22.outputDriveStrength(0x42, "11000000");
  delay(5000);
  KTS_22.outputDriveStrength(0x42, "00000000");
  delay(1000);

  KTS_22.outputDriveStrength(0x42, "00110000");
  delay(5000);
  KTS_22.outputDriveStrength(0x42, "00000000");
  delay(1000);

}


//  -- END OF FILE --

