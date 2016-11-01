/*
http://www.instructables.com/id/Illuminance-Monitor-with-ESP8266-IoT/?ALLSTEPS
https://learn.adafruit.com/tsl2561/use

Use the adafruit libraries above. They work

pins
ESP8266     MPR121
3v3         3.3v
D1          SCL
D2          SDA
GND         GND

- It seems like there is a need for a few delays in the setup. Both after the wire.begin and mpr121_setup
- Also, after compiliing, turn it off and back on again
- Lastly, ensure the electrodes are always put in sequence startign on 0. Don't skip. It seeems to mess it up.
- Don't change the eletrodes while is ON. Turn it off and back on.

 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

void configureSensor(void)
{
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
}

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Light Sensor Test"); Serial.println("");
  
  if(!tsl.begin())
  {
    Serial.println("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  configureSensor();

}


void loop(void) 
{  
  sensors_event_t event;
  tsl.getEvent(&event);
 
  if (event.light)
  {
    Serial.println(String(event.light));
  }
  else
  {
    Serial.println("Sensor overload");
  }
  delay(2000);
}

