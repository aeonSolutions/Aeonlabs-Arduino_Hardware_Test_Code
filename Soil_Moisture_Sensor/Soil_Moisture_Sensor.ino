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

#include <Arduino.h>
#include <Preferences.h>
#include "driver/touch_pad.h"

void setup() {
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor

  touch_pad_init();
  touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
  touch_pad_config(TOUCH_PAD_NUM11);

  Serial.print(30000); // To freeze the lower limit
Serial.print(" ");
Serial.print(80000); // To freeze the upper limit
Serial.print(" ");
}

void loop() {

  uint32_t output;
  touch_pad_read_raw_data(TOUCH_PAD_NUM11,&output);
  uint32_t output2;
  output2=touchRead(11);
  //Serial.print("out2: ");

  //Serial.print(output2);
  //Serial.print("    out1: ");

  Serial.println(output);
  //Serial.println();
  delay(200);
}
