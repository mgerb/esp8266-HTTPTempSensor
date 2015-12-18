void handleRoot(){
    if(SPIFFS.exists("/index.html")){

      File file = SPIFFS.open("/index.html", "r");

      server.streamFile(file, "text/html");

      file.close();
    }
    else{
      server.send(404, "file doesn't exist");
    } 
}

void handleBootstrap(){
    if(SPIFFS.exists("/bootstrap.min.css")){

      File file = SPIFFS.open("/bootstrap.min.css", "r");

      server.streamFile(file, "text/css");

      file.close();
    }
    else{
      server.send(404, "file doesn't exist");
    }
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

void handleCss(){
    if(SPIFFS.exists("/style.css")){

      File file = SPIFFS.open("/style.css", "r");

      server.streamFile(file, "text/css");

      file.close();
    }
    else{
      server.send(404, "file doesn't exist");
    }
}

void handleNotFound(){
    if(SPIFFS.exists("/index.html")){

      File file = SPIFFS.open("/index.html", "r");

      server.streamFile(file, "text/html");

      file.close();
    }
    else{
      server.send(404, "file doesn't exist");
    }
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
