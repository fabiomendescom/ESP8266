/**
 * Pin config
 * ESP8266    DHT11
 * 3.3        +
 * GND        -
 * D4(GPI02)  Data
 * 
 * You must use this DHT library to work
 * https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver/code
 */

#include <DHT.h>  
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//Pubsubclient library obtained from https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>

#define DHTTYPE DHT11
#define DHTPIN  2

DHT dht(DHTPIN, DHTTYPE, 11);

int humidity, temp_f;
float vbatt; 
char s_vbatt[50];

#define INSETUPMODE 1
#define BUTTONLOWBEFORE 10
#define BUTTONHIGHBEFORE 12
#define SETUPDONE 2
#define FUNCTION 3

int state = BUTTONLOWBEFORE; 

const int buttonPin = 5; 
int buttonState = 0;   
bool insetupmode = false;
int buttonhighstart = 0;
char s_humidity[50];
char s_temperature[50];

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";
#define wifi_ssid "mendes"
#define wifi_password "MENDESFAMILY"

#define mqtt_server "192.168.1.29"
#define mqtt_user ""
#define mqtt_password ""

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 4; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

WiFiServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
int numberconnectries = 0;
long startprocess = 0;

ADC_MODE(ADC_VCC);

void setupWiFiStation() {
  WiFi.mode(WIFI_STA);   
  WiFi.begin(wifi_ssid, wifi_password);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {  
    delay(100);
  }
  Serial.println("WIFI connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void setupWiFiAccessPoint()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
  server.begin();
}

void initHardware()
{
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    numberconnectries = numberconnectries + 1;
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Clienttempinside")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

long seconds(int secs) {
    return secs * 1000 * 1000;
}

long minutes(int mins) {
  return seconds(mins*60);
}

void setup()  
{ 
  startprocess=millis();
  Serial.begin(9600);
  Serial.println("Starting");
  //vbatt = ESP.getVcc() / 1024.0;
  //vbatt = vbatt * 1.377; //do this to adapt to the real voltage. Apparently there is a difference. Check if this holds in the future
  //do it by measuring the voltage from the battery and compare it.
  //Serial.println("");
  //Serial.println(vbatt);  
  setupWiFiStation();  
  pinMode(buttonPin, INPUT);
  pinMode(LED_PIN, OUTPUT);

      client.setServer(mqtt_server, 1883);
      if (!client.connected()) {
        reconnect();
      }
      client.loop();  
      //snprintf (s_vbatt, 50, "%d", vbatt);
      //dtostrf(vbatt , 2, 2, s_vbatt);
      //client.publish("mendes/house/sensor/temphumidity/battery/value",s_vbatt, true); 
      //Serial.print("published "); 
      //Serial.println(s_vbatt);       

      //char s_numberconnectries[50];
      //dtostrf(numberconnectries , 2, 2, s_numberconnectries);
      //client.publish("mendes/backyard/sensor/temphumidity/connection/value",s_numberconnectries, true); 
      //Serial.print("published "); 
      //Serial.println(s_numberconnectries);  
      

}

void loop()      
{  
      if (!client.connected()) {
        reconnect();
      }
      client.loop();  
        
    dht.begin();
    delay(200); 
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_f = dht.readTemperature(true);     // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_f)) {
      Serial.println("Failed to read from DHT sensor!");
    } else if (humidity>100 || temp_f > 200) {
      //sleep for a minute and try to get the temp again
      ESP.deepSleep(minutes(1)); 
    } else {
      Serial.println(temp_f);
      Serial.println(humidity);
      //snprintf (s_humidity, 50, "%d", humidity);
      //client.publish("mendes/house/sensor/temphumidity/humidity/value",s_humidity, true); 
      //Serial.print("published "); 
      //Serial.println(s_humidity);     
      snprintf (s_temperature, 50, "%d", temp_f);
      client.publish("mendes/house/sensor/temphumidity/temperatureinside",s_temperature, true); 
      Serial.print("published "); 
      Serial.println(s_temperature);          
    }

      //char s_startprocess[50];
      //dtostrf(millis()-startprocess, 2, 2, s_startprocess);
      //client.publish("mendes/backyard/sensor/temphumidity/connectlength/value",s_startprocess, true); 
      //Serial.print("published "); 
      //Serial.println(s_numberconnectries);  
    
    //ESP.deepSleep(minutes(3));
    delay(60000*3);  
}




