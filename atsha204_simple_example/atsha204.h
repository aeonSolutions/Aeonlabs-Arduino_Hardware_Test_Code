#include <Arduino.h>
#include "m_math.h"
#include "security/sha204_i2c.h"

#ifndef ATSHA204_DEF
  #define ATSHA204_DEF

  String t_CryptoGetRandom(atsha204Class sha204);
  String t_CryptoICserialNumber(atsha204Class sha204);

  String t_macChallengeDataAuthenticity(atsha204Class sha204, String text );
  String t_macChallengeDataAuthenticityOffLine(atsha204Class sha204, char dataRow[] );

  uint8_t t_macChallengeExample(atsha204Class sha204);
  uint8_t t_wakeupExample(atsha204Class sha204);
  uint8_t t_serialNumberExample(atsha204Class sha204);
  uint8_t t_randomExample(atsha204Class sha204);
  void t_runFingerPrintIDtests(atsha204Class sha204);
  String t_ErrorCodeMessage(uint8_t code);

#endif
