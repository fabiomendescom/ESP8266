/*
Pins
BUTTON bottom left side - connectd to 3.3
BUTTON bottom right side - connected to resitor and from resistor to GND
BUTTON top right side - connected to D1(GPIO5)
Resistor used: BROWN-BLACK-ORANGE-GOLD
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//Pubsubclient library obtained from https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// constants won't change. They're used here to
// set pin numbers:
const int dooropenPin   = 2;
const int doorclosedPin = 13;
const int doorlockedPin = 4;     // the number of the pushbutton pin
const int buzzerPin = 5;
const int RX = 14;
const int TX = 12;
  
SoftwareSerial ESPserial(RX, TX); // RX | TX

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
  //Serial.println("WIFI connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());  
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
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("slidingdoor")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("mendes/backyard/sensor/temphumidity/temperature/value");
      client.subscribe("mendes/house/sensor/temphumidity/temperature/value");
      //Serial.println("subscribed");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



// variables will change:
int openbuttonstate = LOW;
int closedbuttonstate = LOW;
int lockedbuttonstate = LOW;         // variable for reading the pushbutton status

int openbuttonprevstate = LOW;
int closedbuttonprevstate = LOW;
int lockedbuttonprevstate = LOW;         // variable for reading the pushbutton status


long openlastDebounceTime = millis();
long closedlastDebounceTime = millis();
long lockedlastDebounceTime = millis();

long lasttime = millis();

bool opentriggered = true; 
bool closedtriggered = true; 
bool lockedtriggered = true; 

bool opentriggeredhighbefore = false; 
bool closedtriggeredhighbefore = false; 
bool lockedtriggeredhighbefore = false; 


char temperature[10] = "";
char temperaturein[10] = "";

void collectdata(bool activatedbymqtt=false) {
   int frequency;
   char s_output[5];
   char doorstate[30];
   char doorclosed[30] = "CLOSED";
   char dooropen[30] = "OPEN";
   char doorlocked[30] = "LOCKED";

   if(!activatedbymqtt) {
      if(openbuttonstate==HIGH) {
         frequency = 500;
         strcpy (doorstate,dooropen);               
      }
      if(closedbuttonstate==HIGH) {
         frequency = 600;
         strcpy (doorstate,doorclosed);            
      }      
      if(lockedbuttonstate==HIGH) {
        frequency = 400;
        strcpy (doorstate,doorlocked);   
      }       
      tone(buzzerPin, frequency, 500);
   }
      
   //Send data to serial port to be picked up by Arduino
   ESPserial.print("TX=");
   ESPserial.print(temperature);
   ESPserial.print(",");
   ESPserial.print(temperaturein);
   ESPserial.print(",");   
   ESPserial.print(openbuttonstate);
   ESPserial.print(",");
   ESPserial.print(closedbuttonstate);
   ESPserial.print(",");   
   ESPserial.print(lockedbuttonstate);
   ESPserial.println("");

   Serial.print("TX=");
   Serial.print(temperature);
   Serial.print(",");
   Serial.print(temperaturein);
   Serial.print(",");   
   Serial.print(openbuttonstate);
   Serial.print(",");
   Serial.print(closedbuttonstate);
   Serial.print(",");   
   Serial.print(lockedbuttonstate);
   Serial.println("");

   if(!activatedbymqtt) {
      client.publish("mendes/familyroom/sensor/slidingdoor",doorstate, true);     
   }
   
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

   char data[10] = "";

   for (int i=0;i<length;i++) {
     if(i<10) {
       data[i] = (char)payload[i];
     }
     Serial.print((char)payload[i]);
   }
   Serial.println();

  if(strcmp(topic,"mendes/backyard/sensor/temphumidity/temperature/value")==0) {
     strcpy(temperature,data);
  }
  if(strcmp(topic,"mendes/house/sensor/temphumidity/temperature/value")==0) {
     strcpy(temperaturein,data);
  }
  collectdata(true);
}

void setup() {
  // initialize the LED pin as an output:
  //pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(dooropenPin, INPUT);
  pinMode(doorclosedPin, INPUT);
  pinMode(doorlockedPin, INPUT);
  pinMode(buzzerPin,OUTPUT);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  Serial.begin(9600);
  
  openbuttonprevstate = digitalRead(dooropenPin);
  closedbuttonprevstate = digitalRead(doorclosedPin);
  lockedbuttonprevstate = digitalRead(doorlockedPin); 
  openlastDebounceTime = millis();
  closedlastDebounceTime = millis();
  lockedlastDebounceTime = millis();  
  //collectdata();
}

void loop() {
  // read the state of the pushbutton value:
  openbuttonstate = digitalRead(dooropenPin);

    //collectedata() is activates by 4 types of events
    // 1. Open button activated
    // 2. Closed button activated
    // 3. Locked button activated
    // 4. Time. This is used for temperature

     //1.
    if(openbuttonstate != openbuttonprevstate) {
        openbuttonprevstate = openbuttonstate;      
        openlastDebounceTime = millis(); 
        opentriggered = false;
    } else if(opentriggered) {
        openlastDebounceTime = millis();  
    } else {
      if(millis()-openlastDebounceTime > 1000 && ((opentriggeredhighbefore && openbuttonstate==LOW) || (openbuttonstate==HIGH))) {
        //Serial.print("Door open trigger: ");
        //Serial.println(openbuttonstate);
        collectdata();  
        openlastDebounceTime = millis();       
        opentriggered=true;
        if(openbuttonstate==HIGH) {
          opentriggeredhighbefore = true;
        } else {
          opentriggeredhighbefore = false;
        }
      }
    }

    //2. 
    closedbuttonstate = digitalRead(doorclosedPin);
    if(closedbuttonstate != closedbuttonprevstate) {
        closedbuttonprevstate = closedbuttonstate;      
        closedlastDebounceTime = millis(); 
        closedtriggered = false;
    } else if(closedtriggered) {
        closedlastDebounceTime = millis();  
    } else {
      if(millis()-closedlastDebounceTime > 1000 && ((closedtriggeredhighbefore && closedbuttonstate==LOW) || (closedbuttonstate==HIGH))) {
        //Serial.print("Door closed trigger: ");
        //Serial.println(closedbuttonstate);
        collectdata();  
        closedlastDebounceTime = millis();       
        closedtriggered=true;
        if(closedbuttonstate==HIGH) {
          closedtriggeredhighbefore = true;
        } else {
          closedtriggeredhighbefore = false;
        }
      }
    }

    //3.
    lockedbuttonstate = digitalRead(doorlockedPin);
    if(lockedbuttonstate != lockedbuttonprevstate) {
        //Serial.print("State changed ");
        //Serial.println(analogRead(doorlockedPin));
        //Serial.println(lockedbuttonstate);
        lockedbuttonprevstate = lockedbuttonstate;      
        lockedlastDebounceTime = millis(); 
        lockedtriggered = false;
    } else if(lockedtriggered) {
        lockedlastDebounceTime = millis();  
    } else {
      if(millis()-lockedlastDebounceTime > 1000 && ((lockedtriggeredhighbefore && lockedbuttonstate==LOW) || (lockedbuttonstate==HIGH))) {
        //Serial.print("Door locked trigger: ");
        //Serial.println(lockedbuttonstate);
        collectdata();  
        lockedlastDebounceTime = millis();       
        lockedtriggered=true;
        if(lockedbuttonstate==HIGH) {
          lockedtriggeredhighbefore = true;
        } else {
          lockedtriggeredhighbefore = false;
        }
      }
    }

    //4.
    //if(millis()-lasttime > 5000) { // 15*(1000*60)) { //15 minutes
    //   Serial.println("Time trigger");
    //   lasttime = millis();
    //   collectdata(true);
    //}

   if (!client.connected()) {
     reconnect();
   }
   client.loop();    
   yield();
}
