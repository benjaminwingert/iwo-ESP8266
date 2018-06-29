WiFiClient espClient;
WiFiManager wifiManager;

void wifiInit() {
  Serial.println(F("Setting up WiFi..."));
  setupWifi();
}

void wifiLoop() {
}

void setupWifi() {
  delay(10);
  
  wifiManager.setTimeout(300);  // Time out after 5 mins.
  if (!wifiManager.autoConnect()) {
    Serial.println("Wifi failed to connect and hit timeout.");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  /* // TODO add dns if wifi setup is initiated
  if (mdns.begin(HOSTNAME, WiFi.localIP())) {
    debug(F("MDNS responder started"));
  }*/

  Serial.println("WiFi connected. IP address: " + WiFi.localIP().toString());
}

void resetWifi() {
  wifiManager.resetSettings();
}

