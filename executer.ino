#define EXECUTEDELAY 500

std::vector<SignalEvent> executeList = std::vector<SignalEvent>();
std::vector<DelayedSignalEvent> delayedExecuteList = std::vector<DelayedSignalEvent>();

static unsigned long lastExecute = 0;

void executerInit() {
  
}

void executerLoop() {
  if (millis() - lastExecute > EXECUTEDELAY) {
    if(executeList.size() > 0) {
      executeSignal(executeList[0].signalId, executeList[0].onCode);
      executeList.erase(executeList.begin());
      lastExecute = millis();
    } else {
      for(int i=0; i<delayedExecuteList.size(); i++) {
        if(millis() > delayedExecuteList[i].executeAfter) {
          executeSignal(delayedExecuteList[i].signalId, delayedExecuteList[i].onCode);
          delayedExecuteList.erase(delayedExecuteList.begin() + i);
          lastExecute = millis();
          break;
        }
      }
    }
  }
}

void executeSignal(int index, bool state) {
  if(state)
    sendIr(signals[index].codeOn);
  else
    sendIr(signals[index].codeOff);
}

void addSignalEventToExecuteList(struct SignalEvent &event) {
  if(event.delayed == 0) {
    executeList.push_back(event);
  } else {
    DelayedSignalEvent delayedEvent = DelayedSignalEvent();
    delayedEvent.executeAfter = millis() + event.delayed;
    delayedEvent.signalId = event.signalId;
    delayedEvent.onCode = event.onCode;
    delayedExecuteList.push_back(delayedEvent);
  }
}

void addSignalToExecuteList(int index, bool state, int delayed) {
  SignalEvent event = SignalEvent();
  event.signalId = index;
  event.onCode = state;
  event.delayed = delayed;
  if(event.delayed == 0) {
    executeList.push_back(event);
  } else {
    DelayedSignalEvent delayedEvent = DelayedSignalEvent();
    delayedEvent.executeAfter = millis() + delayed;
    delayedEvent.signalId = index;
    delayedEvent.onCode = state;
    delayedExecuteList.push_back(delayedEvent);
  }
}

