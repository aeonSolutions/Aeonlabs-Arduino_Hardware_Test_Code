#include "mserial.h"
mSerial* mserial = new mSerial(true);

void setup() {
  // put your setup code here, to run once:
  mserial->DEBUG_TO = mserial->DEBUG_TO_UART;
  mserial->DEBUG_EN = true;
  mserial->DEBUG_TYPE = mserial->DEBUG_TYPE_VERBOSE; // DEBUG_TYPE_INFO;

  mserial->start(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  mserial->printStr("Transmitted TX Value: empty");
  delay(1000);
}
