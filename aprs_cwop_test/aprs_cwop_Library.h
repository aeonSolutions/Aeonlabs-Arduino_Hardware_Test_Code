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
#include "interface_class.h"
#include "onboard_sensors.h"
#include <WiFi.h>
#include "WiFiClient.h"
#include "m_wifi.h"

#ifndef MEASUREMENTS_COMMANDS  
  #define MEASUREMENTS_COMMANDS
  

  // **************************** == Measurements Class == ************************
class APRS_CWOP_CLASS {
    private:
        INTERFACE_CLASS* interface       = nullptr;
        M_WIFI_CLASS* mWifi              = nullptr ;
        ONBOARD_SENSORS* onBoardSensors  = nullptr;

        // GBRL commands  *********************************************
        bool helpCommands(String $BLE_CMD, uint8_t sendTo );
        WiFiClient client;

    public:
        // Reports and measurements
        int aprsRprtHour;                   // Number of APRS reports per hour
        int aprsMsrmMax;                    // Number of measurements per report (keep even)
        int aprsMsrmCount;                  // Measurements counter
        int aprsTlmSeq;                     // Telemetry sequence mumber

        bool PROBE;                         // True if the station is being probed

        String aprsPath;
        String aprsTlmPARM;
        String aprsTlmEQNS;
        String aprsTlmUNIT;
        String aprsTlmBITS;
        
        // Telemetry bits
        char aprsTlmBits;
        String eol;

        String aprsPkt;                     // The APRS packet buffer, largest packet is 82 for v2.1

        // The APRS connection client
        unsigned long   linkLastTime;       // Last connection time

        // ...................................................     
        typedef struct{
            String aprsCallSign;
            String aprsPassCode;
            String aprsLocation;


            // APRS parameters
            String  aprsServer;             // CWOP APRS-IS server address to connect to
            int   aprsPort;                 // CWOP APRS-IS port

            int   altMeters;                // Altitude in Bucharest

            // configuration: PCB specific
            float    MCU_VDD = 3.38;
        } config_strut;

        config_strut config;
        

        // Sensors
        unsigned long snsReadTime;           // Total time to read sensors, repeatedly, for aprsMsrmMax times
        unsigned long snsDelayBfr;           // Delay before sensor readings
        unsigned long snsDelayBtw;           // Delay between sensor readings
        unsigned long snsNextTime;           // Next time to read the sensors

        // Time synchronization and keeping
        String        timeServer;                // Time server address to connect to (RFC868)
        int           timePort;                    // Time server port
        unsigned long timeNextSync;                // Next time to syncronize
        unsigned long timeDelta;                   // Difference between real time and internal clock
        bool          timeOk;                      // Flag to know the time is accurate
        int           timeZone;                    // Time zone

        // Statistics (round median filter for the last 3 values)
        enum      rMedIdx {MD_TEMP, MD_PRES, MD_RSSI, MD_SRAD, MD_VCC, MD_A0, MD_A1, MD_ALL};
        int       rMed[MD_ALL][4];

        // ____________________________________________________
        APRS_CWOP_CLASS();
        
        void init(INTERFACE_CLASS* interface,M_WIFI_CLASS* mWifi, ONBOARD_SENSORS* onBoardSensors );
        
        void settings_defaults();

        // GBRL commands  *********************************************
        bool gbrl_commands(String $BLE_CMD, uint8_t sendTo);

        // Setup configuration and settings *******************************************
        bool readSettings( fs::FS &fs = LittleFS );
        bool saveSettings( fs::FS &fs = LittleFS  );

        // ****************************************************
        long  altFeet(int altMeters);
        float altCorr(int altMeters);

        int rMedOut(int idx);
        void rMedIn(int idx, int x);

        void aprsSend(const char *pkt);
        void aprsAuthenticate();
        void aprsSendWeather(int temp, int hmdt, int pres, int lux);
        void aprsSendTelemetry(int a0, int a1, int rssi, int vcc, int temp, byte bits);
        void aprsSendTelemetrySetup();
        void aprsSendStatus(const char *message);
        void aprsSendPosition(const char *comment = NULL);
        char aprsTime(char *buf, size_t len);
        unsigned long timeUNIX(bool sync = true);

        bool aprsSendDataCWOP();

};


#endif

