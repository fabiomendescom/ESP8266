/*
I had problems using some relays. The one that worked is the one without the leds (big ones). The one with S + and - worked best
If you power the ESP8166 through USB ensure that you ground from ESP8266 to the power supply, ohterwise it will only 
work with the VIN
CAUTION: DO NOT DO NOT DO NOT wire the Vin and Ground AND plug the USB!!!! I burnt one.
Pins
ESP8266   RELAY    OUTSIDE 5V
Nothing   +         VCC
Nothing   -         GND
D4        S
 */
#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...) 
#endif
// constants won't change. They're used here to
// set pin numbers:
const int relaypin = 2;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  //pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin,LOW);
  Serial1.begin(9600);
  Serial1.println("Test");
  DEBUG_MSG("bootup...\n");
}

void loop() {
   delay(30000);
  //digitalWrite(relaypin,HIGH);
}
