void handleRoot(){
    
    server.send(200, "text/html", page_index);
    
}

void handleConfig(){
  
  if (server.arg("password") != "" && server.arg("ssid") != "" && server.arg("name") != ""){
   
    
    server.arg("ssid").toCharArray(ssid, sizeof(ssid) -1);
    server.arg("password").toCharArray(password, sizeof(password) -1);
    server.arg("name").toCharArray(sensorName, sizeof(sensorName) -1);
    
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
  EEPROM.commit();
  EEPROM.end();
}

void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  EEPROM.get(0+sizeof(ssid)+sizeof(password), sensorName);
  EEPROM.end();
  
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(sensorName);
}
