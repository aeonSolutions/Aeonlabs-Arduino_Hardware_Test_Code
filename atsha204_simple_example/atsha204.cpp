
#include "atsha204.h"
#include "security/sha204_i2c.h"

// *************************************************************
String t_CryptoGetRandom( atsha204Class sha204 ){
  uint8_t response[32];
  uint8_t returnValue;
  sha204.simpleWakeup();
  returnValue = sha204.simpleGetRandom(response);
  sha204.simpleSleep();  
  return hexDump(response, sizeof(response));
}

//*****************************************************************
String t_CryptoICserialNumber(atsha204Class sha204){
  uint8_t serialNumber[6];
  uint8_t returnValue;

  sha204.simpleWakeup();
  returnValue = sha204.simpleGetSerialNumber(serialNumber);
  sha204.simpleSleep();
  
  return hexDump(serialNumber, sizeof(serialNumber));
}

//******************************************************************************
String t_macChallengeDataAuthenticity(atsha204Class sha204, String text ){
  static uint32_t n = 0;
  uint8_t command[MAC_COUNT_LONG];
  uint8_t response[MAC_RSP_SIZE];

  int str_len = text.length() + 1;
  char text_char [str_len];
  text.toCharArray(text_char, str_len);
        
  uint8_t challenge[sizeof(text_char)] = {0};
  for (uint32_t i = 0; i < sizeof(challenge); i++){    
    challenge[i] = ' ';
  }
  
  sprintf((char *)challenge, text_char, n++);

  sha204.simpleWakeup();
  uint8_t ret_code = sha204.sha204m_execute(SHA204_MAC, 0, 0, MAC_CHALLENGE_SIZE, 
    (uint8_t *) challenge, 0, NULL, 0, NULL, sizeof(command), &command[0], 
    sizeof(response), &response[0]);
  
  sha204.simpleSleep(); 

  if (ret_code != SHA204_SUCCESS){
    return "SimpleMac failed";
  }

  return hexDump(response, sizeof(response));
}

//***********************************************************************************
String t_macChallengeDataAuthenticityOffLine(atsha204Class sha204, char dataRow[] ){
  static uint32_t n = 0;

  uint8_t challenge[sizeof(dataRow)] = {0}; // MAC_CHALLENGE_SIZE
  uint8_t key[32];

  //Change your key here.
  
  hex2bin("A9CD7F1B6688159B54BBE862F638FF9D29E0FA5F87C69D27BFCD007814BA69C9", key);
  uint8_t mac_offline[MAC_RSP_SIZE];
  for (uint32_t i = 0; i < sizeof(mac_offline); i++){    
    mac_offline[i] = ' ';
  }
  sha204.simpleWakeup();
  int ret_code = sha204.simpleMacOffline(challenge, mac_offline, key);
  sha204.simpleSleep();
  
  char buffer[3] = {' ',' ',' '};
  String hexStr="";
  for (uint32_t i = 0; i < sizeof(mac_offline); i++){    
    snprintf(buffer, sizeof(buffer), "%02X", mac_offline[i]);

    hexStr += String(buffer) + " ";
  }
  return hexStr;
}

//*********************************************************************
uint8_t t_macChallengeExample( atsha204Class sha204 ){
  uint8_t command[MAC_COUNT_LONG];
  uint8_t response[MAC_RSP_SIZE];
  char buffer[3];

  const uint8_t challenge[MAC_CHALLENGE_SIZE] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
  };

  uint8_t ret_code = sha204.sha204m_execute(SHA204_MAC, 0, 0, MAC_CHALLENGE_SIZE, 
    (uint8_t *) challenge, 0, NULL, 0, NULL, sizeof(command), &command[0], 
    sizeof(response), &response[0]);

  for (int i=0; i<SHA204_RSP_SIZE_MAX; i++){
    snprintf(buffer, sizeof(buffer), "%02X", response[i]);
    Serial.print( String(buffer) + " " );

  }
  
  Serial.println("\n Response Code (" + String(ret_code) + ") " + t_ErrorCodeMessage(ret_code) );
  return ret_code;
}

//***************************************************************************
uint8_t t_wakeupExample(atsha204Class sha204){
  uint8_t response[SHA204_RSP_SIZE_MIN];
  uint8_t returnValue;
  char buffer[3];
  
  returnValue = sha204.sha204c_wakeup( &response[0] );
  for (int i=0; i<SHA204_RSP_SIZE_MIN; i++){
    snprintf(buffer, sizeof(buffer), "%02X", response[i]);
    Serial.print( String(buffer)  + " ");

  }
   Serial.println("");
  
  return returnValue;
}

// ******************************************************************
uint8_t t_serialNumberExample(atsha204Class sha204){
  uint8_t serialNumber[9];
  uint8_t returnValue;
  char buffer[3];

  returnValue = sha204.getSerialNumber(serialNumber);
  for (int i=0; i<9; i++){    
    snprintf(buffer, sizeof(buffer), "%02X", serialNumber[i]);
    Serial.print( String(buffer)  + " ");

  }
   Serial.println("");
  
  return returnValue;
}
// ******************************************************************
uint8_t t_randomExample(atsha204Class sha204){
  uint8_t response[32];
  uint8_t returnValue;
  char buffer[65];

  returnValue = sha204.simpleGetRandom(response);
  snprintf(buffer, sizeof(buffer), "%02X", response);
  Serial.print( String(buffer)  + " ");

  return returnValue;
}

//******************************************************
void t_runFingerPrintIDtests(atsha204Class sha204){
    Serial.println("Testing the Unique FingerPrind ID for Sensor Data Measurements ============================");

    Serial.println("\nSending a Wakup Command. Response should be:\r\n4 11 33 43:");
    Serial.println("Response is:");
    t_wakeupExample(sha204);

    Serial.println("\nAsking the SHA204's serial number. Response should be:");
    Serial.println("01 23 xx xx xx xx xx xx xx EE");
    Serial.println("Response is:");
    t_serialNumberExample(sha204);

    Serial.println("\nTesting Random Genenator: ");
    t_randomExample(sha204);

    Serial.println("\nSending a MAC Challenge. Response should be:");
    String challenge = "23 06 67 00 4F 28 4D 6E 98 62 04 F4 60 A3 E8 75 8A 59 85 A6 79 96 C4 8A 88 46 43 4E B3 DB 58 A4 FB E5 73";
    Serial.println(challenge);
    Serial.println("Response is:");
    if (t_macChallengeExample(sha204) == 0x00 ){
    }else{
      Serial.println("Try again. Offline response is: ");
      Serial.println(  t_macChallengeDataAuthenticityOffLine(sha204, (char*) challenge.c_str() ) );
    }
    Serial.println("====================== done ======================================");
   // mserial->printStrln("Testing Sensor Data Validation hashing");
   // mserial->printStrln( t_macChallengeDataAuthenticity(sha204, "TEST IC"));
   // mserial->printStrln("");
}

String t_ErrorCodeMessage(uint8_t code){
  switch(code) {
    case 0x00:
      return "Function succeeded.";
      break;
    case 0x02:
      return "response status byte indicates parsing error";
      break;
    case 0x03:
      return "response status byte indicates command execution error";
      break;
    case 0x04:
      return "response status byte indicates CRC error";
      break;
    case 0x05:
      return "response status byte is unknown";
      break;

    case 0xE0:
      return "Function could not execute due to incorrect condition / state.";
      break;
    case 0xE1:
      return "unspecified error";
      break;
    case 0xE2:
      return "bad argument (out of range, null pointer, etc.)";
      break;
    case 0xE3:
      return "invalid device id, id not set";
      break;
    case 0xE4:
      return "Count value is out of range or greater than buffer size";
      break;
    case 0xE5:
      return "incorrect CRC received";
      break;
    case 0xE6:
      return "Timed out while waiting for response. Number of bytes received is > 0.";
      break;
    case 0xE7:
      return "Not an error while the Command layer is polling for a command response.";
      break;
    case 0xE8:
      return "re-synchronization succeeded, but only after generating a Wake-up";
      break;
    case 0xF0:
      return "Communication with device failed.";
      break;
    case 0xF1:
      return "Timed out while waiting for response. Number of bytes received is 0.";
      break;
      
    default:
      return "error code not found";
      break;
  }

}
