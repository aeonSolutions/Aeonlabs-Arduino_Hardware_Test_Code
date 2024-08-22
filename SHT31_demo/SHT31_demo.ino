//
//    FILE: SHT31_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: https://github.com/RobTillaart/SHT31


#include "Wire.h"
#include "SHT31.h"

#define SHT31_ADDRESS   0x44

uint32_t start;
uint32_t stop;

SHT31 sht;


void setup(){
  pinMode(38, OUTPUT);    // sets the digital pin 38 as output
  digitalWrite(38, HIGH); // sets the digital pin 38 on 
  delay(1000);
  
  Wire.begin(9 , 10 ); //, 100000 );
  Wire.setClock(100000);
  Wire.setTimeout(2000);
  
  delay(5000);
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("SHT31_LIB_VERSION: \t");
  Serial.println(SHT31_LIB_VERSION);

  bool result = sht.begin();
  if (result){
    Serial.println("found SHT");
  }else{
    Serial.print("SHT not found");
  }
  
  uint16_t stat = sht.readStatus();
  Serial.print(stat, HEX);
  Serial.println();
  delay(5000);
}


void loop()
{
  start = micros();
  sht.getError();
  sht.read();         //  default = true/fast       slow = false
  stop = micros();
  Serial.print("\t");
  Serial.print( sht.getError(), HEX );
  
  Serial.print("\t");
  Serial.print(stop - start);
  Serial.print("\t");
  Serial.print(sht.getTemperature(), 1);
  Serial.print("\t");
  Serial.println(sht.getHumidity(), 1);
  delay(100);
}


//  -- END OF FILE --
