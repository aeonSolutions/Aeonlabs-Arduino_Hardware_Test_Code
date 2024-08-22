#include "pitches.h"
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

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
#define BUZZER_CHANNEL 0
// setting PWM properties
const int freq = 5000;
const int resolution = 12;

// onboard LED
const byte LED_RED = 11;
const ledc_channel_t LED_RED_CH = (ledc_channel_t) 0;

const byte LED_BLUE = 13;
const ledc_channel_t LED_BLUE_CH = (ledc_channel_t) 2;

const byte LED_GREEN = 14;
const ledc_channel_t LED_GREEN_CH = (ledc_channel_t) 3;

const int LED_FREQ = 5000;

#define COFFEE_BTN 6
#define WATER_HEATER_BTN 4
#define GRINDER_BTN 5


ledc_timer_config_t led_red_timer;
ledc_channel_config_t led_red_channel;

ledc_timer_config_t led_green_timer;
ledc_channel_config_t led_green_channel;

ledc_timer_config_t led_blue_timer;
ledc_channel_config_t led_blue_channel;

#define LED_MODE              LEDC_LOW_SPEED_MODE
#define LED_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LED_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

//LED RED
#define LED_RED_TIMER              LEDC_TIMER_0
//LED GREEN 
#define LED_GREEN_TIMER              LEDC_TIMER_1
//LED BLUE
#define LED_BLUE_TIMER              LEDC_TIMER_2

void setup(){
  //Initiate Serial communication.
  Serial.begin(115200);


  led_red_timer.speed_mode         = LED_MODE;
  led_red_timer.timer_num          = LED_RED_TIMER;
  led_red_timer.duty_resolution    = LEDC_TIMER_12_BIT;// (ledc_timer_bit_t) 1;
  led_red_timer.freq_hz            = LED_FREQUENCY;
 
  led_red_channel.channel          = LED_RED_CH;
  led_red_channel.gpio_num         = LED_RED;
  led_red_channel.speed_mode       = LED_MODE;
  led_red_channel.timer_sel        = LED_RED_TIMER;
  led_red_channel.duty             = 4095;
 
  ledc_timer_config(&led_red_timer);
  ledc_channel_config(&led_red_channel);

// ******************
  led_red_timer.speed_mode         = LED_MODE;
  led_red_timer.timer_num          = LED_GREEN_TIMER;
  led_red_timer.duty_resolution    = LEDC_TIMER_12_BIT;// (ledc_timer_bit_t) 1;
  led_red_timer.freq_hz            = LED_FREQUENCY;
 
  led_red_channel.channel          = LED_GREEN_CH;
  led_red_channel.gpio_num         = LED_GREEN;
  led_red_channel.speed_mode       = LED_MODE;
  led_red_channel.timer_sel        = LED_GREEN_TIMER;
  led_red_channel.duty             = 4095;
 
  ledc_timer_config(&led_green_timer);
  ledc_channel_config(&led_green_channel);

//******************************
  led_red_timer.speed_mode         = LED_MODE;
  led_red_timer.timer_num          = LED_BLUE_TIMER;
  led_red_timer.duty_resolution    = LEDC_TIMER_12_BIT;// (ledc_timer_bit_t) 1;
  led_red_timer.freq_hz            = LED_FREQUENCY;
 
  led_red_channel.channel          = LED_BLUE_CH;
  led_red_channel.gpio_num         = LED_BLUE;
  led_red_channel.speed_mode       = LED_MODE;
  led_red_channel.timer_sel        = LED_BLUE_TIMER;
  led_red_channel.duty             = 4095;
 
  ledc_timer_config(&led_blue_timer);
  ledc_channel_config(&led_blue_channel);



  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); // sets the digital pin 13 on
  
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
  //relays();
    
  LED();
  //buzzer();
}

void turnOffLedAll(){
    ESP_ERROR_CHECK(ledc_set_duty(LED_MODE, LED_RED_CH, 4095));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LED_MODE, LED_RED_CH));

    ESP_ERROR_CHECK(ledc_set_duty(LED_MODE, LED_BLUE_CH, 4095));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LED_MODE, LED_BLUE_CH));

    ESP_ERROR_CHECK(ledc_set_duty(LED_MODE, LED_GREEN_CH, 4095));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LED_MODE, LED_GREEN_CH));
 };

void LED(){
    Serial.print("Testing RGB LED OFF...");
    turnOffLedAll();
    delay(2000);
    Serial.println("OK");

    Serial.print("Testing LED Red...");
    
    ESP_ERROR_CHECK(ledc_set_duty(LED_MODE, LED_RED_CH, 4000));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LED_MODE, LED_RED_CH));
    delay(2000);

    Serial.println("OK");

    Serial.print("Testing RGB LED OFF...");
    turnOffLedAll();
    delay(2000);
    Serial.println("OK");
    
    Serial.print("Testing LED Green...");
    ESP_ERROR_CHECK(ledc_set_duty(LED_MODE, LED_GREEN_CH, 4000));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LED_MODE, LED_GREEN_CH));
    Serial.println("OK");
    delay(2000);
    Serial.print("Testing RGB LED OFF...");
    turnOffLedAll();
    delay(2000);
    Serial.println("OK");

    Serial.print("Testing LED Blue...");
    ESP_ERROR_CHECK(ledc_set_duty(LED_MODE, LED_BLUE_CH, 4000));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LED_MODE, LED_BLUE_CH));
    delay(2000);
    
    turnOffLedAll();
    Serial.println("OK");
    Serial.println("LED test completed.");
    Serial.println("");

    delay(2000);
}
//**********************************************************
void LED_Digital(){
  
  //LED
  ("LED Blue:");
  digitalWrite(LED_BLUE, LOW );   // turn the LED on (HIGH is the voltage level)
  Serial.print("....ON");
  delay(2000);                       // wait for a second
  digitalWrite(LED_BLUE, HIGH);    // turn the LED off by making the voltage LOW
  Serial.println("....off");
  delay(2000);                       // wait for a second

  Serial.print("LED RED:");
  digitalWrite(LED_RED, LOW);   // turn the LED on (HIGH is the voltage level)
  Serial.print("....ON");
  delay(2000);                       // wait for a second
  digitalWrite(LED_RED, HIGH);    // turn the LED off by making the voltage LOW
  Serial.println("....OFF");
  delay(2000);                       // wait for a second

  Serial.print("LED Green:");
  digitalWrite(LED_GREEN, LOW);   // turn the LED on (HIGH is the voltage level)
  Serial.print("....ON");
  delay(2000);                       // wait for a second
  digitalWrite(LED_GREEN, HIGH);    // turn the LED off by making the voltage LOW
  Serial.println("....OFF");
  delay(2000);     
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
      led=LED_BLUE;
    }
    
    if(led==2){
      led=LED_GREEN;
    }
    
    if(led==3){
      led=LED_RED;
    }
    
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
   
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone32();
    digitalWrite(led, LOW);   // turn the LED on (HIGH is the voltage level)
  }
}
