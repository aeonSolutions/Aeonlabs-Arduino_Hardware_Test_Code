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
