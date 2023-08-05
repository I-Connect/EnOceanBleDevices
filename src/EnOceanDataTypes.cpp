#include <map>
#include "EnOceanDataTypes.h"

namespace EnOcean {

std::string_view deviceTypeToString(const DeviceType type) {
  switch (type) {
    case EnOcean::DeviceType::PTM215B: {
      return "ptm215b";
    }
    case EnOcean::DeviceType::EMDCB: {
      return "emdcb";
    }

    case EnOcean::DeviceType::STM550B: {
      return "stm550b";
    }
  }
  return "";
}

ParameterType parameterTypeFromString(std::string_view str) {
  if (str == "Temperature") {
    return ParameterType::Temperature ;
  }
  if (str == "BatteryVoltage") {
    return ParameterType::BatteryVoltage ;
  }
  if (str == "EnergyLevel") {
    return ParameterType::EnergyLevel ;
  }
  if (str == "LightLevelSolar") {
    return ParameterType::LightLevelSolar ;
  }
  if (str == "LightLevel") {
    return ParameterType::LightLevel ;
  }
  if (str == "Humidity") {
    return ParameterType::Humidity ;
  }
  if (str == "Acceleration") {
    return ParameterType::Acceleration;
  }
  if (str == "Occupancy") {
    return ParameterType::Occupancy ;
  }

  // if (str == "MagnetContact") { also use as default
  return ParameterType::MagnetContact ;

}

} // namespace EnOcean