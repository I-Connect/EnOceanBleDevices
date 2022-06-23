#pragma once
#include "EnOceanDataTypes.h"
#include "PTM215Eventhandler.h"
#include "bitset"
#include "map"
#include "vector"

namespace EnOcean {

// TODO Make configurable
#define INITIAL_REPEAT_WAIT 1000
#define REPEAT_INTERVAL 500


class PTM215EventAdapter {
  public:
    ~PTM215EventAdapter();

    struct HandlerRegistration {
      NimBLEAddress address = NimBLEAddress();
      PTM215EventHandler* handler = nullptr;
      uint8_t referenceId = 0;
      std::bitset<4> buttons; // A0, A1, B0, B1
    };

    void registerHandler(Device& device, PTM215EventHandler* hander, bool buttonA0, bool buttonA1, bool buttonB0, bool buttonB1, const uint8_t refId = 0);
    void registerHandler(Device& device, const uint8_t nodeId, bool buttonA0, bool buttonA1, bool buttonB0, bool buttonB1, const uint8_t refId = 0);
    HandlerRegistration getHandlerRegistration(const Device&) const;

    void handlePayload(Device& device, Payload& payload);

    /**
     * @brief Method used by repeatEventstask to generate a repeat event every XXX ms
     *
     */
    void generateRepeatEvents();
    void cancelRepeat();

  private:
    std::vector<HandlerRegistration> handlers;

    TaskHandle_t repeatEventsTaskHandle = nullptr;
    std::map<NimBLEAddress, PTM215Event> lastEvents;

    void handleSwitchAction(const uint8_t switchStatus, NimBLEAddress& bleAddress);
    PTM215Event mapToPTM215Event(Device& device, Payload& payload);
    void manageEventList(PTM215Event& event);
    void callEventHandlers(PTM215Event& event);
    void suspendRepeatTask();
    void startRepeatTask();
    bool isRepeatTaskSuspended();
};

} // namespace EnOcean
