/*
 Copyright (c) 2024 Miguel Tomas, http://www.aeonlabs.science  https://github.com/aeonSolutions

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

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define log_I(x)  mySerial.println(x)
#else
  #define log_I(x)
#endif

#include <HardwareSerial.h>
#if defined(CONFIG_IDF_TARGET_ESP32) 
    #define mySerial Serial2
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    #define mySerial Serial1
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
   #define mySerial Serial2
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
   #define mySerial Serial1
#endif


#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Pin connected to the ALERT/RDY signal for new sample notification.
constexpr int READY_PIN = 19;
bool ads115_offline = false;

#include <Wire.h>
#define SDA_PIN 5 
#define SCL_PIN 6
TwoWire I2C_BUS(0);
#define  LED_GREEN 18

#include <FS.h>            // Include the SPIFFS or LittleFS library
#include "SPIFFS.h"   // Use this for ESP32
#include "mbedtls/base64.h"

#include "esp_sntp.h"

#include "time.h"
#include "ESP32Time.h"
ESP32Time rtc(0);
uint32_t NTP_update_interval = 0;
long int last_ntp_sync = millis();
// NTP Time Server
const char* ntpServer_1 = "0.pool.ntp.org";
const char* ntpServer_2 = "time.nist.gov";
const char* ntpServer_3 = "1.pool.ntp.org";
const long timezone = 0;  // (utc+) TZ in hours 
const byte daysavetime = 1;

#include <WiFi.h>          // For ESP32 (or use <ESP8266WiFi.h> for ESP8266)
#include <NetworkClient.h>
#include <WiFiClientSecure.h>    // Secure client for HTTPS
#include <HTTPClient.h>
#include <SD.h>            // SD card library (optional, if reading from SD card)
#include <SPI.h>
#include <Base64.h>        // Base64 library (optional, for encoding chunks if necessary)

// Replace with your credentials
const char* ssid = "   ";
const char* password = "  ";

// GitHub credentials
const char* githubToken = "  "; // Replace with your GitHub token
const char* githubUser = "aeonSolutions";              // Replace with your GitHub username
const char* githubRepo = "AeonLabs-Safety-Health";                  // Replace with your GitHub repository
const char* githubBranch = "main";  
// File details
String filePath = "/dataset.csv";                           // The path to the file on the filesystem (SPIFFS/LittleFS)
const char* githubFilePath = "Measurements/datasets/dataset.csv";                      // Path in the GitHub repository

File fileToSend;
// Port and headers
const char* githubHost = "api.github.com";
const int httpsPort = 443;  // HTTPS port

String latestCommitSHA;
String latestTreeSHA;

// *******************************************************************
void BlinkLED(int LED, int duration) {
  digitalWrite(LED, HIGH);   // sets the digital pin 13 on
  delay(duration * 1000);    // waits for a second
  digitalWrite(LED, LOW);   // sets the digital pin 13 off 
}

//*******************************************************************
void get_time(){
  tm timeinfo;
  log_I("Connecting to the Time Server....");
  // Init and get the time
  sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
  configTime( 3600*timezone, daysavetime*3600, ntpServer_1, ntpServer_2, ntpServer_3 );
  timeval tv;
  sntp_sync_time(&tv);
  log_I("Waiting for Time Sync");
  
  if (getLocalTime(&timeinfo) )
    rtc.setTimeStruct(timeinfo);
  
  get_sntp_sync_status();
  get_sntp_sync_mode();

  NTP_update_interval = sntp_get_sync_interval(); // in milisec 
  log_I("NTP Sync Interval = " + String(NTP_update_interval) );

  BlinkLED(LED_GREEN, 1);     
}

//*******************************************************************
void connectWIFI(int attempts, int duration){
    for(int i=0; i<attempts; i++){

      log_I("Attempt n. " + String(i+1) + " to  connect WIFI" );
  
      WiFi.disconnect();
      WiFi.mode(WIFI_STA);
      delay(100);
      WiFi.begin(ssid, password);
      WiFi.setTxPower( WIFI_POWER_11dBm );
      long int startConn = millis();
      while ( WiFi.status() != WL_CONNECTED &&   (millis() - startConn < (duration*1000) ) ) {
        delay(1000);
        log_I(".");
      }
      
      if ( WiFi.status() == WL_CONNECTED ){
        log_I("\nConnected to WiFi!");
        BlinkLED(LED_GREEN, 1);
        return;
      }     
    }
}

//********************************************************************
void get_sntp_sync_status(){
  sntp_sync_status_t syncStatus = sntp_get_sync_status();
  switch (syncStatus) {
    case SNTP_SYNC_STATUS_RESET:
      log_I("SNTP_SYNC_STATUS_RESET");
      break;

    case SNTP_SYNC_STATUS_COMPLETED:
      log_I("SNTP_SYNC_STATUS_COMPLETED");
      break;

    case SNTP_SYNC_STATUS_IN_PROGRESS:
      log_I("SNTP_SYNC_STATUS_IN_PROGRESS");
      break;

    default:
      log_e("Unknown Sync Status");
      break;
  }
}

//******************************************************************
void get_sntp_sync_mode(){
  sntp_sync_mode_t mode = sntp_get_sync_mode();
  switch (mode) {
    case SNTP_SYNC_MODE_IMMED:
      log_I("SNTP_SYNC_MODE_IMMED");
      break;

    case SNTP_SYNC_MODE_SMOOTH:
      log_I("SNTP_SYNC_MODE_SMOOTH");
      break;

    default:
      log_e("Unknown Sync Mode");
      break;
  }
}

// ******************************************************************
void setup() {
  I2C_BUS.begin(SDA_PIN, SCL_PIN, 100000);

  // Initialize device
  #if defined(CONFIG_IDF_TARGET_ESP32) 
    mySerial.begin(115200);
  #elif defined(CONFIG_IDF_TARGET_ESP32C3)
    mySerial.begin(115200, SERIAL_8N1, 20, 21); // rxpin, txpin
  #elif defined(CONFIG_IDF_TARGET_ESP32S2)
    mySerial.begin(115200);
  #elif defined(CONFIG_IDF_TARGET_ESP32S3)
    mySerial.begin(115200, SERIAL_8N1, 18, 17);// rxpin, txpin
  #endif

  log_I("Serial started....");
  delay(2000);

  pinMode(LED_GREEN, OUTPUT);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);

  log_I("Initialize filesystem...");
  // Initialize filesystem
  if (!SPIFFS.begin(true)) {
      log_I("Failed to mount file system");
      while (1){
        BlinkLED(LED_GREEN, 5);
      }
  }else{
      log_I("done!");
      BlinkLED(LED_GREEN, 1);      
  }

  // Connect to WiFi
  connectWIFI(5, 10);
  if ( WiFi.status() == WL_CONNECTED ){
    delay(1000);
    get_time();
  }

  if ( createNewDatasetFile(filePath) == false ){
    while (1){
      BlinkLED(LED_GREEN, 5);
    }
  }

  log_I("Getting differential reading from AIN0 (P) and AIN1 (N)");
  log_I("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin(0x48, &I2C_BUS)) {
    ads115_offline= true;
    log_I("Failed to initialize ADS.");
    BlinkLED(LED_GREEN, 5);
  }
  pinMode(READY_PIN, INPUT);

  log_I("setup end. ");
}

// -----------------------------------------------------------------------------
String getADSmeasurement(){
  log_I("\nStarting measurements....");
  BlinkLED(LED_GREEN, 1);
  int16_t res1;
  int16_t res2 ;
  float tension_A1;
  float tension_A2;
  float tension_dif = 0.0;

  float energy_A1;
  float energy_A2;
  float energy_dif = 0.0;
  
  if (ads115_offline){
    res1= 1.0;
    res2= 2.0; 
    tension_A1  = 1.0;
    tension_A2  = 2.0;
  }else{
    res1= ads.readADC_SingleEnded(1);
    res2= ads.readADC_SingleEnded(2);
    tension_A1  = ads.computeVolts(res1);
    tension_A2  = ads.computeVolts(res2);
  }
  tension_dif = tension_A1 - tension_A2;
    
  log_I("Voltage A1: " +String(tension_A1)+ " mV" ); 
  log_I("Voltage A2: " +String(tension_A2)+ " mV" ); 
  log_I("Differential 1-2: " + String(tension_dif) + " mV");
  
  log_I("Energy A1: " + String(energy_A1,10) + " mW" ); 
  log_I("Energy A2: " + String(energy_A2,10) + " mW" ); 
  log_I("Differential 1-2: " + String(energy_dif,10) + " mW");

  //  E = 1/2 x C x V^2 -- Energy equals one-half times the capacitance (in farads) times the square of the voltage.
  //  The unit is watt-seconds, abbreviated Ws, or W-s
  energy_A1 = (float)0.5 * (float)100 * (float)0.000001 * (float)tension_A1;
  energy_A2 = (float)0.5 * (float)100 * (float)0.000001 * (float)tension_A2;
  energy_dif = energy_A1 - energy_A2;


  String dataRecord = String( rtc.getTimeDate(true) ) + ";" + String(tension_A1)+";"+ String(tension_A2)+";"+ String(tension_dif) + ";" + String(energy_A1,10)+";"+ String(energy_A2,10)+";"+ String(energy_dif,10)  +";\n";     
  return dataRecord;
}

/******************************************************************************************
 * Main Loop
 *****************************************************************************************/
unsigned long MEASUREMENT_INTERVAL        = 0.1 * 60 * 1000; // 10 sec //minutes to mili
unsigned long MEASUREMENT_INTERVAL_GITHUB = 0.2 * 60 * 1000;
long int last_upload                      = millis();
long int last_github_upload               = millis();

// adc_power_release()

void loop() {
  delay(100);
  if (millis() - last_ntp_sync > NTP_update_interval ){
    get_time();
    last_ntp_sync= millis();
  }

  // do measurements
  if (millis() - last_upload > MEASUREMENT_INTERVAL) {
    writeDataRecord( getADSmeasurement() );
    last_upload = millis();
  }

  if (millis() - last_github_upload > MEASUREMENT_INTERVAL_GITHUB) {
    log_I("Starting upload....");
    // get the SHA of the base tree for the file
    String oldFileSHA = getFileSHA();
   // PUSH the new updated file 
    String newfileSHA = uploadFileInChunks();
    // create a new tree
    String newTreeSHA = createTree(newfileSHA);
    // get parent 
    String paretSHA = getParents();
    // Commit the tree
    String commitSHA = createCommit(newTreeSHA, parentSHA);
    // Update the reference (branch) to point to the new commit
    updateReference(commitSHA);

    last_github_upload = millis();
  }

}

//-------------------------------------------------------------------------------
// Function to send a GET request to GitHub and extract the file's SHA blob
String getFileSHA() {
  if (WiFi.status() != WL_CONNECTED) {
    log_I("WIFI not available. skipping upload.");
    return "-2";
  }
  
  log_I("Free internal heap before TLS " + String( ESP.getFreeHeap()) );

  String sha = "";  

  // Create the GitHub API URL
  String githubUrl = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/contents/" + String(githubFilePath) + "?ref=" + String(githubBranch);

  log_I("Requesting URL: " + githubUrl );

  // Begin the GET request
  HTTPClient http;
  http.begin(githubUrl);
  http.addHeader("Authorization", "token " + String(githubToken));
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String responseBody = http.getString();
    http.end();
    log_I("SHA Blob request made successfully!");
    log_I("Response: " + responseBody);
    String FileSHA = extractSHA(responseBody);
    if (FileSHA.length() <40 ){
      log_I("Incorrect Blob size (<40)");
      return;
    }
    return FileSHA;
  } else {
    log_I("Failed to retrieve file: " + http.errorToString(httpResponseCode) );
    http.end();
    return "-3";
  }
}

// ------------------------------------------------------------------------------------------------------
String uploadFileInChunks() {
  WiFiClientSecure client;
  
  client.setInsecure();  // Use this only for testing; skips certificate verification
  if (client.connect(githubHost, 443) == false ) {
    log_I ("failed to connect to " + String(githubHost) );
  }

  String url = "/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/blobs";
  String encodedChunk;
  String fileBlobs = "";
  
  String json_part_1 = "{ \"content\": \"";
  String json_part_2 = "\", \"encoding\": \"base64\" }";

  const size_t chunkSize = 132;  // Adjust based on available memory
  uint8_t buffer[chunkSize];

  File fileToSend = SPIFFS.open(filePath, "r");
  if (!fileToSend) {
      log_I("Failed to open file for reading");
      BlinkLED(LED_GREEN, 5);
      return "-5";
  }
  unsigned long fileSize = fileToSend.size(); 
  unsigned long content_len = encode_base64_length(fileSize);

  log_I("File Content size = " + String( fileSize ) );
  log_I("Base 64 Content length = " + String( content_len ) );
  log_I("ratio = " + String( (float)( (float)content_len/fileSize), 2 ) );

  // Send HTTP POST request to create blob
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + String(githubHost) );
  client.println("Authorization: Bearer " + String(githubToken));
  client.println("User-Agent: AeonSolutions");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(content_len + json_part_1.length() +  json_part_2.length() );  // Calculate content length
  client.print("\r\n");
  client.print("\r\n");
  client.print(json_part_1);

  mySerial.print(json_part_1);
  long int temp = 0;
  while (fileToSend.available()) {
    size_t bytesRead = fileToSend.read(buffer, chunkSize);
    encodedChunk = base64::encode(buffer, bytesRead);
    client.print( encodedChunk);
    temp += encodedChunk.length();
    mySerial.print(encodedChunk);
  }
  mySerial.print(json_part_2);

  fileToSend.close();
  log_I("Content len sent: " + String(temp) );

  client.println(json_part_2);
  client.print("\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  // Read the response body
  String responseBody = client.readString();
  client.stop();

  log_I("Response Body:" + responseBody);

  String blobSHA = extractSHA(responseBody);
  log_I("Blob SHA: " + blobSHA);
  if (blobSHA.length() <40 ){
    log_I("Incorrect Blob size (<40)");
    return "-6";
  }
  return blobSHA;
  // Create the tree with the blob SHA
 // createTree(blobSHA);
}

// ----------------------------------------------------------------------------------
String createTree(String blobSHA) {
  if (WiFi.status() != WL_CONNECTED) {
    log_I("WIFI not available. skipping upload.");
    return "-2";
  }

  HTTPClient http;
  String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/trees";

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(githubToken));
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "AeonSolutions");

  // Create tree payload with the blob SHA  : not the  latestTreeSHA
  String payload = "{"
                    "\"base_tree\": \"" + latestTreeSHA + "\","
                    "\"tree\": ["
                    "{ \"path\": \"" + githubFilePath + "\", \"mode\": \"100644\", \"type\": \"blob\", \"sha\": \"" + blobSHA + "\" }"
                    "]"
                    "}";

  log_I("Create Tree payload: " + payload);      

  int httpResponseCode = http.POST(payload);
  http.end();

  if (httpResponseCode > 0) {
    String response = http.getString();
    log_I("Tree creation response: " + response);

    String newTreeSHA = extractSHA(response);
    log_I("New Tree SHA blob: " + newTreeSHA);
    // Commit the tree
    // createCommit(newTreeSHA);
    return newTreeSHA;
  } else {
    log_I("Error creating tree: " + http.errorToString(httpResponseCode));
    return "-3";
  }  
}


// ------------------------------------------------------------------------------------------------------------------
String getParents() {
  if (WiFi.status() != WL_CONNECTED) {
    log_I("WIFI not available. skipping upload.");
    return "-2";
  }

  HTTPClient http;
  String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/refs/heads/" + String(githubBranch);

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(githubToken));
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "AeonSolutions");
  
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String responseBody = http.getString();
    http.end();
    log_I("Parents response: " + responseBody);
    String blobSHA = extractSHA(responseBody);
    log_I("Blob SHA: " + blobSHA);
    if (blobSHA.length() <40 ){
      log_I("Incorrect Blob size (<40)");
      return "-6";
    }
    return blobSHA;
  } else {
    http.end();
    log_I("Error updating reference: " + http.errorToString(httpResponseCode));
    return "-4";
  }
}

//-------------------------------------------------------------------------------------------
void createCommit(String newTreeSHA, String parentSHA) {
  if (WiFi.status() != WL_CONNECTED) {
    log_I("WIFI not available. skipping upload.");
    return "-2";
  }

  HTTPClient http;
  String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/commits";

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(githubToken));
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "AeonSolutions");

  // Create commit payload
  String commitMessage = "Upload file to GitHub from Arduino";
  String payload = "{"
                    "\"message\": \"" + commitMessage + "\","
                    "\"tree\": \"" + newTreeSHA + "\","
                    "\"parents\": [\"" + parentSHA + "\"]"
                    "}";

  log_I("Create Commit payload: " + payload);      

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    http.end();

    log_I("Commit creation response: " + response);

    String newCommitSHA = extractSHA(response);

    log_I("New Commit SHA blob: " + newCommitSHA);
    if (newCommitSHA.length() <40 ){
      log_I("Incorrect Blob size (<40)");
      return "-6";
    }
    return newCommitSHA;
  } else {
    http.end();
    log_I("Error creating commit: " + http.errorToString(httpResponseCode));
    return "-4";
  }  
}

// ------------------------------------------------------------------------------------------------------------------
String updateReference(String newCommitSHA) {
  if (WiFi.status() != WL_CONNECTED) {
    log_I("WIFI not available. skipping upload.");
    return "-2";
  }

  HTTPClient http;
  String url = "https://api.github.com/repos/" + String(githubUser) + "/" + String(githubRepo) + "/git/refs/heads/" + String(githubBranch);

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(githubToken));
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "AeonSolutions");

  // Update ref payload
  String payload = "{ \"sha\": \"" + newCommitSHA + "\", \"force\": true }";
  
  log_I("Update References payload: " + payload);      
  
  int httpResponseCode = http.PATCH(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    String code = http.errorToString(httpResponseCode);
    http.end();
    log_I("Reference update response ("+ code +"): " + response);
    return code;
  } else {
    log_I("Error updating reference: " + http.errorToString(httpResponseCode));
    http.end();
    return "-4";
  }
}

// --------------------------------------------------------------------------
//       support functions
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Function to parse the JSON response and extract the 'sha' field
String extractSHA(String response) {
  int posStart = response.indexOf("sha");
  int posEnd = response.indexOf("\"", posStart + 8 );
  if (posStart == -1){
    log_I("SHA Blob not found");
    return "-4";
  }

  String sha = response.substring(posStart+6, posEnd);
  return sha;
}

//--------------------------------------------------------------------------------------------------------------------
void writeDataRecord(String dataRecord){
  File file = SPIFFS.open(filePath, FILE_APPEND);
  if (!file) {
    log_I("Failed to open the  file");
    BlinkLED(LED_GREEN, 5);
    return;
  }

  // Write data to the file
  if (file.print(dataRecord)) {
      log_I("Data written successfully!");
  } else {
    log_I("Failed to write data");
    BlinkLED(LED_GREEN, 5);
  }

  // Close the file
  file.close();
}

//--------------------------------------------
bool createNewDatasetFile(String file_Path){
  // Check if the file exists
  if (!SPIFFS.exists(file_Path)) {
    // Create and open a new file
    File file = SPIFFS.open(file_Path, FILE_WRITE);
    if (!file) {
      log_I("Failed to create file");
      return false;
    }  
  
    log_I("File created successfully!");

    BlinkLED(LED_GREEN, 1);
    // Write data to the file
    if (file.print("Date & Tme; V(A1) mV; V(A2) mV; V(diff) mV; E(A1) mW; E(A2) mW; E(diff) mW; \n")) {
      log_I("Data written successfully!");
      // Close the file
      file.close();
      return true;   
    } else {
      log_I("Failed to write data");
      BlinkLED(LED_GREEN, 5);
      // Close the file
      file.close();
      return false;   
    }
  }
}

// -----------------------------------------------------------------------------------------------------
String encodeBase64(uint8_t buffer[], size_t bytesRead){

  unsigned int base64len = encode_base64_length(bytesRead);
  log_I("base64 size = " + String( base64len ) );

  unsigned char encodedContent[base64len];
  
  size_t output_size = 0;
  // note input is consumed in this step: it will be empty afterwards
  // Base64 encode the file content (GitHub API requires base64 encoded content)
  // int err =  base64_encode(output, pBuffer, fileSize); 
  int err = mbedtls_base64_encode(encodedContent, base64len, &output_size, buffer , bytesRead);
  if (err != 0) {
    switch(err){
      case(-0x002A):
        log_I("error base64 encoding, error " +String(err) + " buffer too small. Buff size: " + String(base64len) );
        break;
      case(-0x002C):
        log_I("error base64 encoding, error " +String(err) + " invalid char. buff size: " + String(base64len) );
        break;
      default:
        log_I("error base64 encoding, error " +String(err) + " unk err.  buff size: " + String(base64len) );
        break;
    }
    BlinkLED(LED_GREEN, 5);  
    return "-1";	
  }

return String( (char*)encodedContent);
}

// --------------------------------------------------------------------------------------------------------------------------
 unsigned long encode_base64_length(unsigned long input_length) {
    unsigned long base = input_length * 4 / 3;  // Adjust size for Base64 encoding
    int mod = base % 4;
    return (base + mod); 
 }
