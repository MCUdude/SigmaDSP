#ifndef SIGMADSP_H
#define SIGMADSP_H

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include "parameters.h"
#include "DSPEEPROM.h"

/**
 * @brief ADAU1701 hardware register constants
 *
 */
typedef enum
{
  InterfaceRegister0       = 0x0800,
  InterfaceRegister1       = 0x0801,
  InterfaceRegister2       = 0x0802,
  InterfaceRegister3       = 0x0803,
  InterfaceRegister4       = 0x0804,
  InterfaceRegister5       = 0x0805,
  InterfaceRegister6       = 0x0806,
  InterfaceRegister7       = 0x0807,
  GpioAllRegister          = 0x0808,
  Adc0                     = 0x0809,
  Adc1                     = 0x080A,
  Adc2                     = 0x080B,
  Adc3                     = 0x080C,
  SafeloadData0            = 0x0810,
  SafeloadData1            = 0x0811,
  SafeloadData2            = 0x0812,
  SafeloadData3            = 0x0813,
  SafeloadData4            = 0x0814,
  SafeloadAddress0         = 0x0815,
  SafeloadAddress1         = 0x0816,
  SafeloadAddress2         = 0x0816,
  SafeloadAddress3         = 0x0818,
  SafeloadAddress4         = 0x0819,
  DataCapture0             = 0x081A,
  DataCpature1             = 0x081B,
  CoreRegister             = 0x081C,
  RAMRegister              = 0x081D,
  SerialOutRegister1       = 0x081E,
  SerialInputRegister      = 0x081F,
  MpCfg0                   = 0x0820,
  MpCfg1                   = 0x0821,
  AnalogPowerDownRegister  = 0x0822,
  AnalogInterfaceRegister0 = 0x0824
} dspRegister;

class SigmaDSP
{
  public:
    // Store passed device type and i2c address to private constants
    SigmaDSP(TwoWire &WireObject, uint8_t i2cAddress, float sample_freq, int8_t resetPin = -1);

    // Init and setup
    void begin();
    void i2cClock(uint32_t clock);
    void reset();
    uint8_t ping();

    // DSP functions
    void mux(uint16_t startMemoryAddress,         uint8_t index,   uint8_t numberOfIndexes = 0);
    void demux(uint16_t startMemoryAddress,       uint8_t index,   uint8_t numberOfIndexes);
    void gain(uint16_t startMemoryAddress,          float gainVal, uint8_t channels = 1);
    void gain(uint16_t startMemoryAddress,        int32_t gainVal, uint8_t channels = 1);
    void gain(uint16_t startMemoryAddress,         double gainVal, uint8_t channels = 1) { gain(startMemoryAddress,   (float)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,       uint32_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,       uint16_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,        int16_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,        uint8_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,         int8_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }

    void volume_slew(uint16_t startMemoryAddress,   float dB, uint8_t slew = 12);
    void volume(uint16_t startMemoryAddress,        float dB);
    void mute(uint16_t startMemoryAddress,          bool state);
    void dcSource(uint16_t startMemoryAddress,      uint32_t value);
    void dynamicBass(uint16_t startMemoryAddress,   float dB);
    void dynamicBass(uint16_t startMemoryAddress,   float dB, uint16_t frequency);
    void dynamicBass(uint16_t startMemoryAddress,   float dB, uint16_t frequency, int16_t threshold, uint16_t time_constant);
    void hardClip(uint16_t startMemoryAddress,      float highThreshold, float lowThreshold);
    void softClip(uint16_t startMemoryAddress,      float alpha);
    void dcSource(uint16_t startMemoryAddress,      float level);

    void sineSource(uint16_t startMemoryAddress,    float frequency);
    void sineSource(uint16_t startMemoryAddress,  int32_t frequency);
    void sineSource(uint16_t startMemoryAddress,   double frequency) { sineSource(startMemoryAddress,   (float)frequency); }
    void sineSource(uint16_t startMemoryAddress, uint32_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void sineSource(uint16_t startMemoryAddress, uint16_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void sineSource(uint16_t startMemoryAddress,  int16_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void sineSource(uint16_t startMemoryAddress,  uint8_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void sineSource(uint16_t startMemoryAddress,   int8_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }

    void squareSource(uint16_t startMemoryAddress,    float frequency);
    void squareSource(uint16_t startMemoryAddress,  int32_t frequency);
    void squareSource(uint16_t startMemoryAddress,   double frequency) { sineSource(startMemoryAddress,   (float)frequency); }
    void squareSource(uint16_t startMemoryAddress, uint32_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void squareSource(uint16_t startMemoryAddress, uint16_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void squareSource(uint16_t startMemoryAddress,  int16_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void squareSource(uint16_t startMemoryAddress,  uint8_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }
    void squareSource(uint16_t startMemoryAddress,   int8_t frequency) { sineSource(startMemoryAddress, (int32_t)frequency); }

    void sawtoothSource(uint16_t startMemoryAddress,    float frequency);
    void sawtoothSource(uint16_t startMemoryAddress,  int32_t frequency);
    void sawtoothSource(uint16_t startMemoryAddress,   double frequency) { sawtoothSource(startMemoryAddress,   (float)frequency); }
    void sawtoothSource(uint16_t startMemoryAddress, uint32_t frequency) { sawtoothSource(startMemoryAddress, (int32_t)frequency); }
    void sawtoothSource(uint16_t startMemoryAddress, uint16_t frequency) { sawtoothSource(startMemoryAddress, (int32_t)frequency); }
    void sawtoothSource(uint16_t startMemoryAddress,  int16_t frequency) { sawtoothSource(startMemoryAddress, (int32_t)frequency); }
    void sawtoothSource(uint16_t startMemoryAddress,  uint8_t frequency) { sawtoothSource(startMemoryAddress, (int32_t)frequency); }
    void sawtoothSource(uint16_t startMemoryAddress,   int8_t frequency) { sawtoothSource(startMemoryAddress, (int32_t)frequency); }

    void triangleSource(uint16_t startMemoryAddress,    float frequency);
    void triangleSource(uint16_t startMemoryAddress,  int32_t frequency);
    void triangleSource(uint16_t startMemoryAddress,   double frequency) { triangleSource(startMemoryAddress,   (float)frequency); }
    void triangleSource(uint16_t startMemoryAddress, uint32_t frequency) { triangleSource(startMemoryAddress, (int32_t)frequency); }
    void triangleSource(uint16_t startMemoryAddress, uint16_t frequency) { triangleSource(startMemoryAddress, (int32_t)frequency); }
    void triangleSource(uint16_t startMemoryAddress,  int16_t frequency) { triangleSource(startMemoryAddress, (int32_t)frequency); }
    void triangleSource(uint16_t startMemoryAddress,  uint8_t frequency) { triangleSource(startMemoryAddress, (int32_t)frequency); }
    void triangleSource(uint16_t startMemoryAddress,   int8_t frequency) { triangleSource(startMemoryAddress, (int32_t)frequency); }

    void audioDelay(uint16_t startMemoryAddress,     float delayMs);
    void EQfirstOrder(uint16_t startMemoryAddress,   firstOrderEQ_t &equalizer);
    void EQsecondOrder(uint16_t startMemoryAddress,  secondOrderEQ_t &equalizer);
    void toneControl(uint16_t startMemoryAddress,    toneCtrl_t &toneCtrl);
    void stateVariable(uint16_t startMemoryAddress,  float freq, float q);
    void compressorRMS(uint16_t startMemoryAddress,  compressor_t &compressor);
    void compressorPeak(uint16_t startMemoryAddress, compressor_t &compressor);

    // Hardware functions
    void muteADC(bool mute);
    void muteDAC(bool mute);

    // Data conversion
    void floatToFixed(float value, uint8_t *buffer);
    void intToFixed(int32_t value, uint8_t *buffer);
    int32_t floatToInt(float value);

    // DSP data write methods
    /**
     * @brief Template wrapper for safeload_writeRegister.
     *        Handles both int 28.0 and float 5.23 formatted parameters.
     *        You can add as many data parameters as you want
     *
     * @param address DSP memory address
     * @param data1 First value to write
     * @param dataN Nth value to write
     */
    template <typename Address, typename Data1, typename... DataN>
    void safeload_write(const Address &address, const Data1 &data1, const DataN &...dataN)
    {
      // Store passed address
      _dspRegAddr = address;
      safeload_write_wrapper(data1, dataN...);
    }

    void safeload_writeRegister(uint16_t memoryAddress,  uint8_t *data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,   int32_t data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,     float data, bool finished);
    #if defined(ARDUINO_ARCH_ESP8266) || (defined(ARDUINO_ARCH_ESP32) && ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0))
      void safeload_writeRegister(uint16_t memoryAddress, int16_t data, bool finished);
    #else
      void safeload_writeRegister(uint16_t memoryAddress,     int data, bool finished);
    #endif
    void safeload_writeRegister(uint16_t memoryAddress,  uint32_t data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,  uint16_t data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,   uint8_t data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,    double data, bool finished);
    void writeRegister(uint16_t memoryAddress, uint8_t length, uint8_t *data);
    void writeRegister(uint16_t memoryAddress, uint8_t length, const uint8_t *data);
    void writeRegisterBlock(uint16_t memoryAddress, uint16_t length, const uint8_t *data, uint8_t registerSize);
    int32_t readBack(uint16_t address, uint16_t value, uint8_t length);
    uint32_t readRegister(dspRegister hwReg, uint8_t numberOfBytes);

    // Public constants
    const float FS; // Sample rate in [Hz]


  private:
    // Wrapper template functions for safeload template
    template <typename Data1, typename... DataN>
    void safeload_write_wrapper(const Data1 &data1, const DataN &...dataN)
    {
      safeload_writeRegister(_dspRegAddr, data1, false);
       _dspRegAddr++;
      safeload_write_wrapper(dataN...);  // Recursive call using pack expansion syntax
    }
    // Handles last argument
    template <typename Data1>
    void safeload_write_wrapper(const Data1& data1)
    {
      safeload_writeRegister(_dspRegAddr, data1, true);
    }

    // Math
    void linspace(float x1, float x2, float n, float *vect);

    // Objects
    TwoWire &_WireObject;      // Reference to passed Wire object

    // Private constants
    const uint8_t _dspAddress; // Passed device i2c address
    const int8_t  _resetPin;   // Digital pin to reset the DSP

    // Private variables
    uint16_t _dspRegAddr;      // Used by template safeload functions
};

#endif //SIGMADSP_H
