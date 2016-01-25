#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "page_index.h"

// Data wire is plugged into port 4 on the ESP8266
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress deviceAddress;

char ssid[32] = "";
char password[32] = "";
char sensorName[32] = "";
char serverPassword[32] = "";

const int httpPort = 80;
const char* host = "www.mitchel.io";

boolean connected;

//count to restart if access point has been up for too long
int access_point_timer = 0;

// DNS server
const byte DNS_PORT = 53;

// Web server
ESP8266WebServer server(80);
DNSServer dnsServer;
WiFiClient client;

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);

//function prototypes ----------
void handleRoot();
void handleConfig();
void handleNotFound();
void saveConfig();
void loadCredentials();
//------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  delay(1000);

  //load information from EEPROM
  loadCredentials();
  
  //try to connect to access point
  WiFi.disconnect();
  WiFi.begin(ssid,password);
  
  int connRes = WiFi.waitForConnectResult();
  Serial.print ( "connRes: " );
  Serial.println ( connRes );
  
  Serial.println("Connecting");

  //wait 10 seconds for connection
  for (int i = 0; WiFi.status() != WL_CONNECTED && i <10; i++){
    Serial.print(".");
    delay(1000);
  }
  
  //if connected skip setting up access point
  if(WiFi.status() != WL_CONNECTED){
    connected = false;
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP("Sensor Config", "thereisnospoon");
   
    delay(500);
    
    server.on("/", handleRoot);
    server.on("/config", handleConfig);
    server.onNotFound(handleNotFound);

    dnsServer.start(DNS_PORT, "*", apIP);
    server.begin();
  }
  else{
    connected = true;
    WiFi.mode(WIFI_STA);
    
    sensors.begin();
    sensors.getAddress(deviceAddress, 0);
    sensors.setResolution(deviceAddress, TEMPERATURE_PRECISION);
  }
  
}

void loop() {
  //if could not connect to access point
  if (!connected){
    dnsServer.processNextRequest();
    server.handleClient();

    //start a counter - will reset device if in config mode for 10 seconds
    delay(1);
    access_point_timer++;
    if(access_point_timer > 600000){
      Serial.println("Uptime too long: Restarting");
      ESP.restart();
    }
  }
  else{
    //will restart device if WiFi connection is lost
    if(WiFi.status() != WL_CONNECTED){
      Serial.println("Connection lost - Restarting in 10 seconds");
      delay(10000);
      ESP.restart();
    }
    
    if (!client.connect(host, httpPort)) {
      return;
    }

    boolean bad_reading = true;
    float temp = 0;
    
    while (bad_reading == true){
      sensors.requestTemperatures();
      temp = sensors.getTempF(deviceAddress);

      if (isnan(temp)){
        bad_reading == true;
      }
      else {
        bad_reading = false;
        break;
      }

      delay(5000);
      Serial.println("bad reading");
    }
    
    delay(1000);
    
    // We now create a URI for the request
    String url = "/temperature";
    url += "?temperature=";
    url += temp;
    url += "&key=";
    url += serverPassword;
    url += "&location=";
    url += sensorName;

    url.replace(" ", "+");
    
    Serial.println("URL: " + url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    delay(1000);
    
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print("Response: " + line);
    }
    
    delay(60000);
  }
}

void handleRoot(){
    server.send(200, "text/html", page_index);  
}

void handleConfig(){
  
  if (server.arg("password") != "" && server.arg("ssid") != "" && server.arg("name") != "" && server.arg("serverPassword") != ""){
   
    server.arg("ssid").toCharArray(ssid, sizeof(ssid) -1);
    server.arg("password").toCharArray(password, sizeof(password) -1);
    server.arg("name").toCharArray(sensorName, sizeof(sensorName) -1);
    server.arg("serverPassword").toCharArray(serverPassword, sizeof(serverPassword) -1);
    
    saveConfig();

    server.send(200, "text/html", "Config successful. Rebooting");

    ESP.restart();
  }

  server.send(200, "text/html", "<script>window.onload = function() { window.location = \"/\"; }</script>");
}

void handleNotFound(){
    server.send(404, "text/html", "404");
}

/** Store WLAN credentials to EEPROM */
void saveConfig() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), sensorName);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(sensorName), serverPassword);
  EEPROM.commit();
  EEPROM.end();
  
}

void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  EEPROM.get(0+sizeof(ssid)+sizeof(password), sensorName);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(sensorName), serverPassword);
  EEPROM.end();
  
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(sensorName);
  Serial.println(serverPassword);
}

