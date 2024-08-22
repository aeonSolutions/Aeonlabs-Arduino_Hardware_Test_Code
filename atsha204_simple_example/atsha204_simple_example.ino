// unique figerprint data ID
#include "security/sha204_i2c.h"
#include "atsha204.h"
#include "Wire.h"

// UNIQUE FingerPrint ID for Live data Authenticity and Authentication ******************************
atsha204Class sha204 = atsha204Class();


void setup(){
  pinMode(38, OUTPUT);    // sets the digital pin 38 as output
  digitalWrite(38, HIGH); // sets the digital pin 38 on 
  delay(1000);
  
  Wire.begin(9 , 10 ); //, 100000 );
  Wire.setClock(100000);
  Wire.setTimeout(2000);
  
  delay(5000);
  
  Serial.begin(115200);
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("ATSHA204 test"));

}

void loop(){
  t_runFingerPrintIDtests(sha204);
  delay(2000);
}
