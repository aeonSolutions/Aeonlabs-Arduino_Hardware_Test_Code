#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};

#define BUZZER_PIN 33
#define BUZZER_CHANNEL 1

#define LED_B 18
#define LED_G 10
#define LED_R 11

#define COFFEE_BTN 6
#define WATER_HEATER_BTN 4
#define GRINDER_BTN 5

#define DC_MOTOR_EN 5

// setting PWM properties
const int freq = 3000;
const int resolution = 8;
 
void setup(){
  //Initiate Serial communication.
  Serial.begin(115200);
  
  // configure LED PWM functionalitites
  ledcSetup(BUZZER_CHANNEL, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); // sets the digital pin 13 on
  
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);

  pinMode(DC_MOTOR_EN, OUTPUT);

  pinMode(COFFEE_BTN, OUTPUT);
  pinMode(WATER_HEATER_BTN, OUTPUT);
  pinMode(GRINDER_BTN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
}
 
void Tone32(int ch, double note, double duration){
  ledcWriteTone(ch, note);
  delay(duration);
}

void noTone32(){
  ledcWriteTone(BUZZER_CHANNEL, 0);
}

void loop() {
  DC_MOTOR();
  //relays();
  //LED();
  //buzzer();
}

void DC_MOTOR(){
  Serial.println("DC MOTOR ON");
  digitalWrite(DC_MOTOR_EN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(15000);                       // wait for a second
  Serial.println("DC MOTOR OFF");
  digitalWrite(DC_MOTOR_EN, LOW);    // turn the LED off by making the voltage LOW
  delay(5000);                       // wait for a second
}

void LED(){
  
  //LED
  Serial.println("LED Blue");
  digitalWrite(LED_B, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1500);                       // wait for a second
  digitalWrite(LED_B, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  Serial.println("LED RED");
  digitalWrite(LED_R, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  digitalWrite(LED_R, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  Serial.println("LED Green");
  digitalWrite(LED_G, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
  digitalWrite(LED_G, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);     
  // wait for a second
}

void relays(){
  //water heater
  Serial.println("enable water heater");
  digitalWrite(WATER_HEATER_BTN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1500);        
  Serial.println("disable water heater");           // wait for a second
  digitalWrite(WATER_HEATER_BTN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);    

  //coffee
  Serial.println("enable coffee");
  digitalWrite(COFFEE_BTN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1500);          
  Serial.println("disable coffee");             // wait for a second
  digitalWrite(COFFEE_BTN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);     

  //grinder
  Serial.println("enable coffee");
  digitalWrite(GRINDER_BTN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1500);          
  Serial.println("disable coffee");             // wait for a second
  digitalWrite(GRINDER_BTN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);     
  }


void buzzer() {
  // iterate over the notes of the melody:
  int size = sizeof(noteDurations) / sizeof(int);

  for (int thisNote = 0; thisNote < size; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
   Tone32(BUZZER_CHANNEL, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    int led = random(1,4);
    if(led==1){
      led=LED_B;
    }
    
    if(led==2){
      led=LED_G;
    }
    
    if(led==3){
      led=LED_R;
    }
    
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
   
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone32();
    digitalWrite(led, LOW);   // turn the LED on (HIGH is the voltage level)
  }
}
