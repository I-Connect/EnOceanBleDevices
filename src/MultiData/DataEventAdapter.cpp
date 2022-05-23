#include "DataEventAdapter.h"
#include "EnOceanUtils.h"
#include "esp_task_wdt.h"
#include "mbedtls/aes.h"

namespace EnOcean {

DataEventAdapter::~DataEventAdapter() {
}

void DataEventAdapter::registerHandler(Device& device, const uint8_t nodeId, const uint8_t refId) {
  if (dataEventHandlerMap.count(nodeId)) {
    registerHandler(device, dataEventHandlerMap[nodeId], refId);
  } else {
    log_e("NodeId [%d] not found in DataEventHandlerMap", nodeId);
  }
}

void DataEventAdapter::registerHandler(Device& device, DataEventHandler* handler, const uint8_t refId) {
  HandlerRegistration reg;
  reg.address = device.address;
  reg.handler = handler;
  reg.referenceId = refId;
  handlers.push_back(reg);
}

void DataEventAdapter::handlePayload(Device& device, Payload& payload) {
  DataEvent event = mapToDataEvent(device, payload);
  callEventHandlers(event);
}

void DataEventAdapter::callEventHandlers(DataEvent& event) {
  for (auto const& reg : handlers) {
    if (reg.address == event.device->address) {
      event.referenceId = reg.referenceId;
      reg.handler->handleEvent(event);
    }
  }
}

DataEvent DataEventAdapter::mapToDataEvent(Device& device, Payload& payload) {
  DataEvent event;

  parsePayloadParameters(payload.data.raw, payload.len - 11, event.parameters);
  event.device     = &device;

  return event;
}

void DataEventAdapter::parsePayloadParameters(byte* payload, const uint8_t size, std::vector<Parameter>& result) {
  byte* payloadPtr = payload;

  while (payloadPtr < payload + size) {
    Parameter parameter;
    parameter.size = pow(2, *payloadPtr >> 6);                      // leftmost 2 bits
    parameter.type = (ParameterType)(*payloadPtr & 0b00111111);     // rightmost 6 bits
    payloadPtr++;
    if (parameter.size > 4) { // custom size, specified in first byte of data
      // TODO read custom size parameter, skipped for now
      parameter.size = *payloadPtr++;
    } else {
      memcpy(&parameter.value, payloadPtr, parameter.size);
    }
    payloadPtr += parameter.size;
    result.push_back(parameter);
  }
}

} // namespace EnOcean
