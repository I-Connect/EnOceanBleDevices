#pragma once
#include "../EnOceanDataTypes.h"
#include "DataEventhandler.h"
#include "bitset"
#include "vector"


namespace EnOcean {

/**
 * @brief EventAdapter handling payloads received by BLEScannerSubscriber with multiple data points like from EMDCB and STM550
 *
 */
class DataEventAdapter {
  public:
    ~DataEventAdapter();

    void registerHandler(Device& device, DataEventHandler* hander, const uint8_t refId);
    void registerHandler(Device& device, const uint8_t nodeId, const uint8_t refId);
    void handlePayload(Device& device, Payload& payload);

  private:
    struct HandlerRegistration {
      NimBLEAddress address;
      DataEventHandler* handler;
      uint8_t referenceId;
    };
    std::vector<HandlerRegistration> handlers;

    DataEvent mapToDataEvent(Device& device, Payload& payload);
    void parsePayloadParameters(byte* payload, const uint8_t size, std::vector<Parameter>& result);
    void callEventHandlers(DataEvent& event);
};

} // namespace EnOcean
