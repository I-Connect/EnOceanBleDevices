# EnOcean

This library provides an implementation for EnOcean selfpowered BLE devices on a ESP32 based MCU

https://www.enocean.com/en/products

## Hardware
Hardware supported:
* Tested on an ESP32wroom
* EnOcean PTM215B selfpowered BLE switch
* EnOcean EMDCB IR sensor
* EnOcean STM550B Multi sensor devices


## Development
This library is work in progress and currently includes:
* Register an EnOcean device BLE address (**"MAC Address"**), specifying a 32 character **Security Key**
* Security checks on payload length, source address, GAP AD Type and security key encryption using AES128
* Dispatch events received through BLE to external event handler
* Task based detection of `Pushed`, `Repeat`, `ReleaseShort` and `ReleaseLong` events for PTM215 devices
* Dispatch `Repeat` events to external event handler every 500ms, if a PTM215 button is pushed and held down

All classes and types are defined within the namespace `EnOcean`.

Current implementation for PTM215b switches is based on [](documentation%5CPTM_215B_User_Manual_v2.1.pdf) included in the `documentation` folder

## PTM215 Events
`Pushed` event is generated when a PTM215 button is pushed.
If the button is held down, `Repeat` events will be generated every 500 ms after an initial delay.
When the button is released without generating `Repeat` events, then a `ReleaseShort` event is generated,
otherwise a `ReleaseLong` event is generated on release.

## Usage for PTM215 switches

* Add dependency to `i-connect/EnOceanBleDevices` to your project
* Add `#include "BleScanner.h"'
* Add `#include "EnOceanBLEScannerSubscriber.h"'
* Create an instance of the `BLEScanner::Scanner` and the `EnOcean::BLEScannerSubscriber` class
* Define a class implementing the `EnOcean::PTM215EventHandler` interface and create an instance of that class
* Initialize BLEDevice
* Register `BLEScannerSubscriber` with the `BLEScanner`
* Register with the BLEScannerSubsacriber the switches with `BLEAddress`, 32 character `SecurityKey`, **NodeIds** and a pointer to the eventhandler 
* Call `bleScanner.initialize()`
* Repeatedly call `bleScanner.update()` in the `loop()` method

On every valid switch event from a registered BLEAddress, a call will be made to the `handleEvent()` method of the eventHandler registered with the switch with a `PTM215b::SwitchEvent` as parameter.


## EMDCB and STM550 Events
As both the EMDCB and the STM550 devices work in the exactly the same way, they use the same implementation called 'DataEvent...'
Events received from these devices are passed through to the EventHandler registered with the device. The event contains the payload of the received advertisement, containing the device and a `std::vector` with `EnOcean::Parameter` which contains the parameter type and value received in the advertisement

## Usage for EMDCB / STM550 devices

* Add dependency to `i-connect/EnOceanBleDevices` to your project
* Add `#include "BleScanner.h"'
* Add `#include "EnOceanBLEScannerSubscriber.h"'
* Create an instance of the `BLEScanner::Scanner` and the `EnOcean::BLEScannerSubscriber` class
* Define an evenhandler class implementing the `EnOcean::DataEventHandler` interface and create an instance of that class
* Initialize BLEDevice
* Register `BLEScannerSubscriber` with the `BLEScanner`
* Register with the BLEScannerSubsacriber the devices with `BLEAddress`, 32 character `SecurityKey`, **NodeIds** and a pointer to your eventhandler 
* Call `bleScanner.initialize()`
* Repeatedly call `bleScanner.update()` in the `loop()` method


## Debugging
If `DEBUG_ENOCEAN` is defined in main or platformio, log messages will be output to Serial for debugging



