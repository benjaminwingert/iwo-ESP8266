#define HTTP_PORT 80

ESP8266WebServer server(HTTP_PORT);

void webserverInit() {
  Serial.println(F("Starting web server..."));
  // webpages
  server.on("/", HTTP_GET, handleIndexGet);
  server.on("/index.html", HTTP_GET, handleIndexGet);
  // api
  server.on("/api/config", HTTP_GET, handleApiConfigGet); 
  server.on("/api/config", HTTP_POST, handleApiConfigPost);
  server.on("/api/read/ir", HTTP_POST, handleApiReadIrPost);
  server.on("/api/read/rf433", HTTP_POST, handleApiReadRf433Post);
  server.on("/api/read/rf866", HTTP_POST, handleApiReadRf866Post);
  server.on("/api/send/ir", HTTP_POST, handleApiSendIrPost);
  server.on("/api/send/irRaw", HTTP_POST, handleApiSendIrRawPost);
  server.on("/api/reset", HTTP_GET, handleApiReset);
  // error page
  server.onNotFound(handleNotFound);
  server.begin();
}

void webserverLoop() {
  server.handleClient();
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  if (path.endsWith("/"))
    path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  Serial.print(F("handleFileRead ")); Serial.println(path);
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleIndexGet() {
  //Serial.println(F("handleIndexGet"));
  handleFileRead("/index.html");
}

void handleApiConfigGet() {
  //Serial.println(F("handleApiConfigGet"));
  String output = serializeConfig();
  server.send(200, "text/plain", output);
}

void handleApiConfigPost() {
  Serial.println(F("handleApiConfigPost"));
  if(loadConfigFromString(server.arg("plain")))
    server.send(200, "text/html", "");
  else
    server.send(500, "text/html", "");
}

void handleApiReadIrPost() {
  Serial.println(F("handleApiReadIrPost"));
  String result = readIr();
  Serial.print("read: "); Serial.println(result);
  server.send(200, "text/html", result);
}

void handleApiReadRf433Post() {
  Serial.println(F("handleApiReadRf433Post"));
  server.send(200, "text/html", "handleApiReadRf433Post");
}

void handleApiReadRf866Post() {
  Serial.println(F("handleApiReadRf866Post"));
  server.send(200, "text/html", "handleApiReadRf866Post");
}

void handleApiSendIrPost() {
  Serial.println(F("handleApiSendIrPost"));
  sendIr(server.arg("plain"));
  server.send(200, "text/html", "handleApiSendIrPost");
}

void handleApiSendIrRawPost() {
  Serial.println(F("handleApiSendIrRawPost"));
  sendIrRaw(server.arg("plain"), true);
  server.send(200, "text/html", "handleApiSendIrRawPost");
}

void handleApiReset() {
  server.send(200, "text/html", "");
  resetWifi();
  delay(10);
  ESP.restart();
  delay(1000);
}

void handleNotFound() {
  server.send(404, "text/plain", "");
}
