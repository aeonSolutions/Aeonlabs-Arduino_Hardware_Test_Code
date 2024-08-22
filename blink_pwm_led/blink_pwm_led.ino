#include "driver/ledc.h"
// the number of the LED pin
const uint8_t LED_RED = 10; 
const uint8_t LED_BLUE = 3;
const uint8_t LED_GREEN = 18;  
const uint8_t LED_UV = -1;  
const uint8_t LED_WHITE = -1;  

// setting PWM properties
const int freq = 5000;

const uint8_t LED_RED_CH = 2; //LEDC_CHANNEL_4;
const uint8_t LED_BLUE_CH = 3; //LEDC_CHANNEL_5;
const uint8_t LED_GREEN_CH = 1; //LEDC_CHANNEL_3;
const uint8_t LED_UV_CH = -1;
const uint8_t LED_WHITE_CH = -1;

const int resolution = 8;
 
void setup(){
  Serial.begin(115200);
  Serial.println("setup");
  
  // configure LED PWM functionalitites
  ledcSetup(LED_RED_CH, freq, resolution);
  //ledcSetup(LED_BLUE_CH, freq, resolution);
  //ledcSetup(LED_GREEN_CH, freq, resolution);
  //ledcSetup(LED_UV_CH, freq, resolution);
  //ledcSetup(LED_WHITE_CH, freq, resolution);
    
  // attach the channel to the GPIO to be controlled
}
 void loop(){
    Serial.println("loop");
   statusLED( (const uint8_t[]){1,LED_RED}, 100, 1);
   statusLED( (const uint8_t[]){1,LED_BLUE}, 50, 1);
   statusLED( (const uint8_t[]){1,LED_GREEN}, 50, 1);
   
   //statusLED( (uint8_t*)(const uint8_t[]){LED_WHITE}, 100, 1);
   //statusLED( (uint8_t*)(const uint8_t[]){LED_UV}, 100, 1);
   
   //turnOffAllStatusLED();
   delay(2000);
   
 }

void turnOffAllStatusLED(){
  // TURN OFF ALL LED
  Serial.println("off -> red");

  ledcDetachPin(LED_RED);
  pinMode(LED_RED,INPUT);

  ledcDetachPin(LED_BLUE);
  pinMode(LED_BLUE,INPUT);
  
  ledcDetachPin(LED_GREEN);
  pinMode(LED_GREEN,INPUT);
  };


void statusLED( const uint8_t* led, int brightness, float time) {
  //turnOffAllStatusLED();
  
  // turn ON LED      
  double bf = (double)brightness*0.6/100*255; // 255 - formula to the left 
  int bright = (int)bf;
  int8_t ch;

  Serial.println("brightness:"+ String(bright) );  
  Serial.println("size of led:"+ String(led[0]) );
  
  for(int i=1; i<=led[0]; i++){

    if(led[i]==LED_RED){
      ch=LED_RED_CH;
      pinMode(LED_RED,OUTPUT);
      ledcAttachPin(LED_RED, LED_RED_CH);
      ledcWrite(LED_RED_CH, bright);
          Serial.println("on -> red");
    }

    if(led[i]==LED_GREEN){
      ch=LED_GREEN_CH;
      pinMode(LED_GREEN,OUTPUT);
      ledcAttachPin(LED_GREEN, LED_GREEN_CH);
      ledcWrite(LED_GREEN_CH, bright);
      Serial.println("on -> green");
    }
    if(led[i]==LED_BLUE){
      ch=LED_BLUE_CH;
      pinMode(LED_BLUE,OUTPUT);
      ledcAttachPin(LED_BLUE, LED_BLUE_CH);
      ledcWrite(LED_BLUE_CH, bright);
      Serial.println("on -> blue");
    }

    if(led[i]==LED_UV){
      ch=LED_UV_CH;
      pinMode(LED_UV,OUTPUT);
      ledcAttachPin(LED_UV, LED_UV_CH);

    }

    if(led[i]==LED_WHITE){
      ch=LED_WHITE_CH;
      pinMode(LED_WHITE,OUTPUT);
      ledcAttachPin(LED_WHITE, LED_WHITE_CH);
    }
  } // for

  if (time>0){
    delay(time*1000);
    turnOffAllStatusLED();
  }
}; 

void blinkStatusLED(uint8_t led[], uint8_t brightness, float time, uint8_t numberBlinks){
  for(int i=0; i<numberBlinks; i++){
    statusLED(led, brightness, time);
    turnOffAllStatusLED();
    delay(time*1000);
  }
};
