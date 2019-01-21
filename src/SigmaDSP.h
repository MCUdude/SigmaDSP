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
    void begin(uint8_t sdaPin = SDA, uint8_t sclPin = SCL);
    void i2cClock(uint32_t clock);
    void reset();
    uint8_t ping();
    
    // DSP functions
    void mux(uint16_t memoryAddress,               uint8_t index, uint8_t numberOfIndexes = 0);
    void demux(uint16_t memoryAddress,             uint8_t index, uint8_t numberOfIndexes);
    void gain(uint16_t memoryAddress,              float gain, uint8_t channels = 1);
    void volume_slew(uint16_t memoryAddress,       float dB, uint8_t slew = 12);
    void hardClip(uint16_t startMemoryAddress,     float highThreshold, float lowThreshold);
    void softClip(uint16_t startMemoryAddress,     float alpha);
    void dcSource(uint16_t memoryAddress,          float value);
    void sineSource(uint16_t memoryAddress,        float frequency);
    void squareSource(uint16_t memoryAddress,      float frequency);
    void sawtoothSource(uint16_t memoryAddress,    float frequency);
    void triangleSource(uint16_t memoryAddress,    float frequency);
    void audioDelay(uint16_t memoryAddress,        float delayMs);
    void EQfirstOrder(uint16_t memoryAddress,      firstOrderEQ_t &equalizer);
    void EQsecondOrder(uint16_t memoryAddress,     secondOrderEQ_t &equalizer);
    void toneControl(uint16_t memoryAddress,       toneCtrl_t &toneCtrl);
    void stateVariable(uint16_t memoryAddress,     float freq, float q);
    void compressorRMS(uint16_t memoryAddress,     compressor_t &compressor);
    void compressorPeak(uint16_t memoryAddress,    compressor_t &compressor);
    
    // Data conversion
    void floatToFixed(float value, uint8_t *buffer);
    void intToFixed(int32_t value, uint8_t *buffer);
    int32_t floatToInt(float value);
    
    // DSP data write methods
    void safeload_writeRegister(uint16_t memoryAddress, uint8_t *data, bool finished);
    void writeRegister(uint16_t memoryAddress, uint8_t length, uint8_t *data);
    void writeRegister(uint16_t memoryAddress, uint8_t length, const uint8_t *data);
    void writeRegisterBlock(uint16_t memoryAddress, uint16_t length, const uint8_t *data, uint8_t registerSize);
    
  private:
    // Math
    void linspace(float x1, float x2, float n, float *vect);
    
    // Private constants
    const uint8_t _dspAddress;   // Passed device i2c address
    const uint8_t _deviceType;   // Passed device type
    const int8_t  _resetPin;     // Digital pin to reset the DSP
    
    // Private variables
    uint8_t _safeload_count = 0; // Counter for safeload registers
};


#endif //SIGMADSP_H