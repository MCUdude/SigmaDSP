#ifndef DSPEEPROM_H
#define DSPEEPROM_H

#include <Arduino.h>
#include <Wire.h>
#include "SigmaDSP.h"

class DSPEEPROM
{
  public:
    DSPEEPROM(TwoWire &WireObject, uint8_t i2cAddress, uint16_t kbitSize, int8_t ledPin = -1);
    void begin();
    uint8_t ping();
    uint8_t getFirmwareVersion();
    uint8_t writeFirmware(const uint8_t *firmware, uint16_t size, int8_t firmwareVersion = -1);

  private:
    TwoWire &_WireObject;             // Reference to passed Wire object

    const uint8_t _eepromAddress;     // i2c address for our EEPROM
    const uint16_t _kbitSize;         // Size of our EEPROM in kilobits
    const int8_t _ledPin;             // Pin to toggle while writing to EEPROM

    uint16_t _firmwareVersionAddress; // Holds the current DSP firmware version
    uint8_t ledCounter = 0x00;        // Keeps track of the LED toggle speed
};

#endif
