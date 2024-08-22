#include <Wire.h>
//#include "AHT20.h"

#define I2C_SDA 9
#define I2C_SCL 10

//AHT20 aht20;
float aht_temp, aht_humidity;

void setup() {
  pinMode(38, OUTPUT);    // sets the digital pin 38 as output
  digitalWrite(38, HIGH); // sets the digital pin 38 on 
  
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  //I2Cscanner();
  
  startAHT();

}

void loop() {
  request_onBoard_Sensor_Measurements();
  delay(500);
}


void I2Cscanner() {
  Serial.println ("I2C scanner. \n Scanning ...");  
  uint8_t count = 0;
  String addr;

  for (uint8_t i = 8; i < 120; i++){

    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    uint8_t error = Wire.endTransmission();
    addr="";
    if (i < 16){
       addr="0";
    }
    addr=addr+String(i, HEX);

    if (error == 0) { // Receive 0 = success (ACK response)
      Serial.print ("Found address: ");
      Serial.print (String(i, DEC));
      Serial.print (" (0x");
      Serial.print (String(i, HEX));     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      Serial.println(addr);
    } else{

    }
  }

  Serial.print ("Found ");
  Serial.print (String(count));        // numbers of devices
  Serial.println (" device(s).");
}


void request_onBoard_Sensor_Measurements(){

    Serial.print("AHT20 data: ");
    
    aht_temp = getTemperature();
    aht_humidity = getHumidity();

    if (! isnan(aht_temp)) { // check if 'is not a number'
      Serial.print("Temp *C = ");
      Serial.print(String(aht_temp));
    } else {
      Serial.println("Failed to read temperature");
    }

    if (! isnan(aht_humidity)) { // check if 'is not a number'
      Serial.print("   Hum. % = ");
      Serial.println(String(aht_humidity));
    } else {
      Serial.println("Failed to read humidity");
    }
}


void startAHT() {
    Serial.println("starting AHT sensor....");
    bool result = begin();

  
    if (result){
        Serial.println("AHT status code: " + String(getStatus()));
        Serial.println("AHT is calibrated: " + String(isCalibrated()));
        if (getStatus()==0){
          Serial.print("AHT initialization DONE.");
        }else{
            Serial.print("AHT initialization ERROR.");
        }
        Serial.println("");
    }else{
        Serial.print("AHT sensor not found ");

    }
    Serial.println("AHT completed.");
}


//****************************************************************

enum registers
{
    sfe_aht20_reg_reset = 0xBA,
    sfe_aht20_reg_initialize = 0xBE,
    sfe_aht20_reg_measure = 0xAC,
};


    uint8_t _deviceAddress = 0x38;
    bool measurementStarted = false;

    struct
    {
        uint32_t humidity;
        uint32_t temperature;
    } sensorData;

    struct
    {
        uint8_t temperature : 1;
        uint8_t humidity : 1;
    } sensorQueried;


bool begin()
{
    delay(20);
    if (isConnected() == false)
        return false;

    //Wait 40 ms after power-on before reading temp or humidity. Datasheet pg 8
    delay(40);

    //Check if the calibrated bit is set. If not, init the sensor.
    if (isCalibrated() == false)
    {
        //Send 0xBE0800
        initialize();

        //Immediately trigger a measurement. Send 0xAC3300
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        //This calibration sequence is not completely proven. It's not clear how and when the cal bit clears
        //This seems to work but it's not easily testable
        if (isCalibrated() == false)
        {
		Serial.println("LIb AHT20: not calibrated");
            return (false);
        }
    }

    //Check that the cal bit has been set
    if (isCalibrated() == false)
        return false;

    //Mark all datums as fresh (not read before)
    sensorQueried.temperature = true;
    sensorQueried.humidity = true;

    return true;
}

//Ping the AHT20's I2C address
//If we get a response, we are correctly communicating with the AHT20
bool isConnected()
{
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(30);
	
  long int st = millis();
	while(millis()-st <200 ){
    Wire.beginTransmission(_deviceAddress);
   	uint8_t error = Wire.endTransmission();
    if (error == 0){
        Serial.println("wire OK");
        return true;
	  }else{
        Serial.print(error);
    }
  }


	Serial.println(String("LIb AHT20(wire): unable to connect " + String( _deviceAddress)));    
	return false;
}

/*------------------------ Measurement Helpers ---------------------------*/

uint8_t getStatus()
{
  Wire.beginTransmission(_deviceAddress); // informs the bus that we will be sending data to slave device 8 (0x08)
  Wire.write((uint8_t)0x71);
  Wire.endTransmission();       // informs the bus and the slave device that we have finished sending data
  Wire.requestFrom(_deviceAddress, (uint8_t)1);
  if (Wire.available())
        return (Wire.read());
    return (0);
}

//Returns the state of the cal bit in the status byte
bool isCalibrated()
{
    return (getStatus() & (1 << 3));
}

//Returns the state of the busy bit in the status byte
bool isBusy()
{
    return (getStatus() & (1 << 7));
}

bool initialize()
{
    Wire.beginTransmission(_deviceAddress);
    Wire.write(sfe_aht20_reg_initialize);
    Wire.write((uint8_t)0x08);
    Wire.write((uint8_t)0x00);
    delay(10);
    if (Wire.endTransmission() == 0)
        return true;
    return false;
}

bool triggerMeasurement()
{
    Wire.beginTransmission(_deviceAddress);
    Wire.write(sfe_aht20_reg_measure);
    Wire.write((uint8_t)0x33);
    Wire.write((uint8_t)0x00);
    delay(80);
    if (Wire.endTransmission() == 0)
        return true;
    return false;
}

//Loads the
void readData()
{
    //Clear previous data
    sensorData.temperature = 0;
    sensorData.humidity = 0;

    if (Wire.requestFrom(_deviceAddress, (uint8_t)6) > 0)
    {
        Wire.read(); // Read and discard state

        uint32_t incoming = 0;
        incoming |= (uint32_t)Wire.read() << (8 * 2);
        incoming |= (uint32_t)Wire.read() << (8 * 1);
        uint8_t midByte = Wire.read();

        incoming |= midByte;
        sensorData.humidity = incoming >> 4;

        sensorData.temperature = (uint32_t)midByte << (8 * 2);
        sensorData.temperature |= (uint32_t)Wire.read() << (8 * 1);
        sensorData.temperature |= (uint32_t)Wire.read() << (8 * 0);

        //Need to get rid of data in bits > 20
        sensorData.temperature = sensorData.temperature & ~(0xFFF00000);

        //Mark data as fresh
        sensorQueried.temperature = false;
        sensorQueried.humidity = false;
    }
}

//Triggers a measurement if one has not been previously started, then returns false
//If measurement has been started, checks to see if complete.
//If not complete, returns false
//If complete, readData(), mark measurement as not started, return true
bool available()
{
    if (measurementStarted == false)
    {
        triggerMeasurement();
        measurementStarted = true;
        return (false);
    }

    if (isBusy() == true)
    {
        return (false);
    }

    readData();
    measurementStarted = false;
    return (true);
}

bool softReset()
{
    Wire.beginTransmission(_deviceAddress);
    Wire.write(sfe_aht20_reg_reset);
    if (Wire.endTransmission() == 0)
        return true;
    return false;
}

/*------------------------- Make Measurements ----------------------------*/

float getTemperature()
{
    if (sensorQueried.temperature == true)
    {
        //We've got old data so trigger new measurement
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        readData();
    }

    //From datasheet pg 8
    float tempCelsius = ((float)sensorData.temperature / 1048576) * 200 - 50;

    //Mark data as old
    sensorQueried.temperature = true;

    return tempCelsius;
}

float getHumidity()
{
    if (sensorQueried.humidity == true)
    {
        //We've got old data so trigger new measurement
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        readData();
    }

    //From datasheet pg 8
    float relHumidity = ((float)sensorData.humidity / 1048576) * 100;

    //Mark data as old
    sensorQueried.humidity = true;

    return relHumidity;
}
