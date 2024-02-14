#include "DataEventAdapter.h"
#include "EnOceanUtils.h"
#include "esp_task_wdt.h"
#include "mbedtls/aes.h"

namespace EnOcean {

DataEventAdapter::~DataEventAdapter() {
}

void DataEventAdapter::registerHandler(Device& device, const nodeId_t nodeId) {
  if (dataEventHandlerMap.count(nodeId)) {
    registerHandler(device, dataEventHandlerMap[nodeId]);
  } else {
    log_e("NodeId [%u] not found in DataEventHandlerMap", nodeId);
  }
}

void DataEventAdapter::registerHandler(Device& device, DataEventHandler* handler) {
  HandlerRegistration reg;
  reg.address = device.address;
  reg.handler = handler;
  handlers.push_back(reg);
}

void DataEventAdapter::unregisterDevice(const NimBLEAddress& address) {
  handlers.erase(remove_if(handlers.begin(), handlers.end(), [address](HandlerRegistration & registration) {
    return registration.address == address;
  }), handlers.end());
}

DataEventHandler* DataEventAdapter::getEventHandler(const Device& device) const {
  for (auto const& reg : handlers) {
    if (reg.address == device.address) {
      return reg.handler;
    }
  }
  return nullptr;
}

void DataEventAdapter::handlePayload(Device& device, Payload& payload) {
  DataEvent event = mapToDataEvent(device, payload);
  callEventHandlers(event);
}

void DataEventAdapter::callEventHandlers(DataEvent& event) {
  for (auto const& reg : handlers) {
    if (reg.address == event.device->address) {
      reg.handler->handleEvent(event);
    }
  }
}

DataEvent DataEventAdapter::mapToDataEvent(Device& device, Payload& payload) {
  DataEvent event;

  parsePayloadParameters(payload, event.parameters);
  event.device     = &device;

  return event;
}

} // namespace EnOcean
