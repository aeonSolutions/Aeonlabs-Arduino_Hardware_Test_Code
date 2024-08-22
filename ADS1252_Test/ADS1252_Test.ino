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
#include <SPI.h>

#define MISOPIN 36 //36 //17
#define SCLKPIN 35
#define CLKPIN 37 // 37 //20

// calculation formulas:
float f_ADSCLK = 10000000;// 10 MHz

float f_MCLK = f_ADSCLK / 6; 
float DRATE = f_MCLK / 64;
float p_MCLK = 1 / f_MCLK;
float t2 = 6 * p_MCLK;
float t3 = 6 * p_MCLK;
float t7 =30;

float  DRDY_partition = 36 * p_MCLK;
float DOUT_partition = 348 * p_MCLK;
float CONVCYCLE = DRDY_partition + DOUT_partition; // = 384 * p_MCLK;
float t_RESET_5 = 5 * CONVCYCLE; 

void setup(){
 Serial.begin(115200);
 
 pinMode(SCLKPIN, OUTPUT);
 pinMode(MISOPIN, INPUT);
 pinMode(CLKPIN, OUTPUT);
 
 // put ADC on reset at the outset
 reset_adc();
 
 // SPI.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS); //SCLK, MISO, MOSI, SS
 SPI.begin(SCLKPIN, MISOPIN, 19, CLKPIN );
 SPI.setFrequency(f_ADSCLK);

 // initialize SPI (with default settings, including...
 // CPOL = 0: so that SCLK is normally LOW
 // CPHA = 0: data sampled on rising edge (LOW to HIGH)
 // perhaps try changing CPHA ??
 
 digitalWrite(SCLKPIN, LOW);
 // release ADC from reset; now we're at a known point
 // in the timing diagram, and just have to wait for
 // the beginning of a conversion cycle
Serial.println("setup completed");
}

//***************************************
void loop(){
 Serial.println("reset adc");
 // put ADC on reset at the outset
 reset_adc();
 Serial.println("reset completed");
 long int initTime= millis();
 
  Serial.print("MISO PIN RAW: ");
  Serial.println( digitalRead(MISOPIN) );
  
 while( (digitalRead(MISOPIN) != HIGH) && (millis()-initTime < 5000) ){  // 5 sec timeout
  Serial.print(millis()-initTime);
  Serial.print(" (");
  Serial.print( digitalRead(MISOPIN) );
  Serial.print(")...");
  delay(1000);
 } 
 Serial.println("miso pin probe completed");
 
 if (initTime - millis() >= 5000 ){ 
  Serial.println("Timeout waiting for response");
 }
 Serial.println("read adc"); 
 read_adc();

 delay(1000);
}

//************************************************
// to reset ADC, we need SCLK HIGH for min of 4 CONVCYCLES
// so here, hold SCLK HIGH for 5 CONVCYCLEs = 1440 usec
void reset_adc(){
 digitalWrite(SCLKPIN, HIGH);
 delay(t_RESET_5);
}

// *********************************************
void read_adc(){
  byte byte1; byte byte2; byte byte3;
  byte flipper = 0b11111111; // 8 bit number to flip bits of a byte
  byte addone = 0b1; // to add one to the two's complement

// reset the IC
  digitalWrite(SCLKPIN, HIGH);
  delay(4*CONVCYCLE);
  digitalWrite(SCLKPIN, LOW);
  delay(DRDY_partition);
  
  //SPI.beginTransaction(SPISettings(f_ADSCLK, MSBFIRST, SPI_MODE0));

  byte3 = SPI.transfer(0x00);
  byte2 = SPI.transfer(0x00);
  byte1 = SPI.transfer(0x00);
   
  // read in adc data (sending out don't care bytes)
  // and store read data into three bytes */
  //digitalWrite(MISOPIN, HIGH);
  //SPI.endTransaction();

  Serial.println(byte1, DEC);
  Serial.println(byte2, DEC);
  Serial.println(byte3, DEC);
  Serial.println();

  int check = byte3 << 1;
  if (check == 1){
    // Negative number in two's complement form. need to flip bytes. and add one to them.
    //    negative = "-";
    // flip the bits
    
    byte3 = byte3^flipper;
    byte2 = byte2^flipper;
    byte1 = byte1^flipper;
}else{
  // negative = "+";
  // no need to flip or add one
  addone = 0b0;
}

  float ads_output =0;   
  ads_output = 5*(((byte3 << 16) + (byte2 << 8) + (byte1 + addone))) >> 24; 

  Serial.println(ads_output,10);

  //cout << " two's Complement : " <<  5*(((byte3 << 16) + (byte2 << 8) + (byte1 + addone))/pow(2,24)) << endl;

  // print out data;
  // will these instructions eat into time significantly?
  // possible improvement: store all data from multiple cycles
  // into array, and print out only later at end.

  drdy_wait();
}

//***********************************************
// wait for DRDY to pass and to reach start-point of DOUT
void drdy_wait(){
 delay(DRDY_partition);
}
