#pragma once
#include "Arduino.h"
#include "vector"
#include "EnOceanDataTypes.h"

namespace EnOcean {

void hexStringToByteArray(std::string_view stringInput, byte* output, uint8_t byteLength);
std::string byteArrayToHexString(const byte* bytes, const uint8_t bytelength);

void printBuffer(const byte* buff, const size_t size, const boolean asChars, const char* header);
char* printBuffer(const byte* buff, const size_t size, const boolean asChars, const char* header, char* output);

} // namespace EnOcean
