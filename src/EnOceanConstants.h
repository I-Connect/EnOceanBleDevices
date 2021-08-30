#pragma once
#include "Arduino.h"

/** Manufacturer data */
const uint8_t ENOCEAN_PAYLOAD_TYPE = 0xFF;
const uint8_t ENOCEAN_PAYLOAD_MANUFACTURER[2] = { 0xDA, 0x03 };

const uint8_t PTM_PREFIX_ADDRESS[2] =  { 0xE2, 0x15 };
const uint8_t STM550B_EMDCB_PREFIX_ADDRESS[2] =  { 0xE5, 0x00 };
