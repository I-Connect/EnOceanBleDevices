#include "PTM215EventAdapter.h"
#include "esp_task_wdt.h"
#include "mbedtls/aes.h"
#include <algorithm>

namespace EnOcean {

void repeatEventsTask(void* pvParameters) {
  #ifdef DEBUG_ENOCEAN
  log_d("TASK: PTM215b repeatEvents task started on core: %d", xPortGetCoreID());
  #endif
  esp_task_wdt_add(NULL);
  PTM215EventAdapter* adapter = static_cast<PTM215EventAdapter*>(pvParameters);

  while (1) {
    esp_task_wdt_reset();
    delay(REPEAT_INTERVAL);
    adapter->generateRepeatEvents();
  }
}

PTM215EventAdapter::~PTM215EventAdapter() {
  if (repeatEventsTaskHandle) {
    vTaskDelete(repeatEventsTaskHandle);
  }
}

void PTM215EventAdapter::registerHandler(Device& device, const nodeId_t handlerNodeId) {
  if (ptm215EventHandlerMap.count(handlerNodeId)) {
    registerHandler(device, ptm215EventHandlerMap[handlerNodeId]);
  } else {
    log_e("NodeId [%d] not found in ptm215EventHandlerMap", handlerNodeId);
  }
}

void PTM215EventAdapter::registerHandler(Device& device, PTM215EventHandler* handler) {
  HandlerRegistration reg;
  reg.address    = device.address;
  reg.handler    = handler;
  handlers.push_back(reg);
}

void PTM215EventAdapter::unregisterDevice(const NimBLEAddress& address) {
  handlers.erase(
  remove_if(handlers.begin(), handlers.end(), [address](HandlerRegistration & registration) {
    return registration.address == address;
  }),
  handlers.end()
  );
}

void PTM215EventAdapter::handlePayload(Device& device, Payload& payload) {
  PTM215Event ptm215Event = mapToPTM215Event(device, payload);
  callEventHandlers(ptm215Event);
  manageEventList(ptm215Event);
}

void PTM215EventAdapter::callEventHandlers(PTM215Event& event) {
  for (auto const& reg : handlers) {
    if (reg.address == event.device->address) {
      PTM215Event evt = event; // Make copy to avoid modifications on parameter
      reg.handler->handleEvent(evt);
    }
  }
}

void PTM215EventAdapter::cancelRepeat() {
  lastEvents.clear();
  suspendRepeatTask();
}

void PTM215EventAdapter::manageEventList(PTM215Event& event) {
  NimBLEAddress address = event.device->address;
  if (event.eventType == EventType::Pushed) {
    lastEvents[address] = event;
  } else if ((event.eventType == EventType::ReleaseShort) || (event.eventType == EventType::ReleaseLong)) {
    lastEvents.erase(address);
  }

  if (lastEvents.size() > 0) {
    startRepeatTask();
  } else {
    suspendRepeatTask();
  }
}

void PTM215EventAdapter::startRepeatTask() {
  if (!repeatEventsTaskHandle) {
    xTaskCreatePinnedToCore(&repeatEventsTask, "PMT215RepeatTask", 4096, this, 1, &repeatEventsTaskHandle, CONFIG_BT_NIMBLE_PINNED_TO_CORE);
  }

  if (isRepeatTaskSuspended()) {
    vTaskResume(repeatEventsTaskHandle);
    esp_task_wdt_add(repeatEventsTaskHandle);
    #ifdef DEBUG_ENOCEAN
    log_d("TASK: PTM215b repeatEvents task resumed on core: %d", xPortGetCoreID());
    #endif
  }
}

void PTM215EventAdapter::suspendRepeatTask() {
  if (repeatEventsTaskHandle != NULL) {
    esp_task_wdt_delete(repeatEventsTaskHandle);
    vTaskSuspend(repeatEventsTaskHandle);
  }
  #ifdef DEBUG_ENOCEAN
  log_d("TASK: PTM215b repeatEvents task suspended");
  #endif
}

bool PTM215EventAdapter::isRepeatTaskSuspended() {
  if (repeatEventsTaskHandle) {
    eTaskState state = eTaskGetState(repeatEventsTaskHandle);
    return ((state == eSuspended) || (state == eDeleted));
  }
  return false;
}

PTM215Event PTM215EventAdapter::mapToPTM215Event(Device& device, Payload& payload) {
  PTM215Event event;
  const uint8_t switchStatus = payload.data.raw[0];

  event.device = &device;

  Button button;
  if (switchStatus & 0b00010000) {
    button = Button::ButtonB1;
  } else if (switchStatus & 0b00001000) {
    button = Button::ButtonB0;
  } else if (switchStatus & 0b00000100) {
    button = Button::ButtonA1;
  } else {
    button = Button::ButtonA0;
  }

  event.button = button;

  if ((switchStatus & 0x01) == 0) { // release
    if ((lastEvents.count(device.address) == 0) || (millis() - INITIAL_REPEAT_WAIT < lastEvents[device.address].pushStartTime)) {
      event.eventType = EventType::ReleaseShort;
    } else {
      event.eventType = EventType::ReleaseLong;
    }
  } else { // push
    event.eventType     = EventType::Pushed;
    event.pushStartTime = millis();
  }

  return event;
}

void PTM215EventAdapter::generateRepeatEvents() {
  uint32_t latestStart = 0;
  for (auto& pair : lastEvents) {
    PTM215Event event = pair.second;
    latestStart = max(latestStart, event.pushStartTime);
    if (millis() - INITIAL_REPEAT_WAIT > event.pushStartTime) {
      pair.second.eventType = EventType::Repeat;
      callEventHandlers(pair.second);
    }
  }
  if (millis() - latestStart > 30 * REPEAT_INTERVAL) {
    cancelRepeat();
  }
}

} // namespace EnOcean
