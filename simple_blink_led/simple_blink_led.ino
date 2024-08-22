int red_led_pin=10;
int green_led_pin=10;
int blue_led_pin=10;

void setup() {

pinMode(red_led_pin,OUTPUT);
pinMode(green_led_pin,OUTPUT);
pinMode(blue_led_pin,OUTPUT);

}

void loop(){
if (red_led_pin !=-1){
  digitalWrite(red_led_pin,HIGH);
  delay(1000);
  digitalWrite(red_led_pin,LOW);
  delay(500);
}

if (blue_led_pin !=-1){
  digitalWrite(blue_led_pin,HIGH);
  delay(1000);
  digitalWrite(blue_led_pin,LOW);
  delay(500);
}
if (green_led_pin !=-1){
  digitalWrite(green_led_pin,HIGH);
  delay(1000);
  digitalWrite(green_led_pin,LOW);
  delay(500);
}
}