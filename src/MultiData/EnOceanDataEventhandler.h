#pragma once

#include "Arduino.h"
#include "EnOceanDataTypes.h"
#include "map"
#include "vector"

namespace EnOcean {

struct DataEvent {
  std::vector<Parameter> parameters;
  Device* device;
};

/**
 * @brief Abstract class for handling Data events, handleEvent() methods needs to be implemented on derived classes
 *
 * On construction of a class derived from this, the instance is automatically registered in the global ptm215EventHandlerMap by nodeId
 * This allows registering an eventhandler to the EnOceanBLEScannerSubscriber by nodeId, i.e. from a config file or from a setting stored in non-volatile storage
 *
 * On destruction the mapping is removed.
 *
 */
class DataEventHandler {
  public:
    DataEventHandler(const uint8_t nodeId);
    virtual ~DataEventHandler();

    const uint8_t getId() const {
      return nodeId;
    };

    virtual void handleEvent(DataEvent& event) = 0;

  private:
    const uint8_t nodeId;
};

typedef std::map<uint8_t, DataEventHandler*> DataEventHandlerMap;
extern DataEventHandlerMap dataEventHandlerMap;

} // namespace EnOcean
