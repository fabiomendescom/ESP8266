/*
https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide?_ga=1.170782688.116692663.1457303997#arduino-code
https://github.com/bogde/HX711

I am not quite sure I got this working. I think I need to put all 4 load cells on it to get a better test. 
Calibrating seems to be a bigger challenge than I thought
*/

/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
 outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.
 
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch
 
 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
 calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).
 
 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state
 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND
 
 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
 
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.


 This is calibrated for the ESP8266 and load sensor
 
*/

#include "HX711.h"

#define DOUT  5
#define CLK  4

HX711 scale(DOUT, CLK);

void setup() {
  Serial.begin(9600);
  //Serial.println("HX711 Demo");

  //Serial.println("Before setting up the scale:");
  //Serial.print("read: \t\t");
  //Serial.println(scale.read());      // print a raw reading from the ADC

  //Serial.print("read average: \t\t");
  //Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  //Serial.print("get value: \t\t");
  //Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  //Serial.print("get units: \t\t");
  //Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided 
            // by the SCALE parameter (not set yet)  

  //scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  //scale.tare();               // reset the scale to 0

  //Serial.println("After setting up the scale:");

  //Serial.print("read: \t\t");
 //Serial.println(abs(scale.read()));                 // print a raw reading from the ADC

  //Serial.print("read average: \t\t");
  //Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  //Serial.print("get value: \t\t");
  ///Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  //Serial.print("get units: \t\t");
  //Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided 
            // by the SCALE parameter set with set_scale

  //Serial.println("Readings:");
}

int punch=0;
float strength=0;
long punchstart = 0;

void loop() {
  //Serial.print("one reading:\t");
  //Serial.print(scale.get_units(), 1);
  //Serial.print("\t| average:\t");
  //Serial.println(scale.get_units(10), 1);

  //scale.power_down();             // put the ADC in sleep mode
  //delay(500);
  //scale.power_up();
  float v = 238000-scale.read();
  if(v<0) {v=v*-1;}
  if(v<15000) {v = 0;}
  if(v>0 && punch==0) {
    punchstart = millis();
    punch=1;
    strength=v-15000;
  } else if(punch==1 && (millis()-punchstart)>500) {
    if((strength/300000)*100 > 3) {
      Serial.print("Punch: ");
      Serial.print((strength/300000)*100);
      Serial.println("%");
    }
    punch=0;
  } else if(v>0 && punch==1) {
    if((v-15000)>strength) {
      strength=v-15000;
    }
  }
}
