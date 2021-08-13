#include "EnOceanDataEventhandler.h"

namespace EnOcean {

DataEventHandlerMap dataEventHandlerMap;

DataEventHandler::DataEventHandler(const uint8_t nodeId) : nodeId(nodeId) {
  dataEventHandlerMap[nodeId] = this;
}

DataEventHandler::~DataEventHandler() {
  dataEventHandlerMap.erase(nodeId);
}

} // namespace EnOcean
