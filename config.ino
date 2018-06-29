struct DelayedSignalEvent {
  unsigned long executeAfter;
  int signalId;
  bool onCode;
};

struct SignalEvent {
  int signalId;
  int delayed;
  bool onCode;
};

struct GroupData {
  int id;
  String name;
  std::vector<SignalEvent> signalsOn;
  std::vector<SignalEvent> signalsOff;
  bool state;
};

struct SignalData {
  int id;
  String name;
  String codeOn;
  String codeOff;
  bool state;
};

std::vector<GroupData> groups = std::vector<GroupData>();
std::vector<SignalData> signals = std::vector<SignalData>();

void configInit() {
  Serial.println(F("Mounting file system..."));
  if (!SPIFFS.begin()) {
    Serial.println(F("Failed to mount file system"));
    return;
  }

  // TODO create always empty config for debugging
  Serial.println(F("Loading saved configuration..."));
  if(RESET)
    saveConfig();
  if (!loadConfig()) {
    saveConfig();
  }  
}

bool loadConfig() {
  Serial.println(F("loadConfig()"));
  printFreeHeap();
  
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println(F("Failed to open config file"));
    return false;
  }

  size_t size = configFile.size(); 
  Serial.print(F("Config file size:")); Serial.println(size);
  
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  Serial.print("config: >"); Serial.print(buf.get()); Serial.println("<");
  return loadConfigFromString(buf.get());
}

bool loadConfigFromString(String jsonString) {
  Serial.println(F("loadConfigFromString()"));
  Serial.println(jsonString);
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject &newCfg = jsonBuffer.parseObject(jsonString);
  if (!newCfg.success()) {
    Serial.println("Failed to parse jsonString");
    return false;
  }
  
  groups.clear();
  for(int i=0; i<newCfg["groups"].size(); i++) {
    GroupData group = GroupData();
    group.id = newCfg["groups"][i]["id"].as<int>();
    group.name = newCfg["groups"][i]["name"].as<String>();
    for(int j=0; j<newCfg["groups"][i]["signalsOn"].size(); j++) {
      SignalEvent event = SignalEvent();
      event.signalId = newCfg["groups"][i]["signalsOn"][j]["signalId"].as<int>();
      event.delayed = newCfg["groups"][i]["signalsOn"][j]["delayed"].as<int>();
      event.onCode = newCfg["groups"][i]["signalsOn"][j]["onCode"].as<bool>();
      group.signalsOn.push_back(event);
    }
    for(int j=0; j<newCfg["groups"][i]["signalsOff"].size(); j++) {
      SignalEvent event = SignalEvent();
      event.signalId = newCfg["groups"][i]["signalsOff"][j]["signalId"].as<int>();
      event.delayed = newCfg["groups"][i]["signalsOff"][j]["delayed"].as<int>();
      event.onCode = newCfg["groups"][i]["signalsOff"][j]["onCode"].as<bool>();
      group.signalsOff.push_back(event);
    }
    group.state = newCfg["groups"][i]["state"].as<bool>();
    groups.push_back(group);
  }
  
  signals.clear();
  for(int i=0; i<newCfg["signals"].size(); i++) {
    SignalData signal = SignalData();
    signal.id = newCfg["signals"][i]["id"].as<int>();
    signal.name = newCfg["signals"][i]["name"].as<String>();
    signal.codeOn = newCfg["signals"][i]["codeOn"].as<String>();
    signal.codeOff = newCfg["signals"][i]["codeOff"].as<String>();
    signal.state = newCfg["signals"][i]["state"].as<bool>();
    signals.push_back(signal);
  }

  alexaInit();
  
  return saveConfig();
}

bool saveConfig() {
  Serial.println(F("saveConfig()"));
  printFreeHeap();
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }
  
  configFile.println(serializeConfig());
  Serial.println(serializeConfig());

  configFile.close();
  
  return true;
}

String serializeConfig() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &cfg = jsonBuffer.createObject();
  JsonArray &groupsJson = cfg.createNestedArray("groups");
  JsonArray &signalsJson = cfg.createNestedArray("signals");
  
  for(int i=0; i<groups.size(); i++) {
    JsonObject& groupJson = groupsJson.createNestedObject();
    groupJson["id"] = groups[i].id;
    groupJson["name"] = groups[i].name;
    JsonArray& signalsOnJson = groupJson.createNestedArray("signalsOn");
    for(int j=0; j<groups[i].signalsOn.size(); j++) {
      JsonObject& signalJson = signalsOnJson.createNestedObject();
      signalJson["signalId"] = groups[i].signalsOn[j].signalId;
      signalJson["delayed"] = groups[i].signalsOn[j].delayed;
      signalJson["onCode"] = groups[i].signalsOn[j].onCode;
    }
    JsonArray& signalsOffJson = groupJson.createNestedArray("signalsOff");
    for(int j=0; j<groups[i].signalsOff.size(); j++) {
      JsonObject& signalJson = signalsOffJson.createNestedObject();
      signalJson["signalId"] = groups[i].signalsOff[j].signalId;
      signalJson["delayed"] = groups[i].signalsOff[j].delayed;
      signalJson["onCode"] = groups[i].signalsOff[j].onCode;
    }
    groupJson["state"] = groups[i].state;
  }
  
  for(int i=0; i<signals.size(); i++) {
    JsonObject& signalJson = signalsJson.createNestedObject();
    signalJson["id"] = signals[i].id;
    signalJson["name"] = signals[i].name;
    signalJson["codeOn"] = signals[i].codeOn;
    signalJson["codeOff"] = signals[i].codeOff;
    signalJson["state"] = signals[i].state;
  }

  String output;
  cfg.printTo(output);
  return output;
}

int getSignalCount() {
  return signals.size();
}

int getSignalId(int index) {
  return signals[index].id;
}

String getSignalName(int index) {
  return signals[index].name;
}

String getSignalCodeOn(int index) {
  return signals[index].codeOn;
}

String getSignalCodeOff(int index) {
  return signals[index].codeOff;
}

bool getSignalState(int index) {
  return signals[index].state;
}

void setSignalState(int index, bool b) {
  signals[index].state = b;
}

int getSignalIndexById(int id) {
  for(int i=0; i<signals.size(); i++) {
    if(id == getSignalId(i)) {
      return i;
    }
  }
  return false;
}

void setSignalOrGroupStateByName(String s, bool b) {
  for(int i=0; i<groups.size(); i++) {
    if(s.equals(getGroupName(i))) {
      //Serial.print(s); Serial.print(" == "); Serial.println(getGroupState(i));
      setGroupState(i, b);
      return;
    }
  }
  for(int i=0; i<signals.size(); i++) {
    if(s.equals(getSignalName(i))) {
      //Serial.print(s); Serial.print(" == "); Serial.println(getSignalName(i));
      setSignalState(i, b);
      return;
    }
  }
}

bool getSignalOrGroupStateByName(String s) {
  for(int i=0; i<groups.size(); i++) {
    //Serial.print(s); Serial.print(" == "); Serial.println(getGroupName(i));
    if(s.equals(getGroupName(i))) {
      return getGroupState(i);
    }
  }
  for(int i=0; i<signals.size(); i++) {
    //Serial.print(s); Serial.print(" == "); Serial.println(getSignalName(i));
    if(s.equals(getSignalName(i))) {
      return getSignalState(i);
    }
  }
  return false;
}

int getGroupCount() {
  return groups.size();
}

String getGroupName(int index) {
  return groups[index].name;
}

std::vector<SignalEvent> getGroupOnCodes(int index) {
  return groups[index].signalsOn;
}

std::vector<SignalEvent> getGroupOffCodes(int index) {
  return groups[index].signalsOff;
}

bool getGroupState(int index) {
  return groups[index].state;
}

void setGroupState(int index, bool b) {
  groups[index].state = b;
}

void executeSignalOrGroupByName(String s, bool state) {
  Serial.println("executeSignalOrGroupByName");
  for(int i=0; i<groups.size(); i++) {
    //Serial.print(s); Serial.print(" == "); Serial.println(getGroupName(i));
    if(s.equals(getGroupName(i))) {
      if(state) {
        for(int k=0; k<groups[i].signalsOn.size(); k++)
          addSignalEventToExecuteList(groups[i].signalsOn[k]);
      } else {
        for(int k=0; k<groups[i].signalsOff.size(); k++)
          addSignalEventToExecuteList(groups[i].signalsOff[k]);
      }
      return;
    }
  }
  for(int i=0; i<signals.size(); i++) {
    //Serial.print(s); Serial.print(" == "); Serial.println(getSignalName(i));
    if(s.equals(getSignalName(i))) {
      // TODO add signal type check (ir, rf433, rf866, etc)
      addSignalToExecuteList(i, state, 0);
      return;
    }
  }
}

