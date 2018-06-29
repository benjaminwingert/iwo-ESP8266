fauxmoESP fauxmo;

std::vector<String> fauxmoDeviceIds = std::vector<String>();

void alexaInit() {
  Serial.println(F("Starting alexa devices..."));
  for(int i=0; i<getSignalCount(); i++) {
    if(fauxmoHasDeviceName(getSignalName(i)) == false) {
      unsigned char id = fauxmo.addDevice(getSignalName(i).c_str());
      fauxmoDeviceIds.push_back(getSignalName(i));
    }
  }
  for(int i=0; i<getGroupCount(); i++) {
    if(fauxmoHasDeviceName(getGroupName(i)) == false) {
      unsigned char id = fauxmo.addDevice(getGroupName(i).c_str());
      fauxmoDeviceIds.push_back(getGroupName(i));
    }
  }

  if(fauxmoDeviceIds.size() > 0)
    fauxmo.enable(true);
  else
    fauxmo.enable(false);
  
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
    Serial.print("fauxmo onSetState: "); Serial.print(device_name); Serial.println(state ? " on" : " off");
    setSignalOrGroupStateByName(String(device_name), state);
    executeSignalOrGroupByName(String(device_name), state);
  });
  
  fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
    Serial.print("fauxmo onGetState: "); Serial.print(device_name); Serial.println(getSignalOrGroupStateByName(String(device_name)) ? " on" : " off");
    return getSignalOrGroupStateByName(String(device_name));
  });
  
}

bool fauxmoHasDeviceName(String s) {
  for(int i=0; i<fauxmoDeviceIds.size(); i++) {
    if(s.equals(fauxmoDeviceIds[i])) {
      Serial.print("fauxmo device already found: "); Serial.println(s);
      return true;
    }
  }
  return false;
}

void alexaLoop() {
  fauxmo.handle();
}
