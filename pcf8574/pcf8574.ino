/*  
Library from here:
https://github.com/WereCatf/PCF8574_ESP

NOTE:
- The PCF8574 always keeps the pins HIGH by default. You may get a mistaken idea that it is not working. It is simpy because
all pins are high. Try making them low and you will see it works.
- The PCF8575 is 5v. Make sure to wire it to 5v. however, you must ground it to the ESP8266 GND otherwise it won't work
- Even if 5V, the signal can be 3.3v from ESP8266 so no logical converters necessary
*/

#include <pcf8574_esp.h>

PCF8574 pcf8574(0x20, 2, 0);

void setup() {
  //turn all off
  for(int i=0;i<7;i++) {
     pcf8574.write(i,LOW);
  }
  pcf8574.write(0,HIGH);
}

void loop() {
  pcf8574.toggle(0);
  pcf8574.toggle(3);
  delay(3000);
}
