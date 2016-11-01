#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
#include <SSDP.h>
#include <UPnP.h>

const char* ssid     = "mendessonic";
const char* password = "MENDESFAMILY";
char uuid[100];
char *deviceURN = "urn:schemas-upnp-org:device:BinaryLight:1";

//WebServer HTTP(80);
ESP8266WebServer server(80);

void setup() {

  Serial.begin(9600);
  
  delay(3000);    // Allow you to plug in the console
  
  Serial.println("Sensor system");
  Serial.printf("Boot version %d, flash chip size %d, SDK version %s\n",
                ESP.getBootVersion(), ESP.getFlashChipSize(), ESP.getSdkVersion());
  Serial.printf("Free sketch space %d\n", ESP.getFreeSketchSpace());
  
  Serial.print("Starting WiFi... ");
  WiFi.mode(WIFI_STA);

  int wifi_tries = 3;
  int wcr;
  while (wifi_tries-- >= 0) {
    WiFi.begin(ssid, password);
    //wcr = WiFi.waitForConnectResult();
    if (WiFi.status() == WL_CONNECTED)
      break;
    Serial.printf(" %d ", wifi_tries + 1);
    delay(4000);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("WiFi Failed\n");
    delay(2000);
    //ESP.restart();
  }

  IPAddress ip = WiFi.localIP();
  String ips = ip.toString();
  IPAddress gw = WiFi.gatewayIP();
  String gws = gw.toString();
  Serial.print("MAC "); Serial.print(WiFi.macAddress());
  Serial.printf(", SSID {%s}, IP %s, GW %s\n", WiFi.SSID().c_str(), ips.c_str(), gws.c_str());

  Serial.printf("Starting HTTP...\n");
  server.on("/index.html",  []() {
    server.send(200, "text/plain", "Hello World!");
  });
 
  uint32_t chipId = ESP.getChipId();
  //                      8 2 6 6  E X  D
  sprintf(uuid, "38323636-4558-4dda-9188-cda0e6%02x%02x%02x",
    (uint16_t) ((chipId >> 16) & 0xff),
    (uint16_t) ((chipId >>  8) & 0xff), 
    (uint16_t)   chipId        & 0xff  );
  
  SSDP.begin(uuid,"description.xml", "model name", "model number");
  UPnP.begin(&server, uuid, deviceURN, 80, "description.xml", "model name", "model number", "Fabio device", "123", "fabio man", "fabio.com", "model.com");

  server.begin();
  
  //MotionSensorService ms_srv = MotionSensorService();
  //UPnP.addService(&ms_srv);

  Serial.printf("Ready!\n");

}

void loop() {
    //ms_srv.poll();
    server.handleClient();

/*
  HTTP.handleClient();

  Serial.printf("After HandleClient : Heap %X\n", ESP.getFreeHeap());
  // Serial.printf("Called handleClient()...\n");
  delay(10);
*/
}
