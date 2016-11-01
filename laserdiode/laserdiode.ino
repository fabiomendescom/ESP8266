/*
 * This is easy:
 * S - any pin in ESP8266 that you can set to HIGH to turn on the laser
 * + - 5V power
 * - Ground
 */

void setup() {
  // put your setup code here, to run once:
  pinMode(0,OUTPUT);
  digitalWrite(0,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
 
}
