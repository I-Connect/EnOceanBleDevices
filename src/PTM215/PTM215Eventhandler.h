#pragma once

#include "Arduino.h"
#include "EnOceanDataTypes.h"
#include "map"

namespace EnOcean {

enum class EventType : byte {
  Pushed = 0,
  Repeat,
  ReleaseShort,
  ReleaseLong
};

enum class Button : byte {
  ButtonA0 = 0,
  ButtonA1 = 1,
  ButtonB0 = 2,
  ButtonB1 = 3
};

/**
 * @brief Event send to PTM215 eventhandler
 *
 */
struct PTM215Event {
  Button button;
  EventType eventType;
  uint32_t pushStartTime = 0;
  Device* device;
  nodeId_t referenceId = 0;
};

/**
 * @brief Abstract class for handling PTM215 events, handleEvent() methods needs to be implemented on derived classes
 *
 * On construction of a class derived from this, the instance is automatically registered in the global ptm215EventHandlerMap by nodeId
 * This allows registering an eventhandler to the BLEScannerSubscriber by nodeId, i.e. from a config file or from a setting stored in non-volatile storage
 *
 * On destruction the mapping is removed.
 *
 */
class PTM215EventHandler {
  public:
    PTM215EventHandler(const nodeId_t nodeId);
    virtual ~PTM215EventHandler();

    const nodeId_t getId() const {
      return nodeId;
    };

    virtual void handleEvent(const PTM215Event& event) = 0;

  private:
    const nodeId_t nodeId;
};

typedef std::map<nodeId_t, PTM215EventHandler*> PTM215EventHandlerMap;
extern PTM215EventHandlerMap ptm215EventHandlerMap;

} // namespace EnOcean
