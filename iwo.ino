#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <algorithm>
#include <string>
#include <vector>
#include "fauxmoESP.h"
#include <ArduinoJson.h>
#include "FS.h"

#define BAUD_RATE 115200

#define RESET false

void setup(void) {  
  Serial.begin(BAUD_RATE);
  delay(1500);
  wifiInit();
  configInit();
  webserverInit();
  irInit();
  alexaInit();
  executerInit();
}

void loop(void) {
  wifiLoop();
  yield();
  webserverLoop();
  yield();
  irLoop();
  yield();
  alexaLoop();
  yield();
  executerLoop();
  yield();
  printInfoInterval(5000);
}

static unsigned long last = 0;
void printInfoInterval(int interval) {
  if (millis() - last > interval) {
    last = millis();
    printFreeHeap();
  }
}

void printFreeHeap() {
  Serial.print("Free heap: "); Serial.print(ESP.getFreeHeap()); Serial.println(" bytes");
}


