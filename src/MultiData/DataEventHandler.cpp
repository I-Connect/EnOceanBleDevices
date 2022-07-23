#include "DataEventhandler.h"

namespace EnOcean {

DataEventHandlerMap dataEventHandlerMap;

DataEventHandler::DataEventHandler(const nodeId_t nodeId) : nodeId(nodeId) {
  dataEventHandlerMap[nodeId] = this;
}

DataEventHandler::~DataEventHandler() {
  dataEventHandlerMap.erase(nodeId);
}

} // namespace EnOcean
