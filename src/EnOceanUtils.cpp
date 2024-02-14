#include "EnOceanUtils.h"

namespace EnOcean {

void hexStringToByteArray(std::string stringInput, byte* output, uint8_t byteLength) {
  for (uint8_t i = 0; i < byteLength; i++) {
    output[i] = strtol(stringInput.substr(i * 2, 2).c_str(), NULL, 16);
  }
}

std::string byteArrayToHexString(const byte* bytes, const uint8_t bytelength) {
  std::string result;

  for (auto i = 0; i < bytelength; i++) {
    char b[3] {0};
    sprintf(b, "%02x", bytes[i]);
    result += b;
  }
  return result;
}

void printBuffer(const byte* buff, const size_t size, const boolean asChars, const char* header) {
  char b[strlen(header) + 2 + size * 3 + 1] { 0 };
  Serial.println(printBuffer(buff, size, asChars, header, b));
  delay(5);
}

char* printBuffer(const byte* buff, const size_t size, const boolean asChars, const char* header, char* output) {
  if (size == 0) {
    output[0] = 0x00;
    return output;
  }

  char temp[strlen(header) + 2 + size * 3 + 1] { 0 };
  if (strlen(header) > 0) {
    strcat(temp, header);
    strcat(temp, ": ");
  }
  for (int i = 0; i < size; i++) {
    if (asChars) {
      strncat(temp, (const char*)&buff[i], 1);
    } else {
      char hex[4];
      sprintf(hex, "%02X ", buff[i]);
      strcat(temp, hex);
    }
  }
  strcpy(output, temp);
  return output;
}

void parsePayloadParameters(const Payload& payload, std::vector<Parameter>& result) {
  byte* payloadPtr = const_cast<byte*>(payload.data.raw);
  uint8_t payloadSize = payload.len - 11;

  while (payloadPtr < payload.data.raw + payloadSize) {
    Parameter parameter;
    parameter.size = pow(2, *payloadPtr >> 6);                      // leftmost 2 bits
    parameter.type = (ParameterType)(*payloadPtr & 0b00111111);     // rightmost 6 bits
    payloadPtr++;
    if (parameter.size > 4) { // custom size, specified in first byte of data
      // TODO read custom size parameter, skipped for now
      parameter.size = *payloadPtr++;
    } else {
      memcpy(&parameter.value, payloadPtr, parameter.size);
    }
    payloadPtr += parameter.size;
    result.push_back(parameter);
  }
}

} // namespace EnOcean
