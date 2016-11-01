/*
Pins
BUTTON bottom left side - connectd to 3.3
BUTTON bottom right side - connected to resitor and from resistor to GND
BUTTON top right side - connected to D1(GPIO5)
Resistor used: BROWN-BLACK-ORANGE-GOLD
 */

/*
IOT class
  iot(deviceid);
  getDeviceId()
  setSensorList("sensor1,sensor2")
  getSensorList()
  setAccessPointPortNumber(port);
  getAccessPointPortNumber();
  connectWIFI(SSID,password,PPSK)
  connectMQTT(mqttserver,mqttport,mqttuser,mqttpassword)
  setWIFIMode(Access Point or Station) //default is station
  publishSensorValue(sensorid,value);
  initializeDevice(); //sends the publish messages about ONLINE 
  initializeSensor(sensorname,initialvalue)  //sends the intial value for the sensor
  sensorValueChanges(value) //determines if this value causes a change on the previous value of this sensor. Used to ensure status is only sent on change not on every iteration of value
*/

#include <esp8266defines.h>
#include <Ioteey.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //obtained from https://github.com/knolleary/pubsubclient

//Generic to all devices in network
// WiFi Definitions //
#define WIFI_PPSK  "sparkfun" //private PSK key
#define WIFI_SSID "mendes"
#define WIFI_PASSWORD "MENDESFAMILY"
// MQTT server definitions
#define MQTT_SERVER "192.168.1.29"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_KEEPALIVE 5 //Override the keepalive from teh PubSubClient.h with the specifics of this program (seconds)
//WIFI station
#define SERVER_PORT 80

//Specific to this IOT device
#define MQTT_CLIENTID "buttonxxx"
#define MQTT_TOPIC "things/frontdoor/sensors/switch1/value"
#define MQTT_LASTWILL_TOPIC "things/frontdoor/device/status"
#define MQTT_SENSORS_TOPIC "things/frontdoor/sensors/list"
#define MQTT_SENSORS_LIST "switch1,switch2"
#define MQTT_HEALTH_MSG "ONLINE"
#define MQTT_LASTWILL_MSG "OFFLINE"
//PIN definitions
const int button1Pin = D1;     // the number of the pushbutton pin
const int led = D2;

WiFiServer server(SERVER_PORT);
WiFiClient espClient;
PubSubClient client(espClient);

// variables will change:
int button1State = LOW;         // variable for reading the pushbutton status
int button1PrevState = LOW;

void setupWiFiStation() {
  WiFi.mode(WIFI_STA);   
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

  WiFi.softAP(AP_NameChar, WIFI_PPSK);
  server.begin();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    if (client.connect(MQTT_CLIENTID,MQTT_LASTWILL_TOPIC,0,true,MQTT_LASTWILL_MSG)) {
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

void setup() {
  pinMode(button1Pin, INPUT_PULLUP); 
  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  Serial.begin(9600);
  Serial.println("HERE");
  
  //setupWiFiStation(); 
  //client.setServer(MQTT_SERVER, MQTT_PORT);
  //delay(200);
  //if (!client.connected()) {
  //   reconnect();
  //}
  //client.loop();
  //delay(200);  
  //client.publish(MQTT_LASTWILL_TOPIC,MQTT_HEALTH_MSG, true); 
  //client.publish(MQTT_TOPIC,"0", true); 
  //client.publish(MQTT_SENSORS_TOPIC,MQTT_SENSORS_LIST, true); 
}

void loop() {
  //delay(200);
  //if (!client.connected()) {
  //   reconnect();
 // }
  //client.loop();
        
  // read the state of the pushbutton value:
  //button1State = digitalRead(button1Pin);
  
  digitalWrite(led,digitalRead(button1Pin));
  
/*
  if(button1State!=button1PrevState) {
     button1PrevState = button1State;
     if(button1State==HIGH) {
    //    client.publish(MQTT_TOPIC,"1", true); 
     } else {
      //  client.publish(MQTT_TOPIC,"0", true);       
     }
     Serial.println("published ");        
     Serial.println(button1State);
  }
*/  
}
