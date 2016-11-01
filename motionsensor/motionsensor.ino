/*
Pins (Remove sensor cover to know)
http://www.instructables.com/id/PIR-Motion-Sensor-Tutorial/?ALLSTEPS
VCC - connectd to 3.3
GND - connected to GND
OUTPUT - connected to D1(GPIO5) through a RED-BLACK-ORANGE-GOLD resistor (may want to try with different ones to see
There are variable resistors with a "105" marking on them. If you look directly at them:
the one on left stays with "105" on the middle. The one on right is turned all the way to left
 */

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 5;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
bool detectedmotion=false;
int whendetected;

void setup() {
  // initialize the LED pin as an output:
  //pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    detectedmotion = true;
    whendetected=millis();
    Serial.println("Detected Motion");
  } else {
    //if(millis()-whendetected > 3000) {
      //whendetected=0;
      Serial.println("Silenced");
    //}
  }
}
