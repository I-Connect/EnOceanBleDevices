#include "Arduino.h"
#include "BleScanner.h"
#include "EnOceanBLEScannerSubscriber.h"
#include "PTM215/PTM215EventAdapter.h"
#include "EnOceanUtils.h"


// Fill in PTM215 address and security key
#define PTM_BLE_ADDRESS1 "XX:XX:XX:XX:XX:XX"
#define PTM_SECURITY_KEY1 "????????????????????????????????"
#define PTM_DEVICE_REFERENCE_ID1 77 // Id available in eventhandler when receiving events 

#define PTM_BLE_ADDRESS2 "XX:XX:XX:XX:XX:XX"
#define PTM_SECURITY_KEY2 "????????????????????????????????"
#define PTM_DEVICE_REFERENCE_ID2 88 // Id available in eventhandler when receiving events

#define PTMHANDLER_NODE_ID 23

// Class derived from PTM215EventHandler that will receive PTM215 events through BLEScanner and BLScannerSubscriber
class PTMHandler : public EnOcean::PTM215EventHandler {
  public:
    PTMHandler(const uint8_t id) : EnOcean::PTM215EventHandler(id) {};
    virtual ~PTMHandler() {};

    void handleEvent(const EnOcean::PTM215Event& event) override {
      log_i("Handling PTM Event by handler-node %d", getId());
      log_i("DeviceAddress: %s", event.device->address.toString().c_str());
      log_i("Event: ReferenceId: %d button %d type %d", event.referenceId, event.button, event.eventType);
    }
};

// Generic BLEScanner listening for advertisements.
BleScanner::Scanner scanner;
EnOcean::BLEScannerSubscriber scannerSubscriber;

PTMHandler* ptmHandler;

void setup() {
  Serial.begin(115200);
  log_i("Starting EnOcean BLE Example HandleEventsMultiplePTM215...");

  ptmHandler = new PTMHandler(PTMHANDLER_NODE_ID);

  scanner.setScanDuration(1);

  // On receiving an advertisement, scannerSubscriber::onResult() will be called, which will call the handleEvent methods of all registered devices
  scanner.subscribe(&scannerSubscriber);

  NimBLEDevice::init("ESP32_client");

  scanner.initialize();

  log_d("Adding devices");
  // register handler for A0 and B0 buttons using pointer to handler
  scannerSubscriber.registerPTM215Device(PTM_BLE_ADDRESS1, PTM_SECURITY_KEY2, ptmHandler, true, false, true, false, PTM_DEVICE_REFERENCE_ID1);
  // register handler for A1, B0 and B1 buttons, using nodeId of handler
  scannerSubscriber.registerPTM215Device(PTM_BLE_ADDRESS2, PTM_SECURITY_KEY2, PTMHANDLER_NODE_ID, false, true, true, true, PTM_DEVICE_REFERENCE_ID2);

  log_i("Initialization done");
  log_i("===========================================");

}

void loop() {
  scanner.update();
  delay(100);
}
