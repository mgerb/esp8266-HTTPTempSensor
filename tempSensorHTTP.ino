#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <FS.h>
#include <DHT.h>

DHT dht(5, DHT11);

char ssid[32] = "";
char password[32] = "";
char sensorName[32] = "";

const int httpPort = 80;
const char* host = "162.243.16.83";

boolean connected;

// DNS server
const byte DNS_PORT = 53;

// Web server
ESP8266WebServer server(80);
DNSServer dnsServer;
WiFiClient client;

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  delay(1000);
  
  loadCredentials();
  
  //try to connect to access point
  WiFi.disconnect();
  WiFi.begin(ssid,password);
  
  int connRes = WiFi.waitForConnectResult();
  Serial.print ( "connRes: " );
  Serial.println ( connRes );
  
  Serial.println("Connecting");

  for (int i = 0; WiFi.status() != WL_CONNECTED && i <10; i++){
    Serial.print(".");
    delay(1000);
  }
  
  //if connected skip setting up access point
  if(WiFi.status() != WL_CONNECTED){
    connected = false;
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP("Sensor Config");
   
    delay(500);
    
    server.on("/", handleRoot);
    server.on("/config", handleConfig);
    server.on("/bootstrap.min.css", handleBootstrap);
    server.on("/style.css", handleCss);
    server.onNotFound(handleNotFound);

    SPIFFS.begin();
    dnsServer.start(DNS_PORT, "*", apIP);
    server.begin();
  }
  else{
    connected = true;
    WiFi.mode(WIFI_STA);
    dht.begin();
  }
  
}

void loop() {
  //if could not connect to access point
  if (!connected){
    dnsServer.processNextRequest();
    server.handleClient();
  }
  else{
    if (!client.connect(host, httpPort)) {
      return;
    }
  
    float temp = dht.readTemperature(true);
    float humidity = dht.readHumidity();

    // We now create a URI for the request
    String url = "/temperature";
    url += "?temperature=";
    url += temp;
    url += "&humidity=";
    url += humidity;
    url += "&key=esp1234";
    url += "&location=";
    url += sensorName;

    url.replace(" ", "+");
    
    Serial.println("URL: " + url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    delay(100);
    
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print("Response: " + line);
    }
    
    //delay(10000);
    ESP.deepSleep(120 * 1000000, WAKE_RF_DEFAULT);
  }
}
