#define RECV_PIN D6
#define IR_LED D4
#define CAPTURE_BUFFER_SIZE 650
#define TIMEOUT 15U
#define READSIGNALCOUNT 1

struct Code {
  int type;
  std::vector<unsigned int> raw;
  unsigned long value;
  unsigned int address;
  unsigned int command;
};

IRrecv irRecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT, true);
IRsend irSend = IRsend(IR_LED);

void irInit() {
  pinMode(IR_LED, OUTPUT);
  irRecv.enableIRIn();
}

void irLoop() {
}

struct Code loadCodeFromString(String codeString) {
  Serial.println(F("loadCodeFromString()"));
  Serial.println(codeString);

  DynamicJsonBuffer jsonBuffer;
  JsonObject &codeJson = jsonBuffer.parseObject(codeString);
  if (!codeJson.success()) {
    return Code();
  }
  
  Code code = Code();
  code.type = codeJson["type"].as<int>();
  for(int i=0; i<codeJson["rawData"].size(); i++) {
    code.raw.push_back(codeJson["rawData"][i].as<unsigned int>());
  }
  code.value = (unsigned long)strtoul(codeJson["value"].as<String>().c_str(), 0, 16);
  code.address = (unsigned int)atoi(codeJson["address"].as<String>().c_str());
  code.command = (unsigned int)atoi(codeJson["command"].as<String>().c_str());

  return code;
}

void sendIr(String codeString) {
  sendIrRaw(codeString, false);
}

void sendIrRawCode(std::vector<unsigned int> raw) {
  Serial.println("sendIrRawCode");
  uint16_t freq = 38000;  // Default to 38kHz.
  
  if (raw.size() < 2) 
    return;

  uint16_t count = (uint16_t)raw.size();
  uint16_t raw_array[count];
  for(int i=0; i<count; i++)
    raw_array[i] = (uint16_t)raw[i];

  Serial.print(freq); Serial.print(count); Serial.println(raw_array[0]);
  irSend.sendRaw(&raw_array[0], count, freq);
}

void sendIrRaw(String codeString, bool useRaw) {
  Serial.print("sendIr"); Serial.println(codeString);
  Code code = loadCodeFromString(codeString);

  if(code.value == 0) {
    Serial.println("code to json not successful");
    return;
  }

  Serial.print("type: "); Serial.println(code.type);
  Serial.print("value: "); Serial.println(code.value);
  Serial.print("address: "); Serial.println(code.address);
  Serial.print("command: "); Serial.println(code.command);
  
  uint16_t bits = 0;
  uint16_t repeat = 10;

  if(useRaw)
    code.type = 30;
  
  switch (code.type) {
    #if SEND_RC5
    case RC5:  // 1
      if (bits == 0)
        bits = RC5_BITS;
      irSend.sendRC5(code.value, bits, repeat);
      break;
    #endif
    #if SEND_RC6
    case RC6:  // 2
      if (bits == 0)
        bits = RC6_MODE0_BITS;
      irSend.sendRC6(code.value, bits, repeat);
      break;
    #endif
    #if SEND_NEC
    case NEC:  // 3
      if (bits == 0)
        bits = NEC_BITS;
      Serial.println("send NEC");
      irSend.sendNEC(code.value, bits, repeat);
      break;
    #endif
    #if SEND_SONY
    case SONY:  // 4
      if (bits == 0)
        bits = SONY_12_BITS;
      repeat = std::max(repeat, (uint16_t) SONY_MIN_REPEAT);
      irSend.sendSony(code.value, bits, repeat);
      break;
    #endif
    #if SEND_PANASONIC
    case PANASONIC:  // 5
      if (bits == 0)
        bits = PANASONIC_BITS;
      irSend.sendPanasonic64(code.value, bits, repeat);
      break;
    #endif
    #if SEND_JVC
    case JVC:  // 6
      if (bits == 0)
        bits = JVC_BITS;
      irSend.sendJVC(code.value, bits, repeat);
      break;
    #endif
    #if SEND_SAMSUNG
    case SAMSUNG:  // 7
      if (bits == 0)
        bits = SAMSUNG_BITS;
      irSend.sendSAMSUNG(code.value, bits, repeat);
      break;
    #endif
    #if SEND_WHYNTER
    case WHYNTER:  // 8
      if (bits == 0)
        bits = WHYNTER_BITS;
      irSend.sendWhynter(code.value, bits, repeat);
      break;
    #endif
    #if SEND_AIWA_RC_T501
    case AIWA_RC_T501:  // 9
      if (bits == 0)
        bits = AIWA_RC_T501_BITS;
      repeat = std::max(repeat, (uint16_t) AIWA_RC_T501_MIN_REPEAT);
      irSend.sendAiwaRCT501(code.value, bits, repeat);
      break;
    #endif
    #if SEND_LG
    case LG:  // 10
      if (bits == 0)
        bits = LG_BITS;
      irSend.sendLG(code.value, bits, repeat);
      break;
    #endif
    #if SEND_MITSUBISHI
    case MITSUBISHI:  // 12
      if (bits == 0)
        bits = MITSUBISHI_BITS;
      repeat = std::max(repeat, (uint16_t) MITSUBISHI_MIN_REPEAT);
      irSend.sendMitsubishi(code.value, bits, repeat);
      break;
    #endif
    #if SEND_DISH
    case DISH:  // 13
      if (bits == 0)
        bits = DISH_BITS;
      repeat = std::max(repeat, (uint16_t) DISH_MIN_REPEAT);
      irSend.sendDISH(code.value, bits, repeat);
      break;
    #endif
    #if SEND_SHARP
    case SHARP:  // 14
      if (bits == 0)
        bits = SHARP_BITS;
      irSend.sendSharpRaw(code.value, bits, repeat);
      break;
    #endif
    #if SEND_COOLIX
    case COOLIX:  // 15
      if (bits == 0)
        bits = COOLIX_BITS;
      irSend.sendCOOLIX(code.value, bits, repeat);
      break;
    #endif
    case DAIKIN:  // 16
    case KELVINATOR:  // 18
    case MITSUBISHI_AC:  // 20
    case GREE:  // 24
    case ARGO:  // 27
    case TROTEC:  // 28
    case TOSHIBA_AC:  // 32
    case FUJITSU_AC:  // 33
    case HAIER_AC:  // 38
    case HITACHI_AC:  // 40
    case HITACHI_AC1:  // 41
    case HITACHI_AC2:  // 42
      break;
    #if SEND_DENON
    case DENON:  // 17
      if (bits == 0)
        bits = DENON_BITS;
      irSend.sendDenon(code.value, bits, repeat);
      break;
    #endif
    #if SEND_SHERWOOD
    case SHERWOOD:  // 19
      if (bits == 0)
        bits = SHERWOOD_BITS;
      repeat = std::max(repeat, (uint16_t) SHERWOOD_MIN_REPEAT);
      irSend.sendSherwood(code.value, bits, repeat);
      break;
    #endif
    #if SEND_RCMM
    case RCMM:  // 21
      if (bits == 0)
        bits = RCMM_BITS;
      irSend.sendRCMM(code.value, bits, repeat);
      break;
    #endif
    #if SEND_SANYO
    case SANYO_LC7461:  // 22
      if (bits == 0)
        bits = SANYO_LC7461_BITS;
      irSend.sendSanyoLC7461(code.value, bits, repeat);
      break;
    #endif
    #if SEND_RC5
    case RC5X:  // 23
      if (bits == 0)
        bits = RC5X_BITS;
      irSend.sendRC5(code.value, bits, repeat);
      break;
    #endif
    #if SEND_PRONTO
    case PRONTO:  // 25
      //parseStringAndSendPronto(code.value_str, repeat);
      break;
    #endif
    #if SEND_NIKAI
    case NIKAI:  // 29
      if (bits == 0)
        bits = NIKAI_BITS;
      irSend.sendNikai(code.value, bits, repeat);
      break;
    #endif
    #if SEND_RAW
    case RAW:  // 30
      sendIrRawCode(code.raw);
      break;
    #endif
    #if SEND_GLOBALCACHE
    case GLOBALCACHE:  // 31
      //parseStringAndSendGC(code.value_str);
      break;
    #endif
    #if SEND_MIDEA
    case MIDEA:  // 34
      if (bits == 0)
        bits = MIDEA_BITS;
      irSend.sendMidea(code.value, bits, repeat);
      break;
    #endif
    #if SEND_MAGIQUEST
    case MAGIQUEST:  // 35
      if (bits == 0)
        bits = MAGIQUEST_BITS;
      irSend.sendMagiQuest(code.value, bits, repeat);
      break;
    #endif
    #if SEND_LASERTAG
    case LASERTAG:  // 36
      if (bits == 0)
        bits = LASERTAG_BITS;
      irSend.sendLasertag(code.value, bits, repeat);
      break;
    #endif
    #if SEND_CARRIER_AC
    case CARRIER_AC:  // 37
      if (bits == 0)
        bits = CARRIER_AC_BITS;
      irSend.sendCarrierAC(code.value, bits, repeat);
      break;
    #endif
    #if SEND_MITSUBISHI2
    case MITSUBISHI2:  // 39
      if (bits == 0)
        bits = MITSUBISHI_BITS;
      repeat = std::max(repeat, (uint16_t) MITSUBISHI_MIN_REPEAT);
      irSend.sendMitsubishi2(code.value, bits, repeat);
      break;
    #endif
    #if SEND_GICABLE
    case GICABLE:  // 43
      if (bits == 0)
        bits = GICABLE_BITS;
      repeat = std::max(repeat, (uint16_t) GICABLE_BITS);
      irSend.sendGICable(code.value, bits, repeat);
      break;
    #endif
  }
}

String readIr() {
  String res = "";
  int i=0;
  while(i<READSIGNALCOUNT) {
    yield();
    decode_results results;
    if (irRecv.decode(&results)) {
      uint32_t now = millis();
      Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
      
      if (results.overflow){
        Serial.println(F("WARNING: IR code is too big for buffer (>= 650). This result shouldn't be trusted until this is resolved. Edit & increase CAPTURE_BUFFER_SIZE.\n"));
      }
      //res += resultToHumanReadableBasic(&results);
      yield();  // Feed the WDT as the text output can take a while to print.
      
      res += resultToJsonString(&results);     
      
      yield();  // Feed the WDT (again)
      i++;
    }
  }
  Serial.println(res);
  return res;
}

String resultToJsonString(const decode_results *results) {
  String output = "{";
  
  // Start declaration
  output += "\"type\":";
  output += results->decode_type;
  output += ",\"rawData\":[";
  
  for (uint16_t i = 1; i < results->rawlen; i++) {
    uint32_t usecs;
    for (usecs = results->rawbuf[i] * RAWTICK; usecs > UINT16_MAX; usecs -= UINT16_MAX) {
      output += "0x" + uint64ToString(UINT16_MAX);
      output += ",0,";
    }
    output += uint64ToString(usecs, 10);
    if (i < results->rawlen - 1)
      output += ",";
  }
  output +="]";

  // Comment
  output += "" ;
  // Only display the value if the decode type doesn't have an A/C state.
  if (!hasACState(results->decode_type))
    output += ",\"value\":\"0x" + uint64ToString(results->value, 16) + "\"";

  if (results->decode_type != UNKNOWN) {
    if (hasACState(results->decode_type)) {
      uint16_t nbytes = results->bits / 8;
      output += ",\"state\":[\"" + uint64ToString(nbytes) + "\",";
      for (uint16_t i = 0; i < nbytes; i++) {
        output += "\"0x";
        if (results->state[i] < 0x10) 
          output += "0";
        output += uint64ToString(results->state[i], 16);
        if (i < nbytes - 1)
          output += "\",";
      }
      output += "\"]";
    } else {
      if (results->address > 0 || results->command > 0) {
        Serial.println(results->address);
        Serial.println(results->command);
        output += ",\"address\":";
        output += results->address;
        output += ",\"command\":";
        output += results->command;
      }
    }
  }
  output += "}";
  return output;
}
