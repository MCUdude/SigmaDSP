#ifndef SIGMADSP_H
#define SIGMADSP_H

#include <Arduino.h>
#include <Wire.h>
#include "parameters.h"
#include "DSPEEPROM.h"

// Only ADAU1701 is supported at the moment
#define ADAU1701 0
//#define ADAU1442 1
//#define ADAU1445 1
//#define ADAU1446 1
//#define ADAU144X 1


class SigmaDSP
{
  public:
    // Store passed device type and i2c address to private constants
    SigmaDSP(uint8_t i2cAddress, uint8_t device, int8_t resetPin = -1);

    // Init and setup
    void begin(TwoWire &WireObject = Wire);
    void begin(TwoWire &WireObject, uint8_t sdaPin, uint8_t sclPin);
    void i2cClock(uint32_t clock);
    void reset();
    uint8_t ping();

    // DSP functions
    void mux(uint16_t startMemoryAddress,        uint8_t index,    uint8_t numberOfIndexes = 0);
    void demux(uint16_t startMemoryAddress,      uint8_t index,    uint8_t numberOfIndexes);
    void gain(uint16_t startMemoryAddress,          float gainVal, uint8_t channels = 1);
    void gain(uint16_t startMemoryAddress,        int32_t gainVal, uint8_t channels = 1);
    void gain(uint16_t startMemoryAddress,         double gainVal, uint8_t channels = 1) { gain(startMemoryAddress,   (float)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,       uint32_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,       uint16_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,        int16_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,        uint8_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }
    void gain(uint16_t startMemoryAddress,         int8_t gainVal, uint8_t channels = 1) { gain(startMemoryAddress, (int32_t)gainVal, channels); }

    void volume_slew(uint16_t startMemoryAddress,  float dB,       uint8_t slew = 12);
    void dynamicBass(uint16_t startMemoryAddress, float dB);
    void hardClip(uint16_t startMemoryAddress,     float highThreshold, float lowThreshold);
    void softClip(uint16_t startMemoryAddress,     float alpha);
    void dcSource(uint16_t startMemoryAddress,     float level);

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

    // Data conversion
    void floatToFixed(float value, uint8_t *buffer);
    void intToFixed(int32_t value, uint8_t *buffer);
    int32_t floatToInt(float value);

    // DSP data write methods

    // Template wrapper for safeload_writeRegister
    // Handles both int 28.0 and float 5.23 formatted parameters
    // Usage: safeload_write(uint16_t startMemoryAddress, data1, data2, ..., dataN);
    template <typename Address, typename Data1, typename... DataN>
    void safeload_write(const Address &address, const Data1 &data1, const DataN &...dataN);

    void safeload_writeRegister(uint16_t memoryAddress, uint8_t *data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,  int32_t data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,    float data, bool finished);
    void safeload_writeRegister(uint16_t memoryAddress,  int16_t data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
    void safeload_writeRegister(uint16_t memoryAddress,  uint8_t data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
    void safeload_writeRegister(uint16_t memoryAddress,   double data, bool finished) { safeload_writeRegister(memoryAddress,   (float)data, finished); }
    void writeRegister(uint16_t memoryAddress, uint8_t length, uint8_t *data);
    void writeRegister(uint16_t memoryAddress, uint8_t length, const uint8_t *data);
    void writeRegisterBlock(uint16_t memoryAddress, uint16_t length, const uint8_t *data, uint8_t registerSize);


  private:
    // Wrapper template functions for safeload template
    template <typename Data1, typename... DataN>
		void safeload_write_wrapper(const Data1 &data1, const DataN &...dataN)
		{
			safeload_writeValue(_dspRegAddr, data1, false);
			safeload_write_wrapper(dataN...);  // Recursive call using pack expansion syntax
		}
		// Handles next argument
		template <typename Data1>
		void safeload_write_wrapper(const Data1& data1)
		{
			safeload_writeValue(_dspRegAddr, data1, true);
		}
    // Wrapper template for safeload_writeRegister functions
    template <typename Address, typename Data, typename Finished>
    void safeload_writeValue(Address& regAddr, const Data &data, const Finished &finished)
    {
      safeload_writeRegister(regAddr, data, finished);
      regAddr++;
    }

    // Math
    void linspace(float x1, float x2, float n, float *vect);

    // Objects
    TwoWire _WireObject; // Reference to passed Wire object
    
    // Private constants
    const uint8_t _dspAddress;   // Passed device i2c address
    const uint8_t _deviceType;   // Passed device type
    const int8_t  _resetPin;     // Digital pin to reset the DSP
    
    // Private variables
    uint16_t _dspRegAddr; // Used by template safeload functions
};


#endif //SIGMADSP_H
