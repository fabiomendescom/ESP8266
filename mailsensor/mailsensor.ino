/*
Pins
BUTTON bottom left side - connectd to 3.3
BUTTON bottom right side - connected to resitor and from resistor to GND
BUTTON top right side - connected to D1(GPIO5)
Resistor used: BROWN-BLACK-ORANGE-GOLD
 */

#include <DHT.h>  
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//Pubsubclient library obtained from https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";
#define wifi_ssid "mendes"
#define wifi_password "MENDESFAMILY"

#define mqtt_server "192.168.1.29"
#define mqtt_user ""
#define mqtt_password ""

WiFiServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Clientmail")) {
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

// constants won't change. They're used here to
// set pin numbers:
const int sensorPin = 14;     // the number of the pushbutton pin

// variables will change:
int sensorState = 0;         // variable for reading the pushbutton status
char state[2] = "0";
char prevstate[2] = "0";

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
  setupWiFiStation(); 
   
      client.setServer(mqtt_server, 1883);
    
}

void loop() {
  delay(200);
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
        
  // read the state of the pushbutton value:
  sensorState = digitalRead(sensorPin);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (sensorState == HIGH) {
    strcpy(state,"1");
    //Serial.println("OPEN");
  } else {
    strcpy(state,"0");
    //Serial.println("CLOSED");
  }

  //Serial.println("");
  if(strcmp(prevstate,state)!=0) {
     strcpy(prevstate,state);  
      //if(strcmp(state,"1")!=0) {
        client.publish("mendes/house/sensor/mail",state, true); 
        Serial.println("published ");          
        Serial.println("Chegou correio");    
      //}
     //Serial.println(state);
  }
}
