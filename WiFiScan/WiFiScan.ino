/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#include "WiFi.h"
const char* ssid     = "TheScientist"; // Change this to your WiFi SSID
const char* password = "migalhas"; // Change this to your WiFi password
long timer=0;

wifi_power_t wifiPWR[11];
String wifiPWRstr[11];

int pos = 0;

void setup(){
    /*  https://github.com/G6EJD/ESP32-8266-Adjust-WiFi-RF-Power-Output/blob/main/README.md
     * Available ESP32 RF power parameters: 
     * WIFI_POWER_19_5dBm 19.5dBm (19.5dBm output, highesc:\Users\mtpsi\Desktop\Smart-DoorWindow-sensor-main.zipt supply current ~150mA) 
     * WIFI_POWER_19dBm // 19dBm 
     * WIFI_POWER_18_5dBm // 18.5dBm 
     * WIFI_POWER_17dBm // 17dBm 
     * WIFI_POWER_15dBm // 15dBm 
     * WIFI_POWER_13dBm // 13dBm 
     * WIFI_POWER_11dBm // 11dBm 
     * WIFI_POWER_8_5dBm // 8dBm 
     * WIFI_POWER_7dBm // 7dBm 
     * WIFI_POWER_5dBm // 5dBm 
     * WIFI_POWER_2dBm // 2dBm 
     * WIFI_POWER_MINUS_1dBm // -1dBm (For -1dBm output, lowest supply current ~120mA)
     */
    wifiPWR[0]=WIFI_POWER_19_5dBm;
    wifiPWR[1]=WIFI_POWER_19dBm;
    wifiPWR[2]=WIFI_POWER_18_5dBm;
    wifiPWR[3]=WIFI_POWER_17dBm;
    wifiPWR[4]=WIFI_POWER_15dBm;
    wifiPWR[5]=WIFI_POWER_13dBm;
    wifiPWR[6]=WIFI_POWER_11dBm;
    wifiPWR[7]=WIFI_POWER_8_5dBm;
    wifiPWR[8]=WIFI_POWER_7dBm;
    wifiPWR[9]=WIFI_POWER_5dBm;
    wifiPWR[10]=WIFI_POWER_2dBm;
  
    wifiPWRstr[0] = "19_5dBm";
    wifiPWRstr[1] = "19dBm";
    wifiPWRstr[2] = "18_5dBm";
    wifiPWRstr[3] = "17dBm";
    wifiPWRstr[4] = "15dBm";
    wifiPWRstr[5] = "13dBm";
    wifiPWRstr[6] = "11dBm";
    wifiPWRstr[7] = "8_5dBm";
    wifiPWRstr[8] = "7dBm";
    wifiPWRstr[9] = "5dBm";
    wifiPWRstr[10] = "2dBm";
    
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected.
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
   
    Serial.println("Setup done");
}

void loop(){
    WIFIscanNetworks();
        // We start by connecting to a WiFi network

    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to the SSID ");
    Serial.println(ssid);
    Serial.println("trying to connect using WIFI power("+String(pos)+") " + wifiPWRstr[pos] );
    WiFi.begin(ssid, password);
    WiFi.setTxPower(wifiPWR[pos]);
    timer=millis();
    while (WiFi.status() != WL_CONNECTED && (millis()-timer) < 15000) {
        
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("connected successfully" );
      Serial.println("");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }else{
      Serial.println("failed to connect" );
    }
    pos++;
    if (pos>10)
      pos=0;
    // Wait a bit before scanning again.
    delay(5000);
}


// *********************************************************
void WIFIscanNetworks(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  String dataStr = "";
  if(n == WIFI_SCAN_FAILED){ 
     Serial.println("Wifi Scan Failed");
  }else if (n == 0) {
    Serial.println( "no nearby WIFI networks found\n");
  } else {
    dataStr = "\n=====    " + String(n) + " WiFi networks nearby =============\n";
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      dataStr += padString( String(i + 1), 3)+ ": ";
      dataStr += padString( String(WiFi.SSID(i)), 33);
      dataStr += " RSSI:" + padString( String(WiFi.RSSI(i)), 4) + "db (";
      dataStr +=  padString( String( RSSIToPercent(WiFi.RSSI(i) ) ), 3) + "%)";
      dataStr += " CH:" +  padString( String(WiFi.channel(i)), 2);
      dataStr += WIFIencryptionType(WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " pwd: no\n" : " pwd: yes\n";
    }
    dataStr += "===============================\n";
    Serial.println( dataStr );
  }
  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();
}

//************************************
String WIFIencryptionType(int i){
       switch (i){
            case WIFI_AUTH_OPEN:
                return "open";
                break;
            case WIFI_AUTH_WEP:
                return "WEP";
                break;
            case WIFI_AUTH_WPA_PSK:
                return "WPA";
                break;
            case WIFI_AUTH_WPA2_PSK:
                return "WPA2";
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                return "WPA+WPA2";
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                return "WPA2-EAP";
                break;
            case WIFI_AUTH_WPA3_PSK:
                return "WPA3";
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                return "WPA2+WPA3";
                break;
            case WIFI_AUTH_WAPI_PSK:
                return "WAPI";
                break;
            default:
                return "unknown";
        }
}
/// *************************************
String padString(String str, int size){
  uint8_t padLen = str.length();
  for(int i=0 ; i< ( size - padLen); i++ ){
    str += " ";
  }
return str;
}

//*********************************
uint8_t RSSIToPercent(long rssi) {
  if (rssi >= -50) {
    return 100;
  }else if (rssi <= -100){
    return 0;
  }else{
     return (rssi + 100) * 2;
  }
  // ▂▄▆█
}
// ********************************************************
