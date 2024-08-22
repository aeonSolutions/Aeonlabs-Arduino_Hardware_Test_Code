/*
 Copyright (c) 2023 Miguel Tomas, http://www.aeonlabs.science

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

NOTE:
The current code development is heavily based on the code by cstroie found on this github repository: https://github.com/cstroie/WxUno
*/

#include "Arduino.h"
#include "FS.h"
#include <LittleFS.h>
#include "m_file_functions.h"
#include "aprs_cwop_library.h"
#include "driver/temp_sensor.h"

APRS_CWOP_CLASS::APRS_CWOP_CLASS() {
    // Telemetry bits
    this->aprsTlmBits     = B00000000;
    this->settings_defaults();
}

//****************************************************************
void APRS_CWOP_CLASS::init(INTERFACE_CLASS* interface, M_WIFI_CLASS* mWifi, ONBOARD_SENSORS* onBoardSensors ){    
    this->interface = interface;
    this->interface->mserial->printStr("\nInit measurements library ...");
    this->mWifi = mWifi;
    this->onBoardSensors =  onBoardSensors;

    this->settings_defaults();
    this->interface = interface;

    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    
    float hwTemp = 0;
    temp_sensor_read_celsius(&hwTemp);
  
    // Initialize the random number generator and set the APRS telemetry start sequence
    randomSeed( (int)hwTemp + timeUNIX(false) + millis());
    aprsTlmSeq = random(1000);
    
    this->interface->mserial->printStrln( "Telemetry:" + String(aprsTlmSeq) );
    // Start the sensor timer
    snsNextTime = millis();

    this->interface->mserial->printStrln("done.");
}

// ****************************************************************************
void APRS_CWOP_CLASS::settings_defaults(){

    this->config.aprsServer         = "cwop5.aprs.net";   // CWOP APRS-IS server address to connect to
    this->config.aprsPort           = 14580;              // CWOP APRS-IS port

    this->config.aprsCallSign       = "null";
    this->config.aprsPassCode       = "-1";
    this->config.aprsLocation       = "null";

    // Sensors
    this->snsReadTime              = 30UL * 1000UL;                          // Total time to read sensors, repeatedly, for aprsMsrmMax times
    this->snsDelayBfr              = 3600000UL / this->aprsRprtHour - this->snsReadTime; // Delay before sensor readings
    this->snsDelayBtw              = this->snsReadTime / this->aprsMsrmMax;              // Delay between sensor readings
    this->snsNextTime              = 0UL;                                    // Next time to read the sensors

        // Time synchronization and keeping
    this->timeServer               = "utcnist.colorado.edu";  // Time server address to connect to (RFC868)
    this->timePort                 = 37;                      // Time server port
    this->timeNextSync             = 0UL;                     // Next time to syncronize
    this->timeDelta                = 0UL;                     // Difference between real time and internal clock
    this->timeOk                   = false;                   // Flag to know the time is accurate
    this->timeZone                 = 0;                       // Time zone 

    this->aprsPath                 = ">APRS,TCPIP*:";
    this->aprsTlmPARM              = ":PARM.Light,Soil,RSSI,Vcc,Tmp,PROBE,ATMO,LUX,SAT,BAT,TM,RB,B8";
    this->aprsTlmEQNS              = ":EQNS.0,20,0,0,20,0,0,-1,0,0,0.004,4.5,0,1,-100";
    this->aprsTlmUNIT              = ":UNIT.mV,mV,dBm,V,C,prb,on,on,sat,low,err,N/A,N/A";
    this->aprsTlmBITS              = ":BITS.10011111, ";
    this->eol                      = "\r\n";

    this->PROBE                    = true;                     // True if the station is being probed

    this->aprsPkt                  = "";                       // The APRS packet buffer, largest packet is 82 for v2.1
    this->linkLastTime             = 0UL;                      // Last connection time

        // Reports and measurements
    this->aprsRprtHour             = 10; // Number of APRS reports per hour
    this->aprsMsrmMax              = 3;  // Number of measurements per report (keep even)
    this->aprsMsrmCount            = 0;  // Measurements counter
    this->aprsTlmSeq               = 0;  // Telemetry sequence mumber
}


// --------------------------------------------------------------------------

bool APRS_CWOP_CLASS::saveSettings(fs::FS &fs){
    this->interface->mserial->printStrln( this->interface->DeviceTranslation("save_daq_settings")  + "...");

    if (fs.exists("/aprs.cfg") )
        fs.remove("/aprs.cfg");

    File settingsFile = fs.open("/aprs.cfg", FILE_WRITE); 
    if ( !settingsFile ){
        this->interface->mserial->printStrln( this->interface->DeviceTranslation("err_create_daq_settings") + ".");
        settingsFile.close();
        return false;
    }

    settingsFile.print( String(';'));

    settingsFile.close();
    return true;
}
// --------------------------------------------------------------------

bool APRS_CWOP_CLASS::readSettings(fs::FS &fs){    
    File settingsFile = fs.open("/aprs.cfg", FILE_READ);
    if (!settingsFile){
        this->interface->mserial->printStrln( this->interface->DeviceTranslation("err_notfound_daq_settings")  + ".");
        settingsFile.close();
        return false;
    }
    if (settingsFile.size() == 0){
        this->interface->mserial->printStrln( this->interface->DeviceTranslation("err_invalid_daq_settings") + ".");
        settingsFile.close();
        return false;    
    }

    String temp= settingsFile.readStringUntil(';');

    //this->config.MEASUREMENT_INTERVAL = atol(settingsFile.readStringUntil( ';' ).c_str() ); 

    settingsFile.close();
    return true;
}
// *************************************

bool APRS_CWOP_CLASS::aprsSendDataCWOP(){
  // Connect to APRS server
  if ( !this->client.connect(this->config.aprsServer.c_str(), this->config.aprsPort) ) {
      this->interface->mserial->printStrln("Cloud server URL connection FAILED!");
      this->interface->mserial->printStrln(this->config.aprsServer);
      int server_status = client.connected();
      this->interface->mserial->printStrln("Server status code: " + String(server_status));
      return false;
  }
  
  this->interface->mserial->printStrln("Connected to the ARPS server " + String(this->config.aprsServer) ); 
  this->interface->mserial->printStrln("");

  // Authentication
  this->aprsAuthenticate();
  // Send the position, altitude and comment in firsts minutes after boot
  if (millis() < this->snsDelayBfr) this->aprsSendPosition();
  // Send weather data if the athmospheric sensor is present
  this->aprsSendWeather(rMedOut(MD_TEMP), -1, rMedOut(MD_PRES), rMedOut(MD_SRAD));
  // Send the telemetry
  float hwTemp = 0;
  temp_sensor_read_celsius(&hwTemp);
  this->aprsSendTelemetry(rMedOut(MD_A0) / 20,
                    rMedOut(MD_A1) / 20,
                    rMedOut(MD_RSSI),
                    (rMedOut(MD_VCC) - 4500) / 4,
                    hwTemp / 100 + 100,
                    this->aprsTlmBits);
  //aprsSendStatus("Fine weather");
  // Close the connection
  this->client.stop();
  // Keep the millis the connection worked

}
// **************************************
long APRS_CWOP_CLASS::altFeet(int altMeters){
  return (long)(altMeters * 3.28084);  // Altitude in feet
}

// **************************************
 float APRS_CWOP_CLASS::altCorr(int altMeters){
  return pow((float)(1.0 - 2.25577e-5 * altMeters), (float)(-5.25578));  // Altitude correction for QNH
 }

/** ***************************************************
  Simple median filter: get the median
  2014-03-25: started by David Cary

  @param idx the index in round median array
  @return the median
*/
int APRS_CWOP_CLASS::rMedOut(int idx) {
  // Return the last value if the buffer is not full yet
  if (rMed[idx][0] < 3) return rMed[idx][3];
  else {
    // Get the maximum and the minimum
    int the_max = max(max(rMed[idx][1], rMed[idx][2]), rMed[idx][3]);
    int the_min = min(min(rMed[idx][1], rMed[idx][2]), rMed[idx][3]);
    // Clever code: XOR the max and min, remaining the middle
    return the_max ^ the_min ^ rMed[idx][1] ^ rMed[idx][2] ^ rMed[idx][3];
  }
}

/** *****************************************************
  Simple median filter: add value to array

  @param idx the index in round median array
  @param x the value to add
*/
void APRS_CWOP_CLASS::rMedIn(int idx, int x) {
  // At index 0 there is the number of values stored
  if (rMed[idx][0] < 3) rMed[idx][0]++;
  // Shift one position
  rMed[idx][1] = rMed[idx][2];
  rMed[idx][2] = rMed[idx][3];
  rMed[idx][3] = x;
}


/** ****************************************************
  Send an APRS packet and, eventuall, print it to serial line

  @param *pkt the packet to send
*/
void APRS_CWOP_CLASS::aprsSend(const char *pkt) {
#ifdef DEBUG
  Serial.print(pkt);
#endif
client.print(pkt);
}

/**
  Return time in zulu APRS format: HHMMSSh

  @param *buf the buffer to return the time to
  @param len the buffer length
*/
char APRS_CWOP_CLASS::aprsTime(char *buf, size_t len) {
  // Get the time, but do not open a connection to server
  unsigned long utm = timeUNIX(false);
  // Compute hour, minute and second
  int hh = (utm % 86400L) / 3600;
  int mm = (utm % 3600) / 60;
  int ss =  utm % 60;
  // Return the formatted time
  snprintf_P(buf, len, PSTR("%02d%02d%02dh"), hh, mm, ss);
}

/** **************************************
  Send APRS authentication data
  user FW0727 pass -1 vers WxUno 3.1"
*/
void APRS_CWOP_CLASS::aprsAuthenticate() {
  this->aprsPkt = "user ";
  this->aprsPkt += this->config.aprsCallSign;
  this->aprsPkt += " pass ";
  this->aprsPkt += this->config.aprsPassCode;
  this->aprsPkt += " vers ";
  this->aprsPkt += this->interface->config.DEVICE_NAME;
  this->aprsPkt += " ";
  this->aprsPkt += this->interface->firmware_version;
  this->aprsPkt += eol;
  aprsSend(this->aprsPkt.c_str());
}

/**  ******************************************************
  Send APRS weather data, then try to get the forecast
  FW0690>APRS,TCPIP*:@152457h4427.67N/02608.03E_.../...g...t044h86b10201L001WxUno

  @param temp temperature
  @param hmdt humidity
  @param pres athmospheric pressure
  @param lux illuminance
*/
void APRS_CWOP_CLASS::aprsSendWeather(int temp, int hmdt, int pres, int lux) {
  char buf[8];
  this->aprsPkt = this->config.aprsCallSign;
  this->aprsPkt += aprsPath;
  this->aprsPkt += "@";
  aprsTime(buf, sizeof(buf));
  this->aprsPkt += String(buf);
  this->aprsPkt += this->config.aprsLocation;
  // Wind (unavailable)
   this->aprsPkt +=".../...g...";
  // Temperature
  if (temp >= -460) { // 0K in F
    sprintf_P(buf, PSTR("t%03d"), temp);
     this->aprsPkt += String(buf);
  }
  else {
     this->aprsPkt += "t...";
  }
  // Humidity
  if (hmdt >= 0) {
    if (hmdt == 100) {
       this->aprsPkt += "h00";
    }
    else {
      sprintf_P(buf, PSTR("h%02d"), hmdt);
       this->aprsPkt += String(buf);
    }
  }
  // Athmospheric pressure
  if (pres >= 0) {
    sprintf_P(buf, PSTR("b%05d"), pres);
    this->aprsPkt += String(buf);
  }
  // Illuminance, if valid
  if (lux >= 0 and lux <= 999) {
    sprintf_P(buf, PSTR("L%03d"), lux);
     this->aprsPkt += String(buf);
  }
  // Comment (device name)
   this->aprsPkt += this->interface->config.DEVICE_NAME;
   this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str()  );
}

/**  *****************************************************
  Send APRS telemetry and, periodically, send the telemetry setup
  FW0690>APRS,TCPIP*:T#517,173,062,213,002,000,00000000

  @param a0 read analog A0
  @param a1 read analog A1
  @param rssi GSM RSSI level
  @param vcc voltage
  @param temp internal temperature
  @param bits digital inputs
*/
void APRS_CWOP_CLASS::aprsSendTelemetry(int a0, int a1, int rssi, int vcc, int temp, byte bits) {
  // Increment the telemetry sequence number, reset it if exceeds 999
  if (++this->aprsTlmSeq > 999) this->aprsTlmSeq = 0;
  // Send the telemetry setup if the sequence number is 0
  if (this->aprsTlmSeq == 0) aprsSendTelemetrySetup();
  // Compose the APRS packet
  this->aprsPkt = this->config.aprsCallSign;
  this->aprsPkt += this->aprsPath;
  this->aprsPkt += "T";
  char buf[40];
  snprintf_P(buf, sizeof(buf), PSTR("#%03d,%03d,%03d,%03d,%03d,%03d,"), this->aprsTlmSeq, a0, a1, rssi, vcc, temp);
  this->aprsPkt += String(buf);
  itoa(bits, buf, 2);
  this->aprsPkt += String(buf);
  this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str()  );
}

/**   ******************************
  Send APRS telemetry setup
*/
void APRS_CWOP_CLASS::aprsSendTelemetrySetup() {
  String padCallSign;
  padCallSign =  this->config.aprsCallSign;  // Workaround
  // Parameter names
  this->aprsPkt += this->config.aprsCallSign;
  this->aprsPkt += this->aprsPath;
  this->aprsPkt += ":";
  this->aprsPkt += (padCallSign);
  this->aprsPkt += this->aprsTlmPARM;
  this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str() );
  // Equations
  this->aprsPkt = this->config.aprsCallSign;
  this->aprsPkt += aprsPath;
  this->aprsPkt += ":";
  this->aprsPkt += (padCallSign);
  this->aprsPkt += this->aprsTlmEQNS;
  this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str() );
  // Units
  this->aprsPkt = this->config.aprsCallSign;
  this->aprsPkt += this->aprsPath;
  this->aprsPkt += ":";
  this->aprsPkt = (padCallSign);
  this->aprsPkt += this->aprsTlmUNIT;
  this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str() );
  // Bit sense and project name
  this->aprsPkt = this->config.aprsCallSign;
  this->aprsPkt += this->aprsPath;
  this->aprsPkt += ":";
  this->aprsPkt = (padCallSign);
   this->aprsPkt += String(this->aprsTlmBITS);
   this->aprsPkt += this->interface->config.DEVICE_NAME;
   this->aprsPkt += "/";
   this->aprsPkt += this->interface->firmware_version;
   this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str() );
}

/**  ****************************************
  Send APRS status
  FW0690>APRS,TCPIP*:>Fine weather

  @param message the status message to send
*/
void APRS_CWOP_CLASS::aprsSendStatus(const char *message) {
  // Send only if the message is not empty
  if (message[0] != '\0') {
    // Send the APRS packet
    this->aprsPkt = this->config.aprsCallSign;
    this->aprsPkt += this->aprsPath;
    this->aprsPkt += ">";
    this->aprsPkt += message;
    this->aprsPkt += this->eol;
    aprsSend(this->aprsPkt.c_str()  );
  }
}

/**   ***********************************************
  Send APRS position and altitude
  FW0690>APRS,TCPIP*:!DDMM.hhN/DDDMM.hhW$comments

  @param comment the comment to append
*/
void APRS_CWOP_CLASS::aprsSendPosition(const char *comment) {
  // Compose the APRS packet
  this->aprsPkt = this->config.aprsCallSign;
   this->aprsPkt += aprsPath;
   this->aprsPkt += "!";
   this->aprsPkt += this->config.aprsLocation;
   this->aprsPkt += "/000/000/A=";
  char buf[7];
  sprintf_P(buf, PSTR("%06d"), this->altFeet(this->config.altMeters ));
  this->aprsPkt += String( buf);
  if (comment != NULL) this->aprsPkt += comment;
   this->aprsPkt += this->eol;
  aprsSend(this->aprsPkt.c_str() );
}

// *****************************************************
/**
  Get current time as UNIX time (1970 epoch)
  @param sync flag to show whether network sync is to be performed
  @return current UNIX time
*/
unsigned long APRS_CWOP_CLASS::timeUNIX(bool sync) {
  // Check if we need to sync
  if (millis() >= this->timeNextSync and sync) {
    // Try to get the time from Internet
    unsigned long utm = 0 ; //ntpSync(); //timeSync();
    if (utm == 0) {
      // Time sync has failed, sync again over one minute
      this->timeNextSync += 1UL * 60 * 1000;
      this->timeOk = false;
    } else {
      // Compute the new time delta
      timeDelta = utm - (millis() / 1000);
      // Time sync has succeeded, sync again in 8 hours
      this->timeNextSync += 8UL * 60 * 60 * 1000;
      this->timeOk = true;
      this->interface->mserial->printStr("Network UNIX Time: 0x");
      this->interface->mserial->printStrln( String(utm, 16) );
    }
  }

  // Get current time based on uptime and time delta,
  // or just uptime for no time sync ever
  return (millis() / 1000) + timeDelta;
}

// **************************************************


