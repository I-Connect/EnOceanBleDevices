#pragma once
#include "Arduino.h"
#include "NimBLEAddress.h"
#include <map>

namespace EnOcean {

#ifdef NODE_ID_TYPE
  typedef NODE_ID_TYPE nodeId_t;
#else
  typedef uint8_t nodeId_t;
#endif

enum class PayloadType {
  Data,
  Commissioning
};

enum class DeviceType {
  UNKNOWN = 0,
  PTM215B,  // Switches
  PTM535BZ, // ??
  EMDCB,    // Motion detector
  STM550B   // Multi sensor
};

std::string_view deviceTypeToString(const DeviceType);

typedef byte SecurityKey[16];

struct Payload {
  byte len;
  byte type;
  char manufacturerId[2];
  uint32_t sequenceCounter;
  DeviceType deviceType;
  PayloadType payloadType;
  union {
    struct { // Data
      byte raw[19];
      byte signature[4];
    } data;
    struct { // Commissioning
      SecurityKey securityKey;
      byte staticSourceAddress[6]; // LSB first
    } commissioning;
  };
};

struct Device {
  NimBLEAddress address;
  SecurityKey securityKey {0};
  uint32_t lastSequenceCounter = 0;
  DeviceType type;
};

enum class ParameterType : byte {
  Temperature     = 0x00,
  BatteryVoltage  = 0x01,
  EnergyLevel     = 0x02,
  LightLevelSolar = 0x04,
  LightLevel      = 0x05,
  Humidity        = 0x06,
  Acceleration    = 0x0A,
  Occupancy       = 0x20,
  MagnetContact   = 0x23
};

ParameterType parameterTypeFromString(std::string_view);

const std::map<ParameterType, uint8_t> resolution {
  {ParameterType::Temperature, 100},
  {ParameterType::BatteryVoltage, 2},
  {ParameterType::EnergyLevel, 2},
  {ParameterType::LightLevelSolar, 1},
  {ParameterType::LightLevel, 1},
  {ParameterType::Humidity, 2},
  {ParameterType::Acceleration, 1},
  {ParameterType::Occupancy, 1},
  {ParameterType::MagnetContact, 1},
};

// TODO Handle custom length parameters
struct Parameter {
  ParameterType type;
  uint8_t size;
  union {
    uint32_t uint32;
    uint16_t uint16;
    uint8_t uint8;
  } value{0x00};

  int getRawValueAsInt() const {
    if (size == 1) {
      return value.uint8;
    } else if (size == 2) {
      return value.uint16;
    } else {
      return value.uint32;
    }
  }

  int getValueAsInt() const {
    return round((float)getRawValueAsInt() / resolution.at(type));
  }

};

enum class AccelerometerStatus {
  OutOfBounds    = 0,
  PeriodicUpdate = 1,
  Wake           = 2,
  Disabled       = 3
};

struct AccelerometerValues {
  AccelerometerStatus status;
  int16_t xVector;
  int16_t yVector;
  int16_t zVector;
};

struct CommissioningEvent {
  NimBLEAddress address = NimBLEAddress("");
  DeviceType type;
  SecurityKey securityKey;
};

/**
 * @brief Handler for commissionEvents from a BLE Switch
 *
 * The event is sent when the switch is put into commissioning mode and when the same
 * button is pressed or released when in commissioning mode.
 *
 * Note that the handler must be able to handle receiving the same commission event multiple times!
 */
class CommissioningEventhandler {
  public:
    CommissioningEventhandler() {};
    virtual ~CommissioningEventhandler() {};
    virtual void handleEvent(CommissioningEvent& evt) = 0;
};

} // namespace EnOcean