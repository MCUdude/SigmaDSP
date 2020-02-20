#include "DSPEEPROM.h"


/***************************************
Function: DSPEEPROM()
Purpose:  The constructor creates the object
Inputs:   uint8_t i2cAddress; 7-bit i2c address
          uint16_t kbitSize;  Size of EEPROM in kilobit
          int8_t ledPin;      Pin to toggle when flashing EEPROM
Returns:  None
***************************************/
DSPEEPROM::DSPEEPROM(uint8_t i2cAddress, uint16_t kbitSize, int8_t ledPin)
  : _eepromAddress(i2cAddress), _kbitSize(kbitSize), _ledPin(ledPin)
{  
  // The DSP program itself will never exceed 9248 bytes in total
  // The last byte in EEPROM will be used to keep the firmware version
  // An EEPROM with size larger than 9.3kB is recommended
  switch (_kbitSize)
  {
    case 64:
    default:
      _firmwareVersionAddress = 0x1FFF; // 8191 dec
      break;
    case 128:
      _firmwareVersionAddress = 0x3FFF; // 16383 dec
      break;
    case 256:
      _firmwareVersionAddress = 0x7FFF; // 32767 dec
      break;
    case 512:
      _firmwareVersionAddress = 0xFFFF; // 65535 dec
      break;
  }
}


/***************************************
Function: begin()
Purpose:  Starts the i2c interface
Inputs:   TwoWire Wire;    Wire object (optional parameter)
Returns:  None
***************************************/
void DSPEEPROM::begin(TwoWire *WireObject)
{
  // Store copy the passed object
  _WireObject = WireObject;

  _WireObject->begin();

  // If LED is present
  if(_ledPin >= 0)
  {
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
  }
}


/***************************************
Function: begin()
Purpose:  Starts the i2c interface
Inputs:   TwoWire Wire;    Wire object
          uint8_t sdaPin;  SDA pin
          uint8_t sclPin;  SCL pin
Returns:  None
***************************************/
void DSPEEPROM::begin(TwoWire *WireObject, uint8_t sdaPin, uint8_t sclPin)
{
  // Store copy the passed object
  _WireObject = WireObject;

  // This hardware supports redefining SDA and SCL in begin()
  #if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_STM32)
    _WireObject->begin(sdaPin, sclPin);
  #else // This does not
    (void)sdaPin;
    (void)sclPin;
    _WireObject->begin();
  #endif

  // If LED is present
  if(_ledPin >= 0)
  {
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
  }
}


/***************************************
Function: ping()
Purpose:  Sends a i2c ping message
Inputs:   none
Returns:  0 - success: ack received
          2 - error: address send, nack received
          3 - error: data send, nack received
          4 - error: unknown i2c error
***************************************/
uint8_t DSPEEPROM::ping()
{
  _WireObject->beginTransmission(_eepromAddress);
  return _WireObject->endTransmission();
}


/***************************************
Function: getFirmwareVersion()
Purpose:  Returns the version number of the DSP firmware
Inputs:   None
Returns:  Firmware version. 0 indicated that no EEPROM is found,
          255 indicates that the EEPROM is empty
***************************************/
uint8_t DSPEEPROM::getFirmwareVersion()
{  
  _WireObject->beginTransmission(_eepromAddress);
  _WireObject->write((uint16_t)_firmwareVersionAddress >> 8); // MSB
  _WireObject->write(_firmwareVersionAddress & 0xFF);         // LSB
  _WireObject->endTransmission();
  
  _WireObject->requestFrom((uint8_t)_eepromAddress, (uint8_t)1);
  
  if(_WireObject->available())
    return _WireObject->read();
  else
    return 0;   
}


/***************************************
Function: writeFirmware()
Purpose:  Writes the passed firmware array to the EEPROM and verifies
          that the content was written.
Inputs:   uint8_t *firmware;       Array to hold the firmware to flash
          uint16_t size;           Firmware array length (use sizeof)
          int8_t firmwareVersion;  Version of the DSP firmware to store
Returns:  True if writing succeeded and content was verified to be correct, 
          false otherwise
***************************************/
uint8_t DSPEEPROM::writeFirmware(const uint8_t *firmware, uint16_t size, int8_t firmwareVersion)
{
  // Check if EEPROM already contains the current firmware version
  uint8_t EEPROM_firmwareVersion = getFirmwareVersion();

  // Write new firmware is version doesn't match or if no version is provided
  if(firmwareVersion != EEPROM_firmwareVersion || firmwareVersion < 0)
  {
    // Write new firmware
    for(uint16_t i = 0; i < size; i++)
    {
      _WireObject->beginTransmission(_eepromAddress);
      _WireObject->write(i >> 8);                      // High memory address
      _WireObject->write(i & 0xFF);                    // Low memory address
      _WireObject->write(pgm_read_byte(&firmware[i])); // Content
      _WireObject->endTransmission();                  // End
      // EEPROM is a slooow kind of memory..
      delay(5);

      // Toggle LED every 32th byte if LED is present
      if(_ledPin >= 0 && ledCounter & 0x20)
        digitalWrite(_ledPin, !digitalRead(_ledPin));

      // Increase counter
      ledCounter++;
    }

    // Make sure all tracks of old FW is gone, by overwriting a few more bytes
    for(uint16_t i = size; i < 0x2000; i++)
    {
      _WireObject->beginTransmission(_eepromAddress);
      _WireObject->write(i >> 8);     // High memory address
      _WireObject->write(i & 0xFF);   // Low memory address
      _WireObject->write(0xFF);       // Content
      _WireObject->endTransmission(); // End
      delay(5);

      // Toggle LED every 16th byte if LED is present when filling the rest
      if(_ledPin >= 0 && ledCounter & 0x10)
        digitalWrite(_ledPin, !digitalRead(_ledPin));

      // Increase counter
      ledCounter++;
    }

    // Write firmware version to the last byte in EEPROM
    _WireObject->beginTransmission(_eepromAddress);
    _WireObject->write((uint16_t)_firmwareVersionAddress >> 8); // High memory address
    _WireObject->write(_firmwareVersionAddress & 0xFF);         // Low memory address
    _WireObject->write(firmwareVersion);                        // Content
    _WireObject->endTransmission();                             // End
    delay(5);

   // Turn off LED after flashing is finished if LED is present
   if(_ledPin >= 0)
     digitalWrite(_ledPin, LOW);
  }

  // Does the last byte in memory contain the correct FW version?
  EEPROM_firmwareVersion = getFirmwareVersion();
  if(EEPROM_firmwareVersion == firmwareVersion)
    return 1; // Yes it does
  else
    return 0; // No it doesn't
}
