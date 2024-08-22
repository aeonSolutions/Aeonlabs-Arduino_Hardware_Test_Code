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
