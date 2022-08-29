#include "SigmaDSP.h"
#include "Arduino.h"

/*******************************************************************************************
**                                      PUBLIC METHODS                                    **
*******************************************************************************************/

/**
 * @brief Construct a new SigmaDSP::SigmaDSP object
 *
 * @param WireObject TwoWire i2c object
 * @param i2cAddress 7-bit i2c address
 * @param sample_freq The audio sample frequency in [Hz]
 * @param resetPin pin to reset the DSP with (optional parameter)
 */
SigmaDSP::SigmaDSP(TwoWire &WireObject, uint8_t i2cAddress, float sample_freq, int8_t resetPin)
  : FS(sample_freq), _WireObject(WireObject), _dspAddress(i2cAddress), _resetPin(resetPin)
{

}


/**
 * @brief Starts the i2c interface
 */
void SigmaDSP::begin()
{
  // Reset DSP if pin is present
  if(_resetPin >= 0)
  {
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, HIGH);
    reset();
  }
}


/**
 * @brief Sets the i2c clock speed
 *
 * @param clock i2c clock speed in [Hz]
 */
void SigmaDSP::i2cClock(uint32_t clock)
{
  _WireObject.setClock(clock);
}


/**
 * @brief Perform a DSP hardware reset
 */
void SigmaDSP::reset()
{
  if(_resetPin >= 0) // Only run of reset pin is present
  {
    digitalWrite(_resetPin, LOW);
    delay(200);
    digitalWrite(_resetPin, HIGH);
  }
}


/**
 * @brief Sends an i2c ping message
 *
 * @return ping receive status;
 *         0 - success: ack received;
 *         2 - error: address send, nack received;
 *         3 - error: data send, nack received;
 *         4 - error: unknown i2c error
 */
uint8_t SigmaDSP::ping()
{
  _WireObject.beginTransmission(_dspAddress);
  return _WireObject.endTransmission();
}


/**
 * @brief Controls a mux. The mux can only be a single address block, which is the clickless SW slew mux
 *
 * @param startMemoryAddress DSP memory address
 * @param index Index (the signal) you want to switch to (0, 1, 2, ..., N)
 * @param numberOfIndexes The total number of indexes (2 or more)
 */
void SigmaDSP::mux(uint16_t startMemoryAddress, uint8_t index, uint8_t numberOfIndexes)
{
  // Index number is actually not needed, but kept for compatibility with demux
  (void)numberOfIndexes;

  safeload_write(startMemoryAddress, index);
}


/**
 * @brief Controls a demux
 *
 * @param startMemoryAddress DSP memory address
 * @param index Index (the signal) you want to switch to (0, 1, 2, ..., N)
 * @param numberOfIndexes The total number of indexes (2 or more)
 */
void SigmaDSP::demux(uint16_t startMemoryAddress, uint8_t index, uint8_t numberOfIndexes)
{
  uint8_t i = 0;

  // Load leading zeros
  for(; i < index; i++)
    safeload_writeRegister(startMemoryAddress++, 0x00, false);

  // Load index
  i++;
  safeload_writeRegister(startMemoryAddress++, 0x01, (i == numberOfIndexes ? true : false));

  // Load tailing zeros
  for(; i < numberOfIndexes; i++)
    safeload_writeRegister(startMemoryAddress++, 0x00, (i == numberOfIndexes-1 ? true : false));
}


/**
 * @brief Adjusts a gain block
 *
 * @param startMemoryAddress DSP memory address
 * @param gain Gain value (1 = 0dB gain, 2 = +6dB gain)
 * @param channels Number of channels this gain cell has (defaults to 1 if not defined)
 */
void SigmaDSP::gain(uint16_t startMemoryAddress, float gain, uint8_t channels)
{
  for(uint8_t i = 0; i < channels - 1; i++)
    safeload_writeRegister(startMemoryAddress++, gain, false);

  safeload_writeRegister(startMemoryAddress, gain, true);
}

/**
 * @brief Adjusts a gain block
 *
 * @param startMemoryAddress DSP memory address
 * @param gain Gain value (1 = 0dB gain, 2 = +6dB gain)
 * @param channels Number of channels this gain cell has (defaults to 1 if not defined)
 */
void SigmaDSP::gain(uint16_t startMemoryAddress, int32_t gain, uint8_t channels)
{

  int32_t value = (gain * ((int32_t)1 << 23));

  for(uint8_t i = 0; i < channels - 1; i++)
    safeload_writeRegister(startMemoryAddress++, value, false);

  safeload_writeRegister(startMemoryAddress, value, true);
}


/**
 * @brief Controls a volume control block with slew
 *
 * @param startMemoryAddress DSP memory address
 * @param dB Volume to set in dB
 * @param slew slew rate (optional parameter, default 12)
 */
void SigmaDSP::volume_slew(uint16_t startMemoryAddress, float dB, uint8_t slew)
{
  float volume = pow(10, dB / 20); // 10^(dB / 20)
  int32_t slewrate = 0x400000 / (1 << (slew - 1)); // 0x400000/2^(slew - 1))

  safeload_write(startMemoryAddress, volume, slewrate);
}

/**
 * @brief Controls a volume control block without slew
 * Can be used with various gain controls
 *
 * @param startMemoryAddress DSP memory address
 * @param dB Volume to set in dB
 */
void SigmaDSP::volume(uint16_t startMemoryAddress, float dB)
{
  float volume = pow(10, dB / 20); // 10^(dB / 20)

  safeload_write(startMemoryAddress, volume);
}

/**
 * @brief Control mute/unmute block without slew
 *
 * @param startMemoryAddress DSP memory address
 * @param state true or false
 */
void SigmaDSP::mute(uint16_t startMemoryAddress, bool state)
{
  safeload_write(startMemoryAddress, state ? 0x800000 : 0);
}

/**
 * @brief Control index filters (bass, treble, ect)
 * in 28 0 format.   Please see:
 * https://ez.analog.com/dsp/sigmadsp/f/q-a/165704/tone-control-using-adau1701-sigma-studio
 *
 * @param startMemoryAddress DSP memory address
 * @param Integer value in control field min -(number of steps/2) to +(number of steps/2)
 */
 void SigmaDSP::dcSource(uint16_t startMemoryAddress, uint32_t value)
 {
   safeload_write(startMemoryAddress, value);
 }

/**
 * @brief Controls the dynamic bass boost level block in dB. This function lets you only set
 * set the bass boost. The rest of the settings has to be preset in the SigmaStudio
 * project file.
 *
 * @param startMemoryAddress DSP memory address
 * @param dB Ranges from 0 to +20dB. Represents the boost level
 */
void SigmaDSP::dynamicBass(uint16_t startMemoryAddress, float dB)
{
  float boost = pow(10, -dB / 20); // 10^(-dB / 20)

  // Set boost (8th memory address in this algorithm)
  safeload_write(startMemoryAddress + 8, boost);
}


/**
 * @brief Controls the dynamic bass boost level block where boost and frequency can be adjusted
 *
 * @param startMemoryAddress DSP memory address
 * @param dB Ranges from 0 to +20dB. Represents the boost level
 * @param frequency Ranges from 20 to 300Hz and designates the center frequency for the boosting filter
 */
void SigmaDSP::dynamicBass(uint16_t startMemoryAddress, float dB, uint16_t frequency)
{
  // Formula found here: https://ez.analog.com/dsp/sigmadsp/f/q-a/65338/dynamic-bass-boost-basics
  float omega       = 2 * pi * ((frequency * 3) / FS);
  float alpha       = sin(omega)/(2 * 0.707);
  float a0          = 1 + alpha;
  float gain_lin    = 1.00;

  float freq_varq   = 2 * pi * (frequency / FS);
  float iir_coeff_0 = (1 - cos(omega)) * gain_lin / (2 * a0);
  float iir_coeff_1 = (1 - cos(omega)) * gain_lin / a0;
  float iir_coeff_2 = (1 - cos(omega)) * gain_lin / (2 * a0);
  float iir_coeff_3 = (2 / a0) * cos(omega);
  float iir_coeff_4 = (alpha - 1) / a0;
  // Boost are independent from the frequency and iir coefficients
  float boost       = pow(10, -dB / 20);

  // Set frequency
  safeload_write(startMemoryAddress, freq_varq, iir_coeff_0, iir_coeff_1, iir_coeff_2, iir_coeff_3, iir_coeff_4);
  // Set boost (8th memory address in this algorithm)
  safeload_write(startMemoryAddress + 8, boost);
}


/**
 * @brief Controls the dynamic bass boost level block in where all parameters are adjustable
 *
 * @param startMemoryAddress DSP memory address
 * @param dB Ranges from 0 to +20dB. Represents the boost level
 * @param frequency Ranges from 20 to 300Hz and designates the center frequency for the boosting filter
 * @param threshold Ranges from -24 to +20dB . Any signal into the detector below threshold will not influence the boost calculation and receives a fixed enhancement
 * @param time_constant Ranges from 0 to 500 milliseconds. It controls the RMS time constant of the detector. Attack and release time will get affected by this parameter
 */
void SigmaDSP::dynamicBass(uint16_t startMemoryAddress, float dB, uint16_t frequency, int16_t threshold, uint16_t time_constant)
{
  float tc = 1000 / (time_constant * FS);
  float th = exp(0.029 * threshold);

  // Set frequency and boost
  dynamicBass(startMemoryAddress, dB, frequency);
  // Set threshold and time constant
  safeload_write(startMemoryAddress + 6, tc, th);
}


/**
 * @brief Controls a hard clip block with separate negative and positive thresholds
 *
 * @param startMemoryAddress DSP memory address
 * @param highThreshold High threshold (0.0 to 1.0)
 * @param lowThreshold Low threshold (-1.0 to 0.0)
 */
void SigmaDSP::hardClip(uint16_t startMemoryAddress, float highThreshold, float lowThreshold)
{
  safeload_write(startMemoryAddress, highThreshold, lowThreshold);
}


/**
 * @brief Controls a soft clip block with adjustable curve
 *
 * @param startMemoryAddress DSP memory address
 * @param alpha Clipping coefficient (0.1 to 10.0)
 */
void SigmaDSP::softClip(uint16_t startMemoryAddress, float alpha)
{
  const float oneThird = 0.333;
  const float twoThird = 0.666;

  safeload_write(startMemoryAddress, alpha, (float)1/alpha, oneThird, twoThird);
}


/**
 * @brief Controls a DC source block
 *
 * @param startMemoryAddress DSP memory address
 * @param level DC value level range (-1.0 to 1.0)
 */
void SigmaDSP::dcSource(uint16_t startMemoryAddress, float level)
{
  safeload_write(startMemoryAddress, level);
}


/**
 * @brief Controls a sine wave source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Sine wave frequency in [Hz]
 */
void SigmaDSP::sineSource(uint16_t startMemoryAddress, float frequency)
{
  float value = (1.00/24000.00)*frequency;

  safeload_write(startMemoryAddress, 0xff, value, 0x800000);
}

/**
 * @brief Controls a sine wave source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Sine wave frequency in [Hz]
 */
void SigmaDSP::sineSource(uint16_t startMemoryAddress, int32_t frequency)
{
  // Multiply frequency by 349.5234375 (ideally 349.525333333)
  int32_t value = (frequency << 9) - (frequency << 7) - (frequency << 5) - (frequency << 1) - (frequency >> 1) + (frequency >> 5) - (frequency >> 7);

  safeload_write(startMemoryAddress, 0xff, value, 0x800000);
}


/**
 * @brief Controls a square wave source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Square wave frequency in [Hz]
 */
void SigmaDSP::squareSource(uint16_t startMemoryAddress, float frequency) { sineSource(startMemoryAddress, frequency); }


/**
 * @brief Controls a sine wave source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Square wave frequency in [Hz]
 */
void SigmaDSP::squareSource(uint16_t startMemoryAddress, int32_t frequency) { sineSource(startMemoryAddress, frequency); }


/**
 * @brief Controls a sawtooth source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Sawtooth frequency in [Hz]
 */
void SigmaDSP::sawtoothSource(uint16_t startMemoryAddress, float frequency)
{
  float value = (0.50/24000.00)*frequency;

  safeload_write(startMemoryAddress, value, 0x800000);
}


/**
 * @brief Controls a sawtooth source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Sawtooth frequency in [Hz]
 */
void SigmaDSP::sawtoothSource(uint16_t startMemoryAddress, int32_t frequency)
{
  // Multiply frequency by 174.75 (ideally 174.762666667)
  int32_t value = (frequency << 8) - (frequency << 6) - (frequency << 4) - frequency - (frequency >> 1) + (frequency >> 2);

  safeload_write(startMemoryAddress, value, 0x800000);
}


/**
 * @brief Controls a triangle wave source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Triangle wave frequency in [Hz]
 */
void SigmaDSP::triangleSource(uint16_t startMemoryAddress, float frequency)
{
  float value = (0.50/24000.00)*frequency;

  safeload_write(startMemoryAddress, 0x00, 0x800000, 0x00, -0x800000, 0x03, value, 0x800000);
}


/**
 * @brief Controls a triangle wave source block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Triangle wave frequency in [Hz]
 */
void SigmaDSP::triangleSource(uint16_t startMemoryAddress, int32_t frequency)
{
  // Multiply frequency by 174.75 (ideally 174.762666667)
  int32_t value = (frequency << 8) - (frequency << 6) - (frequency << 4) - frequency - (frequency >> 1) + (frequency >> 2);

  safeload_write(startMemoryAddress, 0x00, 0x800000, 0x00, -0x800000, 0x03, value, 0x800000);
}


/**
 * @brief Controls an audio delay block
 *
 * Delay ranges:
 * 0.0-42.6ms (ADAU170x) @ 48kHz;
 * 0.0-21.3ms (ADAU170x) @ 96kHz;
 * 0.0-10.6ms (ADAU170x) @ 192kHz;
 *
 * WARNING! Delays calculated are theoretical assuming you
 * have 100% data memory available in your Sigma Studio design.
 * Data memory is shared among other blocks in Sigma Studio so
 * in practice, this much data memory is not available to the user
 * because every block in a design uses a few data memory locations
 * for its processing. The SigmaStudio compiler manages the data
 * RAM and indicates if the number of addresses needed in the
 * design exceeds the maximum available.
 *
 * @param startMemoryAddress DSP memory address
 * @param delayMs Delay value in [ms]
 */
void SigmaDSP::audioDelay(uint16_t startMemoryAddress, float delayMs)
{
  int32_t ticks = (int32_t)(delayMs*0.001/(1/FS));

  if(ticks > 2048)
    ticks = 2048;

  safeload_write(startMemoryAddress, ticks);
}


/**
 * @brief Controls a first order EQ block
 *
 * @param startMemoryAddress DSP memory address
 * @param equalizer Equalizer parameter struct
 */
void SigmaDSP::EQfirstOrder(uint16_t startMemoryAddress, firstOrderEQ_t &equalizer)
{
  float w0, gainLinear;
  float b0, b1, a1;
  float coefficients[3];

  w0=2*PI*equalizer.freq/FS; //2*PI*freq/FS
  gainLinear = pow(10,(equalizer.gain/20)); //10^(gain/20)

  switch(equalizer.filterType)
  {
// Lowpass
    case parameters::filterType::lowpass:
    default:
      a1 = pow(2.7,-w0);
      b0 = gainLinear * (1.0 - a1);
      b1 = 0;
      break;

// Highpass
    case parameters::filterType::highpass:
      a1 = pow(2.7,-w0);
      b0 = gainLinear * a1;
      b1 = -a1 * gainLinear;
      break;
  }

  if(equalizer.state == parameters::state::on)
  {
    if(equalizer.phase == parameters::phase::nonInverted) // 0 deg
    {
      coefficients[0] = b0;
      coefficients[1] = b1;
      coefficients[2] = a1;
    }
    else //if(equalizer.phase == parameters::phase::inverted) // 180 deg
    {
      coefficients[0] = -1*b0;
      coefficients[1] = -1*b1;
      coefficients[2] = a1; // This coefficient does not change sign
    }
  }
  else //if(equalizer.state == parameters::state::off)
  {
    coefficients[0] = 1.00;
    coefficients[1] = 0.00;
    coefficients[2] = 0.00;
  }

  safeload_write(startMemoryAddress, coefficients[0], coefficients[1], coefficients[2]);
}


/**
 * @brief Controls a second order EQ block
 *
 * @param startMemoryAddress DSP memory address
 * @param equalizer Equalizer parameter struct
 */
void SigmaDSP::EQsecondOrder(uint16_t startMemoryAddress, secondOrderEQ_t &equalizer)
{
  float A, w0, alpha, gainLinear;
  float b0, b1, b2, a0, a1, a2;
  float coefficients[5];

  A=pow(10,(equalizer.boost/40));           // 10^(boost/40)
  w0=2*PI*equalizer.freq/FS;                // 2*PI*freq/FS
  gainLinear = pow(10,(equalizer.gain/20)); // 10^(gain/20)

  switch(equalizer.filterType)
  {
// Parametric
    case parameters::filterType::parametric:
// Peaking
    case parameters::filterType::peaking:
    default:
      alpha = sin(w0)/(2*equalizer.Q);
      a0 =  1 + alpha/A;
      a1 = -2 * cos(w0);
      a2 =  1 - alpha/A;
      b0 = (1 + alpha*A) * gainLinear;
      b1 = -(2 * cos(w0)) * gainLinear;
      b2 = (1 - alpha*A) * gainLinear;
      break;

// Low shelf
    case parameters::filterType::lowShelf:
      alpha=sin(w0)/2*sqrt((A+1/A)*(1/equalizer.S-1)+2);
      a0 = (A+1)+(A-1)*cos(w0)+2*sqrt(A)*alpha;
      a1 = -2*((A-1)+(A+1)*cos(w0));
      a2 = (A+1)+(A-1)*cos(w0)-2*sqrt(A)*alpha;
      b0 = A*((A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha)*gainLinear;
      b1 = 2*A*((A-1)-(A+1)*cos(w0)) * gainLinear;
      b2 = A*((A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha)*gainLinear;
      break;

// High shelf
    case parameters::filterType::highShelf:
      alpha = sin(w0)/2 * sqrt((A + 1/A)*(1/equalizer.S - 1) + 2);
      a0 = (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha;
      a1 = 2*((A-1) - (A+1)*cos(w0));
      a2 = (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha;
      b0 = A*((A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha) * gainLinear;
      b1 = -2*A*((A-1) + (A+1)*cos(w0)) * gainLinear;
      b2 = A*((A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha) * gainLinear;
      break;

// Lowpass
    case parameters::filterType::lowpass:
      alpha = sin(w0)/(2*equalizer.Q);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 - cos(w0)) * (gainLinear/2);
      b1 = (1 - cos(w0))  * gainLinear;
      b2 = (1 - cos(w0)) * (gainLinear/2);
      break;

// Highpass
    case parameters::filterType::highpass:
      alpha = sin(w0)/(2*equalizer.Q);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 + cos(w0)) * (gainLinear/2);
      b1 = -(1 + cos(w0)) * gainLinear;
      b2 = (1 + cos(w0)) * (gainLinear/2);
      break;

// Bandpass
    case parameters::filterType::bandpass:
      alpha = sin(w0) * sinh(log(2)/(2 * equalizer.bandwidth * w0/sin(w0)));
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = alpha * gainLinear;
      b1 = 0;
      b2 = -alpha * gainLinear;
      break;

// Bandstop
    case parameters::filterType::bandstop:
      alpha = sin(w0) * sinh(log(2)/(2 * equalizer.bandwidth * w0/sin(w0)));
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = 1 * gainLinear;
      b1 = -2*cos(w0) * gainLinear;
      b2 = 1 * gainLinear;
      break;

// Butterworth lowpass
    case parameters::filterType::butterworthLowpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(2);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 - cos(w0)) * gainLinear / 2;
      b1 = (1 - cos(w0)) * gainLinear;
      b2 = (1 - cos(w0)) * gainLinear / 2;
      break;

// Butterworth highpass
    case parameters::filterType::butterworthHighpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(2);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 + cos(w0)) * gainLinear / 2;
      b1 = -(1 + cos(w0)) * gainLinear;
      b2 = (1 + cos(w0)) * gainLinear / 2;
      break;

// Bessel lowpass
    case parameters::filterType::besselLowpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(3) ;
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 - cos(w0)) * gainLinear / 2;
      b1 = (1 - cos(w0)) * gainLinear;
      b2 = (1 - cos(w0)) * gainLinear / 2;
      break;

// Bessel highpass
    case parameters::filterType::besselHighpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(3) ;
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 + cos(w0)) * gainLinear / 2;
      b1 = -(1 + cos(w0)) * gainLinear;
      b2 = (1 + cos(w0)) * gainLinear / 2;
      break;
  }

  // For Sigma DSP implementation we need to normalize all the coefficients respect to a0
  // and inverting by sign a1 and a2
  if(a0 != 0.00 && equalizer.state == parameters::state::on)
  {
    if(equalizer.phase == parameters::phase::nonInverted) // 0 deg
    {
      coefficients[0] = b0/a0;
      coefficients[1] = b1/a0;
      coefficients[2] = b2/a0;
      coefficients[3] = -1*a1/a0;
      coefficients[4] = -1*a2/a0;
    }
    else //if(equalizer.phase == parameters::phase::inverted) // 180 deg
    {
      coefficients[0] = -1*b0/a0;
      coefficients[1] = -1*b1/a0;
      coefficients[2] = -1*b2/a0;
      coefficients[3] = -1*a1/a0; // This coefficient does not change sign!
      coefficients[4] = -1*a2/a0; // This coefficient does not change sign!
    }
  }
  else //if(equalizer.state == parameters::state::off)
  {
    coefficients[0] = 1.00;
    coefficients[1] = 0;
    coefficients[2] = 0;
    coefficients[3] = 0;
    coefficients[4] = 0;
  }

  safeload_write(startMemoryAddress, coefficients[0], coefficients[1], coefficients[2], coefficients[3], coefficients[4]);
}


/**
 * @brief Controls a baxandall low-high dual tone control block
 *
 * @param startMemoryAddress DSP memory address
 * @param toneCtrl Tone control parameter struct
 */
void SigmaDSP::toneControl(uint16_t startMemoryAddress, toneCtrl_t &toneCtrl)
{
  float tb,bb,wT,wB,Knum_T,Kden_T,Knum_B,Kden_B,alpha0,beta1,alpha1,beta2,alpha2,beta3,alpha3,beta4;
  float b0,b1,b2,a0,a1,a2;
  float coefficients[5];

  tb = pow(10, toneCtrl.boost_Treble_dB / 20.0);
  bb = pow(10, toneCtrl.boost_Bass_dB / 20.0);

  wT = tan(PI * toneCtrl.freq_Treble / FS);
  wB = tan(PI * toneCtrl.freq_Bass / FS);

  Knum_T = 2 / (1 + (1.0 / tb));
  Kden_T = 2 / (1 + tb);
  Knum_B = 2.0 / (1.0 + (1.0 / bb));
  Kden_B = 2.0 / (1.0 + bb);

  alpha0 = wT + Kden_T;
  beta1 = wT + Knum_T;
  alpha1 = wT - Kden_T;
  beta2 = wT - Knum_T;

  alpha2 = (wB*Kden_B) + 1;
  beta3 = (wB*Knum_B) - 1;
  alpha3 = (wB*Kden_B) - 1;
  beta4 = (wB*Knum_B) + 1;

  a0 = alpha0 * alpha2;
  a1 = (alpha0 * alpha3) + (alpha1 * alpha2);
  a2 = alpha1 * alpha3;
  b0 = beta1 * beta3;
  b1 = (beta1 * beta4) + (beta2 * beta3);
  b2 = beta2 * beta4;

  // For Sigma DSP implementation we need to normalize all the coefficients respect to a0
  // and inverting a1 and a2 by sign
  if(a0 != 0.00 && toneCtrl.state == parameters::state::on)
  {
    if(toneCtrl.phase == parameters::phase::nonInverted) // 0 deg
    {
      coefficients[0] = b0/a0;
      coefficients[1] = b1/a0;
      coefficients[2] = b2/a0;
      coefficients[3] = -1*a1/a0;
      coefficients[4] = -1*a2/a0;
    }
    else //if(toneCtrl.phase == parameters::phase::inverted) // 180 deg
    {
      coefficients[0] = -1*b0/a0;
      coefficients[1] = -1*b1/a0;
      coefficients[2] = -1*b2/a0;
      coefficients[3] = a1/a0;
      coefficients[4] = a2/a0;
    }
  }
  else //if(toneCtrl.state == parameters::state::off)
  {
    coefficients[0] = 1.00;
    coefficients[1] = 0;
    coefficients[2] = 0;
    coefficients[3] = 0;
    coefficients[4] = 0;
  }

  safeload_write(startMemoryAddress, coefficients[0], coefficients[1], coefficients[2], coefficients[3], coefficients[4]);
}


/**
 * @brief Controls a state variable filter block
 *
 * @param startMemoryAddress DSP memory address
 * @param frequency Filter frequency (1 to 19148 Hz)
 * @param q Filter Q value (1.28 to 10.00)
 */
void SigmaDSP::stateVariable(uint16_t startMemoryAddress, float frequency, float q)
{
  float param1 = 2*sin(PI*frequency/FS);
  float param2 = 1/q;

  safeload_write(startMemoryAddress, param1, param2);
}


/**
 * @brief Calculates the curve and the other parameters of an RMS compressor block.
 *        Set ratio = 1 to disable compressor
 *
 * @param startMemoryAddress DSP memory address
 * @param compressor Compressor parameter struct
 */
void SigmaDSP::compressorRMS(uint16_t startMemoryAddress, compressor_t &compressor)
{
  uint8_t i;
  uint8_t count;

  float curve[34];
  float x[34];
  float y[34];
  float delta = 0.00;
  float coeff = 0.00;

  float dbps = 0.00;
  float attack_par = 0.00;
  float hold_par = 0.00;
  float decay_par = 0.00;
  float postgain_par = 0.00;

  linspace(-90, 6, 34, x);

  count = 0;

  coeff = 1/compressor.ratio;
  for(i = 0; i < 34; i++) // This algorithm creates the curve with 1 1 1 1 1 1 1 0.9 0.8 0.72 0.64 and so on coefficients
  {
    if(x[i] >= compressor.threshold)
    {
      count++;
      if(count == 1)
        delta = x[i]*coeff-x[i]*1;
      y[i] = (x[i] * coeff)-delta;
    }
    else
      y[i] = x[i] * 1;
  }

  for(i = 0; i < 34; i++)  // Coefficients of the curve calculation
    curve[i] = (pow(10, y[i]/20)) / (pow(10, x[i]/20));  // Coefficients are the ratio between the linearized values of vect. y and x

  // Create buffer to store converted data
  uint8_t storeData[5];

  // Parameter load into Sigma DSP
  for(i = 0; i < 34; i++)
  {
    floatToFixed(curve[i], storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, false);
  }

  // Conversion dBps -> ms
  // dBps = 121;
  // TCms = (20/(dBps*2.3))*1000

  // RMS TC (dB/s)
  dbps = (20/(compressor.rms_tc*2.3))*1e3;
  attack_par = abs(1.0 - pow(10,(dbps/(10*FS))));
  floatToFixed(attack_par, storeData);
  safeload_writeRegister(startMemoryAddress++, storeData, true);

  //#ifdef COMPRESSORWITHPOSTGAIN
    postgain_par = pow(10, compressor.postgain/20);
    floatToFixed(postgain_par, storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, true);
  //#endif

  // Hold
  hold_par = compressor.hold*FS/1000;
  floatToFixed(hold_par, storeData);
  safeload_writeRegister(startMemoryAddress++, storeData, true);

  // Decay (dB/s)
  dbps = (20/(compressor.decay*2.3))*1e3;
  decay_par = dbps/(96*FS);
  floatToFixed(decay_par, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/**
 * @brief This function calculates the curve and the other parameters of a peak compressor block.
          Set ratio = 1 to disable compressor
 *
 * @param startMemoryAddress DSP memory address
 * @param compressor Compressor parameter struct
 */
void SigmaDSP::compressorPeak(uint16_t startMemoryAddress, compressor_t &compressor)
{
  uint8_t i;
  uint8_t count;

  float curve[33];
  float x[33];
  float y[33];
  float delta = 0.00;
  float coeff = 0.00;

  float dbps = 0.00;
  float hold_par = 0.00;
  float decay_par = 0.00;
  float postgain_par = 0.00;

  linspace(-90, 6, 33, x);

  count = 0;

  coeff = 1/compressor.ratio;
  for(i = 0; i < 33; i++) // This algorithm creates the curve with 1 1 1 1 1 1 1 0.9 0.8 0.72 0.64 and so on coefficients
  {
    if(x[i] >= compressor.threshold)
    {
      count++;
      if(count == 1)
        delta = x[i]*coeff-x[i]*1;
      y[i] = (x[i] * coeff)-delta;
    }
    else
      y[i] = x[i] * 1;
  }

  for(i = 0; i < 33; i++)  // Coefficients of the curve calculation
    curve[i] = (pow(10, y[i]/20)) / (pow(10, x[i]/20));  // Coefficients are the ratio between the linearized values of vect. y and x

  // Create buffer to store converted data
  uint8_t storeData[5];

  // Parameter load into Sigma DSP
  for(i = 0; i < 33; i++)
  {
    floatToFixed(curve[i], storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, false);
  }

 // #ifdef COMPRESSORWITHPOSTGAIN
    postgain_par = pow(10, compressor.postgain/40);
    floatToFixed(postgain_par, storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, true);
  //#endif

  // Hold
  hold_par = compressor.hold*FS/1000;
  floatToFixed(hold_par, storeData);
  safeload_writeRegister(startMemoryAddress++, storeData, true);

  // Decay (dB/s)
  dbps = (20/(compressor.decay*2.3))*1e3;
  decay_par = dbps/(96*FS);
  floatToFixed(decay_par, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/**
 * @brief Mutes the internal analog to digital converter in the DSP
 *
 * @param mute Mute state. Muted if true, unmuted if false
 */
void SigmaDSP::muteADC(bool mute)
{
  // Read two bytes from the DSP core register
  uint16_t coreRegister = readRegister(dspRegister::CoreRegister, 2);

  uint8_t muteADCdata[2];

  muteADCdata[0] = (coreRegister >> 8) & 0xFF;

  // Set or clear only the DAC mute/unmute bit
  if(mute == true)
    muteADCdata[1] = coreRegister & 0xEF;
  else
    muteADCdata[1] = coreRegister | 0x10;

  writeRegister(dspRegister::CoreRegister, sizeof(muteADCdata), muteADCdata);
}


/**
 * @brief Mutes the internal digital to analog in the DSP
 *
 * @param mute Mute state. Muted if true, unmuted if false
 */
void SigmaDSP::muteDAC(bool mute)
{
  // Read two bytes from the DSP core register
  uint16_t coreRegister = readRegister(dspRegister::CoreRegister, 2);

  uint8_t muteDACdata[2];

  muteDACdata[0] = (coreRegister >> 8) & 0xFF;

  // Set or clear only the DAC mute/unmute bit
  if(mute == true)
    muteDACdata[1] = coreRegister & 0xF7;
  else
    muteDACdata[1] = coreRegister | 0x08;

  writeRegister(dspRegister::CoreRegister, sizeof(muteDACdata), muteDACdata);
}


/**
 * @brief Writes 5 bytes of data to the parameter memory of the DSP. The first byte is 0x00
 *
 * @param memoryAddress DSP memory address
 * @param data Data array to write
 * @param finished Indicates if this is the last packet or not
 */
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress, uint8_t *data, bool finished)
{
  static uint8_t _safeload_count = 0; // Keeps track of the safeload count

  uint8_t addr[2]; // Address array

  addr[0] = (memoryAddress >> 8) & 0xFF;
  addr[1] = memoryAddress & 0xFF;

  writeRegister(dspRegister::SafeloadAddress0 + _safeload_count, sizeof(addr), addr); // Place passed 16-bit memory address in safeload address area

  // Q: Why is the safeload registers five bytes long, while I'm loading four-byte parameters into the RAM using these registers?
  // A: The safeload registers are also used to load the slew RAM data, which is five bytes long. For parameter RAM writes using safeload,
  // the first byte of the safeload register can be set to 0x00.

  // Needs 5 bytes of data
  writeRegister(dspRegister::SafeloadData0 + _safeload_count, 5, data); // Placed passed data (5 bytes) in the next safeload data space

  _safeload_count++; // Increase counter

  if(finished == true || _safeload_count >= 5) // Max 5 safeload memory registers
  {
    addr[0] = 0x00;
    addr[1] = 0x3C; // Set the IST bit (initiate safeload transfer bit)
    writeRegister(dspRegister::CoreRegister, sizeof(addr), addr); // Load content from the safeload registers
    _safeload_count = 0;
  }
}

/**
 * @brief Writes 28.0 data to the parameter memory of the DSP.
 *
 * @param memoryAddress DSP memory address
 * @param data Data array to write
 * @param finished Indicates if this is the last packet or not
 */
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress, int32_t data, bool finished)
{
  uint8_t dataArray[5];
  intToFixed(data, dataArray);
  safeload_writeRegister(memoryAddress, dataArray, finished);
}

/**
 * @brief Writes 5.23 data to the parameter memory of the DSP.
 *
 * @param memoryAddress DSP memory address
 * @param data Data array to write
 * @param finished Indicates if this is the last packet or not
 */
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress, float data, bool finished)
{
  uint8_t dataArray[5];
  floatToFixed(data, dataArray);
  safeload_writeRegister(memoryAddress, dataArray, finished);
}

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress, int16_t data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
#else
  void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress,     int data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
#endif
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress,  uint32_t data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress,  uint16_t data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress,   uint8_t data, bool finished) { safeload_writeRegister(memoryAddress, (int32_t)data, finished); }
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress,    double data, bool finished) { safeload_writeRegister(memoryAddress,   (float)data, finished); }


/**
 * @brief Writes data to the DSP (max 32 bytes a time due to the default i2c buffer size of the Wire library)
 *
 * @param memoryAddress DSP memory address
 * @param length Number of bytes to write
 * @param data Data array to write
 */
void SigmaDSP::writeRegister(uint16_t memoryAddress, uint8_t length, uint8_t *data)
{
  uint8_t LSByte = (uint8_t)memoryAddress & 0xFF;
  uint8_t MSByte = memoryAddress >> 8;

  _WireObject.beginTransmission(_dspAddress); // Begin write

  _WireObject.write(MSByte); // Send high address
  _WireObject.write(LSByte); // Send low address

  for(uint8_t i = 0; i < length; i++)
    _WireObject.write(data[i]); // Send all bytes in passed array

  _WireObject.endTransmission(); // Write out data to I2C and stop transmitting
}


/**
 * @brief Writes data to the DSP from PROGMEM (max 32 bytes a time due to the default i2c buffer size of the Wire library)
 *
 * @param memoryAddress DSP memory address
 * @param length Number of bytes to write
 * @param data Data array to write
 */
void SigmaDSP::writeRegister(uint16_t memoryAddress, uint8_t length, const uint8_t *data)
{
  uint8_t LSByte = (uint8_t)memoryAddress & 0xFF;
  uint8_t MSByte = memoryAddress >> 8;

  _WireObject.beginTransmission(_dspAddress); // Begin write

  _WireObject.write(MSByte); // Send high address
  _WireObject.write(LSByte); // Send low address

  for(uint8_t i = 0; i < length; i++)
    _WireObject.write(pgm_read_byte(&data[i])); // Send all bytes in passed array

  _WireObject.endTransmission(); // Write out data to I2C and stop transmitting
}


/**
 * @brief Writes data to the DSP from PROGMEM
 *
 * @param memoryAddress DSP memory address
 * @param length Number of bytes to write
 * @param data Data array to write
 * @param registerSize Number of bytes each register can hold
 */
void SigmaDSP::writeRegisterBlock(uint16_t memoryAddress, uint16_t length, const uint8_t *data, uint8_t registerSize)
{
  uint8_t MSByte = 0;
  uint8_t LSByte = 0;
  uint16_t bytesSent = 0;

  // Run until all bytes are sent
  while(bytesSent < length)
  {
    // Convert address to 8-bit
    MSByte = memoryAddress >> 8;
    LSByte = (uint8_t)memoryAddress & 0xFF;

    _WireObject.beginTransmission(_dspAddress);
    _WireObject.write(MSByte); // Send high address
    _WireObject.write(LSByte); // Send low address
    for(uint8_t i = 0; i < registerSize; i++) // Send n bytes
    {
      _WireObject.write(pgm_read_byte(&data[bytesSent]));
      bytesSent++;
    }
    _WireObject.endTransmission();

    memoryAddress++; // Increase address
  }
}


/**
 * @brief Reads a DSP memory block
 *
 * @param memoryAddress DSP memory address
 * @param readout Which register to read
 * @param numberOfBytes How manu bytes to read
 * @return int32_t value where four bytes in the block are concatenated
 */
int32_t SigmaDSP::readBack(uint16_t memoryAddress, uint16_t readout, uint8_t numberOfBytes)
{
  uint8_t LSByte = (uint8_t)memoryAddress & 0xFF;
  uint8_t MSByte = memoryAddress >> 8;

  _WireObject.beginTransmission(_dspAddress); // Begin write
  _WireObject.write(MSByte); // Send high address
  _WireObject.write(LSByte); // Send low address
  LSByte = (uint8_t)readout & 0xFF;
  MSByte = readout >> 8;
  _WireObject.write(MSByte); // Send high register to read
  _WireObject.write(LSByte); // Send low register to read
  _WireObject.endTransmission();

  _WireObject.beginTransmission(_dspAddress);
  LSByte = (uint8_t)memoryAddress & 0xFF;
  MSByte = memoryAddress >> 8;
  _WireObject.write(MSByte);
  _WireObject.write(LSByte);
  _WireObject.endTransmission(false);

  int32_t returnVal = 0;
  _WireObject.requestFrom(_dspAddress, numberOfBytes);
  for(uint8_t i = 0; i < numberOfBytes; i++)
    returnVal = returnVal << 8 | _WireObject.read();

  return returnVal;
}


/**
 * @brief Reads a DSP hardware register
 *
 * @param reg DSP register address
 * @param numberOfBytes Number of bytes to read from the register
 * @return uint32_t value the register contains
 */
uint32_t SigmaDSP::readRegister(dspRegister reg, uint8_t numberOfBytes)
{
  uint8_t LSByte = (uint8_t)reg & 0xFF;
  uint8_t MSByte = reg >> 8;

  _WireObject.beginTransmission(_dspAddress); // Begin write
  _WireObject.write(MSByte); // Send high address
  _WireObject.write(LSByte); // Send low address
  _WireObject.endTransmission(false);

  uint32_t returnVal = 0;
  _WireObject.requestFrom(_dspAddress, numberOfBytes);
  for(uint8_t i = 0; i < numberOfBytes; i++)
    returnVal = returnVal << 8 | _WireObject.read();

  return returnVal;
}


/**
 * @brief Converts a 5.23 float value to 5-byte HEX and stores it in the passed buffer
 *
 * @param value Value to convert
 * @param buffer Buffer to store the converted data in
 */
void SigmaDSP::floatToFixed(float value, uint8_t *buffer)
{
  // Convert float to 4 byte hex
  int32_t fixedval = (value * ((int32_t)1 << 23));

  // Store the 4 bytes in the passed buffer
  buffer[0] = 0x00; // First must be empty
  buffer[1] = (fixedval >> 24) & 0xFF;
  buffer[2] = (fixedval >> 16) & 0xFF;
  buffer[3] = (fixedval >>  8) & 0xFF;
  buffer[4] = fixedval & 0xFF;
}


/**
 * @brief Converts a 28.0 integer value to 5-byte HEX and stores it the passed buffer
 *
 * @param value Value to convert
 * @param buffer Buffer to store the converted data in
 */
void SigmaDSP::intToFixed(int32_t value, uint8_t *buffer)
{
  // Store the 4 bytes in the passed buffer
  buffer[0] = 0x00; // First must be empty
  buffer[1] = (value >> 24) & 0xFF;
  buffer[2] = (value >> 16) & 0xFF;
  buffer[3] = (value >>  8) & 0xFF;
  buffer[4] = value & 0xFF;
}


/**
 * @brief Converts a 5.23 float value to int 28.0
 *
 * @param value Value to convert
 * @return int32_t converted value in 28.0 format
 */
int32_t SigmaDSP::floatToInt(float value)
{
  // Convert float 5.23 to int 28.0
  return (value * ((int32_t)1 << 23));
}


/*******************************************************************************************
**                                     PRIVATE METHODS                                    **
*******************************************************************************************/


/**
 * @brief Calculates the points of a linearly spaced vector
 *
 * @param x1 Start point
 * @param x2 End point
 * @param n Number of points to calculate
 * @param vect Array to store calculated data in
 */
void SigmaDSP::linspace(float x1, float x2, float n, float *vect)
{
  float k = (abs(x1) + abs(x2))/(n);

  for(uint8_t i = 0; i < n; i++)
    vect[i] = x1+(k*i);
}
