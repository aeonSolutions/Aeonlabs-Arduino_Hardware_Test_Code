// Need this for the lower level access to set them up.
#include <HardwareSerial.h>

static const uint8_t TX_PIN = 21;
static const uint8_t RX_PIN = 20;

//Define two Serial devices mapped to the two internal UARTs
HardwareSerial MySerial0(0);
HardwareSerial MySerial1(1);

void setup()
{
    // For the USB, just use Serial as normal:
    Serial.begin(115200);
    Serial.print("Serial");

    // Configure MySerial0 on pins TX=6 and RX=7 (-1, -1 means use the default)
    MySerial0.begin(9600, SERIAL_8N1, -1, -1);
    MySerial0.print("MySerial0");

    // And configure MySerial1 on pins RX=D9, TX=D10
    MySerial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    MySerial1.print("MySerial1");
}

void loop(){
// For the USB, just use Serial as normal:
Serial.println("Serial");

// Configure MySerial0 on pins TX=6 and RX=7 (-1, -1 means use the default)
MySerial0.println("MySerial0");
// And configure MySerial1 on pins RX=D9, TX=D10
MySerial1.println("MySerial1");

delay(1000);

}