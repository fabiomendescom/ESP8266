extern "C" {
#include "user_interface.h"
}
#include <Ioteey.h>

//#define PIN D3
//#define MQTT_KEEPALIVE 120 //Override the keepalive from teh PubSubClient.h with the specifics of this program (seconds). Must be 20 or more to not have timing problems.

//receiveMessageCallback deviceCallout;
execCommandCallback execCommandCallout;
//getAttributeValueCallback getAttributeValueCallout;

Ioteey iot;   //sourceofrequest(http,mqtt,serial),parms,sensororactuator,actuatororsensorid,messageprocessedsuccessfully) 

long timer = millis();

void setup() {
    Serial.begin(9600);
    //Serial.print("Reset reason: ");
    //Serial.println(ESP.getResetReason());
    //pinMode(PIN, INPUT_PULLUP);  
    
    //ESP.eraseConfig();
    //ESP.reset();
    
    Serial.println("");
    Serial.println("");
    Serial.print("Sketch size: ");
    Serial.println(ESP.getSketchSize());
    Serial.print(F("Free size: "));
    Serial.println(ESP.getFreeSketchSpace());
    Serial.print(F("% Sketch: "));
    float a = ((float)ESP.getSketchSize()/(float)(ESP.getSketchSize()+ESP.getFreeSketchSpace()))*100;
    Serial.print(a);
    Serial.println("%");    
 
    iot.loadSettings();  //load from EEPROM and put values in Hashmap in iot object

    //Default setting
    iot.setThingPassword("1234567890");
    iot.saveSettingsToEEPROM();

    Serial.println("ADD DEVICE BEGIN");
    Serial.println(iot.addDevice("TemperatureSensor"));   
    Serial.println(iot.addDevice("Irrigation"));
    //add same name. THis should not be added. It should return id 0 because it is the same name
    Serial.println(iot.addDevice("TEMPERATURESENSOR"));   
    Serial.println("ADD DEVICE END");

    Serial.println(iot.getDeviceCode(0));
    Serial.println(iot.getDeviceCode(1));    
    Serial.println(iot.getDeviceCount());

    Serial.println(iot.getDeviceName());

    iot.addCapability(0, "Capability 1");
    iot.addCapability(0, "Capability 2");
    iot.addCapability("Capability 3");

    Serial.println("GET CAPABILITY");
    Serial.println(iot.getCapabilityCount());
    Serial.println(iot.getCapability(0,0));
    Serial.println(iot.getCapability(0,1));
    Serial.println(iot.getCapability(0,2));

    iot.addCommand(0, "Command1");
    iot.addCommand(0, "Command2"); 
    Serial.println("GET COMMAND");
    Serial.println(iot.getCommandCount());
    Serial.println(iot.getCommand(0,0));
    Serial.println(iot.getCommand(0,1));

    iot.addCommandValue(0,0, "ON");
    iot.addCommandValue(0,0, "OFF"); 
    Serial.println("GET COMMAND VALUE");
    Serial.println(iot.getCommandValueCount(0,0));
    Serial.println(iot.getCommandValue(0,0,0));
    Serial.println(iot.getCommandValue(0,0,1));

    iot.addCommandValueParameter(0,0,0,"temperaturetoset");
    iot.addCommandValueParameter(0,0,0,"valuetomodify");
    Serial.println(iot.getCommandValueParameter(0,0,0,0));
    Serial.println(iot.getCommandValueParameter(0,0,0,1));

    iot.addCommandValueParameter(0,0,1,"abc");
    iot.addCommandValueParameter(0,0,1,"def");

    iot.addAttribute(0, "Temperature");
    iot.setAttributeValue(0,0,"80F");
    iot.addAttribute(0, "Humidity");
    iot.setAttributeValue(0,1,"76%"); 
    Serial.println("GET ATTRIBUTE");
    Serial.println(iot.getAttributeCount());
    Serial.println(iot.getAttribute(0,0));
    Serial.println(iot.getAttribute(0,1));
    Serial.println("GET ATTRIBUTE VALUES");
    Serial.println(iot.getAttributeValue(0,0));
    Serial.println(iot.getAttributeValue(0,1));

    iot.addSetting(0, "Setting 1");
    iot.addSetting(0, "Setting 2"); 
    Serial.println("GET SETTING");
    Serial.println(iot.getSettingCount());
    Serial.println(iot.getSetting(0,0));
    Serial.println(iot.getSetting(0,1));

    //Default settings. They only get added if they don't exist
    iot.addSettingValue(0,0, "Setting Value 1");
    iot.addSettingValue(0,1, "Setting Value 2"); 
    Serial.println("GET SETTING VALUE");
    Serial.println(iot.getSettingCount());
    Serial.println(iot.getSettingValue(0,0));
    Serial.println(iot.getSettingValue(0,1));

    iot.setExecCommandCallback(execCommandCallout);

    iot.connectAll();
}

void loop() {
    iot.process();  
    
    //iot.publishDeviceValueIfNeeded(DEVICE,iot.digitalConsistentRead(PIN)==LOW);
}

bool execCommandCallout(const char* sourceaddress, int deviceid, int commandid, const char* command, int commandparametercount, const char commandparameternames[][PARAMETERSIZE], const char commandparametervalues[][PARAMETERSIZE]) { 
    Serial.println("===============================");
    Serial.println(sourceaddress);
    Serial.println(deviceid);
    Serial.println(command);
    Serial.println("===============================");
    for(int i=0;i<commandparametercount;i++) {
      Serial.print(commandparameternames[i]);
      Serial.print("=");
      Serial.println(commandparametervalues[i]);
    }
    return true;
};


