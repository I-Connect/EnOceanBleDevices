#include <map>
#include "EnOceanDataTypes.h"

namespace EnOcean {

ParameterType parameterTypeFromString(const std::string str) {
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