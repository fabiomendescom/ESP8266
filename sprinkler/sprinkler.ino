#include <esp8266defines.h>
#include <Ioteey.h>

#define SSID "mendes"
#define WIFI_PASSWORD "MENDESFAMILY"
#define PPSK "Mendes"
#define MQTT_SERVER "192.168.1.29"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""

#define DEVICE "sprinkler"
#define DEVICECLASS "toggle"
#define PIN D1
#define SENSOR "sprinkler"
#define MQTT_KEEPALIVE 120 //Override the keepalive from teh PubSubClient.h with the specifics of this program (seconds). Must be 20 or more to not have timing problems.

Ioteey iot;

void internalcallback(char* topic, byte* payload, unsigned int length) {
  iot.loadCallbackInfo(topic,payload,length);
};

long starthightime;

void commandReceived(char* sensorid, char* command) {
  Serial.print("SENSOR ID: ");
  Serial.println(sensorid);
  Serial.print("COMMAND: ");
  Serial.print(command);
  Serial.println("");    
  if(strcmp(command,"0")==0) {
    digitalWrite(PIN,LOW);
  } else {
    digitalWrite(PIN,HIGH);
    starthightime = millis();
  };  
  iot.publishSensorValueIfNeeded(SENSOR,iot.digitalConsistentRead(PIN));
};



void setup() {
    Serial.begin(9600);
    
    pinMode(PIN,OUTPUT);
    digitalWrite(PIN,LOW);
   
    iot.connectWIFI(SSID,WIFI_PASSWORD,PPSK);
    iot.connectMQTT(DEVICE, MQTT_SERVER,MQTT_PORT,MQTT_USER,MQTT_PASSWORD,&internalcallback);     
    iot.initializeDevice(DEVICE,DEVICECLASS, MQTT_KEEPALIVE);        
    iot.initializeSensor(SENSOR,"0");     
    iot.setCallback(&commandReceived);
    iot.start();   
    Serial.println(iot.getDeviceId());    
   
}

void loop() {
    iot.process();   
    //security measure. Stop after a period of time if don't receive communications
    if(iot.digitalConsistentRead(PIN)==HIGH) {
      if((millis()-starthightime) > 60*15*1000) {
        digitalWrite(PIN,LOW); 
        delay(100);
        iot.publishSensorValueIfNeeded(SENSOR,iot.digitalConsistentRead(PIN));
      }
    }
}


